float4x4 g_world_view_projection;
float4 g_light_direction;
float4x4 g_world;
float4 g_camera_position;

float g_fog_end;
float g_fog_start;

sampler g_tex_sampler;

void vertexShaderFogLinear(in float4 ipos  : POSITION,
						   in float4 inor  : NORMAL,
						   in float4 idiff : COLOR,
						   in float2 itex  : TEXCOORD0,

						   out float4 opos : POSITION,
						   out float4 odiff: COLOR0,
						   out float2 otex : TEXCOORD0,
						   out float ofog  : FOG) {
	opos = mul(ipos, g_world_view_projection);
	inor.w = 0;
	float4 normal = mul(inor, g_world);
	// îÕàÕ 0Å`1
	odiff = -dot(g_light_direction, normal) * idiff;
	odiff.a = 1.0f;
	otex = itex;
	ofog = (g_fog_end - opos.z) / (g_fog_end - g_fog_start);
}

void vertexShaderFogLength(in float4 ipos  : POSITION,
						   in float4  inor : NORMAL,
						   in float4  idiff: COLOR,
						   in float2  itex : TEXCOORD0,

						   out float4 opos : POSITION,
						   out float4 odiff: COLOR0,
						   out float2 otex : TEXCOORD0,
						   out float  ofog : FOG) {
	opos = mul(ipos, g_world_view_projection);
	inor.w = 0;
	float4 normal = mul(inor, g_world);
	// îÕàÕ 0Å`1
	odiff = -dot(g_light_direction, normal) * idiff;
	odiff.a = 1.0f;
	otex = itex;
	float4 world_position = mul(ipos, g_world);
	float len = length(g_camera_position - world_position);
	ofog = (g_fog_end - len) / (g_fog_end - g_fog_start);
	ofog = clamp(ofog, 0.0f, 1.0f);
}

void vertexShaderFogHeight(in float4 ipos  : POSITION,
						   in float4  inor : NORMAL,
						   in float4  idiff: COLOR,
						   in float2  itex : TEXCOORD0,

						   out float4 opos : POSITION,
						   out float4 odiff: COLOR0,
						   out float2 otex : TEXCOORD0,
						   out float  ofog : FOG) {
	opos = mul(ipos, g_world_view_projection);
	inor.w = 0;
	float4 normal = mul(inor, g_world);
	// îÕàÕ 0Å`1
	odiff = -dot(g_light_direction, normal) * idiff;
	odiff.a = 1.0f;
	otex = itex;
	float4 world_position = mul(ipos, g_world);
	if(world_position.y < 0)
		ofog = 1 - world_position.y*-0.02f;
	else
		ofog = 1.0f;
	ofog = clamp(ofog, 0.0f, 1.0f);
}

