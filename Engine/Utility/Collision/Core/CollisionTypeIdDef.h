#pragma once
// C++
#include <cstdint>
// コリジョン種別IDを定義
enum class CollisionTypeIdDef : uint32_t
{
	kDefault = 0,	// デフォルト
	kPlayer,		// プレイヤー
	kNextFramePlayer,
	kPlayerBody,	// プレイヤー体
	kStuckGrass,	// 草が詰まった場所
	kGrass,			// 草
	kGrowthArea,	// 草の成長範囲
	kBranch,		// 枝
	kEnemy,			// 敵
	kBook,			// タイトルシーンの本
	kNone			// 当たり判定なし
};