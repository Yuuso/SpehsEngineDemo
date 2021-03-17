#pragma once

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"



class TestMaterial : public se::graphics::PhongMaterial
{
public:

	TestMaterial(se::graphics::DefaultShaderManager& _shaderManager);
};

class AnimMaterial : public se::graphics::PhongMaterial
{
public:

	AnimMaterial(se::graphics::DefaultShaderManager& _shaderManager);
};
