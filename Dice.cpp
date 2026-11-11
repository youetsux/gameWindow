#include "Dice.h"

void Dice::Initialize()
{
    // [面番号][頂点番号]。各行が1面の左上・右上・右下・左下。
    const XMFLOAT3 positions[6][4] = {
        { // 手前：z = -1
            { -1.0f,  1.0f, -1.0f },
            {  1.0f,  1.0f, -1.0f },
            {  1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f }
        },
        { // 奥：z = +1
            {  1.0f,  1.0f,  1.0f },
            { -1.0f,  1.0f,  1.0f },
            { -1.0f, -1.0f,  1.0f },
            {  1.0f, -1.0f,  1.0f }
        },
        { // 右：x = +1
            {  1.0f,  1.0f, -1.0f },
            {  1.0f,  1.0f,  1.0f },
            {  1.0f, -1.0f,  1.0f },
            {  1.0f, -1.0f, -1.0f }
        },
        { // 左：x = -1
            { -1.0f,  1.0f,  1.0f },
            { -1.0f,  1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f,  1.0f }
        },
        { // 上：y = +1
            { -1.0f,  1.0f,  1.0f },
            {  1.0f,  1.0f,  1.0f },
            {  1.0f,  1.0f, -1.0f },
            { -1.0f,  1.0f, -1.0f }
        },
        { // 下：y = -1
            { -1.0f, -1.0f, -1.0f },
            {  1.0f, -1.0f, -1.0f },
            {  1.0f, -1.0f,  1.0f },
            { -1.0f, -1.0f,  1.0f }
        }
    };

    const XMFLOAT3 normals[6] = {
        {  0.0f,  0.0f, -1.0f }, // 手前
        {  0.0f,  0.0f,  1.0f }, // 奥
        {  1.0f,  0.0f,  0.0f }, // 右
        { -1.0f,  0.0f,  0.0f }, // 左
        {  0.0f,  1.0f,  0.0f }, // 上
        {  0.0f, -1.0f,  0.0f }  // 下
    };

    const int numbers[6] = { 1, 6, 3, 4, 2, 5 };

    for (int i = 0; i < 6; ++i)
    {
        // DiceFace で追加した機能を使う。
        faces_[i].SetPositions(positions[i]);
        faces_[i].SetNormal(
            normals[i].x, normals[i].y, normals[i].z
        );

        // 1～6の目を、画像内の列番号と行番号へ変換する。
        const int cell = numbers[i] - 1;
        const int column = cell % 3;
        const int row = cell / 3;

        const float left   = column / 3.0f;
        const float top    = row / 2.0f;
        const float right  = (column + 1) / 3.0f;
        const float bottom = (row + 1) / 2.0f;

        faces_[i].SetUV(left, top, right, bottom);

        // Quad から継承した機能を使う。
        // 頂点の設定を済ませてから GPU に送る。
        faces_[i].Initialize("sainome.png");
    }
}

void Dice::Draw(Transform& transform)
{
    for (int i = 0; i < 6; ++i)
    {
        faces_[i].Draw(transform);
    }
}

void Dice::Release()
{
    for (int i = 0; i < 6; ++i)
    {
        faces_[i].Release();
    }
}
