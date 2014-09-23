sampler g_tex_sampler1;
sampler g_tex_sampler2;
sampler g_tex_sampler3;

void pixelShaderMulthTex(in float2 in_tex: TEXCOORD0, out float4 out_diff: COLOR) {
	out_diff = tex2D(g_tex_sampler1, in_tex) + tex2D(g_tex_sampler2, in_tex);
}

void pixelShaderFocus(in float2 in_tex: TEXCOORD0, out float4 out_diff: COLOR) {
	float depth = tex2D(g_tex_sampler3, in_tex).r;
	out_diff = tex2D(g_tex_sampler1, in_tex) * (1 - depth)
			 + tex2D(g_tex_sampler2, in_tex) * depth;
}