$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include "bgfx_shader.sh"
#include "se_shader.sh"

void main()
{
	vec4 position = vec4(a_position, 1.0);
	gl_Position = mul(u_modelViewProj, position);
	v_position = mul(u_model[0], position).xyz;

	v_normal = normalize(mul(u_normal, vec4(a_normal, 1.0)).xyz);
	v_tangent = normalize(mul(u_normal, vec4(a_tangent, 1.0)).xyz);
	v_bitangent = normalize(mul(u_normal, vec4(a_bitangent, 1.0)).xyz);

	v_texcoord0 = a_texcoord0;
}
