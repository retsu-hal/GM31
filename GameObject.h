#pragma once

#include "main.h"
#include "renderer.h"
#include "Vector3.h"
#include "Component.h"

class GameObject
{
protected://外部からアクセスできないが、継承したクラスからアクセスできる
	Vector3 m_Position{ 0.0f, 0.0f, 0.0f };
	Vector3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	Vector3 m_Scale{ 1.0f, 1.0f, 1.0f };

	ID3D11Buffer* m_vertexBuffer;					// 頂点バッファ
	ID3D11InputLayout* m_VertexLayout;		// 頂点レイアウト
	ID3D11VertexShader* m_VertexShader;	// 頂点シェーダー
	ID3D11PixelShader* m_PixelShader;			// ピクセルシェーダー	
	ID3D11Buffer* m_indexBuffer;					// インデックスバッファ
	ID3D11ShaderResourceView* m_Texture;	// テクスチャ

	std::list<Component*> m_Components;

public:
	void SetPosition(const Vector3& position) { m_Position = position; }
	Vector3 GetPosition() const { return m_Position; }
	void SetRotation(const Vector3& rotation) { m_Rotation = rotation; }
	Vector3 GetRotation() const { return m_Rotation; }

	//ImGui関連
	virtual const char* GetName() const { return "GameObject"; }
	virtual void DrawImGui()
	{
		float w = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;

		ImGui::PushItemWidth(w);
		ImGui::SliderFloat("##PositionX", &m_Position.x, -50.0f, 50.0f, "X %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##PositionY", &m_Position.y, -50.0f, 50.0f, "Y %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##PositionZ", &m_Position.z, -50.0f, 50.0f, "Z %.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Position");

		ImGui::PushItemWidth(w);
		ImGui::SliderFloat("##RotationX", &m_Rotation.x, -50.0f, 50.0f, "X %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##RotationY", &m_Rotation.y, -50.0f, 50.0f, "Y %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##RotationZ", &m_Rotation.z, -50.0f, 50.0f, "Z %.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Rotation");

		ImGui::PushItemWidth(w);
		ImGui::SliderFloat("##ScaleX", &m_Scale.x, -50.0f, 50.0f, "X %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##ScaleY", &m_Scale.y, -50.0f, 50.0f, "Y %.1f"); ImGui::SameLine();
		ImGui::SliderFloat("##ScaleZ", &m_Scale.z, -50.0f, 50.0f, "Z %.1f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Scale");
	};

	virtual void Init() {};
	virtual void Uninit() 
	{
		for (Component* component : m_Components)
		{
			if (component != nullptr)
			{
				component->Uninit();
				delete component;
			}
		}
	};

	virtual void Update() 
	{
		for (Component* component : m_Components)
		{
			if (component != nullptr)
			{
				component->Update();
			}
		}
	};

	virtual void Draw()
	{
		for (Component* component : m_Components)
		{
			if (component != nullptr)
			{
				component->Draw();
			}
		}
	};

	template<typename T>
    T* AddComponent(GameObject* gameObject)
	{
		T* component = new T(gameObject);
		component->Init();
		m_Components.push_back(component);	
		return component;
	}

	virtual Vector3 GetFront() 
	{
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 Forward;
		XMStoreFloat3((XMFLOAT3*)&Forward, RotMatrix.r[2]);
		return Forward;

	}

	virtual Vector3 GetRight() 
	{
		XMMATRIX RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);

		Vector3 Forward;
		XMStoreFloat3((XMFLOAT3*)&Forward, RotMatrix.r[0]);
		return Forward;

	}
};