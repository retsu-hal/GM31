#include "main.h"
#include "Editor.h"
#include "Manager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Component.h"
#include "Camera.h"
#include "Gizmo.h"
#include "Profiler.h"	// TypeName

#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

#include "imgui_internal.h"	// DockBuilder（レイアウトの初期配置）に必要

#include <typeinfo>
#include <algorithm>
#include <cfloat>

//==============================================================================
// static メンバー
//==============================================================================
#if _DEBUG
bool Editor::m_Enable = true;
#else
bool Editor::m_Enable = false;	// Release ではゲーム画面だけ
#endif
Editor::PlayState Editor::m_PlayState = Editor::PlayState::Playing;
bool Editor::m_StepRequested = false;
bool Editor::m_ResetLayout = false;

unsigned int Editor::m_SelectedID = 0;

bool Editor::m_ShowHierarchy = true;
bool Editor::m_ShowInspector = true;
bool Editor::m_ShowScene = true;
bool Editor::m_ShowConsole = true;
bool Editor::m_ShowStats = true;
bool Editor::m_ShowDemo = false;

std::vector<Editor::LogEntry> Editor::m_Logs;
bool Editor::m_AutoScroll = true;

// ウィンドウ名（DockBuilder でも使うのでまとめておく）
static const char* WINDOW_HIERARCHY = "Hierarchy";
static const char* WINDOW_INSPECTOR = "Inspector";
static const char* WINDOW_SCENE     = "Scene";
static const char* WINDOW_CONSOLE   = "Console";
static const char* WINDOW_STATS     = "Stats";

static const char* PAYLOAD_GAMEOBJECT = "GAMEOBJECT_ID";	// Hierarchy のドラッグ＆ドロップ用

static const size_t LOG_MAX = 1000;

//==============================================================================
// 小物
//==============================================================================
namespace
{
	// Hierarchy の操作はツリーを描いている途中で親子を変えると壊れるので、描き終わってからまとめて行う
	struct PendingParent { unsigned int Child; unsigned int Parent; };	// Parent=0 で親子解除
	std::vector<PendingParent> g_PendingParents;
	std::vector<unsigned int>  g_PendingDestroy;
	bool g_PendingCreate = false;
	unsigned int g_PendingCreateParent = 0;

	ImGuiTextFilter g_HierarchyFilter;
	ImGuiTextFilter g_ConsoleFilter;

	ImVec2 Add(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }

	std::string ClassName(const void* object, const std::type_info& info)
	{
		(void)object;
		return TypeName(info.name());
	}

	// ゲーム画面のアスペクト比を保ったまま avail に収まる大きさ
	ImVec2 FitGameAspect(const ImVec2& avail)
	{
		const float aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
		ImVec2 size = avail;
		if (size.x / size.y > aspect) size.x = size.y * aspect;
		else                          size.y = size.x / aspect;
		if (size.x < 1.0f) size.x = 1.0f;
		if (size.y < 1.0f) size.y = 1.0f;
		return size;
	}

	// ワールド座標 → 画面座標。カメラの後ろなら false
	bool WorldToScreen(const Vector3& world, const XMMATRIX& viewProj, const ImVec2& pos, const ImVec2& size, ImVec2* out, float* depth = nullptr)
	{
		XMVECTOR clip = XMVector4Transform(XMVectorSet(world.x, world.y, world.z, 1.0f), viewProj);
		float w = XMVectorGetW(clip);
		if (w <= 0.0001f) return false;
		float x = XMVectorGetX(clip) / w;
		float y = XMVectorGetY(clip) / w;
		out->x = pos.x + (x * 0.5f + 0.5f) * size.x;
		out->y = pos.y + (-y * 0.5f + 0.5f) * size.y;
		if (depth) *depth = w;
		return true;
	}

	ImTextureID ToTextureID(ID3D11ShaderResourceView* srv)
	{
		return (ImTextureID)(intptr_t)srv;
	}
}

