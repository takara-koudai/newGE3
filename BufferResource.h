#pragma once
#include <d3d12.h>
#include <cassert>

#include <DirectXTex.h>

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInByte);

	 //テクスチャリソースを作成する関数
ID3D12Resource* CreateTexureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
