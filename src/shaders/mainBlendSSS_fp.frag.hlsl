#pragma pack_matrix(row_major)

uniform float ThicknessScale;
uniform float Offset;
uniform float Luminance;

struct PS_IN
{
    float4 Pos : SV_POSITION; // Projection coord
    float2 Tex : TEXCOORD0; // Texture coord
};


 // main procedure, the original name was mainBlendSSS_fp
float4 main(in PS_IN input, uniform sampler _Scene : register(s0), uniform sampler _SSS : register(s1)) : COLOR0
{

    float4 _colour;

    float4 _TMP0 = tex2D(_SSS, input.Tex);
    float4 _TMP1 = tex2D(_Scene, input.Tex);
    float _a0005 = ThicknessScale/_TMP0.x;
    float _TMP3 = rsqrt(_a0005);
    float _TMP2 =  1/_TMP3;
    _colour = _TMP1*(_TMP2 + Offset)*Luminance;
    return _colour;
} // main end