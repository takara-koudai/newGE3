#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

#include <DirectXMath.h>


#include "SpriteCommon.h"


class Sprite
{
private:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	struct Transform {
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 rotate;
		DirectX::XMFLOAT3 tlanslate;
	};

	struct VertexData {
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texcoord;
		
	};
	struct MaterialDate {
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX uvTransform;
		
	};

public:

	void Initialize(DirectXCommon* dxCommon, SpriteCommon* common);

	void Update();

	void Draw();


	//Getter/Setter
	DirectX::XMFLOAT2 GetPosition() { return position; }
	float GetRotation() { return rotation; }
	DirectX::XMFLOAT4 GetColor() { return color_; }
	DirectX::XMFLOAT2 GetSize() { return size; }

	void SetPosition(DirectX::XMFLOAT2 pos) { position = pos; }
	void SetRotation(float rot) { rotation = rot; }
	void SetColor(DirectX::XMFLOAT4 color) { color_ = color; }
	void SetSize(DirectX::XMFLOAT2 size) { this->size = size; }

private:

	//頂点情報作成
	void CreateVertex();
	//インデックス情報作成
	void CreateIndex();

	//マテリアル情報作成
	void CreateMaterial();
	 //行列情報作成
	void CreateWVP();

private:

	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;
	 //頂点情報
	ComPtr<ID3D12Resource> vertexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexDate = nullptr;

	//インデックス
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};


	//マテリアル情報
	ComPtr<ID3D12Resource> materialResource;
	MaterialDate* materialData = nullptr;

	//行列情報
	ComPtr<ID3D12Resource> wvpResource;
	DirectX::XMMATRIX* wvpData = nullptr;

	//メンバ変数
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	 //パラメータ
	DirectX::XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//左からscale rotate translate
	Transform uvTransform = { {1,1,1},{0,0,0},{0,0,0} };

	//左からscale rotate translate
	Transform transform = { {1,1,1}, {0,0,0}, {0,0,0} };
	DirectX::XMFLOAT2 position = { 0,0 };
	float rotation = 0;

	DirectX::XMFLOAT2 size = { 1,1 };

	Transform cameraTransform = { {1,1,1}, {0,0,0}, {0,0,-5} };


};

