float4x4 g_light_view;
float4x4 g_world;
float4x4 g_view;
float4x4 g_proj;

float4 g_light_dir;

sampler g_tex_sampler;

void vertexShadowDepth(in float4 inpos : POSITION, out float4 outpos : POSITION, out float4 outdepth: TEXCOORD0) {
	float4x4 mat;
	mat  = mul( g_world, g_view );
	mat  = mul( mat, g_proj );
	outpos = mul(inpos, mat);

	outdepth = 0;
	outdepth.x = outpos.z/300.0f;
}
void pixelShaderDepth(in float4 intex: TEXCOORD0, out float4 outdiff: COLOR) {
	outdiff = intex.x;
	outdiff.a = 1.0f;
}

void vertexShaderDepthShadow(in float4 inPosition: POSITION, out float4 outPosition: POSITION, out float4 outshadow: TEXCOORD0) {
	float4x4 mat;
	mat  = mul( g_world, g_view );
	mat  = mul( mat, g_proj );
	outPosition = mul(inPosition, mat);

	mat = mul( g_world, g_light_view);
	mat = mul( mat, g_proj );
	outshadow = mul(inPosition, mat);
	outshadow.x = outshadow.x * 0.5f + 0.5f * outshadow.w;
	outshadow.y = outshadow.y * 0.5f + 0.5f * 20.0f;
	outshadow.z /= 20.0f;
}

void pixelShaderDepthShadow(in float4 inshadow: TEXCOORD0, out float4 outDiff : COLOR) {
	outDiff = 1.0f;
	if(inshadow.z > tex2Dproj(g_tex_sampler, inshadow).r) {
		outDiff *= 0.5f;
	}
	outDiff.a = 1.0f;
}