//==============================================================================
// 初期化・終了
//==============================================================================
void Editor::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	// ウィンドウをドッキングできるようにする
	io.ConfigWindowsMoveFromTitleBarOnly = true;		// Scene ビューをドラッグしてもウィンドウが動かないように

	// 日本語を表示できるフォント（なければ ImGui 標準フォントのまま）
	const char* fontPath = "C:\\Windows\\Fonts\\meiryo.ttc";
	if (GetFileAttributesA(fontPath) != INVALID_FILE_ATTRIBUTES)
	{
		io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);	// 1.92 以降は必要な文字をその都度読むので文字範囲の指定は不要
	}

	ApplyStyle();
	Log("Editor initialized (F2: toggle editor)");
}

void Editor::Uninit()
{
	m_Logs.clear();
	g_PendingParents.clear();
	g_PendingDestroy.clear();
}

//==============================================================================
// Unity っぽい暗めの配色
//==============================================================================
void Editor::ApplyStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 2.0f;
	style.FrameRounding = 2.0f;
	style.TabRounding = 2.0f;
	style.ScrollbarRounding = 2.0f;
	style.GrabRounding = 2.0f;
	style.WindowBorderSize = 1.0f;
	style.FrameBorderSize = 0.0f;
	style.WindowPadding = ImVec2(6.0f, 6.0f);
	style.FramePadding = ImVec2(6.0f, 3.0f);
	style.ItemSpacing = ImVec2(6.0f, 4.0f);

	ImVec4* c = style.Colors;
	const ImVec4 bg      = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	const ImVec4 bgDark  = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	const ImVec4 frame   = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	const ImVec4 hover   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	const ImVec4 active  = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	const ImVec4 accent  = ImVec4(0.17f, 0.36f, 0.53f, 1.00f);	// 選択中の青
	const ImVec4 accentH = ImVec4(0.23f, 0.45f, 0.65f, 1.00f);

	c[ImGuiCol_WindowBg]           = bg;
	c[ImGuiCol_ChildBg]            = bg;
	c[ImGuiCol_PopupBg]            = bgDark;
	c[ImGuiCol_MenuBarBg]          = bgDark;
	c[ImGuiCol_Border]             = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	c[ImGuiCol_FrameBg]            = frame;
	c[ImGuiCol_FrameBgHovered]     = hover;
	c[ImGuiCol_FrameBgActive]      = active;
	c[ImGuiCol_TitleBg]            = bgDark;
	c[ImGuiCol_TitleBgActive]      = bgDark;
	c[ImGuiCol_TitleBgCollapsed]   = bgDark;
	c[ImGuiCol_Button]             = frame;
	c[ImGuiCol_ButtonHovered]      = hover;
	c[ImGuiCol_ButtonActive]       = active;
	c[ImGuiCol_Header]             = accent;
	c[ImGuiCol_HeaderHovered]      = hover;
	c[ImGuiCol_HeaderActive]       = accentH;
	c[ImGuiCol_Tab]                = bgDark;
	c[ImGuiCol_TabHovered]         = hover;
	c[ImGuiCol_TabSelected]        = bg;
	c[ImGuiCol_TabSelectedOverline] = accentH;
	c[ImGuiCol_TabDimmed]          = bgDark;
	c[ImGuiCol_TabDimmedSelected]  = bg;
	c[ImGuiCol_DockingPreview]     = accentH;
	c[ImGuiCol_DockingEmptyBg]     = bgDark;
	c[ImGuiCol_CheckMark]          = accentH;
	c[ImGuiCol_SliderGrab]         = accentH;
	c[ImGuiCol_SliderGrabActive]   = accentH;
	c[ImGuiCol_SeparatorHovered]   = accentH;
	c[ImGuiCol_SeparatorActive]    = accentH;
	c[ImGuiCol_ResizeGrip]         = frame;
	c[ImGuiCol_ResizeGripHovered]  = hover;
	c[ImGuiCol_ResizeGripActive]   = accentH;
}

