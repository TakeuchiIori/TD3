#pragma once
#include "Systems/MapChip/MapChipCollision.h"
class PlayerMapCollision :
    public MapChipCollision
{
public:
    void Init(const ColliderRect& colliderRect, Vector3& position);

    // 指定した位置と速度をもとに衝突判定と解決を行う
    // colliderRect: 衝突判定に使用する矩形
    // position: 現在位置（参照渡しで修正可能）
    // velocity: 現在速度（参照渡しで修正可能）
    // checkFlags: チェックする方向のフラグ
    // collisionCallback: 衝突時に呼び出されるコールバック関数
    void DetectAndResolveCollision(
        const ColliderRect& colliderRect,
        Vector3& position,
        Vector3& velocity,
        int checkFlags = CollisionFlag::All,
        std::function<void(const CollisionInfo&)> collisionCallback = nullptr) override;

    bool GetIsCollision() { return isCollision_; }

    bool GetIsCollisionBody() { return isCollisionBody_; }

    bool GetIsPopBody() { return (isPopLeft_ || isPopRight_ || isPopTop_ || isPopBottom_); }

    Vector3 GetPopPos() { 
        mapChipField_->SetMapChipTypeByIndex(popBlock.xIndex, popBlock.yIndex, MapChipType::kBody);
        return MapChipField::GetMapChipPositionByIndex(popBlock.xIndex, popBlock.yIndex);
    }

    void ElasePos(Vector3 pos) {
        MapChipField::IndexSet elaseBlock = mapChipField_->GetMapChipIndexSetByPosition(pos);
        mapChipField_->SetMapChipTypeByIndex(elaseBlock.xIndex, elaseBlock.yIndex, MapChipType::kBlank);
    }

private:
    bool isCollision_ = false;

    bool isCollisionBody_ = false;

    bool isPopLeft_ = false;
    bool isPopRight_ = false;
    bool isPopTop_ = false;
    bool isPopBottom_ = false;

    MapChipField::IndexSet popBlock;

    MapChipField::Rect currentBlock;

    MapChipField::Rect prevBlock;
};

