cbuffer ModelBuffer : register(b0)
{
    float4x4 matWVP;
};

float4 VS(float3 position : POSITION) : SV_POSITION
{
    return mul(float4(position, 1.0f), matWVP);
}

float4 PS() : SV_TARGET
{
    return float4(1.0f, 0.65f, 0.15f, 1.0f);
}
