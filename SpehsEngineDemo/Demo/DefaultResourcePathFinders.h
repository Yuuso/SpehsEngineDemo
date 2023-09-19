#pragma once

#include "SpehsEngine/Core/ResourcePathFinder.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include <string>


static const std::string DATA_PATH = "data/";


class ShaderPathFinder : public se::ResourcePathFinder
{
public:

	std::string getPath(const std::string_view _resource) const override
	{
		const se::gfx::RendererBackend rendererBackend = se::gfx::Renderer::getRendererBackend();

		switch (rendererBackend)
		{
			case se::gfx::RendererBackend::Auto:
			case se::gfx::RendererBackend::Direct3D12:
			case se::gfx::RendererBackend::Gnm:
			case se::gfx::RendererBackend::Nvn:		break;

			case se::gfx::RendererBackend::Direct3D9:	return DATA_PATH + "shaders/dx9/" + _resource;
			case se::gfx::RendererBackend::Direct3D11: return DATA_PATH + "shaders/dx11/" + _resource;
			case se::gfx::RendererBackend::Metal:		return DATA_PATH + "shaders/metal/" + _resource;
			case se::gfx::RendererBackend::OpenGLES:
			case se::gfx::RendererBackend::OpenGL:		return DATA_PATH + "shaders/glsl/" + _resource;
			case se::gfx::RendererBackend::Vulkan:		return DATA_PATH + "shaders/spirv/" + _resource;
		}

		se_assert_m(false, "Unknown RendererBackend: " + std::to_string((int)rendererBackend));
		return "";
	}
};


class TexturePathFinder : public se::ResourcePathFinder
{
public:

	std::string getPath(const std::string_view _resource) const override
	{
		return DATA_PATH + "textures/" + _resource;
	}
};


class FontPathFinder : public se::ResourcePathFinder
{
public:

	std::string getPath(const std::string_view _resource) const override
	{
		return DATA_PATH + "fonts/" + _resource;
	}
};


class ModelPathFinder : public se::ResourcePathFinder
{
public:

	std::string getPath(const std::string_view _resource) const override
	{
		return DATA_PATH + "models/" + _resource;
	}
};


class AudioPathFinder : public se::ResourcePathFinder
{
public:

	std::string getPath(const std::string_view _resource) const override
	{
		return DATA_PATH + "audio/" + _resource;
	}
};
