#include "MapChipInfo.h"
#ifdef _DEBUG
#include "imgui.h"

#endif // _DEBUG


// Math
#include "Matrix4x4.h"
#include "MathFunc.h"

MapChipInfo::~MapChipInfo()
{

	for (auto& row : wt_) {
		for (auto& ptr : row) {
			delete ptr;
			ptr = nullptr;
		}
	}
}

void MapChipInfo::Initialize()
{
	mpField_ = std::make_unique<MapChipField>();
	mpField_->LoadMapChipCsv("Resources/images/MapChip.csv");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	obj_->SetMaterialColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	GenerateBlocks();
}

void MapChipInfo::Update()
{

	ImGui();

	for (std::vector<WorldTransform*>& row : wt_) {
		for (WorldTransform* wt : row) {
			if (wt) {
				Matrix4x4 scaleMatrix = MakeScaleMatrix(wt->scale_);
				// 各軸の回転行列
				Matrix4x4 rotX = MakeRotateMatrixX(wt->rotation_.x);
				Matrix4x4 rotY = MakeRotateMatrixY(wt->rotation_.y);
				Matrix4x4 rotZ = MakeRotateMatrixZ(wt->rotation_.z);
				Matrix4x4 rotXYZ = Multiply(rotX, Multiply(rotY, rotZ));
				// 平行移動行列
				Matrix4x4 translateMatrix = MakeTranslateMatrix(wt->translation_);
				wt->UpdateMatrix();
			}
		}
	}


}

void MapChipInfo::Draw()
{

	for (std::vector<WorldTransform*>& wt : wt_) {
		for (WorldTransform* worldTransformBlock : wt) {
			if (!worldTransformBlock)
				continue;
			obj_->Draw(camera_, *worldTransformBlock);

		}
	}


}

void MapChipInfo::GenerateBlocks()
{
	// 要素数
	uint32_t numBlockVirtical = mpField_->GetNumBlockVertical();
	uint32_t numBlockHorizotal = mpField_->GetNumBlockHorizontal();
	// 列数を設定 (縦方向のブロック数)
	wt_.resize(numBlockVirtical);
	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定 (横方向のブロック数)
		wt_[i].resize(numBlockHorizotal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizotal; ++j) {
			// どちらも2で割り切れる時またはどちらも割り切れない時
			//i % 2 == 0 && j % 2 == 0 || i % 2 != 0 && j % 2 != 0 02_02の穴あき
			if (mpField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				wt_[i][j] = worldTransform;
				wt_[i][j]->translation_ = mpField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void MapChipInfo::ImGui()
{
#ifdef _DEBUG

	// --- ImGui UI ---
	ImGui::Begin("MapChip Control");

	// 現在のCSVファイル表示
	ImGui::Text("Current CSV: %s", currentCsvFileName_.c_str());

	// 入力用バッファ（外に出してもOK）
	static char csvPathBuffer[256] = "Resources/images/MapChip.csv";

	// ファイル名入力と読み込みボタン
	ImGui::InputText("CSV Path", csvPathBuffer, IM_ARRAYSIZE(csvPathBuffer));
	if (ImGui::Button("Load CSV")) {
		currentCsvFileName_ = std::string(csvPathBuffer);
		mpField_->LoadMapChipCsv(currentCsvFileName_);
		GenerateBlocks(); // 再生成！
	}

	// インデックス指定でマップチップ編集
	static int editX = 0;
	static int editY = 0;
	static int selectedType = 0;

	ImGui::InputInt("X Index", &editX);
	ImGui::InputInt("Y Index", &editY);
	ImGui::Combo("Chip Type", &selectedType, "Blank\0Block\0Body\0DropEnemy\0SideEnemy\0");

	if (ImGui::Button("Set Chip")) {
		if (editX >= 0 && editX < MapChipField::GetNumBlockHorizontal() &&
			editY >= 0 && editY < MapChipField::GetNumBlockVertical()) {
			mpField_->SetMapChipTypeByIndex(editX, editY, static_cast<MapChipType>(selectedType));
			GenerateBlocks(); // 表示再構築（または最小限更新）
		}
	}

	if (ImGui::Button("Save CSV")) {
		try {
			mpField_->SaveMapChipCsv(currentCsvFileName_);
		}
		catch (const std::exception& e) {
			ImGui::OpenPopup("Save Error");
		}
	}

	// Popup 定義は毎フレーム書く必要あり
	if (ImGui::BeginPopup("Save Error")) {
		ImGui::Text("Save failed!");
		if (ImGui::Button("OK")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}



	ImGui::End();

	//// ImGui テーブルで 2Dマップを表現！
	//ImGui::Begin("MapChip Grid");

	//if (ImGui::BeginTable("MapChipTable", MapChipField::GetNumBlockHorizontal())) {
	//	for (int y = 0; y < MapChipField::GetNumBlockVertical(); ++y) {
	//		ImGui::TableNextRow();
	//		for (int x = 0; x < MapChipField::GetNumBlockHorizontal(); ++x) {
	//			ImGui::TableSetColumnIndex(x);

	//			// 現在のチップ
	//			MapChipType type = mpField_->GetMapChipTypeByIndex(x, y);
	//			const char* label = ""; // 表示ラベル
	//			switch (type) {
	//			case MapChipType::kBlank: label = " "; break;
	//			case MapChipType::kBlock: label = "■"; break;
	//			case MapChipType::kDropEnemy: label = "▼"; break;
	//			case MapChipType::kSideEnemy: label = "→"; break;
	//			case MapChipType::kBody: label = "◎"; break;
	//			}

	//			// ボタンとして描画＆クリックで選択
	//			if (ImGui::Button(label, ImVec2(20, 20))) {
	//				editX = x;
	//				editY = y;
	//			}
	//		}
	//	}
	//	ImGui::EndTable();
	//}
	//ImGui::End();

#endif // _DEBUG
}



