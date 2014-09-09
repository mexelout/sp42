float4x4 g_world_view_projection;
float4 g_inv_light_direction;
float4 g_material_diffuse;

texture g_texture;
sampler texture_sampler = sampler_state {
	texture = <g_texture>;
	MipFilter = LINEAR; 
	MinFilter = LINEAR; 
	MagFilter = LINEAR; 
};


texture g_normal_map;
sampler normal_sampler = sampler_state {
	texture = <g_normal_map>;
	MipFilter = LINEAR; 
	MinFilter = LINEAR; 
	MagFilter = LINEAR; 
};

struct VS_OUT {
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
	float3 light_tangent_direct : TEXCOORD3;
};

VS_OUT vsNormalMap(float4 pos:POSITION, float3 tangent:TANGENT, float3 binormal:BINORMAL, float3 normal:NORMAL, float2 uv:TEXCOORD0) {
	VS_OUT _out = (VS_OUT)0;

	_out.pos = mul(pos, g_world_view_projection);
	_out.uv = uv;
	float4x4 inv = {float4(-binormal, 0.0f), float4(-tangent, 0.0f), float4(normal, 0.0f), {0, 0, 0, 1}};
	inv = transpose(inv);
	_out.light_tangent_direct = mul(g_inv_light_direction, inv).xyz;

	return _out;
}

float4 psNormalMap(float2 uv:TEXCOORD0, float3 light_tangent_direct: TEXCOORD3) : COLOR0 {
	float3 normal_color = tex2D(normal_sampler, uv).xyz;
	float3 normal_vec = 2.0f * normal_color - 1.0f;
	normal_vec = normalize(normal_vec);

	float3 bright = -dot(normalize(light_tangent_direct), normal_vec) * 0.8f + 0.2f;
	bright = max(0.0f, bright);
	bright *= tex2D(texture_sampler, uv).rgb;

	return float4(bright, 1.0f);
}
