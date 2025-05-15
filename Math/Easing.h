#ifndef EASING_H
#define EASING_H

#include <string>
#include <unordered_map>
#include <numbers>
#include <algorithm>

class Easing {
public:
    enum class Function {
        Linear,

        EaseInSine,     EaseOutSine,     EaseInOutSine,
        EaseInQuad,     EaseOutQuad,     EaseInOutQuad,
        EaseInCubic,    EaseOutCubic,    EaseInOutCubic,
        EaseInQuart,    EaseOutQuart,    EaseInOutQuart,
        EaseInQuint,    EaseOutQuint,    EaseInOutQuint,
        EaseInExpo,     EaseOutExpo,     EaseInOutExpo,
        EaseInCirc,     EaseOutCirc,     EaseInOutCirc,
        EaseInBack,     EaseOutBack,     EaseInOutBack,
        EaseInElastic,  EaseOutElastic,  EaseInOutElastic,
        EaseInBounce,   EaseOutBounce,   EaseInOutBounce,
        EaseOutGrowBounce
    };

    // 指定されたイージング関数で値を計算
    static float ease(Function func, float x);

    // 文字列から列挙型への変換
    static Function functionFromString(const std::string& name);

    // 個別の関数を直接利用する場合のAPI
    static float linear(float x);

    static float easeInSine(float x);
    static float easeOutSine(float x);
    static float easeInOutSine(float x);

    static float easeInQuad(float x);
    static float easeOutQuad(float x);
    static float easeInOutQuad(float x);

    static float easeInCubic(float x);
    static float easeOutCubic(float x);
    static float easeInOutCubic(float x);

    static float easeInQuart(float x);
    static float easeOutQuart(float x);
    static float easeInOutQuart(float x);

    static float easeInQuint(float x);
    static float easeOutQuint(float x);
    static float easeInOutQuint(float x);

    static float easeInExpo(float x);
    static float easeOutExpo(float x);
    static float easeInOutExpo(float x);

    static float easeInCirc(float x);
    static float easeOutCirc(float x);
    static float easeInOutCirc(float x);

    static float easeInBack(float x);
    static float easeOutBack(float x);
    static float easeInOutBack(float x);

    static float easeInElastic(float x);
    static float easeOutElastic(float x);
    static float easeInOutElastic(float x);

    static float easeInBounce(float x);
    static float easeOutBounce(float x);
    static float easeInOutBounce(float x);


    //  徐々に大きくなり、最後にバウンドするイージング
    static float easeOutGrowBounce(float t);

private:
	static constexpr float PI = std::numbers::pi_v<float>;
    static constexpr float c1 = 1.70158f;
    static constexpr float c2 = c1 * 1.525f;
    static constexpr float c3 = c1 + 1.0f;
    static constexpr float c4 = (2.0f * PI) / 3.0f;
    static constexpr float c5 = (2.0f * PI) / 4.5f;
};

#endif // EASING_H