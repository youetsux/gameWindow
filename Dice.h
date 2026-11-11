#pragma once
#include "Transform.h"
#include "DiceFace.h"

//クラスのコンポジット（合成）を使って、サイコロを表現する。
class Dice
{
public:
    void Initialize();
    void Draw(Transform& transform);
    void Release();

private:
    DiceFace faces_[6]; // コンポジション：6枚の面を部品として持つ
};