//==============================================================================
// フレーム開始：メニューバーと DockSpace
// DockSpace は、そこにドッキングするウィンドウより先に出しておく必要がある
//==============================================================================
void Editor::BeginFrame()
{
#if _DEBUG
	if (Input::GetKeyTrigger(VK_F2))
	{
		m_Enable = !m_Enable;
		Log(m_Enable ? "Editor: ON" : "Editor: OFF (game view only)");
	}
#endif

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImGuiID dockspaceID = ImGui::GetID("EditorDockSpace");

	if (!m_Enable)
	{
		// 非表示でも DockSpace は生かしておく（ドッキング中のウィンドウが勝手に外れないように）
		ImGui::DockSpaceOverViewport(dockspaceID, viewport, ImGuiDockNodeFlags_KeepAliveOnly);
		Input::SetGameViewFocused(false);
		return;
	}

	DrawMainMenuBar();	// メニューバーぶん WorkSize が縮むので DockSpace より先

	if (m_ResetLayout || ImGui::DockBuilderGetNode(dockspaceID) == nullptr)
	{
		BuildDefaultLayout(dockspaceID);
		m_ResetLayout = false;
	}

	ImGui::DockSpaceOverViewport(dockspaceID, viewport);
}

//==============================================================================
// 初期レイアウト（imgui.ini にレイアウトがないとき / Window > Reset Layout）
//==============================================================================
void Editor::BuildDefaultLayout(unsigned int dockspaceID)
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::DockBuilderRemoveNode(dockspaceID);
	ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->WorkSize);

	ImGuiID center = dockspaceID;
	ImGuiID left   = ImGui::DockBuilderSplitNode(center, ImGuiDir_Left,  0.18f, nullptr, &center);
	ImGuiID right  = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.26f, nullptr, &center);
	ImGuiID bottom = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down,  0.28f, nullptr, &center);
	ImGuiID rightBottom = ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.40f, nullptr, &right);

	ImGui::DockBuilderDockWindow(WINDOW_HIERARCHY, left);
	ImGui::DockBuilderDockWindow(WINDOW_INSPECTOR, right);
	ImGui::DockBuilderDockWindow(WINDOW_SCENE, center);
	ImGui::DockBuilderDockWindow(WINDOW_CONSOLE, bottom);
	ImGui::DockBuilderDockWindow(WINDOW_STATS, bottom);

	// 既存のデバッグウィンドウも右下にまとめておく（あとからドラッグで自由に動かせる）
	ImGui::DockBuilderDockWindow("Gizmo", rightBottom);
	ImGui::DockBuilderDockWindow("PlayerDebug", rightBottom);

	ImGui::DockBuilderFinish(dockspaceID);
}

