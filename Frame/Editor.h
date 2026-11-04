#pragma once
#include <string>
#include <vector>
#include <cstdarg>
#include "main.h"

class GameObject;

//==============================================================================
// Unity / UE 風のエディター画面（Dear ImGui の docking 版で作っている）
//
//  ┌ メニューバー（File / Scene / Window ・ Play / Pause / Step）─────┐
//  │ Hierarchy │            Scene（ゲーム画面）          │ Inspector │
//  │           │                                         ├───────────┤
//  │           ├─────────────────────────────────────────┤ Gizmo 等  │
//  │           │ Console / Stats                         │           │
//  └───────────┴─────────────────────────────────────────┴───────────┘
//
// 使い方（Manager から呼ぶ）
//   Init()       : ImGui のコンテキストを作った直後
//   BeginFrame() : ImGui::NewFrame と Input::Update の直後（DockSpace は他のウィンドウより先に出す必要がある）
//   Draw()       : ゲーム画面をオフスクリーンに描き終わったあと、ImGui::Render の前
//
// F2 でエディター表示 ⇔ ゲーム画面だけの表示を切り替える（Debug ビルドのみ）
//==============================================================================
class Editor
{
public:
	enum class PlayState
	{
		Playing,	// ゲームを動かす
		Paused,		// 止める（Step で 1 フレームだけ進める）
	};

	enum class LogLevel
	{
		Info,
		Warning,
		Error,
	};

private:
	struct LogEntry
	{
		LogLevel    Level;
		std::string Text;
	};

	static bool      m_Enable;			// エディターを表示しているか
	static PlayState m_PlayState;
	static bool      m_StepRequested;	// 一時停止中に 1 フレームだけ進める
	static bool      m_ResetLayout;		// 次のフレームでレイアウトを初期状態に戻す

	static unsigned int m_SelectedID;	// 選択中の GameObject（ポインタだと削除されたとき危ないので ID で持つ）

	// ウィンドウの表示フラグ
	static bool m_ShowHierarchy;
	static bool m_ShowInspector;
	static bool m_ShowScene;
	static bool m_ShowConsole;
	static bool m_ShowStats;
	static bool m_ShowDemo;

	// Console
	static std::vector<LogEntry> m_Logs;
	static bool m_AutoScroll;

	// 各パネル
	static void DrawMainMenuBar();
	static void DrawHierarchy();
	static void DrawHierarchyNode(GameObject* object);
	static void DrawInspector();
	static void DrawSceneView();
	static void DrawConsole();
	static void DrawStats();
	static void DrawGameOnly();		// エディター非表示のときはウィンドウ全体にゲーム画面を貼る

	static void BuildDefaultLayout(unsigned int dockspaceID);
	static void ApplyStyle();

	// Scene ビュー上の補助
	static void DrawSelection(ImDrawList* drawList, const ImVec2& pos, const ImVec2& size);
	static GameObject* PickObject(const ImVec2& mouse, const ImVec2& pos, const ImVec2& size);

	static void AddLog(LogLevel level, const char* format, va_list args);

public:
	static void Init();
	static void Uninit();
	static void BeginFrame();
	static void Draw();

	static bool IsEnable() { return m_Enable; }
	static void SetEnable(bool enable) { m_Enable = enable; }

	// このフレームにゲーム（シーン・オブジェクト）を更新してよいか
	// Step 要求はここで消費するので 1 フレームに 1 回だけ呼ぶこと
	static bool ShouldUpdateGame();
	static PlayState GetPlayState() { return m_PlayState; }
	static void SetPlayState(PlayState state) { m_PlayState = state; }

	static GameObject* GetSelected();
	static void Select(GameObject* object);

	// Console に出す（printf と同じ書き方）。Visual Studio の出力ウィンドウにも出る
	static void Log(const char* format, ...);
	static void LogWarning(const char* format, ...);
	static void LogError(const char* format, ...);
};
