#pragma once
#include "DiceFace.h"

//クラスのコンポジット（合成）を使って、サイコロを表現する。
class Dice
{
public:
    void Initialize();
    void Draw(XMMATRIX& worldMatrix);
    void Release();

private:
    DiceFace faces_[6]; // コンポジション：6枚の面を部品として持つ
};
