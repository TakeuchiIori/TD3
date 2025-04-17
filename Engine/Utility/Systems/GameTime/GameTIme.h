#pragma once

// Engine
#include "float.h"
#include "MathFunc.h"

// C++
#include <chrono>

class GameTime
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	static void Initailzie();

	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	/// <summary>
	/// ImGuiで表示
	/// </summary>
	static void ImGui();

	/// <summary>
	/// 一時停止
	/// </summary>
	static void Pause();

	/// <summary>
	/// 一時停止解除
	/// </summary>
	static void Resume();

	/// <summary>
	/// 固定時間過ぎたらtrue
	/// </summary>
	static bool ShouldUpdateOneFrame();

	/// <summary>
	/// デバッグ用
	/// </summary>
	static void StepOneFrame();


public:

	/*--------------------------------------------------------------------------

									アクセッサ

	---------------------------------------------------------------------------*/


	static float GetDeltaTime()
	{
		return deltaTime_;
	}

	static float GetUnscaledDeltaTime()
	{
		return unscaledDeltaTime_;
	}

	static float GetAccumulatedTime()
	{
		return accumulatedTime_;
	}

	static float GetTotalTime()
	{
		return totalTime_;
	}

	static float GetFixedDeltaTime()
	{
		return fixedDeltaTime_;
	}

	static void SetTimeScale(float timeScale)
	{
		timeScale_ = timeScale;
	}

	static float GetTimeScale()
	{
		return timeScale_;
	}

	static bool IsPause()
	{
		return isPause_;
	}

	static float GetFPS()
	{
		return (deltaTime_ > 0.0f) ? 1.0f / deltaTime_ : 0.0f;
	}

	/// <summary>
	/// 5秒ごとの平均FPSを取得
	/// </summary>
	static float GetAverageFPS();




private:

	using Clock = std::chrono::steady_clock;

	/// <summary>
	/// 前フレームの時刻
	/// </summary>
	static Clock::time_point prevTime_;

	/// <summary>
	/// 経過時間
	/// </summary>
	static float deltaTime_;

	/// <summary>
	/// ポーズ中でも計測する経過時間
	/// </summary>
	static float unscaledDeltaTime_;

	/// <summary>
	/// ゲーム開始からの合計時間
	/// </summary>
	static float totalTime_;

	/// <summary>
	/// 固定時間
	/// </summary>
	static float fixedDeltaTime_;

	/// <summary>
	/// 累積した固定時間
	/// </summary>
	static float accumulatedTime_;

	/// <summary>
	/// 時間のスケール　: 1.0f = 通常, 0.5f = 半分の速さ, 2.0f = 2倍の速さ
	/// </summary>
	static float timeScale_;

	/// <summary>
	/// ポーズ中
	/// </summary>
	static bool isPause_;

	/// <summary>
	/// 1フレームだけ進める
	/// </summary>
	static bool stepOneFrame_;

};


/*-----------------------------------------------------------------

							FPS関連

-----------------------------------------------------------------*/
namespace {


	/// <summary>
	/// 平均のFPS
	/// </summary>
	float averageFps_ = 0.0f;

	/// <summary>
	/// FPSを算出
	/// </summary>
	float fpsCounter_ = 0.0f;

	/// <summary>
	/// フレーム数をカウント
	/// </summary>
	int frameCount_ = 0;

	/// <summary>
	/// FPSを計算する間隔（秒）
	/// </summary>
	constexpr float fpsInterval_ = 2.0f;

}
