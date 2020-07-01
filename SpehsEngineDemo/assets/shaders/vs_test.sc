$input a_position, a_normal, a_color0, a_texcoord0
$output v_position, v_normal, v_color0, v_texcoord0

#include "bgfx_shader.sh"
#include "se_shader.sh"

void main()
{
	vec4 position = vec4(a_position, 1.0);
	gl_Position = mul(u_modelViewProj, position);
	v_position = mul(u_model[0], position).xyz;

	vec4 normal = vec4(a_normal, 1.0);
	v_normal = mul(u_normal, normal).xyz;

	v_color0 = a_color0;
	v_texcoord0 = a_texcoord0;
}
