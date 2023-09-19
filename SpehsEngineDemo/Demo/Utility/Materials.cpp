#include "stdafx.h"
#include "Materials.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"


std::shared_ptr<se::gfx::Material> createTestMaterial(se::gfx::ShaderManager& _shaderManager)
{
	auto material = createMaterial(se::gfx::DefaultMaterialType::Phong, _shaderManager);
	material->setShader(_shaderManager.find("test"), se::gfx::ShaderVariant::Default);
	material->setShader(_shaderManager.find("test_anim"), se::gfx::ShaderVariant::Skinned);
	return material;
}
