#pragma once

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"



class TestMaterial : public se::graphics::PhongMaterial
{
public:

	TestMaterial(se::graphics::ShaderManager& _shaderManager);
};
