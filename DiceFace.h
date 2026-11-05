#pragma once
#include "Quad.h"

class DiceFace : public Quad
{
public:
    // 頂点の順番は Quad と同じ：左上、右上、右下、左下。
    // 各面を外側から見た順番で指定する。
    void SetPositions(const XMFLOAT3 (&positions)[4]);

    // テクスチャから切り出す長方形の範囲を指定する。
    void SetUV(float left, float top, float right, float bottom);

    // 4頂点に共通の法線を設定する。長さ1のベクトルを渡す。
    void SetNormal(float x, float y, float z);
};
