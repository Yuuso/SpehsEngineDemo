#include "stdafx.h"
#include "Materials.h"



TestMaterial::TestMaterial(se::graphics::ShaderManager& _shaderManager)
	: se::graphics::PhongMaterial(_shaderManager)
{
	setShader(_shaderManager.find("test"), se::graphics::ShaderVariant::Default);
	setShader(_shaderManager.find("test_anim"), se::graphics::ShaderVariant::Skinned);
}
