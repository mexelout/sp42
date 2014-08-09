float4x4 g_world_view_projection;
float4 g_light_direction;
float4x4 g_world;
float4 g_ambient;
float4 g_material_diffuse;
float4x4 g_light_matrix;

float4 g_player_position;


sampler g_tex_sampler;

void vertexShader3D(in float4 inPosition: POSITION, in float4 inDiff: COLOR0, in float4 inNormal: NORMAL, out float4 outPosition: POSITION, out float4 outDiffuse: COLOR0) {
	outPosition = mul(inPosition, g_world_view_projection);
	inNormal.w = 0;
	float4 normal = mul(inNormal, g_world);
	// ”ÍˆÍ 0`1
	outDiffuse = -dot(g_light_direction, normal) + g_ambient;
	outDiffuse.a = 1.0f;
	outDiffuse *= inDiff;
	outDiffuse *= g_material_diffuse;
}

void vertexShaderTexture(in float4 inPosition: POSITION, in float4 inNormal: NORMAL, in float4 inDiff : COLOR, in float2 inTex : TEXCOORD0, out float4 outPosition: POSITION, out float4 outDiffuse: COLOR0, out float2 outTex : TEXCOORD0) {
	outPosition = mul(inPosition, g_world_view_projection);
	inNormal.w = 0;
	float4 normal = mul(inNormal, g_world);
	// ”ÍˆÍ 0`1
	outDiffuse = (-dot(g_light_direction, normal) + g_ambient) * inDiff;
	outDiffuse.a = 1.0f;
	outDiffuse *= g_material_diffuse;
	outTex = inTex;
}

void vertexShaderShadow(in float4 inPosition: POSITION, in float4 inNormal: NORMAL, in float4 inDiff : COLOR, out float4 outPosition: POSITION, out float4 outDiffuse: COLOR0, in float2 inTex : TEXCOORD0, out float2 outTex : TEXCOORD0) {
	outPosition = mul(inPosition, g_world_view_projection);
	inNormal.w = 0;
	float4 normal = mul(inNormal, g_world);
	// ”ÍˆÍ 0`1
	outDiffuse = -dot(g_light_direction, normal) + g_ambient * inDiff;
	outDiffuse.a = 1.0f;
	outDiffuse *= g_material_diffuse;

	outTex.x = (g_player_position.x - inPosition.x) * 0.5f;
	outTex.y = (g_player_position.z - inPosition.z) * 0.5f;
}

void vertexShaderCell(in float4 inPosition: POSITION, in float4 inNormal: NORMAL, in float4 inDiff : COLOR, out float4 outPosition: POSITION, out float4 outDiff: COLOR0, in float2 inTex : TEXCOORD0, out float2 outTex : TEXCOORD0) {
	outPosition = mul(inPosition, g_world_view_projection);
	outDiff = inDiff;
	outDiff *= g_material_diffuse;
	inNormal.w = 0;
	float4 normal = mul(inNormal, g_world);
	outTex.x = (dot(g_light_direction, normal)+1)/2;
	outTex.y = 0.5f;
}

void vertexShaderLine(in float4 inPosition: POSITION, in float4 inNormal: NORMAL, in float4 inDiff : COLOR, out float4 outPosition: POSITION, out float4 outDiff: COLOR0, in float2 inTex : TEXCOORD0, out float2 outTex : TEXCOORD0) {
	inNormal.w = 0;
	inPosition += (inNormal * 0.015f);
	outPosition = mul(inPosition, g_world_view_projection);
	outDiff = g_material_diffuse;
	outTex.x = 0.5f;
	outTex.y = 0.5f;
}

void vertexShaderShadowVol(in float4 inPosition: POSITION, in float4 inNormal: NORMAL, out float4 outPosition: POSITION, in float2 inTex : TEXCOORD0, out float2 outTex : TEXCOORD0) {
/*
	outTex = inTex;
	inNormal.w = 0;
	float4 normal = mul(inNormal, g_world);
	float d = dot(normal, g_light_direction);
	if(d > 0.0f)
		inPosition += g_light_direction * 10.0f;
	viewPosition = mul(worldPosition, g_view);
	outPosition = mul(viewPosition, g_projection);
	*/
}

void pixelShader3D(in float4 inDiff: COLOR, out float4 outDiff: COLOR) {
	outDiff = inDiff;
}

void pixelShaderTexture(in float2 inTex : TEXCOORD0, in float4 inDiff : COLOR, out float4 outDiff : COLOR) {
	outDiff = tex2D(g_tex_sampler, inTex) * inDiff;
};

