float4x4 g_wvp: WORLDVIEWPROJECTION;
float4x4 g_owvp: WORLDVIEWPROJECTION;
int blur_frame_num = 1;

sampler g_tex_sampler;

void vertexShaderMotionBlur(in float4 ipos: POSITION, in float4 inor: NORMAL, out float4 opos: POSITION, out float2 otex: TEXCOORD0, out float2 ovel: TEXCOORD1) {
	float4 oldpos = mul(ipos, g_owvp);
	float4 newpos = mul(ipos, g_wvp);

	float2 velocity;
	velocity.x = newpos.x / newpos.w - oldpos.x / oldpos.w;
	velocity.y = newpos.y / newpos.w - oldpos.y / oldpos.w;

	inor.w = 0;
	float4 nor = mul(inor, g_wvp);
	float d = dot(nor.xy, velocity);
	if(d >= 0) {
		opos = newpos;
	} else {
		opos = oldpos;
	}
	ovel.x = velocity.x  * 0.5f;
	ovel.y = -velocity.y * 0.5f;
	otex.x = opos.x / opos.w * 0.5f + 0.5f;
	otex.y = -opos.y / opos.w * 0.5f + 0.5f;

}

void pixelShaderMotionBlur(in float2 itex: TEXCOORD0, in float2 ivel: TEXCOORD1, out float4 odif: COLOR) {
	odif = 0.0f;
	for(int i = 0; i < 10; i++) {
		odif += tex2D(g_tex_sampler,  itex + (ivel * 0.1f * i * blur_frame_num)) * 0.1f;
	}
	odif.a = 1.0f;
}
