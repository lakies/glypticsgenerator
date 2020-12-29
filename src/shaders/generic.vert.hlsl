struct VS_OUTPUT {
   float4 pos: POSITION;
   float2 texCoord0: TEXCOORD0;
};

float4x4 worldViewProj;

VS_OUTPUT main(
    float4 Pos: POSITION, 
    float3 normal: NORMAL,
    float2 texCoord0: TEXCOORD0
){
   VS_OUTPUT Out;
   
   Out.pos = mul(worldViewProj, Pos);
   Out.texCoord0 = texCoord0;

   return Out;
}