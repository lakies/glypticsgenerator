#pragma pack_matrix(row_major)

struct PS_IN
{
    float4 Pos : SV_POSITION; // Projection coord
    float2 Tex : TEXCOORD0; // Texture coord
};

 // main procedure, the original name was mainFmB_fp
float4 main(in PS_IN input, uniform sampler _Front : register(s0), uniform sampler _Back : register(s1)) : COLOR0
{

    float _colour;

    float4 _TMP0 = tex2D(_Front, input.Tex);
    float4 _TMP1 = tex2D(_Back, input.Tex);
    _colour = (_TMP0 - _TMP1).x;

    float4 output;
    output[0] = _colour;
    output[1] = _colour;
    output[2] = _colour;
    output[3] = _colour;

    return output;
} // main end