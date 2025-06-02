#include "MenuOverlay.h"
#include "Systems/GameTime/GameTime.h"
#include "MathFunc.h"
#include "Easing.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void MenuOverlay::Initialize()
{
	if(!isInit_)
	{
		input_ = Input::GetInstance();
		volumeManager_ = AudioVolumeManager::GetInstance();

		backGround_ = std::make_unique<Sprite>();
		backGround_->Initialize("Resources/Textures/In_Game/transparent.png");
		backGround_->SetSize({ 1280,720 });
		backGround_->SetColor({ 0.5f,0.5f,0.5f,1.0f });

		menuTex_ = std::make_unique<Sprite>();
		menuTex_->Initialize(controllerPath_);
		menuTex_->SetPosition(menuGoal_);
		menuTex_->SetAnchorPoint({ 0.5f, 0.5f });
		menuTex_->SetSize(menuTex_->GetTextureSize() * goalScale_);

		kirinL_ = std::make_unique<Sprite>();
		kirinL_->Initialize("Resources/Textures/Menu/menu_kirin_L.png");
		kirinL_->SetPosition(kirinLGoal_);
		kirinL_->SetAnchorPoint({ 0.0f, 0.0f });
		kirinL_->SetSize(kirinL_->GetTextureSize() * goalScale_);

		kirinR_ = std::make_unique<Sprite>();
		kirinR_->Initialize("Resources/Textures/Menu/menu_kirin_R.png");
		kirinR_->SetPosition(kirinRGoal_);
		kirinR_->SetAnchorPoint({ 0.0f, 0.0f });
		kirinR_->SetSize(kirinR_->GetTextureSize() * goalScale_);


		maxY += volumeUIOffset_.x;
		minY += volumeUIOffset_.x;

		for (int i = 0; i < volumeSprites_.size(); ++i) {
			volumeSprites_[i] = std::make_unique<Sprite>();
			volumeSprites_[i]->Initialize("Resources/Textures/Menu/BGM_SE_Slider.png");
			volumeSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		}
		InitJson();
		int i = 0;
		for (VolumeControl& vol : volumeControl_)
		{
			vol.pos.x = (i * intervalX_) + volumeUIOffset_.y; 
			vol.pos.y = minY + (maxY - minY) * volume[i];
			volumeControl_[i].pos.y = std::clamp(volumeControl_[i].pos.y, maxY, minY);
			float liner = maxY - minY;
			float current = maxY - volumeControl_[i].pos.y;
			float t = 1.0f - (current / liner);
			volume[static_cast<AudioType>(i)] = t;
			volumeManager_->SetVolume(static_cast<AudioType>(i), t);
			++i;
		}

		StartScaling();
		menuSoundData_ = Audio::GetInstance()->LoadAudio(L"Resources/Audio/menu.mp3");
		isInit_ = true;
	}
}

void MenuOverlay::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Menu", "Resources/JSON/");
	jsonManager_->SetCategory("Menu");
	jsonManager_->SetSubCategory("Volumes");
	jsonManager_->Register("BGM音量", &volume[static_cast<int>(AudioType::kBGM)]);
	jsonManager_->Register("SE音量", &volume[static_cast<int>(AudioType::kSE)]);
}

void MenuOverlay::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Volume");
	ImGui::Text("BGM音量 : %.2f", volume[static_cast<int>(AudioType::kBGM)]);
	ImGui::Text("SE音量  : %.2f", volume[static_cast<int>(AudioType::kSE)]);
	ImGui::End();
#endif // _DEBUG

	if(menuTimer_ <= 0)
	{
		VolumeSlider();
		for (int i = 0; i < volumeSprites_.size(); ++i) {
			volumeSprites_[i]->SetPosition(Vector3{ volumeControl_[i].pos.x,volumeControl_[i].pos.y, 0 });
		}
		for (int i = 0; i < volumeSprites_.size(); ++i) {
			volumeSprites_[i]->Update();
		}
		isAnimation_ = false;
	}
	else
	{
		menuTimer_ -= 1.0f / 60.0f;
		if (menuTimer_ <= 0)
		{
			menuTimer_ = 0;
		}
		float menuT = 1.0f - (menuTimer_ / kMenuTime_);
		float kirinT = 1.0f;
		if (((menuTimer_ - kirinTime_) / kMenuTime_) > 0)
		{
			kirinT = 1.0f - ((menuTimer_ - kirinTime_) / kMenuTime_);
		}
		if (isAnimation_)
		{
			
			menuTex_->SetSize(menuTex_->GetTextureSize() * Lerp(startScale_, goalScale_, Easing::ease(Easing::Function::EaseOutBounce , menuT)));
			kirinL_->SetPosition(Lerp(kirinLStart_, kirinLGoal_, kirinT));
			kirinR_->SetPosition(Lerp(kirinRStart_, kirinRGoal_, kirinT));
		}
		else if (isEndAnimation_)
		{
			menuTex_->SetSize(menuTex_->GetTextureSize() * Lerp(goalScale_, startScale_, Easing::ease(Easing::Function::EaseOutQuart, menuT)));
			kirinL_->SetPosition(Lerp(kirinLGoal_, kirinLStart_, menuT));
			kirinR_->SetPosition(Lerp(kirinRGoal_, kirinRStart_, menuT));
			if (menuTimer_ <= 0)
			{
				isVisible_ = false;
				GameTime::Resume();
			}
		}
	}
	backGround_->Update();
	menuTex_->Update();
	kirinL_->Update();
	kirinR_->Update();
}

