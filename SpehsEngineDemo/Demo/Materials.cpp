#include "stdafx.h"
#include "Materials.h"



TestMaterial::TestMaterial(se::graphics::ShaderManager& _shaderManager)
{
	shader = _shaderManager.find("test");

	textures.resize(2);
	textures[0] = std::make_unique<se::graphics::MaterialTexture>();
	textures[0]->type = se::graphics::MaterialTextureType::Color;
	textures[0]->uniform = std::make_unique<se::graphics::Uniform>("s_texColor", se::graphics::UniformType::Sampler);
	textures[1] = std::make_unique<se::graphics::MaterialTexture>();
	textures[1]->type = se::graphics::MaterialTextureType::Normal;
	textures[1]->uniform = std::make_unique<se::graphics::Uniform>("s_texNormal", se::graphics::UniformType::Sampler);

	attributesUniform = se::graphics::makePhongAttributesUniform();
}

void TestMaterial::bind()
{
	Bind(textures);
	Bind(attributes, attributesUniform);
}
std::shared_ptr<se::graphics::Shader> TestMaterial::getShader()
{
	return shader;
}

void TestMaterial::setTexture(const se::graphics::MaterialTextureType _type, std::shared_ptr<se::graphics::Texture> _texture)
{
	auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::shared_ptr<se::graphics::MaterialTexture>& _tex){ return _tex->type == _type; });
	if (it != textures.end())
	{
		it->get()->texture = _texture;
		return;
	}
	se::log::error("Texture of type '" + std::to_string((int)_type) + "' not found in TestMaterial!");
}
std::shared_ptr<se::graphics::Texture> TestMaterial::getTexture(const se::graphics::MaterialTextureType _type)
{
	auto it = std::find_if(textures.begin(), textures.end(), [_type](const std::shared_ptr<se::graphics::MaterialTexture>& _tex){ return _tex->type == _type; });
	if (it != textures.end())
		return it->get()->texture;
	se::log::error("Texture of type '" + std::to_string((int)_type) + "' not found in TestMaterial!");
	return nullptr;
}

void TestMaterial::setAttributes(const se::graphics::PhongAttributes& _attributes)
{
	attributes = _attributes;
}
se::graphics::PhongAttributes& TestMaterial::getAttributes()
{
	return attributes;
}
