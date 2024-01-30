#pragma once

#include <Windows.h>
#include <wrl.h>
#include <string>
#include <dxcapi.h>
#include <d3dx12.h>

#include "DirectXCommon.h"

#include <DirectXTex.h>

class SpriteCommon
{
private:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;


public:

	void Initialize(DirectXCommon* dxCommon);

	//Getter
	ID3D12RootSignature* GetRootSignature() { return rootSignature.Get(); }
	ID3D12PipelineState* GetPipelineState() { return pipelineState.Get(); }

	//画像読み込み
	DirectX::ScratchImage Loadtexture(const std::wstring& filePath);

	// 読み込んだ画像をGPU(シェーダーに送る)
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage);


private:
	static IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

private:
	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;



};

