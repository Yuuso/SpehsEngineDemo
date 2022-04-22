#include "stdafx.h"
#include "Materials.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"

using namespace se::graphics;


std::shared_ptr<Material> createTestMaterial(ShaderManager& _shaderManager)
{
	auto material = createMaterial(DefaultMaterialType::Phong, _shaderManager);
	material->setShader(_shaderManager.find("test"), ShaderVariant::Default);
	material->setShader(_shaderManager.find("test_anim"), ShaderVariant::Skinned);
	return material;
}