//==============================================================================
// メニューバー + 再生ボタン
//==============================================================================
void Editor::DrawMainMenuBar()
{
	if (!ImGui::BeginMainMenuBar()) return;

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit")) PostMessage(GetWindow(), WM_CLOSE, 0, 0);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty")) { g_PendingCreate = true; g_PendingCreateParent = 0; }
		GameObject* selected = GetSelected();
		if (ImGui::MenuItem("Create Empty Child", nullptr, false, selected != nullptr))
		{
			g_PendingCreate = true;
			g_PendingCreateParent = selected->GetID();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Delete", "Del", false, selected != nullptr)) g_PendingDestroy.push_back(selected->GetID());
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scene"))
	{
		if (ImGui::MenuItem("Title"))  Manager::ChangeScene<TitleScene>();
		if (ImGui::MenuItem("Game"))   Manager::ChangeScene<GameScene>();
		if (ImGui::MenuItem("Result")) Manager::ChangeScene<ResultScene>();
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		ImGui::MenuItem(WINDOW_HIERARCHY, nullptr, &m_ShowHierarchy);
		ImGui::MenuItem(WINDOW_INSPECTOR, nullptr, &m_ShowInspector);
		ImGui::MenuItem(WINDOW_SCENE, nullptr, &m_ShowScene);
		ImGui::MenuItem(WINDOW_CONSOLE, nullptr, &m_ShowConsole);
		ImGui::MenuItem(WINDOW_STATS, nullptr, &m_ShowStats);
		ImGui::Separator();
		bool gizmo = Gizmo::IsEnable();
		if (ImGui::MenuItem("Show Gizmo", "F1", &gizmo)) Gizmo::SetEnable(gizmo);
		ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowDemo);
		ImGui::Separator();
		if (ImGui::MenuItem("Reset Layout")) m_ResetLayout = true;
		if (ImGui::MenuItem("Hide Editor", "F2")) m_Enable = false;
		ImGui::EndMenu();
	}

	//------------------------------------------------------------
	// 真ん中に再生ボタン（Unity のツールバー）
	//------------------------------------------------------------
	const float buttonWidth = 56.0f;
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float toolbarWidth = buttonWidth * 3.0f + spacing * 2.0f;
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - toolbarWidth) * 0.5f);

	const bool playing = (m_PlayState == PlayState::Playing);
	const ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_HeaderActive];

	if (playing) ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	if (ImGui::Button("Play", ImVec2(buttonWidth, 0.0f))) m_PlayState = PlayState::Playing;
	if (playing) ImGui::PopStyleColor();

	if (!playing) ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
	if (ImGui::Button("Pause", ImVec2(buttonWidth, 0.0f))) m_PlayState = PlayState::Paused;
	if (!playing) ImGui::PopStyleColor();

	ImGui::BeginDisabled(playing);
	if (ImGui::Button("Step", ImVec2(buttonWidth, 0.0f))) m_StepRequested = true;
	ImGui::EndDisabled();

	// 右端に FPS
	char fps[32];
	snprintf(fps, sizeof(fps), "%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(fps).x - 12.0f);
	ImGui::TextDisabled("%s", fps);

	ImGui::EndMainMenuBar();
}

//==============================================================================
// 各パネルを描く（ゲーム画面をオフスクリーンに描き終わったあと）
//==============================================================================
void Editor::Draw()
{
	Gizmo::DrawSettingsWindow();

	if (!m_Enable)
	{
		DrawGameOnly();
		return;
	}

	if (m_ShowScene) DrawSceneView();
	else
	{
		Gizmo::Draw(nullptr, ImVec2(), ImVec2());	// 表示しなくてもためた線は捨てる
		Input::SetGameViewFocused(false);
	}

	if (m_ShowHierarchy) DrawHierarchy();
	if (m_ShowInspector) DrawInspector();
	if (m_ShowConsole)   DrawConsole();
	if (m_ShowStats)     DrawStats();
	if (m_ShowDemo)      ImGui::ShowDemoWindow(&m_ShowDemo);

	//------------------------------------------------------------
	// 描画中にためておいた Hierarchy の操作を反映
	//------------------------------------------------------------
	for (const PendingParent& p : g_PendingParents)
	{
		GameObject* child = Manager::FindGameObjectByID(p.Child);
		GameObject* parent = Manager::FindGameObjectByID(p.Parent);
		if (child == nullptr) continue;
		child->SetParent(parent);
	}
	g_PendingParents.clear();

	for (unsigned int id : g_PendingDestroy)
	{
		GameObject* object = Manager::FindGameObjectByID(id);
		if (object == nullptr) continue;
		Log("Destroy: %s", object->GetName().c_str());
		object->SetDestroy();
		if (m_SelectedID == id) m_SelectedID = 0;
	}
	g_PendingDestroy.clear();

	if (g_PendingCreate)
	{
		GameObject* object = Manager::AddGameObject<GameObject>();
		GameObject* parent = Manager::FindGameObjectByID(g_PendingCreateParent);
		if (parent) object->SetParent(parent);
		Select(object);
		Log("Create: %s (ID %u)", object->GetName().c_str(), object->GetID());
		g_PendingCreate = false;
	}
}

