float4x4 g_world_view_projection;
float4 g_inv_light_direction;
float4 g_inv_camera_position;
float4 g_material_diffuse;
float g_material_ambient;
bool g_normal_flag = false;

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

texture g_height_map;
sampler height_sampler = sampler_state {
	texture = <g_height_map>;
	MipFilter = LINEAR; 
	MinFilter = LINEAR; 
	MagFilter = LINEAR; 
};

struct VS_OUT {
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
	float3 light_tangent_direct : TEXCOORD3;
	float3 eye: TEXCOORD4;
};

VS_OUT vsNormalMap(float4 pos:POSITION, float3 tangent:TANGENT, float3 binormal:BINORMAL, float3 normal:NORMAL, float2 uv:TEXCOORD0) {
	VS_OUT _out = (VS_OUT)0;

	_out.pos = mul(pos, g_world_view_projection);
	_out.uv = uv;
	float4x4 inv = {float4(-binormal, 0.0f), float4(-tangent, 0.0f), float4(normal, 0.0f), {0, 0, 0, 1}};
	inv = transpose(inv);
	_out.light_tangent_direct = mul(g_inv_light_direction, inv).xyz;
	float4 eye = pos - g_inv_camera_position;
	eye.w = 0;
	_out.eye = mul(eye, inv).xyz;

	return _out;
}

float4 psNormalMap(float2 uv:TEXCOORD0, float3 light_tangent_direct: TEXCOORD3, float3 eye: TEXCOORD4) : COLOR0 {
	eye = normalize(eye);
	float height = (tex2D(height_sampler, uv).x - 0.5f) * 0.2f;
	float2 texcoord = uv.xy + eye.xy * height;
	if(g_normal_flag) {
		texcoord = uv;
	}
	float3 para = tex2D(texture_sampler, texcoord).rgb;

	float3 normal_color = tex2D(normal_sampler, texcoord).xyz;
	float3 normal_vec = 2.0f * normal_color - 1.0f;
	normal_vec = normalize(normal_vec);

	float3 bright = -dot(normalize(light_tangent_direct), normal_vec) * 0.7f + g_material_ambient;
	bright = max(0.0f, bright);

	float4 diff = float4(bright * para, 1.0f);

	//フォンブリン
	float3 half = eye + light_tangent_direct;
	half = -normalize( half );
	float hnd = dot( half, normal_vec );
	//diff += pow( hnd, 100 ) * 0.3f;

	return diff;
}

