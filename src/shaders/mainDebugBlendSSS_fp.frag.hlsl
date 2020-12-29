#pragma pack_matrix(row_major)

uniform float ThicknessScale;
uniform float Offset;
uniform float Luminance;

 // main procedure, the original name was mainDebugBlendSSS_fp
float4 main(in float2 _uv : TEXCOORD0, uniform sampler _SSS : TEXUNIT0) : COLOR0
{

    float4 _colour;

    float4 _TMP0 = tex2D((sampler2D) _SSS, _uv);
    float _a0004 = ThicknessScale/_TMP0.x;
    float _TMP2 = rsqrt(_a0004);
    float _TMP1 =  1.00000000000000000E000f/_TMP2;
    _colour = ((_TMP1 + Offset)*Luminance).xxxx;
    return _colour;
} // main end