//==============================================================================
// エディター非表示：ウィンドウ全体にゲーム画面を貼る（黒帯つき）
//==============================================================================
void Editor::DrawGameOnly()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 size = FitGameAspect(viewport->Size);
	ImVec2 pos(viewport->Pos.x + (viewport->Size.x - size.x) * 0.5f,
		viewport->Pos.y + (viewport->Size.y - size.y) * 0.5f);

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	drawList->AddImage(ToTextureID(Renderer::GetSceneTexture()), pos, Add(pos, size));
	Gizmo::Draw(drawList, pos, size);
}

//==============================================================================
// Scene ビュー：オフスクリーンに描いたゲーム画面を表示
//==============================================================================
void Editor::DrawSceneView()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	bool visible = ImGui::Begin(WINDOW_SCENE, &m_ShowScene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleVar();

	if (!visible)
	{
		ImGui::End();
		Gizmo::Draw(nullptr, ImVec2(), ImVec2());
		Input::SetGameViewFocused(false);
		return;
	}

	// アスペクト比を保って真ん中に置く
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImVec2 size = FitGameAspect(avail);
	ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursor.x + (avail.x - size.x) * 0.5f, cursor.y + (avail.y - size.y) * 0.5f));

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::Image(ToTextureID(Renderer::GetSceneTexture()), size);
	const bool hovered = ImGui::IsItemHovered();

	// 再生中は Scene ビュー上のマウスをゲームに渡す
	const bool playing = (m_PlayState == PlayState::Playing);
	Input::SetGameViewFocused(hovered && playing);

	// クリックで選択（一時停止中はそのまま、再生中は Ctrl+クリック）
	ImGuiIO& io = ImGui::GetIO();
	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && (!playing || io.KeyCtrl))
	{
		Select(PickObject(io.MousePos, pos, size));
	}

	// ギズモ・選択表示（はみ出さないように画面の範囲で切る）
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->PushClipRect(pos, Add(pos, size), true);
	Gizmo::Draw(drawList, pos, size);
	DrawSelection(drawList, pos, size);

	// 左上に状態表示
	const char* state = playing ? "Playing" : "Paused";
	ImU32 stateColor = playing ? IM_COL32(120, 220, 120, 255) : IM_COL32(255, 200, 80, 255);
	drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 6.0f), stateColor, state);
	if (!playing)
	{
		drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 22.0f), IM_COL32(220, 220, 220, 200), "Click: select");
		drawList->AddRect(pos, Add(pos, size), IM_COL32(255, 200, 80, 180), 0.0f, 0, 2.0f);
	}
	drawList->PopClipRect();

	ImGui::End();
}

//==============================================================================
// 選択中オブジェクトの軸（X 赤 / Y 緑 / Z 青）と名前
//==============================================================================
void Editor::DrawSelection(ImDrawList* drawList, const ImVec2& pos, const ImVec2& size)
{
	GameObject* selected = GetSelected();
	CAMERA* camera = Manager::GetGameObject<CAMERA>();
	if (selected == nullptr || camera == nullptr || selected == camera) return;

	XMMATRIX viewProj = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	XMMATRIX world = selected->GetWorldMatrix();

	Vector3 origin = selected->GetWorldPosition();
	ImVec2 origin2D;
	if (!WorldToScreen(origin, viewProj, pos, size, &origin2D)) return;

	const ImU32 colors[3] = { IM_COL32(230, 70, 70, 255), IM_COL32(110, 210, 70, 255), IM_COL32(70, 130, 240, 255) };
	const float axisLength = 1.5f;
	for (int i = 0; i < 3; i++)
	{
		// ワールド行列の各行がその軸の向き（スケールが乗っているので正規化）
		XMVECTOR axis = XMVector3Normalize(world.r[i]);
		Vector3 end(origin.x + XMVectorGetX(axis) * axisLength,
			origin.y + XMVectorGetY(axis) * axisLength,
			origin.z + XMVectorGetZ(axis) * axisLength);

		ImVec2 end2D;
		if (WorldToScreen(end, viewProj, pos, size, &end2D))
		{
			drawList->AddLine(origin2D, end2D, colors[i], 2.5f);
		}
	}

	drawList->AddCircleFilled(origin2D, 4.0f, IM_COL32(255, 255, 255, 255));
	drawList->AddText(ImVec2(origin2D.x + 8.0f, origin2D.y - 18.0f), IM_COL32(255, 255, 255, 255), selected->GetName().c_str());
}

