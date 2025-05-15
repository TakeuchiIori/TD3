#include "Easing.h"


float Easing::ease(Function func, float x) {
    switch (func) {
    case Function::Linear: return linear(x);

    case Function::EaseInSine: return easeInSine(x);
    case Function::EaseOutSine: return easeOutSine(x);
    case Function::EaseInOutSine: return easeInOutSine(x);

    case Function::EaseInQuad: return easeInQuad(x);
    case Function::EaseOutQuad: return easeOutQuad(x);
    case Function::EaseInOutQuad: return easeInOutQuad(x);

    case Function::EaseInCubic: return easeInCubic(x);
    case Function::EaseOutCubic: return easeOutCubic(x);
    case Function::EaseInOutCubic: return easeInOutCubic(x);

    case Function::EaseInQuart: return easeInQuart(x);
    case Function::EaseOutQuart: return easeOutQuart(x);
    case Function::EaseInOutQuart: return easeInOutQuart(x);

    case Function::EaseInQuint: return easeInQuint(x);
    case Function::EaseOutQuint: return easeOutQuint(x);
    case Function::EaseInOutQuint: return easeInOutQuint(x);

    case Function::EaseInExpo: return easeInExpo(x);
    case Function::EaseOutExpo: return easeOutExpo(x);
    case Function::EaseInOutExpo: return easeInOutExpo(x);

    case Function::EaseInCirc: return easeInCirc(x);
    case Function::EaseOutCirc: return easeOutCirc(x);
    case Function::EaseInOutCirc: return easeInOutCirc(x);

    case Function::EaseInBack: return easeInBack(x);
    case Function::EaseOutBack: return easeOutBack(x);
    case Function::EaseInOutBack: return easeInOutBack(x);

    case Function::EaseInElastic: return easeInElastic(x);
    case Function::EaseOutElastic: return easeOutElastic(x);
    case Function::EaseInOutElastic: return easeInOutElastic(x);

    case Function::EaseInBounce: return easeInBounce(x);
    case Function::EaseOutBounce: return easeOutBounce(x);
    case Function::EaseInOutBounce: return easeInOutBounce(x);

    case Function::EaseOutGrowBounce: return easeOutGrowBounce(x);

    default: return x; // デフォルトは線形
    }
}

// 文字列から列挙型への変換
Easing::Function Easing::functionFromString(const std::string& name) {
    static const std::unordered_map<std::string, Function> functionMap = {
        {"linear", Function::Linear},

        {"easeInSine", Function::EaseInSine},
        {"easeOutSine", Function::EaseOutSine},
        {"easeInOutSine", Function::EaseInOutSine},

        {"easeInQuad", Function::EaseInQuad},
        {"easeOutQuad", Function::EaseOutQuad},
        {"easeInOutQuad", Function::EaseInOutQuad},

        {"easeInCubic", Function::EaseInCubic},
        {"easeOutCubic", Function::EaseOutCubic},
        {"easeInOutCubic", Function::EaseInOutCubic},

        {"easeInQuart", Function::EaseInQuart},
        {"easeOutQuart", Function::EaseOutQuart},
        {"easeInOutQuart", Function::EaseInOutQuart},

        {"easeInQuint", Function::EaseInQuint},
        {"easeOutQuint", Function::EaseOutQuint},
        {"easeInOutQuint", Function::EaseInOutQuint},

        {"easeInExpo", Function::EaseInExpo},
        {"easeOutExpo", Function::EaseOutExpo},
        {"easeInOutExpo", Function::EaseInOutExpo},

        {"easeInCirc", Function::EaseInCirc},
        {"easeOutCirc", Function::EaseOutCirc},
        {"easeInOutCirc", Function::EaseInOutCirc},

        {"easeInBack", Function::EaseInBack},
        {"easeOutBack", Function::EaseOutBack},
        {"easeInOutBack", Function::EaseInOutBack},

        {"easeInElastic", Function::EaseInElastic},
        {"easeOutElastic", Function::EaseOutElastic},
        {"easeInOutElastic", Function::EaseInOutElastic},

        {"easeInBounce", Function::EaseInBounce},
        {"easeOutBounce", Function::EaseOutBounce},
        {"easeInOutBounce", Function::EaseInOutBounce},

	    {"easeOutGrowBounce", Function::EaseOutGrowBounce}


    };

    auto it = functionMap.find(name);
    return it != functionMap.end() ? it->second : Function::Linear;
}

float Easing::linear(float x) {
    return x;
}

float Easing::easeInSine(float x) {
    return 1.0f - cos((x * PI) / 2.0f);
}

float Easing::easeOutSine(float x) {
    return sin((x * PI) / 2.0f);
}

float Easing::easeInOutSine(float x) {
    return -(cos(PI * x) - 1.0f) / 2.0f;
}

float Easing::easeInQuad(float x) {
    return x * x;
}

float Easing::easeOutQuad(float x) {
    return 1.0f - (1.0f - x) * (1.0f - x);
}

