#pragma pack_matrix(row_major)

struct PS_IN
{
    float4 pos: POSITION;
    float oZDepth: TEXCOORD0;
};

 // main procedure, the original name was main_fp
float4 main(in PS_IN input) : COLOR0
{
    return input.oZDepth;
} // main end