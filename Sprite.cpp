#include "Sprite.h"
#include <DirectXMath.h>
#include "BufferResource.h"
#include "ImGuiManager.h"

#include"externals/imgui/imgui.h"

#include "TextureManager.h"

using namespace Microsoft::WRL;
using namespace DirectX;


void Sprite::Initialize(SpriteCommon* common, std::wstring textureFilePath)
{
	common_ = common;

	dxCommon_ = common_->GetDirectXCommon();

	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexFilePath(textureFilePath);


	//���_ ��� 
	CreateVertex();
	//�C���f�b�N�X���쐬
	CreateIndex();
	//�F
	CreateMaterial();
	//�s��
	CreateWVP();

	//�摜�̃T�C�Y�𐮗�����
	AdjustTextureSize();

}

void Sprite::Update()
{
	transform.tlanslate = { position.x,position.y ,0};
	transform.rotate = { 0,0,rotation };
	materialData->color = color_;
	transform.scale = { size.x,size.y,1.0f};

	//�A���J�[�|�C���g�X�V
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	//�t���b�v
	if (isFlipX == true)
	{
		//���E���]
		left = -left;
		right = -right;
	}
	if (isFlipY == true)
	{
		//�㉺���]
		top = -top;
		bottom = -bottom;
	}


	//���_���
	vertexDate[0].position = { left, bottom, 0.0f, 1.0f };
	vertexDate[1].position = { left, top, 0.0f, 1.0f };
	vertexDate[2].position = { right, bottom, 0.0f, 1.0f };
	vertexDate[3].position = { right, top, 0.0f, 1.0f };

	const DirectX::TexMetadata& metaData = TextureManager::GetInstance()->GetMetaData(textureIndex_);
	float tex_left = textureLeftTop.x / metaData.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metaData.width;
	float tex_top = textureLeftTop.y / metaData.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metaData.height;


	//UV
	vertexDate[0].texcoord = { tex_left,tex_bottom };
	vertexDate[1].texcoord = { tex_left,tex_top };
	vertexDate[2].texcoord = { tex_right,tex_bottom };
	vertexDate[3].texcoord = { tex_right,tex_top };



	ImGui::Begin("texture");

	ImGui::DragFloat3("pos", &transform.tlanslate.x, 0.01f);

	ImGui::DragFloat3("UV-Pos", &uvTransform.tlanslate.x, 0.01f,-10.f,10.f);
	ImGui::SliderAngle("UV-Rot", &uvTransform.rotate.z);
	ImGui::DragFloat3("UV-Scale", &uvTransform.scale.x, 0.01f, -10.f, 10.f);

	ImGui::Checkbox("FlipX", &isFlipX);
	ImGui::Checkbox("FlipY", &isFlipY);

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

	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(0,WinApp::window_width,WinApp::window_height,0,0.1f,100.f);

	/*XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.f),
		(float)WinApp::window_width / (float)WinApp::window_height,
		0.1f,
		100.f

	);*/
	//WVP
	XMMATRIX worldViewProjectionMatrix = worldMatrix * (view * proj);
	//�s��̑��
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
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_)); 
	
	//���_���̂ݕ`��
	//dxCommon_->GetCommandList()->DrawInstanced(6, 1,0, 0);
	//�C���f�b�N�X��񂪂���ꍇ�̕`��
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}


void Sprite::SetTexture(std::wstring textureFilePath)
{
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexFilePath(textureFilePath);
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
	
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexDate));

	//����	
	vertexDate[0].position = { 0.0f, 1.0f, 0.0f, 1.0f };
	vertexDate[0].texcoord = { 0.0f,1.0f };
	//��
	vertexDate[1].position = { 0.f, 0.0f, 0.0f, 1.0f };
	vertexDate[1].texcoord = { 0.0f,0.0f };
	//�E��
	vertexDate[2].position = { 1.0f, 1.0f, 0.0f, 1.0f };
	vertexDate[2].texcoord = { 1.0f,1.0f };

	//��
	vertexDate[3].position = { 1.0f, 0.0f, 0.0f, 1.0f };
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

void Sprite::AdjustTextureSize()
{
	const DirectX::TexMetadata& metaData = TextureManager::GetInstance()->GetMetaData(textureIndex_);

	textureSize.x = static_cast<float>(metaData.width);
	textureSize.y = static_cast<float>(metaData.height);

	size = textureSize;
}

