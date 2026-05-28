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