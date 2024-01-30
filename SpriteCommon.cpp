#include "SpriteCommon.h"
#include <cassert>

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	HRESULT result{};
	dxCommon_ = dxCommon;

	//DXC初期化
	ComPtr<IDxcUtils>dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	ComPtr<IDxcIncludeHandler>	includeHandler ;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(result));

	
	result = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(result));

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptorRootSignature{};
	descriptorRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//画像用ディスクリプタ
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	//RootParamter
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	 //色
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	//行列
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	//画像
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数


	descriptorRootSignature.pParameters = rootParameters;
	descriptorRootSignature .NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixeelShaderで使う

	descriptorRootSignature.pStaticSamplers = staticSamplers;
	descriptorRootSignature.NumStaticSamplers = _countof(staticSamplers);

								    
	//シリアライズとしてバイナリにする
	ComPtr<ID3D10Blob> signatureBlob;
	ComPtr<ID3D10Blob> errorBlob;
	result = D3D12SerializeRootSignature(&descriptorRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		assert(false);
	}
	  //バイナリを基に作成
	
	  result = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	  assert(SUCCEEDED(result));

	  //InputLayout
	  D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	  inputElementDescs[0].SemanticName = "POSITION";
	  inputElementDescs[0].SemanticIndex = 0;
	  inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	  inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	 
	  inputElementDescs[1].SemanticName = "TEXCOORD";
	  inputElementDescs[1].SemanticIndex = 0;
	  inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	  inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	  D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};

	  inputLayoutDesc.pInputElementDescs = inputElementDescs;
	  inputLayoutDesc.NumElements = _countof(inputElementDescs);

	  //BlendStateの設定
	  D3D12_BLEND_DESC blendDesc{};
	  //すべての色素を書き込む
	  blendDesc.RenderTarget[0].RenderTargetWriteMask =
		  D3D12_COLOR_WRITE_ENABLE_ALL;

	  //RasiterzerStateの設定
	  D3D12_RASTERIZER_DESC rasterizerDesc{};
	  //裏面(時計回り)を表示しない
	  rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	  //三角形の中を塗りつぶす
	  rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	  //Shader
	  ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"Resources/shaders/Sprite.VS.hlsl",
		  L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	  assert(vertexShaderBlob != nullptr);

	  ComPtr <IDxcBlob> pixelShaderBlob = CompileShader(L"Resources/shaders/Sprite.PS.hlsl",
		  L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	  assert(pixelShaderBlob != nullptr);

	  //PSO
	  D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	  graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	  graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	  graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	  vertexShaderBlob->GetBufferSize() };
	  graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	  pixelShaderBlob->GetBufferSize() };
	  graphicsPipelineStateDesc.BlendState = blendDesc;
	  graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	  //書き込むRTVの情報
	  graphicsPipelineStateDesc.NumRenderTargets = 1;
	  graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	  //利用するトポロジ(形状)のタイプ
	  graphicsPipelineStateDesc.PrimitiveTopologyType =
		  D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	  //どのように画面に色
	  graphicsPipelineStateDesc.SampleDesc.Count = 1;
	  graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	 

	  //実際に生成
	  
	  result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		  IID_PPV_ARGS(&pipelineState));

	  assert(SUCCEEDED(result));




}

DirectX::ScratchImage SpriteCommon::Loadtexture(const std::wstring& filePath)
{
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


	return image;
}

void SpriteCommon::UploadTextureData(ID3D12Resource* texture, 
	const DirectX::ScratchImage& mipImage)
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

IDxcBlob* SpriteCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	HRESULT result{};

	//hlsl
	IDxcBlobEncoding* shaderSource = nullptr;
	 result = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	assert(SUCCEEDED(result));

	DxcBuffer shaderSourceBuffer = {};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	//2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)

	);
	assert(SUCCEEDED(result));

	//3.警告・エラーがでていないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		
		//警告・エラーダメゼッタイ
		assert(false);
	}

	//4.Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* ShaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//もう使わないリソースを解散
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return ShaderBlob;

	
}
