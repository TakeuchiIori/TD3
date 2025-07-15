#include "MyGame.h"
#include "Particle./ParticleManager.h"
#include "Loaders/Model/Mesh/MeshPrimitive.h"

const std::string defaultTexturePath = "Resources/Textures/Particle/";
void MyGame::Initialize()
{
	// 基盤の初期化
	Framework::Initialize();

	// シーンファクトリを生成し、 シーンマネージャに最初のシーンをセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	SceneManager::GetInstance()->SetTransitionFactory(std::make_unique<FadeTransitionFactory>());
	SceneManager::GetInstance()->Initialize();
	offScreen_ = OffScreen::GetInstance();
	offScreen_->Initialize();
	//offScreen_->SetEffectType(OffScreen::OffScreenEffectType::RadialBlur);
#ifdef _DEBUG
	SceneManager::GetInstance()->ChangeScene("Title");
#else//Title
	SceneManager::GetInstance()->ChangeScene("Title");
#endif
	// パーティクルマネージャ生成

	ParticleManager::GetInstance()->Initialize(srvManager_);
	auto planeMesh = MeshPrimitive::Createplane(1.0f, 1.0f);

	
	
	
	ParticleManager::GetInstance()->CreateParticleGroup("GrowthParticle", defaultTexturePath + "star.png");
	ParticleManager::GetInstance()->CreateParticleGroup("YodareParticle", defaultTexturePath + "yodare.png");
	ParticleManager::GetInstance()->CreateParticleGroup("TitleParticle", defaultTexturePath + "circle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("ClearParticle", defaultTexturePath + "star.png");

	ParticleManager::GetInstance()->SetPrimitiveMesh("GrowthParticle", planeMesh);
	ParticleManager::GetInstance()->SetPrimitiveMesh("YodareParticle", planeMesh);
	ParticleManager::GetInstance()->SetPrimitiveMesh("TitleParticle", planeMesh);
	ParticleManager::GetInstance()->SetPrimitiveMesh("ClearParticle", planeMesh);
	
}

void MyGame::Finalize()
{
	Framework::Finalize();
}

void MyGame::Update()
{

	// 基盤の更新
	Framework::Update();

}

void MyGame::Draw()
{
	dxCommon_->PreDrawScene();
	// Srvの描画準備
	srvManager_->PreDraw();

	// ゲームの描画
	SceneManager::GetInstance()->Draw();

	dxCommon_->PreDrawImGui();
	offScreen_->SetProjection(SceneManager::GetInstance()->GetScene()->GetViewProjection());
	offScreen_->Draw();

	dxCommon_->DepthBarrier();
	SceneManager::GetInstance()->DrawForOffscreen();


	imguiManager_->Draw();
	// DirectXの描画終了
	dxCommon_->PostDraw();
}
