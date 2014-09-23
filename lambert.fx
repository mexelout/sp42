float4x4 g_world_view_proj;
float4 g_inv_light_direction;

struct VS_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL;
	float2 uv  : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float4 dif : COLOR0;
	float2 uv  : TEXCOORD0;
};

VS_OUT vsLambert(VS_IN _in) {
	VS_OUT _out = (VS_OUT)0;

	_out.pos = mul(_in.pos, g_world_view_proj);
	_out.dif = -dot(_in.nor, g_inv_light_direction) * 0.8f;
	_out.dif = max(0.0f, _out.dif);
	_out.dif += 0.2f;
	_out.dif.a = 1;
	_out.uv = _in.uv;

	return _out;
}

