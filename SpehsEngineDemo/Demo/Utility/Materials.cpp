#include "stdafx.h"
#include "Materials.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/DefaultShaders.h"
#include "SpehsEngine/Graphics/Shader.h"


std::shared_ptr<se::gfx::Material> createTestMaterial(se::AssetManager& _assetManager)
{
	auto material = createMaterial(se::gfx::DefaultMaterialType::Phong, _assetManager);
	material->setShader(_assetManager.find<se::gfx::Shader>("test"), se::gfx::ShaderVariant::Default);
	material->setShader(_assetManager.find<se::gfx::Shader>("test_anim"), se::gfx::ShaderVariant::Skinned);
	return material;
}
