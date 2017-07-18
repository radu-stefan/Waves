#version 330

in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

layout(location = 0) out vec4 out_color;

void main()
{

	vec3 L = normalize( light_position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );

	float ambient_light = material_kd * 0.4;
	float diffuse_light = material_kd * max(dot( world_normal, L),0);
	float specular_light = 0;
	
	if( dot(world_normal, L ) > 0 )
		specular_light = material_ks * pow( max( dot( world_normal, H), 0), material_shininess);

	float atenuare = 1 / (pow(distance(world_position, light_position), 1.5));
	float light = ambient_light +  atenuare * ( diffuse_light + specular_light );

	
	out_color = vec4( object_color * light, 1);
}