//==============================================================================
// クリックした場所にいちばん近いオブジェクトを探す（原点が画面上で近いもの）
//==============================================================================
GameObject* Editor::PickObject(const ImVec2& mouse, const ImVec2& pos, const ImVec2& size)
{
	CAMERA* camera = Manager::GetGameObject<CAMERA>();
	if (camera == nullptr) return nullptr;

	XMMATRIX viewProj = camera->GetViewMatrix() * camera->GetProjectionMatrix();

	const float pickRadius = 30.0f;	// ピクセル
	GameObject* best = nullptr;
	float bestScore = pickRadius * pickRadius;
	float bestDepth = FLT_MAX;

	for (GameObject* object : Manager::GetAllGameObjects())
	{
		if (object == nullptr || object->IsDestroyed() || object == camera) continue;

		ImVec2 screen;
		float depth = 0.0f;
		if (!WorldToScreen(object->GetWorldPosition(), viewProj, pos, size, &screen, &depth)) continue;

		float dx = screen.x - mouse.x;
		float dy = screen.y - mouse.y;
		float dist = dx * dx + dy * dy;
		// 同じくらいの距離なら手前を優先
		if (dist < bestScore || (dist == bestScore && depth < bestDepth))
		{
			best = object;
			bestScore = dist;
			bestDepth = depth;
		}
	}
	return best;
}

//==============================================================================
// Hierarchy
//==============================================================================
void Editor::DrawHierarchy()
{
	if (!ImGui::Begin(WINDOW_HIERARCHY, &m_ShowHierarchy))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("+")) { g_PendingCreate = true; g_PendingCreateParent = 0; }
	if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create Empty");
	ImGui::SameLine();
	g_HierarchyFilter.Draw("##HierarchyFilter", -FLT_MIN);
	ImGui::Separator();

	ImGui::BeginChild("HierarchyTree");

	// Manager のリストは描画のたびにカメラ距離で並び替わるので、表示は ID 順に固定する
	std::vector<GameObject*> objects;
	for (GameObject* object : Manager::GetAllGameObjects())
	{
		if (object == nullptr || object->IsDestroyed()) continue;
		objects.push_back(object);
	}
	std::sort(objects.begin(), objects.end(), [](GameObject* a, GameObject* b) { return a->GetID() < b->GetID(); });

	if (g_HierarchyFilter.IsActive())
	{
		// 検索中は親子を無視して一致したものだけ並べる
		for (GameObject* object : objects)
		{
			if (!g_HierarchyFilter.PassFilter(object->GetName().c_str())) continue;
			ImGui::PushID((int)object->GetID());
			if (ImGui::Selectable(object->GetName().c_str(), object->GetID() == m_SelectedID)) Select(object);
			ImGui::PopID();
		}
	}
	else
	{
		for (GameObject* object : objects)
		{
			if (object->GetParent() == nullptr) DrawHierarchyNode(object);
		}
	}

	// 何もないところ：クリックで選択解除、ドロップで親子解除、右クリックで作成
	ImVec2 rest = ImGui::GetContentRegionAvail();
	if (rest.y < 40.0f) rest.y = 40.0f;
	ImGui::InvisibleButton("##HierarchyEmpty", ImVec2(-FLT_MIN, rest.y));
	if (ImGui::IsItemClicked()) Select(nullptr);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT))
		{
			g_PendingParents.push_back({ *(const unsigned int*)payload->Data, 0 });
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::BeginPopupContextItem("##HierarchyEmptyMenu"))
	{
		if (ImGui::MenuItem("Create Empty")) { g_PendingCreate = true; g_PendingCreateParent = 0; }
		ImGui::EndPopup();
	}

	// Delete キーで削除
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && m_SelectedID != 0 && ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		g_PendingDestroy.push_back(m_SelectedID);
	}

	ImGui::EndChild();
	ImGui::End();
}

