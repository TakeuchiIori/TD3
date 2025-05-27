#pragma once

// Engine
#include "Systems/Input/Input.h"
#include "PlayerMapCollision.h"
#include "Systems/MapChip/MapChipCollision.h"
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Loaders/Json/JsonManager.h"
#include "Particle/ParticleEmitter.h"
#include "Sprite/Sprite.h"

// Collision
#include "Collision/Sphere/SphereCollider.h"
#include "Collision/OBB/OBBCollider.h"
#include "Collision/AABB/AABBCollider.h"
#include "Collision/Core/ColliderFactory.h"
#include "Collision/Core/CollisionDirection.h"

// Application
#include "BaseObject/BaseObject.h"
#include "PlayerBody.h"
#include "StuckGrass.h"
#include "PlayerHaert.h"

#include "Systems/Audio/Audio.h"

enum class BehaviorPlayer
{
	Root,
	Moving,
	Boost,
	Return,
};

struct PointWithDirection {
	Vector3 position;
	float radian; // XY平面での向き（進行方向ベクトルの角度）
};

class Player 
	: public BaseObject
{
public:
	~Player() override;

	Player(MapChipField* mapChipField)
		: velocity_(0, 0, 0),
		mpCollision_(mapChipField) {
		// プレイヤーの衝突判定用矩形を設定
		colliderRect_ = { 2.0f, 2.0f, 0.0f, 0.0f };
		worldTransform_.translation_ = { 0.0f, 0.0f, 0.0f };
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera) override;
	void InitCollision();
	void InitJson();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	void DrawAnimation();

	void DrawCollision();

	void DrawSprite();

	void MapChipOnCollision(const CollisionInfo& info);


	void Reset();

public: // 衝突判定用
	Vector3 GetCenterPosition() const { 
		return
		{
			worldTransform_.matWorld_.m[3][0],
			worldTransform_.matWorld_.m[3][1],
			worldTransform_.matWorld_.m[3][2]
		};
	}
	//virtual Vector3 GetEulerRotation() override { return{}; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

	// 衝突イベント（共通で受け取る）
	void OnEnterCollision(BaseCollider* self, BaseCollider* other);
	void OnCollision(BaseCollider* self, BaseCollider* other);
	void OnExitCollision(BaseCollider* self, BaseCollider* other);
	void OnDirectionCollision(BaseCollider* self, BaseCollider* other, HitDirection dir);



private:
	/// 全行列の転送
	void UpdateMatrices();

	// 移動
	void Move();

	void ChangeDir();

	void ChangeDirRoot();

	void UpBody();
	void DownBody();
	void LeftBody();
	void RightBody();

	// 移動へ移行
	void EntryMove();

	// ブーストへ移行
	void EntryBoost();

	// 帰還へ移行
	void EntryReturn();


	void TimerManager();
	void TimerZero();

	void UpdateSprite();

	void ExtendBody();

	void ShrinkBody();

	void TakeDamage();

	void DamageProcessBodys();

	void GrassGaugeUpdate();

	void Eliminate(); // 敵を倒した時

	void HeartPos();

	void AddCombo(int amount);

	void HeadDir() 
	{
		worldTransform_.rotation_.z = 0;
		if (moveHistory_.size() > 1)
		{

			// 進行方向（XY平面）からラジアン角を計算
			Vector3 dir = Normalize(worldTransform_.translation_ - moveHistory_.back()); // 方向ベクトル（単位ベクトル）
			float angle = std::atan2(dir.y, dir.x);  // XY平面での角度
			worldTransform_.rotation_.z = angle + 3.0f * std::numbers::pi_v<float> / 2.0f;
			nextWorldTransform_.rotation_ = worldTransform_.rotation_;
		}

	}

	void UpdateCombo();


#ifdef _DEBUG
	// デバッグ用 (ImGuiとか)
	void DebugPlayer();
#endif // _DEBUG


private: // プレイヤーのふるまい
	// ふるまい全体の初期化
	void BehaviorInitialize();
	// ふるまい全体の更新
	void BehaviorUpdate();


	// 停止状態初期化
	void BehaviorRootInit();
	// 停止状態更新
	void BehaviorRootUpdate();


	// 移動状態初期化
	void BehaviorMovingInit();
	// 移動状態更新
	void BehaviorMovingUpdate();


	// 加速状態初期化
	void BehaviorBoostInit();
	// 加速状態更新
	void BehaviorBoostUpdate();


	// 帰還状態初期化
	void BehaviorReturnInit();
	// 帰還状態更新
	void BehaviorReturnUpdate();


public: // getter&setter

	void SetMapInfo(MapChipField* mapChipField) { 
		//mapCollision_.SetMap(mapChipField);
		//mapCollision_.Init(colliderRct_, worldTransform_.translation_);
	}

	bool IsBoost() { return behavior_ == BehaviorPlayer::Boost; }

	bool EndReturn() { return behaviortRquest_ == BehaviorPlayer::Root; }

	bool IsPopGrass();

	// タイムリミットを取得
	float GetTimeLimit() { return extendTimer_ > 0 ? extendTimer_ : 0; }
	// 最大HPの取得
	int32_t GetMaxHP() { return kMaxHP_; }	
	// 現在のHPの取得
	int32_t GetHP() { return HP_; }				
	// 唾を吐けるか
	bool CanSpitting() { return canSpitting_; }	
	// 草ゲージのUIを表示するための値
	float GetUIGrassGauge() { return UIGauge_; }

	Vector3 GetColor() { return changeColor_; }

	float GetUIBoostGauge() { 
		if (behavior_ == BehaviorPlayer::Boost)
		{
			return 0.0f;
		}
		return 1.0f - boostCoolTimer_ / kBoostCT_;
	}

	void SetTimeLimit(float time) { kTimeLimit_ = time; }

	bool IsAddTime()
	{
		if (isAddTime_)
		{
			isAddTime_ = false;
			return true;
		}
		return false;
	}

	float GetAddtime() { return addTime_; }

private:
	Input* input_ = nullptr;
	
	std::unique_ptr<ParticleEmitter> emitter_;

	std::unique_ptr<JsonManager> jsonManager_;
	std::unique_ptr<JsonManager> jsonCollider_;

	std::shared_ptr<OBBCollider> obbCollider_;
	std::shared_ptr<AABBCollider> nextAabbCollider_;
	WorldTransform nextWorldTransform_;
	WorldTransform modelWT_;
	
	MapChipCollision mpCollision_;
	MapChipCollision::ColliderRect colliderRect_;

	WorldTransform legWT_;
	std::shared_ptr<Object3d> legObj_;

	bool isCollisionBody = false;

	bool isRed_ = false;
	bool isAnimation_ = false;


	const Vector4 defaultColorV4_ = { 0.90625f,0.87109f,0.125f,1.0f };
	const Vector3 defaultColorV3_ = { 0.90625f,0.87109f,0.125f };
	Vector3 changeColor_ = {};


	// 移動
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };			// 加速度
	Vector3 moveDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き(nowFrame)
	Vector3 beforeDirection_ = { 0.0f,0.0f,0.0f };	// 動く向き(beforeFrame)
	float defaultSpeed_ = 0.05f;					// 通常時の移動速度
	float boostSpeed_ = 0.2f;						// ブースト時の速度
	float returnSpeed_ = 1.0f;						// 帰還時の速度
	float speed_ = defaultSpeed_;					// 動く速度

	float moveInterval_ = 2.1f;

	bool isMove_ = false;

	static bool isHit;

	// 移動履歴
	std::list<Vector3> moveHistory_;
	std::list<std::unique_ptr<PlayerBody>> playerBodys_;
	std::list<std::unique_ptr<StuckGrass>> stuckGrassList_;


	// 草ゲージ
	int32_t kMaxGrassGauge_ = 2;			// 草ゲージ最大値
	int32_t grassGauge_ = 0;				// 草ゲージ
	float UIGauge_ = 0.0f;					// 草ゲージのUIに渡すための値

	//Haert
	std::vector<std::unique_ptr<PlayerHaert>> haerts_;
	int drawCount_ = 0;

	// 時間制限 : 単位(sec)
	float kTimeLimit_ = 10.0f;				// タイマーの限界値
	float extendTimer_ = 0;					// 伸びられる残り時間
	float grassTime_ = 1.0f;				// 草を食べて追加される時間
	float largeGrassTime_ = 3.0f;			// 大きい草

	float kBoostTime_ = 1.0f;				// ブーストの最大効果時間
	float boostTimer_ = 0;					// 現在のブーストの残り時間

	float kBoostCT_ = 5.0f;					// ブーストのクールタイム
	float boostCoolTimer_ = 0;				// 現在のクールタイムトの残り時間


	float kCreateGrassTime_ = 2.5f;			// 草が詰まるまでの時間
	float createGrassTimer_ = 0.0f;			// 草が詰まるまでのタイマー
	bool isCreateGrass_ = false;			// 草を吐いたか

	bool canSpitting_ = false;				// 唾を吐けるか

	float kInvincibleTime_ = 2.0f;			// 無敵時間
	float invincibleTimer_ = 0.0f;			// 無敵タイマー


	const float deltaTime_ = 1.0f / 60.0f;	// 仮対応

	float addTime_ = 0;
	bool isAddTime_ = false;


	// コンボ用
	int comboCount_ = 0;                   // 現在のコンボ数
	int kMaxCombo_ = 3;                    // 最大コンボ数
	float comboTimer_ = 0.0f;              // コンボ残り時間
	float kComboTimeLimit_ = 1.5f;         // コンボ持続時間（秒）
	int lastPlayedComboCount_ = 0;
	uint32_t eatingComboId_ = 0;


	// ヒットポイント
	int32_t kMaxHP_ = 3;
	int32_t HP_ = kMaxHP_;

	//PlayerMapCollision mapCollision_;
	//MapChipCollision::ColliderRect colliderRct_;
	//MapChipCollision::CollisionFlag collisionFlag_ = MapChipCollision::CollisionFlag::None;

	// コントローラー用
	Vector2 stick = {};
	float threshold = 0.5f;


	// サウンド
	Audio::SoundData soundDataGrow = {};
	IXAudio2SourceVoice* sourceVoiceGrow = nullptr;

	Audio::SoundData soundDataBoost = {};
	IXAudio2SourceVoice* sourceVoiceBoost = nullptr;

	Audio::SoundData soundDataDamage = {};
	IXAudio2SourceVoice* sourceVoiceDamage = nullptr;

	Audio::SoundData soundDataEat[3] = {};
	IXAudio2SourceVoice* sourceVoiceEat[3];

	Audio::SoundData soundDataYodare = {};
	IXAudio2SourceVoice* sourceVoiceYodare = nullptr;


	float time_ = 0;
	float rootTimer_ = 0;
	float kShowRootTime_ = 10.0f;
	std::unique_ptr<Sprite> uiA_;
	Vector3 offsetUI_ = {};
	bool showUI_ = true;

public:
	// 振る舞い
	BehaviorPlayer behavior_ = BehaviorPlayer::Root;
	BehaviorPlayer beforebehavior_ = behavior_;
	// 次の振る舞いリクエスト
	std::optional<BehaviorPlayer> behaviortRquest_ = std::nullopt;
};

