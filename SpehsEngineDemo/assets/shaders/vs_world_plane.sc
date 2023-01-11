$input a_position
$output v_position

#include "se_shader.sh"

void main()
{
	vec4 position = mul(u_model[0], vec4(a_position, 1.0));
	v_position = position.xyz;
	gl_Position = mul(u_viewProj, position);
}
