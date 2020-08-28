#pragma once

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"



class TestMaterial : public se::graphics::Material
{
public:

	TestMaterial(se::graphics::ShaderManager& _shaderManager);

	void bind() override;
	std::shared_ptr<se::graphics::Shader> getShader() override;

	void setTexture(const se::graphics::MaterialTextureType _type, std::shared_ptr<se::graphics::Texture> _texture);
	std::shared_ptr<se::graphics::Texture> getTexture(const se::graphics::MaterialTextureType _type);

	void setAttributes(const se::graphics::PhongAttributes& _attributes);
	se::graphics::PhongAttributes& getAttributes();

private:

	std::shared_ptr<se::graphics::Shader> shader;
	std::vector<std::unique_ptr<se::graphics::MaterialTexture>> textures;
	se::graphics::PhongAttributes attributes;
	std::shared_ptr<se::graphics::Uniform> attributesUniform;
};
