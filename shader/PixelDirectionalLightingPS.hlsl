
#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 OutDiffuse : SV_TARGET)
{
    float4 normal = normalize(In.Normal);
    float light = max(0.0, dot(normal, -Light.Direction)); //ライトの方向と法線の内積
    light = saturate(light); //lightの値を0～1に収める
    
    OutDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    OutDiffuse.rgb *= light; //テクスチャの色にライトの明るさを乗算
    OutDiffuse.a = In.Diffuse.a; //頂点色のアルファを出力
}