void Editor::DrawHierarchyNode(GameObject* object)
{
	const std::vector<GameObject*>& children = object->GetChildren();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (children.empty()) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (object->GetID() == m_SelectedID) flags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx((void*)(intptr_t)object->GetID(), flags, "%s", object->GetName().c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) Select(object);

	// 右クリックメニュー
	if (ImGui::BeginPopupContextItem())
	{
		Select(object);
		if (ImGui::MenuItem("Create Empty Child")) { g_PendingCreate = true; g_PendingCreateParent = object->GetID(); }
		if (ImGui::MenuItem("Unparent", nullptr, false, object->GetParent() != nullptr)) g_PendingParents.push_back({ object->GetID(), 0 });
		ImGui::Separator();
		if (ImGui::MenuItem("Delete", "Del")) g_PendingDestroy.push_back(object->GetID());
		ImGui::EndPopup();
	}

	// ドラッグ＆ドロップで親子付け（Unity と同じ）
	if (ImGui::BeginDragDropSource())
	{
		unsigned int id = object->GetID();
		ImGui::SetDragDropPayload(PAYLOAD_GAMEOBJECT, &id, sizeof(id));
		ImGui::Text("%s", object->GetName().c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT))
		{
			unsigned int childID = *(const unsigned int*)payload->Data;
			if (childID != object->GetID()) g_PendingParents.push_back({ childID, object->GetID() });	// 循環は SetParent 側で弾かれる
		}
		ImGui::EndDragDropTarget();
	}

	if (open && !children.empty())
	{
		for (GameObject* child : children)
		{
			if (child && !child->IsDestroyed()) DrawHierarchyNode(child);
		}
		ImGui::TreePop();
	}
}

//==============================================================================
// Inspector
//==============================================================================
void Editor::DrawInspector()
{
	if (!ImGui::Begin(WINDOW_INSPECTOR, &m_ShowInspector))
	{
		ImGui::End();
		return;
	}

	GameObject* object = GetSelected();
	if (object == nullptr)
	{
		ImGui::TextDisabled("Select an object in Hierarchy or Scene.");
		ImGui::End();
		return;
	}

	ImGui::PushID((int)object->GetID());

	// 名前
	char name[128];
	strncpy(name, object->GetName().c_str(), sizeof(name) - 1);
	name[sizeof(name) - 1] = '\0';
	ImGui::SetNextItemWidth(-FLT_MIN);
	if (ImGui::InputText("##Name", name, sizeof(name))) object->SetName(name);

	ImGui::TextDisabled("%s  |  ID %u  |  Layer %d", ClassName(object, typeid(*object)).c_str(), object->GetID(), object->GetLayer());
	if (GameObject* parent = object->GetParent())
	{
		ImGui::TextDisabled("Parent: %s", parent->GetName().c_str());
	}
	ImGui::Separator();

	// Transform（回転は内部がラジアンなので度で見せる）
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 position = object->GetPosition();
		if (ImGui::DragFloat3("Position", &position.x, 0.05f)) object->SetPosition(position);

		Vector3 rotation = object->GetRotation();
		float degrees[3] = { XMConvertToDegrees(rotation.x), XMConvertToDegrees(rotation.y), XMConvertToDegrees(rotation.z) };
		if (ImGui::DragFloat3("Rotation", degrees, 0.5f))
		{
			object->SetRotation(Vector3(XMConvertToRadians(degrees[0]), XMConvertToRadians(degrees[1]), XMConvertToRadians(degrees[2])));
		}

		Vector3 scale = object->GetScale();
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f)) object->SetScale(scale);

		if (ImGui::Button("Reset"))
		{
			object->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
			object->SetRotation(Vector3(0.0f, 0.0f, 0.0f));
			object->SetScale(Vector3(1.0f, 1.0f, 1.0f));
		}
		if (object->GetParent())
		{
			Vector3 world = object->GetWorldPosition();
			ImGui::SameLine();
			ImGui::TextDisabled("World (%.2f, %.2f, %.2f)", world.x, world.y, world.z);
		}
	}

	// コンポーネント：各コンポーネントの OnInspectorGUI をそのまま呼ぶ
	for (Component* component : object->GetComponents())
	{
		if (component == nullptr) continue;
		ImGui::PushID(component);
		std::string label = ClassName(component, typeid(*component));
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Indent(4.0f);
			component->OnInspectorGUI();
			ImGui::Unindent(4.0f);
		}
		ImGui::PopID();
	}

	if (object->GetComponents().empty())
	{
		ImGui::Spacing();
		ImGui::TextDisabled("No components.");
	}

	ImGui::PopID();
	ImGui::End();
}

