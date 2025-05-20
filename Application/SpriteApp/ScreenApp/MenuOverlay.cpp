#include "MenuOverlay.h"
#include "Systems/GameTime/GameTime.h"
#include "MathFunc.h"

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
		backGround_->Initialize("Resources/Textures/Option/white2x2.png");
		backGround_->SetSize({ 1280,720 });
		backGround_->SetColor({ 0.0f,0.0f,0.0f,0.9f });


		minX += volumeUIOffset_.x;
		maxX += volumeUIOffset_.x;

		for (int i = 0; i < volumeSprites_.size(); ++i) {
			volumeSprites_[i] = std::make_unique<Sprite>();
			volumeSprites_[i]->Initialize("Resources/Textures/Option/VolumeBottun.png");
			volumeSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
		}
		InitJson();
		int i = 0;
		for (VolumeControl& vol : volumeControl_)
		{
			vol.pos.x = minX + (maxX - minX) * volume[i];
			vol.pos.y = (i * intervalY_) + volumeUIOffset_.y;
			++i;
		}

		isInit_ = true;
	}
}

void MenuOverlay::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("Menu", "Resources/JSON/");
	jsonManager_->SetCategory("Menu");
	jsonManager_->SetSubCategory("Volumes");
	jsonManager_->Register("主音量", &volume[static_cast<int>(AudioType::kMaster)]);
	jsonManager_->Register("BGM音量", &volume[static_cast<int>(AudioType::kBGM)]);
	jsonManager_->Register("SE音量", &volume[static_cast<int>(AudioType::kSE)]);
	jsonManager_->Register("UI音量", &volume[static_cast<int>(AudioType::kUISound)]);
}

void MenuOverlay::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Volume");
	ImGui::Text("主音量  : %.2f", volume[static_cast<int>(AudioType::kMaster)]);
	ImGui::Text("BGM音量 : %.2f", volume[static_cast<int>(AudioType::kBGM)]);
	ImGui::Text("SE音量  : %.2f", volume[static_cast<int>(AudioType::kSE)]);
	ImGui::Text("UI音量  : %.2f", volume[static_cast<int>(AudioType::kUISound)]);
	ImGui::End();
#endif // _DEBUG

	VolumeSlider();
	for (int i = 0; i < volumeSprites_.size(); ++i) {
		volumeSprites_[i]->SetPosition(Vector3{ volumeControl_[i].pos.x,volumeControl_[i].pos.y, 0 });
	}
	for (int i = 0; i < volumeSprites_.size(); ++i) {
		volumeSprites_[i]->Update();
	}
	backGround_->Update();
}

void MenuOverlay::Draw()
{
	backGround_->Draw();
	for (const auto& sprite : volumeSprites_) {
		sprite->Draw();
	}
}

void MenuOverlay::ShowHide()
{
	if (isVisible_)
	{
		Hide();
		GameTime::Pause();
	}
	else
	{
		Show();
		GameTime::Resume();
	}
}

void MenuOverlay::Show()
{
	if (input_->TriggerKey(DIK_TAB) || input_->IsPadTriggered(0, GamePadButton::Start))
	{
		isVisible_ = true;
	}
}

void MenuOverlay::Hide()
{
	if (input_->TriggerKey(DIK_TAB) || input_->IsPadTriggered(0, GamePadButton::Start))
	{
		isVisible_ = false;
	}
}

void MenuOverlay::VolumeSlider()
{
	if(input_->IsControllerConnected())
	{
		static int index = 0;
		static bool isSelect = false;
		float threshold = 0.3f;

		Vector2 LStick = input_->GetLeftStickInput(0);
		if (std::abs(LStick.x) < threshold && std::abs(LStick.y) < threshold) {
			LStick = {};
		}
		if (LStick.y > 0)
		{
			if (index > 0 && isSelect == false)
			{
				index--;
				isSelect = true;

				for (auto&& volTex : volumeSprites_)
				{
					volTex->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}
		}
		else if (LStick.y < 0)
		{
			if (index < 3 && isSelect == false)
			{
				index++;
				isSelect = true;

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
		if (RStick.x < 0)
		{
			volumeControl_[index].pos.x -= changeVolume_;
		}
		else if(RStick.x > 0)
		{
			volumeControl_[index].pos.x += changeVolume_;
		}
		volumeControl_[index].pos.x = std::clamp(volumeControl_[index].pos.x, minX, maxX);
		float liner = maxX - minX;
		float current = maxX - volumeControl_[index].pos.x;
		float t = 1.0f - (current / liner);
		volume[static_cast<AudioType>(index)] = t;
		volumeManager_->SetVolume(static_cast<AudioType>(index), t);
		volumeSprites_[index]->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
		if (RStick.x != 0) jsonManager_->Save();
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
			if (vol.isPush)
			{
				vol.pos.x = std::clamp(input_->GetMousePosition().x, minX, maxX);
				float liner = maxX - minX;
				float current = maxX - vol.pos.x;
				float t = 1.0f - (current / liner);
				volume[static_cast<AudioType>(i)] = t;
				volumeManager_->SetVolume(static_cast<AudioType>(i), t);
				volumeSprites_[i]->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
				jsonManager_->Save();
				break;
			}
			++i;
		}
	}
	else if (isPush)
	{
		for (VolumeControl& vol : volumeControl_)
		{
			vol.isPush = false;
		}
		for (auto&& volTex : volumeSprites_)
		{
			volTex->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		isPush = false;
	}
}

float MenuOverlay::Length(const Vector2& v)
{
	return std::sqrtf(v.x * v.x + v.y * v.y);
}
