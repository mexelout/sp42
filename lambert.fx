float4x4 g_world_view_proj;
float4 g_inv_light_direction;
float4 g_material_diffuse;
float4 g_inv_camera_position;

struct VS_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL;
	float2 uv  : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
	float4 eye : TEXCOORD1;
	float4 nor : TEXCOORD2;
};

struct PS_IN {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
	float4 eye : TEXCOORD1;
	float4 nor : TEXCOORD2;
};

VS_OUT vsLambert(VS_IN _in) {
	VS_OUT _out = (VS_OUT)0;

	_out.pos = mul(_in.pos, g_world_view_proj);
	_out.dif = dot(_in.nor, g_inv_light_direction) * 0.7f;
	_out.dif = max(0.0f, _out.dif);
	_out.dif = min(1.0f, _out.dif);
	_out.dif += 0.3f;
	_out.dif.a = 1;
	_out.dif *= g_material_diffuse;
	_out.uv = _in.uv;

	float4 eye = _in.pos - g_inv_camera_position;
	eye = normalize(eye);
	_out.eye = eye;

	_out.nor = _in.nor;

	return _out;
}

float4 psLambert(PS_IN _in) : COLOR0 {
	float4 dif = 0;
	float4 eye = normalize(_in.eye);
	float4 nor = normalize(_in.nor);

	dif = _in.dif;
	float4 half_vec = eye + g_inv_light_direction;
	half_vec = normalize(half_vec);
	float d = (dot(half_vec, nor) + 1) / 2;
	dif += pow(abs(d), 10) * 0.8f;

	return dif;
}