#pragma once

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/Renderer.h"


inline std::string getShaderPath(std::string_view _filename)
{
	const se::gfx::RendererBackend rendererBackend = se::gfx::Renderer::getBackend();

	switch (rendererBackend)
	{
		case se::gfx::RendererBackend::Auto:
		case se::gfx::RendererBackend::Direct3D12:
		case se::gfx::RendererBackend::Gnm:
		case se::gfx::RendererBackend::Nvn:		break;

		case se::gfx::RendererBackend::Direct3D9:	return "data/shaders/dx9/" + _filename;
		case se::gfx::RendererBackend::Direct3D11:	return "data/shaders/dx11/" + _filename;
		case se::gfx::RendererBackend::Metal:		return "data/shaders/metal/" + _filename;
		case se::gfx::RendererBackend::OpenGLES:
		case se::gfx::RendererBackend::OpenGL:		return "data/shaders/glsl/" + _filename;
		case se::gfx::RendererBackend::Vulkan:		return "data/shaders/spirv/" + _filename;
	}

	se_assert_m(false, "Unknown RendererBackend: " + std::to_string((int)rendererBackend));
	return "";
}
inline std::string getTexturePath(std::string_view _filename)
{
	return "data/textures/" + _filename;
}
inline std::string getFontPath(std::string_view _filename)
{
	return "data/fonts/" + _filename;
}
inline std::string getModelPath(std::string_view _filename)
{
	return "data/models/" + _filename;
}
inline std::string getAudioPath(std::string_view _filename)
{
	return "data/audio/" + _filename;
}
