$input v_position

#include "se_shader.sh"

float custom_fract(float value)
{
    return abs(fract(value + 0.5) - 0.5);
}

void main()
{
    float viewDistance = distance(getViewPosition(), v_position);
    vec4 ambientColor = vec4_splat(0.02);
    float comp = 0.1;
    float grid = min(custom_fract(v_position.x), custom_fract(v_position.z));
    if (grid < comp)
    {
        float mixer = max(pow(grid / comp, 0.1), pow(viewDistance * 0.005, 0.1));
        gl_FragColor = mix(u_primitiveColor, ambientColor, clamp(mixer, 0.0, 1.0));
    }
    else
    {
        gl_FragColor = ambientColor;
    }
}
