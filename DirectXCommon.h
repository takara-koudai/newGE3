
#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
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
	//初期化
	void Initialize(WinApp* winApp);
	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();

	//Getter
	ID3D12Device*  GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList()	 const { return commandList.Get(); }

	//スワップチェイン
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }

	//RTVディスク
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }

	//SRVディスクリプタヒープ
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }



private:
	//デバイス
	void DeviceInitialize();
	//コマンド
	void CommandInitialize();
	//スワップチェーン
	void SwapChainInitialize();
	//レンダターゲット
	void RenderTargetInitialize();
	//深度バッファ
	void DepthBufferTargetInitialize();
	//フェンス
	void FenceTargetInitialize();

	//	 ディスクリプタヒープ作成
	ID3D12DescriptorHeap* CreateDescripterHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescripots,bool shaderVisible);





	void InitializeFixFPS();
	void UpdateFixFPS();
	//記録時間(FPS固定用)
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

	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//ディスクリプタヒープ
	//RTV(ゲームの画面を保存)
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

	//SRV(画像などを保存しておくもの)
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;



};