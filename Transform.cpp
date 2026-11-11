#include "Transform.h"

Transform::Transform()
    : matTranslate_(XMMatrixIdentity()),
      matRotate_(XMMatrixIdentity()),
      matScale_(XMMatrixIdentity()),
      position_(0.0f, 0.0f, 0.0f),
      rotate_(0.0f, 0.0f, 0.0f),
      scale_(1.0f, 1.0f, 1.0f)
{
}

Transform::~Transform()
{
}

void Transform::Calculation()
{
    matTranslate_ = XMMatrixTranslation(
        position_.x, position_.y, position_.z);

    matRotate_ = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(rotate_.x),
        XMConvertToRadians(rotate_.y),
        XMConvertToRadians(rotate_.z));

    matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
}

XMMATRIX Transform::GetWorldMatrix()
{
    // 拡大縮小 → 回転 → 移動。転置はDraw側で行う。
    return matScale_ * matRotate_ * matTranslate_;
}
