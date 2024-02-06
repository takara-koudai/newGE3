#include "TextureManager.h"
#include "BufferResource.h"

TextureManager* TextureManager::instance = nullptr;
uint32_t TextureManager::kSRVIndexTop = 1;


TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new TextureManager();
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::Loadtexture(const std::wstring& filePath)
{
	//読み込み済みか確認する処理
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end())
	{
		return;
	}

	//一定の数を越えないように
	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);


	//テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	HRESULT result = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

	assert(SUCCEEDED(result));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB, 0, mipImages);

	assert(SUCCEEDED(result));

	//画像データの追加
	textureDatas.resize(textureDatas.size() + 1);
	//追加した画像データの編集
	TextureData& data = textureDatas.back();

	data.filePath = filePath;
	data.metaData = mipImages.GetMetadata();
	data.resource = CreateTexureResource(dxCommon_->GetDevice(), data.metaData);
	UploadTextureData(data.resource.Get(), mipImages);

	//画像が保存されてるメモリ
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	handleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;
	handleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	data.srvHandleCPU = handleCPU;
	data.srvHandleGPU = handleGPU;


	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = data.metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(data.metaData.mipLevels);

	////SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(data.resource.Get(), &srvDesc, data.srvHandleCPU);


	//return image;
}

uint32_t TextureManager::GetTextureIndexFilePath(const std::wstring& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end())
	{
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	//無かったら停止する
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{
	//メモリの範囲外
	assert(textureIndex < DirectXCommon::kMaxSRVCount);

	//要素番号のTextureDataを受け取る
	TextureData& data = textureDatas[textureIndex];

	return data.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{
	//メモリの範囲外
	assert(textureIndex < DirectXCommon::kMaxSRVCount);

	//要素番号のTextureDataを受け取る
	TextureData& data = textureDatas[textureIndex];

	return data.metaData;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
	//全MipMap
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {

		const DirectX::Image* img = mipImage.GetImage(mipLevel, 0, 0);

		//転送(シェーダー)テクスチャの全情報を送る
		HRESULT result = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)

		);
		assert(SUCCEEDED(result));
	}
}
