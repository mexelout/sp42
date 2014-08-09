sampler g_tex_sampler;



float4 pixelShaderBlurX(in float2 intex: TEXCOORD0): COLOR {
	return
	 tex2D(g_tex_sampler, float2(intex.x           , intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x+0.0015625f, intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x+0.003125f , intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x+0.0046875f, intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x+0.00625f  , intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x+0.0078125f, intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x-0.0015625f, intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x-0.003125f , intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x-0.0046875f, intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x-0.00625f  , intex.y))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x-0.0078125f, intex.y))*0.1f
	;
}

float4 pixelShaderBlurY(in float2 intex: TEXCOORD0): COLOR {
	return
	 tex2D(g_tex_sampler, float2(intex.x, intex.y           ))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y+0.0015625f))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y+0.003125f ))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y+0.0046875f))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y+0.00625f  ))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y+0.0078125f))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y-0.0015625f))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y-0.003125f ))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y-0.0046875f))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y-0.00625f  ))*0.1f
	+tex2D(g_tex_sampler, float2(intex.x, intex.y-0.0078125f))*0.1f
	;
}