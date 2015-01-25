float4x4 world_view_proj;
float4x4 world_matrix;

struct VS_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
};

struct PS_IN {
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
};

struct PS_OUT {
	float4 dep : COLOR0;
};

VS_OUT vsWithDepth(VS_IN _in) {
	VS_OUT _out;

	_out.pos = mul(_in.pos, world_view_proj);
	_out.col = 0;
	_out.world_pos = mul(_in.pos, world_view_proj);
	_out.tex = _in.tex;

	return _out;
}

PS_OUT psWithDepth(PS_IN _in) {
	PS_OUT _out = (PS_OUT)0;

	_out.dep = _in.world_pos.z / 20.0f;
	_out.dep.a = 1;

	return _out;
}

struct VS_IN_AA {
	float4 pos: POSITION;
	float4 nor: NORMAL;
	float2 tex: TEXCOORD0;
};

struct VS_OUT_AA {
	float4 pos: POSITION0;
	float4 pos2: TEXCOORD0;
};

texture tex0 : TEXTURE0;
sampler2D tex_depth = sampler_state {
	Texture = <tex0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};


VS_OUT_AA vsAntiAliasing(VS_IN_AA _in) {
	VS_OUT_AA _out = (VS_OUT_AA)0;
	_out.pos = mul(_in.pos, world_view_proj);
	_out.pos2 = _out.pos;
	return _out;
}

float4 psAntiAliasing(in float4 pos2: TEXCOORD0) : COLOR0 {
	float4 dif;

	float2 texCoord;
	texCoord.x = (pos2.x / pos2.w) * 0.5f + 0.5f;
	texCoord.y = -(pos2.x / pos2.w) * 0.5f + 0.5f;

	float2 offset;
	offset.x = 1.0f / 2240.0f;
	offset.y = 1.0f / 1260.0f;

	float depth = pos2.z / 20.0f;
	float a = 1.0f;
	a -= tex2D(tex_depth, float2(texCoord.x           , texCoord.y           )).r - depth;
	a -= tex2D(tex_depth, float2(texCoord.x + offset.x, texCoord.y           )).r - depth;
	a -= tex2D(tex_depth, float2(texCoord.x           , texCoord.y + offset.y)).r - depth;
	a -= tex2D(tex_depth, float2(texCoord.x + offset.x, texCoord.y + offset.y)).r - depth;

	dif = 1.0f;
	dif.a = a;

	return dif;
}
