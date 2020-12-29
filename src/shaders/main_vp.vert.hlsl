#pragma pack_matrix(row_major)

struct VS_OUTPUT {
   float4 pos: POSITION;
   float oZDepth: TEXCOORD0;
};


uniform float4x4 worldViewProj;

 // main procedure, the original name was main_vp
VS_OUTPUT main(float4 _position : POSITION)
{
    float4 _r0001;

    _r0001.x = dot(worldViewProj._11_12_13_14, _position);
    _r0001.y = dot(worldViewProj._21_22_23_24, _position);
    _r0001.z = dot(worldViewProj._31_32_33_34, _position);
    _r0001.w = dot(worldViewProj._41_42_43_44, _position);

    VS_OUTPUT Out;
    Out.pos = _r0001;
    Out.oZDepth = _r0001.z;

    return Out;
} // main end