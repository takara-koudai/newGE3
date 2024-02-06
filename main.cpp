#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "ImGuiManager.h"
#include <vector>
#include "TextureManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    //ポインタ置き場
    Input* input_ = nullptr;
    WinApp* winApp_ = nullptr;
    DirectXCommon* dxCommon_ = nullptr;

#pragma region WindowsAPI初期化処理
    winApp_ = new WinApp();
    winApp_->Initialize();

#pragma endregion

#pragma region DirectX初期化処理
    dxCommon_ = new DirectXCommon();
    dxCommon_->Initialize(winApp_);

  #pragma endregion

    input_ = new Input();
    input_->Initialize(winApp_);

    ImGuiManager* imgui = ImGuiManager::Create();
    ImGuiManager::Initialize(
        winApp_->GetHwnd(), dxCommon_);

    //スプライトコモン
    SpriteCommon* spriteCommon = new SpriteCommon();
    spriteCommon->Initialize(dxCommon_);

    //テクスチャマネージャ
    TextureManager::GetInstance()->Initialize(dxCommon_);
    TextureManager::GetInstance()->Loadtexture(L"Resources/mario.jpg");


    //画像
    //Sprite* sprite = new Sprite();
    std::vector<Sprite*> sprite;

    //数字を1にすれば1枚だけ描画できる
    for (int i = 0; i < 5; i++)
    {
        Sprite* temp = new Sprite();
        temp->Initialize(spriteCommon, L"Resources/mario.jpg");
        temp->SetPosition({ (float)i * 120,0 });
        //temp->SetPosition({ 400,200 });
        sprite.push_back(temp);
    }

    //sprite->Initialize(dxCommon_, spriteCommon);


    // ゲームループ
    while (true) {
        //更新
        if (winApp_->Update() == true) {

            break;
        }
        ImGuiManager::NewFrame();
        imgui->ShowDemo();

         //入力
        input_->Update();


        //移動
        /*DirectX::XMFLOAT2 pos = sprite->GetPosition();
        pos.x += 0.01;
        sprite->SetPosition(pos);

        //回転
        float rot = sprite->GetRotation();
        rot += 0.005f;
        sprite->SetRotation(rot);

        //色
        DirectX::XMFLOAT4 color = sprite->GetColor();
        color.x -= 0.01f;
        if (color.x < 0)
        {
            color.x = 1.0f;
        }
        sprite->SetColor(color);

        //サイズ
        DirectX::XMFLOAT2 size = sprite->GetSize();
        size.y += 0.01;
        sprite->SetSize(size);
        */

        for (int i = 0; i < 5; i++)
        {
            sprite[i]->Update();
        }
        //sprite->Update();

       

        //描画前処理
        ImGuiManager::CreateCommand();
        dxCommon_->PreDraw();
        

        for (int i = 0; i < 5; i++)
        {
            sprite[i]->Draw();
        }

        //sprite->Draw();

        //描画後処理
        ImGuiManager::CommandExcute(dxCommon_->GetCommandList());
        dxCommon_->PostDraw();
       

    }

    for (int i = 0; i < 5; i++)
    {
        delete sprite[i];
    }

    TextureManager::GetInstance()->Finalize();

    //delete sprite;
    delete spriteCommon;

    delete imgui;

    delete input_;

    winApp_->Finalize();

    delete winApp_;
    delete dxCommon_;


    return 0;
}
