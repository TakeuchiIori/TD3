#include "ParticleEmitter.h"
#ifdef _DEBUG
#include "imgui.h"
#endif
ParticleEmitter::ParticleEmitter(const std::string& name, const Vector3& transform, uint32_t count)
	: emitter_{ name, Vector3{transform,}, count, 0.0025f, 0.0f } {
}

void ParticleEmitter::Initialize(const std::string& jsonPath) {

	InitJson(jsonPath);
}

void ParticleEmitter::InitJson(const std::string& jsonPath)
{
	jsonManager_ = std::make_unique<JsonManager>(jsonPath, "Resources/Json/Emitters/");
	jsonManager_->SetCategory("Emitter");
	jsonManager_->SetSubCategory(jsonPath + "Emitter");
	jsonManager_->Register("Name", &emitter_.name);
	jsonManager_->Register("Transform", &emitter_.transform);
	jsonManager_->Register("Count", &emitter_.count);
	jsonManager_->Register("Frequency", &emitter_.frequency);
	jsonManager_->Register("FrequencyTime", &emitter_.frequencyTime);
}

void ParticleEmitter::UpdateTime()
{
	emitter_.frequencyTime += deltaTime_;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		// パーティクルを生成してグループに追加
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

}

void ParticleEmitter::FollowEmit(const Vector3& transform)
{
	ParticleManager::GetInstance()->Emit(emitter_.name, transform, emitter_.count);
}

void ParticleEmitter::Emit()
{
	ParticleManager::GetInstance()->Emit(emitter_.name, emitter_.transform, emitter_.count);
}

/// <summary>
/// from から to に向かってパーティクルを発生させる
/// </summary>
void ParticleEmitter::EmitFromTo(const Vector3& from, const Vector3& to)
{
	Vector3 direction = Normalize(to - from); // 単位方向ベクトル
	ParticleManager::ParticleParameters& params = ParticleManager::GetInstance()->GetParameters(emitter_.name);

	// 明示的に中心方向に向かわせる設定
	params.randomFromCenter = false;
	params.direction= direction;

	ParticleManager::GetInstance()->Emit(emitter_.name, from, emitter_.count);
}


void ParticleEmitter::ShowImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Particle");

	if (ImGui::Button("Add Particle")) {
		Emit();
	}
	ImGui::End();
#endif // _DEBUG

#ifdef _DEBUG
	ImGui::Begin("Emitter");

	// 周波数の調整 (ボタンで増減)
	ImGui::Text("Frequency:");
	if (ImGui::Button("-##Frequency")) {
		emitter_.frequency -= 0.1f; // 減少
		if (emitter_.frequency < 0.1f) emitter_.frequency = 0.1f; // 最小値を保持
	}
	ImGui::SameLine(); // 同じ行に次の要素を表示
	ImGui::Text("%.2f", emitter_.frequency);
	ImGui::SameLine();
	if (ImGui::Button("+##Frequency")) {
		emitter_.frequency += 0.1f; // 増加
		if (emitter_.frequency > 100.0f) emitter_.frequency = 100.0f; // 最大値を保持
	}

	// 周波数時間の調整 (ボタンで増減)
	ImGui::Text("Frequency Time:");
	if (ImGui::Button("-##FrequencyTime")) {
		emitter_.frequencyTime -= 0.1f; // 減少
		if (emitter_.frequencyTime < 0.0f) emitter_.frequencyTime = 0.0f; // 最小値を保持
	}
	ImGui::SameLine();
	ImGui::Text("%.2f", emitter_.frequencyTime);
	ImGui::SameLine();
	if (ImGui::Button("+##FrequencyTime")) {
		emitter_.frequencyTime += 0.1f; // 増加
		if (emitter_.frequencyTime > 100.0f) emitter_.frequencyTime = 100.0f; // 最大値を保持
	}


	ImGui::End();
#endif // _DEBUG

}