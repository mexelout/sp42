float4x4 world_view_proj;
float4x4 world_matrix;

float4 light_direction;

float depth_max;

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

texture tex0 : TEXTURE0;
texture tex1 : TEXTURE1;
texture tex2 : TEXTURE2;
sampler2D tex_sampler = sampler_state {
	Texture = <tex0>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
sampler2D tex_diffuse = sampler_state {
	Texture = <tex1>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
sampler2D tex_depth = sampler_state {
	Texture = <tex2>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

float4 diffuse;

struct PS_IN {
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
	float4 world_pos : TEXCOORD1;
};

struct PS_OUT {
	float4 dif : COLOR0;
	float4 dep : COLOR1;
};

VS_OUT vsWithDepth(VS_IN _in) {
	VS_OUT _out;

	_out.pos = mul(_in.pos, world_view_proj);

	_in.nor.w = 0;
	_in.nor = normalize(_in.nor);
	float4 color = -dot(_in.nor, light_direction) * 0.7f;// diffuse
	color = max(0, color);
	color += 0.3f;// ambient
	color = min(1, color);
	color.a = 1;// not clear object
	_out.col = color;

	_out.world_pos = mul(_in.pos, world_view_proj);

	_out.tex = _in.tex;

	return _out;
}

PS_OUT psWithDepth(PS_IN _in) {
	PS_OUT _out = (PS_OUT)0;

	_out.dif = _in.col;//tex2D(tex_sampler, _in.tex);
	_out.dif *= diffuse;
	_out.dif.a = 1;

	_out.dep = _in.world_pos.z / depth_max;
	_out.dep.a = 1;

	return _out;
}

float occlusion;

float2 rand2[32];

float4 psSSAO(in float2 tex : TEXCOORD0) : COLOR0 {
	float4 dif;
	float color, depth, depth1;
	float2 vec;

	depth = tex2D(tex_depth, tex).r;

	int count = 0;
	for(int i = 0; i < 32; i++) {
		vec = rand2[i] * 10.0f / depth;
		depth1 = tex2D(tex_depth, float2(tex.x + vec.x / 1120.0f, tex.y + vec.y / 630.0f)).r;
		if(depth > depth1 && depth - depth1 < 0.1f ) {
			count++;
		}
	}
	if(count > 20) {
		color = 1.0f - (count - 20) / 32.0f * 1;
	} else {
		color = 1.0f;
	}
	dif = tex2D(tex_diffuse, tex);
	dif *= color;
	dif.a = 1;

	return dif;
}

