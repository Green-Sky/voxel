#version 330 core
// or #version 300 es

#ifdef GL_ES
	precision mediump float;
#endif

in vec2 _uv;
in vec2 _ao_uv; // used for ao interpolation

flat in vec4 _ao;

uniform sampler2D _tex0;

out vec4 _out_color;

float interpolate_ao(vec4 ao, vec2 uv) {
	return mix(mix(ao.x, ao.y, uv.x), mix(ao.z, ao.w, uv.x), uv.y);
}

void main() {
	vec4 tmp_col = texture(_tex0, _uv);

	// remove it you dont have alpha, ifs in frag are expensive
	if (tmp_col.a == 0.0) {
		discard;
	}

	tmp_col.rgb *= interpolate_ao(_ao, _ao_uv);

	_out_color = tmp_col;
}