void MenuOverlay::Draw()
{
	backGround_->Draw();
	menuTex_->Draw();
	kirinL_->Draw();
	kirinR_->Draw();

	if (menuTimer_ <= 0)
	{
		for (const auto& sprite : volumeSprites_) {
			sprite->Draw();
		}
	}
}

void MenuOverlay::ShowHide()
{
	if (menuTimer_ <= 0)
	{
		if (isVisible_)
		{
			Hide();
			GameTime::Pause();
		}
		else
		{
			Show();
		}
	}
}

void MenuOverlay::Show()
{
	if (input_->IsPadTriggered(0, GamePadButton::Start))
	{
		menuSourceVoice_ = Audio::GetInstance()->SoundPlayAudio(menuSoundData_);
		AudioVolumeManager::GetInstance()->SetSourceToSubmix(menuSourceVoice_, kSE);
		isVisible_ = true;
		isAnimation_ = true;
		menuTimer_ = kMenuTime_;
		if (input_->IsControllerConnected())
		{
			menuTex_->ChangeTexture(controllerPath_);
		}
		else 
		{
			menuTex_->ChangeTexture(mousePath_);
		}
	}
}

void MenuOverlay::Hide()
{
	if (input_->IsPadTriggered(0, GamePadButton::Start))
	{
		menuSourceVoice_ = Audio::GetInstance()->SoundPlayAudio(menuSoundData_);
		AudioVolumeManager::GetInstance()->SetSourceToSubmix(menuSourceVoice_, kSE);
		isEndAnimation_ = true;
		menuTimer_ = kMenuTime_;
	}
}

void MenuOverlay::VolumeSlider()
{
	static int index = 0;
	if(input_->IsControllerConnected())
	{
		static bool isSelect = false;
		float threshold = 0.3f;

		Vector2 LStick = input_->GetLeftStickInput(0);
		if (std::abs(LStick.x) < threshold && std::abs(LStick.y) < threshold) {
			LStick = {};
		}
		if (LStick.x > 0)
		{
			if (index < volumeSprites_.size() - 1 && isSelect == false)
			{
				index++;
				isSelect = true;
				StartScaling();

				for (auto&& volTex : volumeSprites_)
				{
					volTex->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}
		else if (LStick.x < 0)
		{
			if (index > 0 && isSelect == false)
			{
				index--;
				isSelect = true;
				StartScaling();

				for (auto&& volTex : volumeSprites_)
				{
					volTex->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}
		else
		{
			isSelect = false;
		}



		Vector2 RStick = input_->GetRightStickInput(0);
		if (std::abs(RStick.x) < threshold && std::abs(RStick.y) < threshold) {
			RStick = {};
		}
		if (RStick.y < 0)
		{
			volumeControl_[index].pos.y += changeVolume_;
		}
		else if(RStick.y > 0)
		{
			volumeControl_[index].pos.y -= changeVolume_;
		}
		volumeControl_[index].pos.y = std::clamp(volumeControl_[index].pos.y, maxY, minY);
		float liner = maxY - minY;
		float current = maxY - volumeControl_[index].pos.y;
		float t = 1.0f - (current / liner);
		volume[static_cast<AudioType>(index)] = t;
		volumeManager_->SetVolume(static_cast<AudioType>(index), t);
		volumeSprites_[index]->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
		if (RStick.y != 0) jsonManager_->Save();
	}


	if (input_->IsTriggerMouse(0))
	{
		if (!isPush)
		{
			for (VolumeControl& vol : volumeControl_)
			{
				if (Length(vol.pos - input_->GetMousePosition()) <= vol.radius)
				{
					vol.isPush = true;
					isPush = true;
					StartScaling();
					break;
				}
			}
		}
	}
	int i = 0;
	if (input_->IsPressMouse(0) && isPush)
	{
		for (VolumeControl& vol : volumeControl_)
		{
			volumeSprites_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			if (vol.isPush)
			{
				vol.pos.y = std::clamp(input_->GetMousePosition().y, maxY, minY);
				float liner = maxY - minY;
				float current = maxY - vol.pos.x;
				float t = 1.0f - (current / liner);
				volume[static_cast<AudioType>(i)] = t;
				volumeManager_->SetVolume(static_cast<AudioType>(i), t);
				volumeSprites_[i]->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
				jsonManager_->Save();
				index = i;
			}
			++i;
		}
	}
	else if (isPush)
	{
		int i = 0;
		for (auto&& volTex : volumeSprites_)
		{
			if(!volumeControl_[i].isPush)volTex->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			++i;
		}
		for (VolumeControl& vol : volumeControl_)
		{
			vol.isPush = false;
		}
		isPush = false;
	}
	for (auto&& vol : volumeSprites_)
	{
		vol->SetSize(vol->GetTextureSize());
	}
	ButtonScaling();
	volumeSprites_[index]->SetSize(volumeSprites_[index]->GetTextureSize() * Lerp(volStartScale_, volGoalScale_, Easing::ease(Easing::Function::EaseOutBounce, volT_)));

}

float MenuOverlay::Length(const Vector2& v)
{
	return std::sqrtf(v.x * v.x + v.y * v.y);
}

void MenuOverlay::StartScaling()
{
	volTimer_ = kVolTime_;
}

void MenuOverlay::ButtonScaling()
{
	if (volTimer_ > 0)
	{
		volTimer_ -= 1.0f / 60.0f;

		volT_ = 1.0f - (volTimer_ / kVolTime_);
	}
}
