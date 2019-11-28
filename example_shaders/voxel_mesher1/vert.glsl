#version 330 core
// or #version 300 es

layout (location = 0) in vec3 _vert_pos;
layout (location = 1) in vec2 _base_uv;

// _vox0 and _vox1 are together the mesher output
layout (location = 2) in uint _vox0;
layout (location = 3) in uint _vox1;

uniform mat4x4 _vp;
uniform vec3 _chunk_pos;

out vec2 _uv;
out vec2 _ao_uv;

flat out vec4 _ao;

void decompress_position(in uint vox, out vec3 position) {
	position.x = float((vox      ) & 31u);
	position.y = float((vox >> 5 ) & 31u);
	position.z = float((vox >> 10) & 31u);
}

void decompress_ao(in uint vox, out vec4 ao) {
	ao.x = float((vox >> 15) & 3u);
	ao.y = float((vox >> 17) & 3u);
	ao.z = float((vox >> 19) & 3u);
	ao.w = float((vox >> 21) & 3u);

	ao /= 3.0;
}

void scale_ao(inout vec4 ao) {
	const float min = 0.2; // tweek for ao impact
	ao.x = mix(min, 1.0, ao.x);
	ao.y = mix(min, 1.0, ao.y);
	ao.z = mix(min, 1.0, ao.z);
	ao.w = mix(min, 1.0, ao.w);
}

void main() {
	_ao_uv = _base_uv;

	vec4 tmp_ao;
	decompress_ao(_vox0, tmp_ao);
	scale_ao(tmp_ao);
	_ao = tmp_ao;

	vec3 position;
	decompress_position(_vox0, position);

	const uint atlas_dim = 16u;
	const uint texture_dim = 16u;

	uint atlas_index = (_vox1) & 65535u; // make sure its a ushort
	int atlas_row = int(atlas_index / atlas_dim);
	int atlas_column = int(atlas_index % atlas_dim);

	// (y grows up and row grows down)
	_uv = (_base_uv + vec2(atlas_column, 15-atlas_row)) / float(texture_dim);

	position += _vert_pos;
	position += _chunk_pos;
	gl_Position = _vp * vec4(position, 1.0);
}

