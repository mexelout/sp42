float4x4 g_world_view_projection;
float4 g_inv_light_direction[2];

struct VS_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL;
	float2 uv  : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 local_pos : TEXCOORD2;
};

struct PS_IN {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 local_pos : TEXCOORD2;
};

VS_OUT vsPointLight(VS_IN _in) {
	VS_OUT _out = (VS_OUT)0;

	_out.pos = mul(_in.pos, g_world_view_projection);
	_out.uv = _in.uv;
	_out.dif = 0;
	_out.dif.a = 1;
	_out.nor = _in.nor.xyz;
	_out.local_pos = _in.pos.xyz;

	return _out;
}

float4 psPointLight(PS_IN _in) : COLOR0 {
	float4 color = 0;
	_in.nor = normalize(_in.nor);

	for(int i = 0; i < 2; i++) {
		float3 vec_light = _in.local_pos - g_inv_light_direction[i].xyz;
		float len_light = length(vec_light);

		vec_light = normalize(vec_light);
		color += (-dot(_in.nor, vec_light) / (len_light * len_light)) * 80.0f;
	}

	color.a = 1;

	return color;
}