//==============================================================================
// Console
//==============================================================================
void Editor::DrawConsole()
{
	if (!ImGui::Begin(WINDOW_CONSOLE, &m_ShowConsole))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Clear")) m_Logs.clear();
	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
	ImGui::SameLine();
	g_ConsoleFilter.Draw("Filter", 180.0f);
	ImGui::Separator();

	ImGui::BeginChild("ConsoleLog", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
	for (const LogEntry& entry : m_Logs)
	{
		if (!g_ConsoleFilter.PassFilter(entry.Text.c_str())) continue;

		ImVec4 color = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
		if (entry.Level == LogLevel::Warning) color = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
		if (entry.Level == LogLevel::Error)   color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
		ImGui::TextColored(color, "%s", entry.Text.c_str());
	}
	if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();

	ImGui::End();
}

//==============================================================================
// Stats
//==============================================================================
void Editor::DrawStats()
{
	if (!ImGui::Begin(WINDOW_STATS, &m_ShowStats))
	{
		ImGui::End();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("FPS        : %.1f (%.2f ms)", io.Framerate, 1000.0f / io.Framerate);
	ImGui::Text("Objects    : %d", (int)Manager::GetAllGameObjects().size());
	ImGui::Text("Game view  : %d x %d", SCREEN_WIDTH, SCREEN_HEIGHT);
	ImGui::Text("Window     : %u x %u", Renderer::GetBackBufferWidth(), Renderer::GetBackBufferHeight());
	ImGui::Text("State      : %s", m_PlayState == PlayState::Playing ? "Playing" : "Paused");

	ImGui::End();
}

//==============================================================================
// 再生制御・選択
//==============================================================================
bool Editor::ShouldUpdateGame()
{
	if (!m_Enable) return true;	// エディターを閉じているときは常に動かす
	if (m_PlayState == PlayState::Playing) return true;
	if (m_StepRequested)
	{
		m_StepRequested = false;
		return true;
	}
	return false;
}

GameObject* Editor::GetSelected()
{
	return Manager::FindGameObjectByID(m_SelectedID);
}

void Editor::Select(GameObject* object)
{
	m_SelectedID = object ? object->GetID() : 0;
}

//==============================================================================
// ログ
//==============================================================================
void Editor::AddLog(LogLevel level, const char* format, va_list args)
{
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), format, args);

	const char* prefix = "";
	if (level == LogLevel::Warning) prefix = "[Warning] ";
	if (level == LogLevel::Error)   prefix = "[Error] ";

	std::string text = std::string(prefix) + buffer;
	OutputDebugStringA((text + "\n").c_str());

	m_Logs.push_back({ level, text });
	if (m_Logs.size() > LOG_MAX) m_Logs.erase(m_Logs.begin(), m_Logs.begin() + (m_Logs.size() - LOG_MAX));
}

void Editor::Log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	AddLog(LogLevel::Info, format, args);
	va_end(args);
}

void Editor::LogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	AddLog(LogLevel::Warning, format, args);
	va_end(args);
}

void Editor::LogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	AddLog(LogLevel::Error, format, args);
	va_end(args);
}
