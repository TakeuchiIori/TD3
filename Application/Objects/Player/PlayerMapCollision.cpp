#include "PlayerMapCollision.h"

void PlayerMapCollision::DetectAndResolveCollision(
	const ColliderRect& colliderRect,
	Vector3& position,
	Vector3& velocity,
	int checkFlags,
	std::function<void(const CollisionInfo&)> collisionCallback)
{
	////
	isCollision_ = false;

	// 速度が大きい場合はサブステップに分割
	const float maxSafeDisplacement = mapChipField_->GetBlockSize() * 0.5f;
	float totalDisplacement = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	int numSteps = static_cast<int>(std::ceil(totalDisplacement / maxSafeDisplacement));
	numSteps = std::max(1, numSteps);

	// 1ステップあたりの速度を計算
	Vector3 stepVelocity = velocity / static_cast<float>(numSteps);
	Vector3 originalVelocity = velocity;


	// 現在位置からマップチップインデックスを取得
	MapChipField::IndexSet currentIndex = mapChipField_->GetMapChipIndexSetByPosition(position);

	// 検査範囲（速度に基づいて動的に調整）
	int searchRadius = 2 + static_cast<int>(std::max(std::abs(velocity.x), std::abs(velocity.z)) / mapChipField_->GetBlockSize());

	// **現在のブロックインデックス**
	currentBlock = currentIndex;

	// オブジェクトの現在の矩形を計算
	MapChipField::Rect objectRect = {
		position.x - colliderRect.width / 2.0f + colliderRect.offsetX,
		position.x + colliderRect.width / 2.0f + colliderRect.offsetX - 0.01f,
		position.z - colliderRect.height / 2.0f + colliderRect.offsetY,
		position.z + colliderRect.height / 2.0f + colliderRect.offsetY - 0.01f
	};

	// 各ステップごとに衝突処理を実行
	for (int step = 0; step < numSteps; step++) {
		// このステップでの速度を設定
		velocity = stepVelocity * (step + 1);

		// 衝突情報のリスト
		std::vector<CollisionInfo> collisions;

		// 移動後のブロックインデックス
		nextBlock = mapChipField_->GetMapChipIndexSetByPosition(position + velocity);

		// 移動後の予測位置
		MapChipField::Rect nextObjectRect = {
			objectRect.left + velocity.x,
			objectRect.right + velocity.x,
			objectRect.bottom + velocity.z,
			objectRect.top + velocity.z
		};

		// ---【完全に抜けたかチェック】---
			/// 左方向の衝突チェック
			//if ((checkFlags & CollisionFlag::Left)) {
		isPopLeft_ = (velocity.x < 0 && 
			mapChipField_->GetMapChipIndexSetByPosition({ nextObjectRect.right, 0 ,position.z }).xIndex !=
			mapChipField_->GetMapChipIndexSetByPosition({ objectRect.right, 0 ,position.z }).xIndex); // 左に抜けた
		if (isPopLeft_) {
			popBlock = mapChipField_->GetMapChipIndexSetByPosition({ objectRect.right, 0 ,position.z });
		}
		//}

		/// 右方向の衝突チェック
		//if ((checkFlags & CollisionFlag::Right)) {
		isPopRight_ = (velocity.x > 0 && 
			mapChipField_->GetMapChipIndexSetByPosition({ nextObjectRect.left, 0 ,position.z }).xIndex !=
			mapChipField_->GetMapChipIndexSetByPosition({ objectRect.left, 0 ,position.z }).xIndex); // 右に抜けた
		if (isPopRight_) {
			popBlock = mapChipField_->GetMapChipIndexSetByPosition({ objectRect.left, 0 ,position.z });
		}
		//}

		/// 上方向の衝突チェック
		//if ((checkFlags & CollisionFlag::Top)) {
		isPopTop_ = (velocity.z > 0 && 
			mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,nextObjectRect.bottom }).yIndex !=
			mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,objectRect.bottom }).yIndex); // 上に抜けた
		if (isPopTop_) {
			popBlock = mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,objectRect.bottom });
		}
		//}

		/// 下方向の衝突チェック
		//if ((checkFlags & CollisionFlag::Bottom)) {
		isPopBottom_ = (velocity.z < 0 &&
			mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,nextObjectRect.top }).yIndex !=
			mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,objectRect.top }).yIndex);  // 下に抜けた
		if (isPopBottom_) {
			popBlock = mapChipField_->GetMapChipIndexSetByPosition({ position.x, 0 ,objectRect.top });
		}
		//}

	// 検査範囲内のブロックをチェック
		for (int dz = -searchRadius; dz <= searchRadius; dz++) {
			for (int dx = -searchRadius; dx <= searchRadius; dx++) {
				// チェック対象のインデックスを計算
				int checkX = static_cast<int>(currentIndex.xIndex) + dx;
				int checkZ = static_cast<int>(currentIndex.yIndex) + dz;

				// マップ範囲外ならスキップ
				if (checkX < 0 || checkZ < 0 ||
					checkX >= static_cast<int>(MapChipField::GetNumBlockHorizontal()) ||
					checkZ >= static_cast<int>(MapChipField::GetNumBlockVertical())) {
					continue;
				}

				uint32_t xIndex = static_cast<uint32_t>(checkX);
				uint32_t zIndex = static_cast<uint32_t>(checkZ);

				// ブロックタイプを取得
				MapChipType blockType = mapChipField_->GetMapChipTypeByIndex(xIndex, zIndex);


				// 空白ブロックならスキップ
				if (blockType == MapChipType::kBlank) {
					continue;
				}

				// ブロックの矩形を取得
				MapChipField::Rect blockRect = mapChipField_->GetRectByIndex(xIndex, zIndex);

				nextObjectRect = {
					objectRect.left + velocity.x,
					objectRect.right + velocity.x,
					objectRect.bottom + velocity.z,
					objectRect.top + velocity.z
				};

				// 矩形同士の衝突をチェック
				bool willCollideX = nextObjectRect.right > blockRect.left &&
					nextObjectRect.left < blockRect.right;
				bool willCollideZ = nextObjectRect.top > blockRect.bottom &&
					nextObjectRect.bottom < blockRect.top;

				if (willCollideX && willCollideZ) {
					// 衝突情報を作成
					CollisionInfo info;
					info.xIndex = xIndex;
					info.yIndex = zIndex;
					info.blockType = blockType;
					info.blockRect = blockRect;

					// 衝突方向と深さを計算
					// 水平方向
					float leftPenetration = nextObjectRect.right - blockRect.left;
					float rightPenetration = blockRect.right - nextObjectRect.left;

					// 垂直方向
					float topPenetration = nextObjectRect.top - blockRect.bottom;
					float bottomPenetration = blockRect.top - nextObjectRect.bottom;

					// 最小めり込み方向を探す
					float minPenetration = std::numeric_limits<float>::max();
					CollisionDirection collisionDirection{};

					/// 左方向の衝突チェック
					if ((checkFlags & CollisionFlag::Left) && velocity.x > 0 && leftPenetration < minPenetration) {
						minPenetration = leftPenetration;
						collisionDirection = CollisionDirection::LeftDir;
					}

					/// 右方向の衝突チェック
					if ((checkFlags & CollisionFlag::Right) && velocity.x < 0 && rightPenetration < minPenetration) {
						minPenetration = rightPenetration;
						collisionDirection = CollisionDirection::RightDir;
					}

					/// 上方向の衝突チェック
					if ((checkFlags & CollisionFlag::Top) && velocity.z > 0 && topPenetration < minPenetration) {
						minPenetration = topPenetration;
						collisionDirection = CollisionDirection::TopDir;
					}

					/// 下方向の衝突チェック
					if ((checkFlags & CollisionFlag::Bottom) && velocity.z < 0 && bottomPenetration < minPenetration) {
						minPenetration = bottomPenetration;
						collisionDirection = CollisionDirection::BottomDir;
					}

					// 衝突情報を設定
					info.direction = collisionDirection;
					info.penetrationDepth = minPenetration;

					// 衝突リストに追加
					collisions.push_back(info);
				}
			}
		}

		// 衝突が検出されなければ次のステップへ
		if (collisions.empty()) {
			// 位置を更新して次のステップへ
			position.x += velocity.x;
			position.z += velocity.z;
			continue;
		}

		// 衝突解決（最も浅い衝突から順に処理）
		std::sort(collisions.begin(), collisions.end(),
			[](const CollisionInfo& a, const CollisionInfo& b) {
				return a.penetrationDepth < b.penetrationDepth;
			});

		// 各衝突を処理
		for (const auto& collision : collisions) {
			// 衝突方向に応じた処理
			switch (collision.direction) {
			case CollisionDirection::LeftDir:
				position.x = collision.blockRect.left - colliderRect.width / 2.0f - colliderRect.offsetX;
				velocity.x = 0;
				isCollision_ = true;
				break;

			case CollisionDirection::RightDir:
				position.x = collision.blockRect.right + colliderRect.width / 2.0f - colliderRect.offsetX;
				velocity.x = 0;
				isCollision_ = true;
				break;

			case CollisionDirection::TopDir:
				position.z = collision.blockRect.bottom - colliderRect.height / 2.0f - colliderRect.offsetY;
				velocity.z = 0;
				isCollision_ = true;
				break;

			case CollisionDirection::BottomDir:
				position.z = collision.blockRect.top + colliderRect.height / 2.0f - colliderRect.offsetY;
				velocity.z = 0;
				isCollision_ = true;
				break;
			}

			// コールバック関数が設定されていれば呼び出す
			if (collisionCallback) {
				collisionCallback(collision);
			}
		}
	}

	// 最終的な速度を設定（すべてのサブステップ後）
	if (velocity.x == 0 || velocity.z == 0) {
		// 衝突によって速度がゼロになった方向の速度を維持
		if (velocity.x == 0) {
			originalVelocity.x = 0;
		}
		if (velocity.z == 0) {
			originalVelocity.z = 0;
		}
		velocity = originalVelocity;
	}
}