float Easing::easeInOutQuad(float x) {
    return x < 0.5f ? 2.0f * x * x : 1.0f - pow(-2.0f * x + 2.0f, 2.0f) / 2.0f;
}

float Easing::easeInCubic(float x) {
    return x * x * x;
}

float Easing::easeOutCubic(float x) {
    return 1.0f - pow(1.0f - x, 3.0f);
}

float Easing::easeInOutCubic(float x) {
    return x < 0.5f ? 4.0f * x * x * x : 1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
}

float Easing::easeInQuart(float x) {
    return x * x * x * x;
}

float Easing::easeOutQuart(float x) {
    return 1.0f - pow(1.0f - x, 4.0f);
}

float Easing::easeInOutQuart(float x) {
    return x < 0.5f ? 8.0f * x * x * x * x : 1.0f - pow(-2.0f * x + 2.0f, 4.0f) / 2.0f;
}

float Easing::easeInQuint(float x) {
    return x * x * x * x * x;
}

float Easing::easeOutQuint(float x) {
    return 1.0f - pow(1.0f - x, 5.0f);
}

float Easing::easeInOutQuint(float x) {
    return x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - pow(-2.0f * x + 2.0f, 5.0f) / 2.0f;
}

float Easing::easeInExpo(float x) {
    return x == 0.0f ? 0.0f : pow(2.0f, 10.0f * x - 10.0f);
}

float Easing::easeOutExpo(float x) {
    return x == 1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * x);
}

float Easing::easeInOutExpo(float x) {
    return x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f
        ? pow(2.0f, 20.0f * x - 10.0f) / 2.0f
        : (2.0f - pow(2.0f, -20.0f * x + 10.0f)) / 2.0f;
}

float Easing::easeInCirc(float x) {
    return 1.0f - sqrt(1.0f - pow(x, 2.0f));
}

float Easing::easeOutCirc(float x) {
    return sqrt(1.0f - pow(x - 1.0f, 2.0f));
}

float Easing::easeInOutCirc(float x) {
    return x < 0.5f
        ? (1.0f - sqrt(1.0f - pow(2.0f * x, 2.0f))) / 2.0f
        : (sqrt(1.0f - pow(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

float Easing::easeInBack(float x) {
    return c3 * x * x * x - c1 * x * x;
}

float Easing::easeOutBack(float x) {
    return 1.0f + c3 * pow(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);
}

float Easing::easeInOutBack(float x) {
    return x < 0.5f
        ? (pow(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
        : (pow(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

float Easing::easeInElastic(float x) {
    if (x == 0.0f) return 0.0f;
    if (x == 1.0f) return 1.0f;
    return -pow(2.0f, 10.0f * x - 10.0f) * sin((x * 10.0f - 10.75f) * c4);
}

float Easing::easeOutElastic(float x) {
    if (x == 0.0f) return 0.0f;
    if (x == 1.0f) return 1.0f;
    return pow(2.0f, -10.0f * x) * sin((x * 10.0f - 0.75f) * c4) + 1.0f;
}

float Easing::easeInOutElastic(float x) {
    if (x == 0.0f) return 0.0f;
    if (x == 1.0f) return 1.0f;
    return x < 0.5f
        ? -(pow(2.0f, 20.0f * x - 10.0f) * sin((20.0f * x - 11.125f) * c5)) / 2.0f
        : (pow(2.0f, -20.0f * x + 10.0f) * sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
}

float Easing::easeInBounce(float x) {
    return 1.0f - easeOutBounce(1.0f - x);
}

float Easing::easeOutBounce(float x) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (x < 1.0f / d1) {
        return n1 * x * x;
    } else if (x < 2.0f / d1) {
        x -= 1.5f / d1;
        return n1 * x * x + 0.75f;
    } else if (x < 2.5f / d1) {
        x -= 2.25f / d1;
        return n1 * x * x + 0.9375f;
    } else {
        x -= 2.625f / d1;
        return n1 * x * x + 0.984375f;
    }
}

float Easing::easeInOutBounce(float x) {
    return x < 0.5f
        ? (1.0f - easeOutBounce(1.0f - 2.0f * x)) / 2.0f
        : (1.0f + easeOutBounce(2.0f * x - 1.0f)) / 2.0f;
}

float Easing::easeOutGrowBounce(float t)
{
    // クランプして安全に（念のため）
    t = std::clamp(t, 0.0f, 1.0f);

    if (t < 0.2f) {
        // 前半：急激に成長（1.6倍まで）
        float f = t / 0.2f;
        float grow = (1.0f - powf(1.0f - f, 2.0f)) * 0.6f; // 0〜0.6の範囲で増加
        return 1.0f + grow;
    } else {
        // 後半：バウンドしながら1.0に収束
        float f = (t - 0.2f) / 0.8f; // 0〜1
        float damping = powf(1.0f - f, 2.0f); // 減衰（指数は安定的な2.0）
        float bounce = sinf(f * 8.0f * PI) * damping * 0.3f; // 周期と振幅を調整
        return 1.0f + bounce;
    }
}