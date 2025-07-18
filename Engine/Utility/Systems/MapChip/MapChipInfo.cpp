#include "MapChipInfo.h"

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

	//delete obj_;
	delete mpField_;
}

void MapChipInfo::Initialize()
{
	mpField_ = new MapChipField();
	mpField_->LoadMapChipCsv("Resources/images/MapChip.csv");

	GenerateBlocks();

	InitJson();
}

void MapChipInfo::SetMapChip(std::string path)
{
	for (auto& row : wt_) {
		for (auto& wt : row) {
			delete wt;
			wt = nullptr;
		}
	}
	wt_.clear();
	objects_.clear();
	floors_.clear();
	mpField_->ResetMapChipData();
	mpField_->LoadMapChipCsv(path);
	GenerateBlocks();
}

void MapChipInfo::InitJson()
{
	jsonManager_ = std::make_unique<JsonManager>("MapChipInfo", "Resources/JSON/MapChip/Info");
	jsonManager_->SetCategory("MapChip");
	jsonManager_->Register("1:Color", &color_[0]);
	jsonManager_->Register("1:Alpha", &alpha_[0]);

	jsonManager_->Register("4:Color", &color_[1]);
	jsonManager_->Register("4:Alpha", &alpha_[1]);

	jsonManager_->Register("6:Color", &color_[2]);
	jsonManager_->Register("6:Alpha", &alpha_[2]);
}

void MapChipInfo::Update()
{

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
	for (auto& obj : objects_) {
		for (auto& obj2 : obj) {
			if (obj2) {
				obj2->SetMaterialColor(color_[0]);
				//obj2->SetAlpha(alpha_[0]);
			}
		}
	}

	for (auto& obj : floors_) {
		for (auto& obj2 : obj) {
			if (obj2) {
				//obj2->SetMaterialColor(color_[1]);
				//obj2->SetAlpha(alpha_[1]);
			}
		}
	}

	for (auto& obj : ceilings_) {
		for (auto& obj3 : obj) {
			if (obj3) {
				obj3->SetMaterialColor(color_[2]);
				//obj2->SetAlpha(alpha_[1]);
			}
		}
	}

}

void MapChipInfo::Draw() {
	for (uint32_t i = 0; i < wt_.size(); ++i) {
		for (uint32_t j = 0; j < wt_[i].size(); ++j) {
			MapChipType type = mpField_->GetMapChipTypeByIndex(j, i);

			if (type == MapChipType::kBlank) {
				continue; // 空白なら何も描画しない！
			}

			if (wt_[i][j]) {
				if (objects_[i][j] && type == MapChipType::kBlock) {
					objects_[i][j]->Draw(camera_, *wt_[i][j]);
				}
				if (floors_[i][j] && type == MapChipType::kFloor) {
					floors_[i][j]->Draw(camera_, *wt_[i][j]);
				}
				if (ceilings_[i][j] && type == MapChipType::kCeiling) {
					ceilings_[i][j]->Draw(camera_, *wt_[i][j]);
				}
				if (soils_[i][j] && type == MapChipType::kSoil) {
					soils_[i][j]->Draw(camera_, *wt_[i][j]);
				}
			}
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
	objects_.resize(numBlockVirtical);
	floors_.resize(numBlockVirtical);
	ceilings_.resize(numBlockVirtical);
	soils_.resize(numBlockVirtical);
	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定 (横方向のブロック数)
		wt_[i].resize(numBlockHorizotal);
		objects_[i].resize(numBlockHorizotal);
		floors_[i].resize(numBlockHorizotal);
		ceilings_[i].resize(numBlockHorizotal);
		soils_[i].resize(numBlockHorizotal);
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

				auto obj = std::make_unique<Object3d>();
				obj->Initialize();
				obj->SetModel("cube.obj");
				objects_[i][j] = std::move(obj);

			} else if (mpField_->GetMapChipTypeByIndex(j, i) == MapChipType::kFloor) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				wt_[i][j] = worldTransform;
				wt_[i][j]->translation_ = mpField_->GetMapChipPositionByIndex(j, i);
				auto obj = std::make_unique<Object3d>();
				obj->Initialize();
				obj->SetModel("groundBlock.obj");
				floors_[i][j] = std::move(obj);

			} else if (mpField_->GetMapChipTypeByIndex(j, i) == MapChipType::kCeiling) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				wt_[i][j] = worldTransform;
				wt_[i][j]->translation_ = mpField_->GetMapChipPositionByIndex(j, i);
				auto obj = std::make_unique<Object3d>();
				obj->Initialize();
				obj->SetModel("cube.obj");
				ceilings_[i][j] = std::move(obj);
			}else if(mpField_->GetMapChipTypeByIndex(j, i) == MapChipType::kSoil) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				wt_[i][j] = worldTransform;
				wt_[i][j]->translation_ = mpField_->GetMapChipPositionByIndex(j, i);
				auto obj = std::make_unique<Object3d>();
				obj->Initialize();
				obj->SetModel("soilBlock.obj");
				soils_[i][j] = std::move(obj);
			}
		}
	}
}


