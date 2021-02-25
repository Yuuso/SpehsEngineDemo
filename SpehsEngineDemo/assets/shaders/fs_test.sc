$input v_position, v_normal, v_color0, v_texcoord0

#include "bgfx_shader.sh"
#include "se_shader.sh"
#include "se_lights.sh"
#include "se_materials.sh"

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texNormal, 1);


void main()
{
	vec3 position = v_position;
	vec3 viewPosition = getViewPosition();
	vec3 viewDirection = normalize(viewPosition - position);
	vec3 normal = v_normal + texture2D(s_texNormal, v_texcoord0).rgb * viewDirection;
	normal = normalize(normal);

	float specularStrength = u_phongShininess;
	float shininess = u_phongSpecularStrength;

	vec3 ambientColor = vec3_splat(0.0);
	vec3 diffuseColor = vec3_splat(0.0);
	vec3 specularColor = vec3_splat(0.0);

	int lightCount = u_LightCount;
	for (int i = 0; i < lightCount; i++)
	{
		vec3 surfaceToLight;
		float attenuation = u_LightIntensity(i);

		if (attenuation <= 0.0)
			continue;

		if (u_LightIsGlobal(i))
		{
			if (length(u_LightDirection(i)) == 0.0)
			{
				// Ambient light
				ambientColor = ambientColor + u_LightColor(i) * attenuation;
				continue;
			}
			
			// Directional light
			surfaceToLight = -normalize(u_LightDirection(i));
		}
		else
		{
			// Point light
			surfaceToLight = normalize(u_LightPosition(i) - position);
			attenuation = attenuation * getLinearDistanceAttenuation(i, position);

			if (attenuation <= 0.0)
				continue;

			if (length(u_LightDirection(i)) > 0.0)
			{
				// Spot light
				attenuation = attenuation * getLinearConeAttenuation(i, surfaceToLight);
			}
		}

		if (attenuation <= 0.0)
			continue;

		// Diffuse
		float diffuseFactor = max(0.0, dot(normal, surfaceToLight));
		diffuseColor = diffuseColor + diffuseFactor * attenuation * u_LightColor(i);

		// Specular
		float specularFactor;
		if (true)
		{
			// Blinn
			vec3 halfwayDirection = normalize(surfaceToLight + viewDirection);
			specularFactor = pow(max(0.0, dot(normal, halfwayDirection)), shininess);
		}
		else
		{
			vec3 reflectDirection = reflect(-surfaceToLight, normal);
			specularFactor = pow(max(0.0, dot(viewDirection, reflectDirection)), shininess);
		}
		specularColor = specularColor + specularFactor * specularStrength * attenuation * u_LightColor(i);	
	}

	gl_FragColor = v_color0 * texture2D(s_texColor, v_texcoord0) * vec4(ambientColor + diffuseColor + specularColor, 1.0);
}
