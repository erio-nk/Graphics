//matrix Proj;
//matrix View;
//matrix World;
//float4 color;

cbuffer cb : register(b0)
{
//	matrix Proj;
//	matrix View;
	matrix World;
//	float4 color;
};

struct VS_INPUT 
{
	float3 position	: POSITION;
};

struct PS_INPUT
{
	float4 position	: SV_POSITION;
};

PS_INPUT VS(VS_INPUT input) 
{
	PS_INPUT output;
	float4 pos4 = float4(input.position, 1.0);
	output.position = mul(pos4, World);
//	output.position = mul(output.position, View);
//	output.position = mul(output.position, Proj);
    return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
    return float4(1.0, 1.0, 1.0, 1.0);
}
