#pragma once
#include <d3d12.h>
#include <cassert>

#include <DirectXTex.h>

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInByte) {

	ID3D12Resource* resource = nullptr;


	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInByte;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	HRESULT result = device->
		CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(result));

	return	resource;
}

	 //�e�N�X�`�����\�[�X���쐬����֐�
ID3D12Resource* CreateTexureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//metadata�����Resource�̐ݒ�
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Texture�̕�
	resourceDesc.Height = UINT(metadata.height);//Texture�̍���
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmap�̐�
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//���s���@or�@�z��Texture�̔z��
	resourceDesc.Format = metadata.format;//Texture��Format
	resourceDesc.SampleDesc.Count = 1;//�T���v�����O�J�E���g�B1�Œ�B
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Texture�̎������B���i�g���Ă���̂�2����

	//���p����Heap�̐ݒ�B
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//�ׂ����ݒ���s��
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBack�|���V�[��CPU�A�N�Z�X�\
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//�v���Z�b�T�̋߂��ɐݒu
	
	//Resource�̐���
	ID3D12Resource* resource = nullptr;
	HRESULT result = device->CreateCommittedResource(
		&heapProperties,//Heap�̐ݒ�
		D3D12_HEAP_FLAG_NONE,//Heap�̓���Ȑݒ�B���ɂȂ��B
		&resourceDesc,//Resource�̐ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,//�����ResourceState�BTexture�͊�{�ǂނ���
		nullptr,//Clear�œK�l�B�g��Ȃ���nullptr
		IID_PPV_ARGS(&resource));//�쐬����Resource�|�C���^�ւ̃|�C���^
	assert(SUCCEEDED(result));

	return resource;
}
