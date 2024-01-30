
#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInput�̃o�[�W�����w��
#include <dinput.h>
#include "WinApp.h"
#include <vector>
#include<chrono>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;



class DirectXCommon
{
private:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//������
	void Initialize(WinApp* winApp);
	//�`��O����
	void PreDraw();
	//�`��㏈��
	void PostDraw();

	//Getter
	ID3D12Device*  GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList()	 const { return commandList.Get(); }

	//�X���b�v�`�F�C��
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }

	//RTV�f�B�X�N
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }

	//SRV�f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }



private:
	//�f�o�C�X
	void DeviceInitialize();
	//�R�}���h
	void CommandInitialize();
	//�X���b�v�`�F�[��
	void SwapChainInitialize();
	//�����_�^�[�Q�b�g
	void RenderTargetInitialize();
	//�[�x�o�b�t�@
	void DepthBufferTargetInitialize();
	//�t�F���X
	void FenceTargetInitialize();

	//	 �f�B�X�N���v�^�q�[�v�쐬
	ID3D12DescriptorHeap* CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescripots,bool shaderVisible);





	void InitializeFixFPS();
	void UpdateFixFPS();
	//�L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point reference_;

private:
	//WindowsAPI
	WinApp* winApp = nullptr;

	

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory;

	
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;

	ComPtr<IDXGISwapChain4> swapChain;

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};

	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	
	D3D12_RESOURCE_BARRIER barrierDesc{};
	ComPtr<ID3D12Resource> depthBuff;

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//�f�B�X�N���v�^�q�[�v
	//RTV(�Q�[���̉�ʂ�ۑ�)
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

	//SRV(�摜�Ȃǂ�ۑ����Ă�������)
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;



};