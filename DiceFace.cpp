#include "DiceFace.h"

void DiceFace::SetPositions(const XMFLOAT3 (&positions)[4])
{
    for (int i = 0; i < 4; ++i)
    {
        vertices[i].pos = XMVectorSet(
            positions[i].x,
            positions[i].y,
            positions[i].z,
            1.0f
        );
    }
}

void DiceFace::SetUV(float left, float top, float right, float bottom)
{
    vertices[0].uv = XMVectorSet(left,  top,    0.0f, 0.0f);
    vertices[1].uv = XMVectorSet(right, top,    0.0f, 0.0f);
    vertices[2].uv = XMVectorSet(right, bottom, 0.0f, 0.0f);
    vertices[3].uv = XMVectorSet(left,  bottom, 0.0f, 0.0f);
}

void DiceFace::SetNormal(float x, float y, float z)
{
    for (int i = 0; i < 4; ++i)
    {
        vertices[i].normal = XMVectorSet(x, y, z, 0.0f);
    }
}
