#pragma once
#include "DirectXCommon.h"

class ImGuiManager
{
public:
	//������
	static void Initialize(HWND hwnd, DirectXCommon* dxCommon);

	//�X�V�J�n
	static void NewFrame();

	//�X�V�I��
	static void CreateCommand();

	//�R�}���h��ς�
	static 	void CommandExcute(ID3D12GraphicsCommandList* commandList);

	//new����
	static ImGuiManager* Create();

public:

	~ImGuiManager();

public:
	void ShowDemo();


};

