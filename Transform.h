#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
    XMMATRIX matTranslate_;
    XMMATRIX matRotate_;
    XMMATRIX matScale_;

public:
    XMFLOAT3 position_; // 位置
    XMFLOAT3 rotate_;   // 各軸の回転角度（度）
    XMFLOAT3 scale_;    // 各軸の拡大率

    Transform();
    ~Transform();
    void Calculation();
    XMMATRIX GetWorldMatrix();
};
