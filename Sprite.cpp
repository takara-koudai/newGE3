#include "Sprite.h"
#include <DirectXMath.h>
#include "BufferResource.h"
#include "ImGuiManager.h"

#include"externals/imgui/imgui.h"


using namespace Microsoft::WRL;
using namespace DirectX;


void Sprite::Initialize(DirectXCommon* dxCommon, SpriteCommon* common)
{
	dxCommon_ = dxCommon;

	common_ = common;

	//�摜�̓ǂݎ��
	DirectX::ScratchImage mipImages = common->Loadtexture(L"Resources/mario.jpg");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTexureResource(dxCommon_->GetDevice(),metaData);
	common_->UploadTextureData(textureResource, mipImages);
	
	

	//metaData�����SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//�擪��ImGui���g���Ă���̂ł��̎����g��
	textureSrvHandleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	////SRV�̐���
	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);


	//���_ ��� 
	CreateVertex();
	//�C���f�b�N�X���쐬
	CreateIndex();
	//�F
	CreateMaterial();
	//�s��
	CreateWVP();
}

void Sprite::Update()
{
	transform.tlanslate = position;

	ImGui::Begin("texture");

	ImGui::DragFloat3("pos", &transform.tlanslate.x, 0.01f);

	ImGui::DragFloat3("UV-Pos", &uvTransform.tlanslate.x, 0.01f,-10.f,10.f);
	ImGui::SliderAngle("UV-Rot", &uvTransform.rotate.z);
	ImGui::DragFloat3("UV-Scale", &uvTransform.scale.x, 0.01f, -10.f, 10.f);

	ImGui::End();


}

void Sprite::Draw()
{
	//Y�����S�ɉ�]
	//transform.rotate.y += 0.06f;

	//���[���h
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&transform. scale));
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.rotate));
	XMMATRIX translateMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&transform.tlanslate));

	//��]�s��ƃX�P�[���s��̊|���Z
	XMMATRIX rotateAndScaleMatrix = XMMatrixMultiply(rotateMatrix, scaleMatrix);
	//�ŏI�I�ȍs��ϊ�
	XMMATRIX worldMatrix = XMMatrixMultiply(rotateAndScaleMatrix, translateMatrix);

	//�J����
	XMMATRIX cameraScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&cameraTransform.scale));
	XMMATRIX cameraRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&cameraTransform.rotate));
	XMMATRIX cameraTranslateMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&cameraTransform.tlanslate));

	//��]�s��ƃX�P�[���s��̊|���Z
	XMMATRIX cameraRotateAndScaleMatrix = XMMatrixMultiply(cameraRotateMatrix, cameraScaleMatrix);
	//�ŏI�I�ȍs��ϊ�
	XMMATRIX cameraMatrix = XMMatrixMultiply(cameraRotateAndScaleMatrix, cameraTranslateMatrix);

	//View
	XMMATRIX view = XMMatrixInverse(nullptr, cameraMatrix);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.f),
		(float)WinApp::window_width / (float)WinApp::window_height,
		0.1f,
		100.f

	);
	//WVP
	XMMATRIX worldViewProjectionMatrix = worldMatrix * (view * proj);


	//
	*wvpData = worldViewProjectionMatrix;


	//UV���[���h
	XMMATRIX uvScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&uvTransform.scale));
	XMMATRIX uvRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&uvTransform.rotate));
	XMMATRIX uvTranslateMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&uvTransform.tlanslate));

	//��]�s��ƃX�P�[���s��̊|���Z
	XMMATRIX uvRotateAndScaleMatrix = XMMatrixMultiply(uvRotateMatrix, uvScaleMatrix);
	//�ŏI�I�ȍs��ϊ�
	XMMATRIX uvWorldMatrix = XMMatrixMultiply(uvRotateAndScaleMatrix, uvTranslateMatrix);
	materialData->uvTransform = uvWorldMatrix;


	dxCommon_->GetCommandList()->SetGraphicsRootSignature(common_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(common_->GetPipelineState());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_���
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	//�C���f�b�N�X
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//�F���
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//�s��
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//�摜
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU); 
	
	//���_���̂ݕ`��
	//dxCommon_->GetCommandList()->DrawInstanced(6, 1,0, 0);
	//�C���f�b�N�X��񂪂���ꍇ�̕`��
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::CreateVertex()
{
	vertexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4);

	//���_�o�b�t�@�r���[���쐬����

	//���\�[�X�̐擪�̃A�h���X����g��
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//�g�p���郊�\�[�X�̃T�C�Y�͒��_3���̃T�C�Y
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//1���_������̃T�C�Y
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//���_ ���
	VertexData* vertexDate = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));

	//����	
	vertexDate[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexDate[0].texcoord = { 0.0f,1.0f };
	//��
	vertexDate[1].position = { -0.5f, +0.5f, 0.0f, 1.0f };
	vertexDate[1].texcoord = { 0.0f,0.0f };
	//�E��
	vertexDate[2].position = { +0.5f, -0.5f, 0.0f, 1.0f };
	vertexDate[2].texcoord = { 1.0f,1.0f };

	//��
	vertexDate[3].position = { +0.5f, +0.5f, 0.0f, 1.0f };
	vertexDate[3].texcoord = { 1.0f, 0.0f };
	
}

void Sprite::CreateIndex()
{

	indexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//�C���f�b�N�X���\�[�X�Ƀf�[�^����������
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

}

void Sprite::CreateMaterial()
{
	 materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(MaterialDate));

	

	 materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	
	 materialData->color = color_;
	 materialData->uvTransform = XMMatrixIdentity();

}

void Sprite::CreateWVP()
{
	wvpResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(XMMATRIX));

	

	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	*wvpData = XMMatrixIdentity();

}

