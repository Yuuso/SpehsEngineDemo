#include "stdafx.h"
#include "Materials.h"



TestMaterial::TestMaterial(se::graphics::DefaultShaderManager& _shaderManager)
	: se::graphics::PhongMaterial(_shaderManager)
{
	setShader(_shaderManager.find("test"));
}

AnimMaterial::AnimMaterial(se::graphics::DefaultShaderManager& _shaderManager)
	: se::graphics::PhongMaterial(_shaderManager)
{
	setShader(_shaderManager.find("test_anim"));
}
