float4x4 world_view_proj;
float4x4 world_matrix;
float4x4 view_matrix;

float4 light_direction;

struct VS_IN {
	float4 pos : POSITION;
	float4 nor : NORMAL0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT {
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float4 world_nor : TEXCOORD1;
};

texture tex0 : TEXTURE0;
texture tex1 : TEXTURE1;
texture tex2 : TEXTURE2;
sampler tex_sampler = sampler_state {
	Texture = <tex0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
sampler tex_diffuse = sampler_state {
	Texture = <tex1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
sampler tex_normal = sampler_state {
	Texture = <tex2>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

float4 diffuse;
float4 specular;

struct PS_IN {
	float2 tex : TEXCOORD0;
	float4 world_nor : TEXCOORD1;
};

struct PS_OUT {
	float4 dif : COLOR0;
	float4 nor : COLOR1;
};

VS_OUT vsDeferred(VS_IN _in) {
	VS_OUT _out;

	_out.pos = mul(_in.pos, world_view_proj);

	_in.nor.w = 0;
	_out.world_nor = mul(_in.nor, world_matrix);

	_out.tex = _in.tex;

	return _out;
}

PS_OUT psDeferred(PS_IN _in) {
	PS_OUT _out = (PS_OUT)0;

	_out.dif = tex2D(tex_sampler, _in.tex);
	_out.dif *= diffuse;
	_out.dif.a = 1;

	_out.nor = normalize(_in.world_nor);
	_out.nor.a = 1;

	return _out;
}

void psDeferredLight(in float2 tex : TEXCOORD0, out float4 dif : COLOr0) {
	float4 diffuse = tex2D(tex_diffuse, tex);
	float4 normal = tex2D(tex_normal, tex);

	float4 light_pos = float4(-10.0f, 10.0f, -10.0f, 0);
	float4 light_dir = normalize(float4(0, 0, 0, 0) - light_pos);

	normal.w = 0;
	dif = max(-dot(normal, light_dir), 0) + 0.25f;

	if(diffuse.r == 1 && diffuse.g == 0 && diffuse.b == 0) {
		dif.r = 1;
	}
	if(diffuse.r == 0 && diffuse.g == 1 && diffuse.b == 0) {
		dif.g = 1;
	}
	if(diffuse.r == 0 && diffuse.g == 0 && diffuse.b == 1) {
		dif.b = 1;
	}
	if(diffuse.r == 1 && diffuse.g == 1 && diffuse.b == 1) {
		dif.rgb = 1;
	}
	dif *= diffuse;
	dif.a = 1;
}
