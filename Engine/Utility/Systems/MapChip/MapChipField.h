#pragma once

// C++
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <optional>
#include <stdexcept>
#include <iostream>

// Math
#include "Vector3.h"

enum class MapChipType {
    kBlank = 0,
    kBlock = 1,
    kDropEnemy = 2,
    kSideEnemy = 3,
    kBody = 4,
    kFloor = 5,
    kCeiling = 6,
    kSoil = 7,
};


class MapChipField {
public: // 構造体
    struct MapChipData {
        // 二次元vector
        std::vector<std::vector<MapChipType>> data;
    };

    struct Rect {
        float left = 0.0f;
        float right = 1.0f;
        float bottom = 0.0f;
        float top = 1.0f;
    };

    struct IndexSet {
        uint32_t xIndex;
        uint32_t yIndex;
    };

public:
    MapChipField();

    /// <summary>
    /// リセット
    /// </summary>
    void ResetMapChipData();

    /// <summary>
    /// ファイル読み込み
    /// </summary>
    /// <param name="filePath">CSVファイルのパス</param>
    /// <returns>読み込みに成功したかどうか</returns>
    void LoadMapChipCsv(const std::string& filePath);

    /// <summary>
    /// マップチップの種別を取得
    /// </summary>
    /// <returns>指定位置のマップチップタイプ</returns>
    MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) const;

    /// <summary>
    /// マップチップの種別を取得
    /// </summary>
    /// <returns>指定位置のマップチップタイプ</returns>
    void SetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, MapChipType type);

    /// <summary>
    /// 座標を取得
    /// </summary>
    /// <returns>マップチップの世界座標</returns>
    static Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

    /// <summary>
    /// 指定座標がマップチップの何番の位置にあるか探す
    /// </summary>
    /// <returns>マップチップのインデックス</returns>
    IndexSet GetMapChipIndexSetByPosition(const Vector3& position) const;

    /// <summary>
    /// マップチップ番号を指定して指定ブロックの全方向の境界の座標を得る
    /// </summary>
    /// <returns>ブロックの境界座標</returns>
    Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex) const;

    /// <summary>
    /// マップチップタイプの登録
    /// </summary>
    /// <param name="key">CSVファイル内の文字列キー</param>
    /// <param name="type">対応するマップチップタイプ</param>
    void RegisterMapChipType(const std::string& key, MapChipType type);

    float GetBlockSize() const { return blockSize; }

private:
    
    MapChipData mapChipData_;
    std::map<std::string, MapChipType> mapChipTable_;

    // 1ブロックのサイズ(2で固定)
    static inline const float kBlockWidth = 2.0f;
    static inline const float kBlockHeight = 2.0f;

    static inline const float blockSize = 2.0f;

    // ブロックの個数
    static inline uint32_t kNumBlockVertical = 154;      // 縦
    static inline uint32_t kNumBlockHorizontal = 18;   // 横  

public:

    // 1ブロックのサイズ
    static inline float GetBlockWidth() { return kBlockWidth; }
    static inline float GetBlockHeight() { return kBlockHeight; }

    // 縦
	static inline void SetNumBlockVertical(uint32_t num) { kNumBlockVertical = num; }
	// 横
	static inline void SetNumBlockHorizontal(uint32_t num) { kNumBlockHorizontal = num; }

    // ブロックの個数
    static inline uint32_t GetNumBlockVertical() { return kNumBlockVertical; }
    static inline uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
    void SaveMapChipCsv(const std::string& filePath) const;
};
