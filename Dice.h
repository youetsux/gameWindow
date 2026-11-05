#pragma once
#include "DiceFace.h"

class Dice
{
public:
    void Initialize();
    void Draw(XMMATRIX& worldMatrix);
    void Release();

private:
    DiceFace faces_[6]; // コンポジション：6枚の面を部品として持つ
};
