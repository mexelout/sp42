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
	float4 nor : TEXCOORD1;
};

VS_OUT vsPPL(VS_IN _in) {
	VS_OUT _out = (VS_OUT)0;
	_out.pos = mul(_in.pos, g_world_view_proj);
	_out.uv = _in.uv;
	_out.dif = 1;
	_out.nor = _in.nor;

	return _out;
}

float4 psPPL(VS_OUT _in) :COLOR0 {
	float4 dif = 0;

	_in.nor = normalize(_in.nor);
	dif = -dot(_in.nor, g_inv_light_direction);
	dif = max(0, dif);
	dif += 0.2f;
	dif.a = 1;

	return dif;
}

