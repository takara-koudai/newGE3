#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION     0x0800   // DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//using namespace Microsoft::WRL;
#include "WinApp.h"


class Input
{
public:


	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	//全キーの状態
	BYTE key[256] = {};

	//前回のキーの状態
	BYTE preKey[256] = {};

	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	WinApp* winApp_;

public:
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0　等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0　等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

};

