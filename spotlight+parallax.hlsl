float4x4 g_world_view_projection;
float4x4 g_world;
float4 g_inv_light_direction;
float4 g_inv_camera_position;
float4 g_inv_light_position;
float4 g_material_diffuse;
float g_material_ambient;

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

struct VS_NMSL_OUT {
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 local_pos : TEXCOORD2;
	float3 light_tangent_direct : TEXCOORD3;
	float3 eye: TEXCOORD4;
};

VS_NMSL_OUT vsNormalMapWithSpotLight(float4 pos:POSITION, float3 tangent:TANGENT, float3 binormal:BINORMAL, float3 normal:NORMAL, float2 uv:TEXCOORD0) {
	VS_NMSL_OUT _out = (VS_NMSL_OUT)0;

	_out.pos = mul(pos, g_world_view_projection);
	_out.uv = uv;
	float4x4 inv = {float4(-binormal, 0.0f), float4(-tangent, 0.0f), float4(normal, 0.0f), {0, 0, 0, 1}};
	inv = transpose(inv);
	_out.light_tangent_direct = mul(g_inv_light_direction, inv).xyz;
	float4 eye = pos - g_inv_camera_position;
	eye.w = 0;
	_out.eye = mul(eye, inv).xyz;
	_out.nor = normal.xyz;
	_out.local_pos = mul(pos, g_world).xyz;

	return _out;
}

float4 psNormalMapWithSpotLight(float2 uv:TEXCOORD0, float3 nor: TEXCOORD1, float3 local_pos: TEXCOORD2, float3 light_tangent_direct: TEXCOORD3, float3 eye: TEXCOORD4) : COLOR0 {
	eye = normalize(eye);
	float height = (tex2D(height_sampler, uv).x - 0.5f) * 0.02f;
	float2 texcoord = uv.xy + eye.xy * height;
	float3 para = tex2D(texture_sampler, texcoord).rgb;

	float3 normal_color = tex2D(normal_sampler, texcoord).xyz;
	float3 normal_vec = 2.0f * normal_color - 1.0f;
	normal_vec = normalize(normal_vec);

	//float3 bright = -dot(normalize(light_tangent_direct), normal_vec) * 0.7f;
	//bright = max(0.0f, bright);

	float4 diff = float4(para, 1.0f);

	//ƒtƒHƒ“ƒuƒŠƒ“
	float3 half = eye + light_tangent_direct;
	half = -normalize(half);
	float hnd = dot(half, normal_vec);
	diff += pow(abs(hnd), 100 ) * 0.3f;


	float3 vec_light = local_pos - g_inv_light_position.xyz;
	float len_light = length(vec_light);
	vec_light = normalize(vec_light);
	diff *= (1.0f - len_light / 20.0f)/(1.0f+len_light*0.125f*0.125f);
	float range = abs(dot(vec_light, g_inv_light_direction.xyz));
	if(range < 0.99f) {
		diff += (range - 0.99f)*3;
	}

	diff.a = 1;

	return diff;
}

struct VS_SL_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL;
	float2 uv  : TEXCOORD0;
};

struct VS_SL_OUT {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 local_pos : TEXCOORD2;
};

struct PS_SL_IN {
	float2 uv  : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 local_pos : TEXCOORD2;
};


VS_SL_OUT vsSpotLight(VS_SL_IN _in) {
	VS_SL_OUT _out = (VS_SL_OUT)0;

	_out.pos = mul(_in.pos, g_world_view_projection);
	_out.uv = _in.uv;
	_out.dif = 0;
	_out.dif.a = 1;
	_out.nor = _in.nor.xyz;
	_out.local_pos = _in.pos.xyz;

	return _out;
}

float4 psSpotLight(PS_SL_IN _in) : COLOR0 {
	float4 color = 0;
	_in.nor = normalize(_in.nor);

	float3 vec_light = _in.local_pos - g_inv_light_position.xyz;
	float len_light = length(vec_light);

	vec_light = normalize(vec_light);
	color = tex2D(texture_sampler, _in.uv);
	color *= (1.0f - len_light / 20.0f)/(1.0f+len_light*0.125f*0.125f);

	float range = abs(dot(vec_light, g_inv_light_direction.xyz));
	if(range < 0.99f) {
		color += (range - 0.99f)*3;
	}

	color.a = 1;

	return color;
}

