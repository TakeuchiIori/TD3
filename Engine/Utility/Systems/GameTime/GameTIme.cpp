#include "GameTime.h"
#include "imgui.h"

/*------------------------------------------------------------------

						静的メンバ変数の定義

-------------------------------------------------------------------*/
GameTime::Clock::time_point GameTime::prevTime_;
float GameTime::deltaTime_ = 0.0f;
float GameTime::unscaledDeltaTime_ = 0.0f;
float GameTime::totalTime_ = 0.0f;
float GameTime::fixedDeltaTime_ = 1.0f / 60.0f;
float GameTime::accumulatedTime_ = 0.0f;
float GameTime::timeScale_ = 1.0f;
bool GameTime::isPause_ = false;
bool GameTime::stepOneFrame_ = false;


void GameTime::Initailzie()
{
	prevTime_ = Clock::now();
	deltaTime_ = 0.0f;
	unscaledDeltaTime_ = 0.0f;
	totalTime_ = 0.0f;
	accumulatedTime_ = 0.0f;
	timeScale_ = 1.0f;
	fixedDeltaTime_ = 1.0f / 60.0f;
	isPause_ = false;
	stepOneFrame_ = false;
}

void GameTime::Update()
{
	auto now = Clock::now();
	std::chrono::duration<float> elapsed = now - prevTime_;
	prevTime_ = now;

	// 実時間の記録
	unscaledDeltaTime_ = elapsed.count();

	if (isPause_ && !stepOneFrame_) {
		deltaTime_ = 0.0f;
		return;
	}

	// スケーリング
	deltaTime_ = unscaledDeltaTime_ * timeScale_;
	// 総合時間の記録
	totalTime_ += deltaTime_;
	// 固定時間の記録
	accumulatedTime_ += deltaTime_;
	stepOneFrame_ = false;



	// FPS計算（5秒ごとの平均）
	fpsCounter_ += unscaledDeltaTime_;
	frameCount_++;

	if (fpsCounter_ >= fpsInterval_) {
		averageFps_ = static_cast<float>(frameCount_) / fpsCounter_;
		frameCount_ = 0;
		fpsCounter_ = 0.0f;
	}

}

void GameTime::ImGui()
{
	ImGui::Begin("ゲーム時間");

	ImGui::Text("FPS : %.2f", GameTime::GetAverageFPS());
	ImGui::Text("デルタタイム : %.6f", GameTime::GetDeltaTime());
	ImGui::Text("累積デルタタイム : %.2f", GameTime::GetAccumulatedTime());
	ImGui::Text("スケーリング無視デルタタイム : %f", GameTime::GetUnscaledDeltaTime());
	ImGui::Text("総ゲーム時間 : %.2f", GameTime::GetTotalTime());

	ImGui::Checkbox("一時停止", &isPause_);
	ImGui::Checkbox("1フレーム進める", &stepOneFrame_);
	ImGui::DragFloat("タイムスケール", &timeScale_, 0.5f, 0.0f, 2.0f);

	ImGui::End();

}

void GameTime::Pause()
{
	isPause_ = true;
}

void GameTime::Resume()
{
	isPause_ = false;
}

bool GameTime::ShouldUpdateOneFrame()
{
	if (accumulatedTime_ >= fixedDeltaTime_) {
		accumulatedTime_ -= fixedDeltaTime_;
		return true;
	}
	return false;
}

void GameTime::StepOneFrame()
{
	stepOneFrame_ = true;

	// ポーズ中も進める
	isPause_ = true;
}

float GameTime::GetAverageFPS()
{
	return averageFps_;
}
