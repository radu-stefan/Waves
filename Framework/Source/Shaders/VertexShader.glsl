#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;

// Output value to fragment shader
out vec3 color;

uniform float distance[200];
uniform float frequence[200];
uniform float amplitude[200];
uniform float wavelength[200];
uniform float phase[200];
uniform vec2 wave_center[200];
uniform int wave_num;

void main()
{

	float h = 0;
	float b = 0;
	float t = 0;
	vec3 n = vec3( 0, 1, 0);

	for( int i = 0 ; i < wave_num ; i++ ) {
		float dist = distance( wave_center[i], vec2(v_position.x,v_position.z));
		float distx = distance( vec2(wave_center[i].x,0), vec2(v_position.x,0));
		float disty = distance( vec2( 0, wave_center[i].y), vec2( 0, v_position.z));

		if( dist >= distance[i] && dist <= distance[i]+wavelength[i] ){
			float x = -dist*frequence[i] + phase[i];

			//water height
			h += amplitude[i] * sin(x);
			//binormala si tangenta pentru iluminare
			b += frequence[i] * distx * amplitude[i]*cos(x);
			t += frequence[i] * disty * amplitude[i]*cos(x);
		}
	}
	
	if( h != 0) {
		vec3 tn = vec3( 0, t, 1);
		vec3 bn = vec3( 1, b, 0);
		n = cross( tn, bn);
	}

	world_position = (Model * vec4( v_position, 1) ).xyz;
	world_normal = normalize( mat3(Model) * n );
 
	gl_Position = Projection * View * Model * vec4(vec3( v_position.x, h, v_position.z), 1.0);
}
