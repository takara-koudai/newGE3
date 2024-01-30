#pragma once
#include <Windows.h>

class WinApp
{
public:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	//������
	void Initialize();
	//�X�V
	bool Update();
	
	//�㏈��
	void Finalize();

	//Getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetInstance() const{ return w.hInstance; }


	// �E�B���h�E�T�C�Y
	static const int window_width = 1280;  // ����
	static const int window_height = 720;  // �c��


private:

	HWND hwnd;
	WNDCLASSEX w{};

	MSG msg{};  // ���b�Z�[�W

};

