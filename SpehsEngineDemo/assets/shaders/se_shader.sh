
#ifndef SE_SHADER_HEADER_GUARD
#define SE_SHADER_HEADER_GUARD

#define ASSERT(statement) if (!(statement)) { gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0); return; }

uniform mat4 u_normal;

vec3 getViewPosition()
{
	return vec3(u_invView[3][0], u_invView[3][1], u_invView[3][2]);
}

#endif // SE_SHADER_HEADER_GUARD
