$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

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

	vec3 normal = texture2D(s_texNormal, v_texcoord0).rgb;
	normal = normal * 2.0 - 1.0;
	
	mat3 TBNmatrix = mtxFromCols(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal));
	normal = mul(TBNmatrix, normal);
	normal = normalize(normal);

	float specularStrength = u_phongSpecularStrength;
	float shininess = u_phongShininess;

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

	gl_FragColor = u_primitiveColor * texture2D(s_texColor, v_texcoord0) * vec4(ambientColor + diffuseColor + specularColor, 1.0);
}
