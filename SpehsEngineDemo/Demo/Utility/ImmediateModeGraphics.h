#pragma once

#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Model.h"
#include "SpehsEngine/Graphics/ModelDataManager.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/Graphics/View.h"
#include <vector>


class ImmediateModeGraphics
{
public:

	ImmediateModeGraphics(se::graphics::View& _view, se::graphics::ShaderManager& _shaderManager,
						  se::graphics::TextureManager& _textureManager, se::graphics::FontManager& _fontManager,
						  se::graphics::ModelDataManager& _modelDataManager, se::graphics::ShapeGenerator& shapeGenerator);

	void init();
	void endFrame();
	void cleanup();


	template <typename T>
	class GraphicsObjectModifier
	{
	public:
		GraphicsObjectModifier() = delete;
		GraphicsObjectModifier(const GraphicsObjectModifier&) = delete;
		GraphicsObjectModifier& operator=(const GraphicsObjectModifier&) = delete;
		GraphicsObjectModifier& operator=(GraphicsObjectModifier&&) = delete;

		GraphicsObjectModifier(T& _object)
			: object(_object) {}
		~GraphicsObjectModifier() = default;
		GraphicsObjectModifier(GraphicsObjectModifier&&) = default;

		GraphicsObjectModifier<T>& position(const glm::vec3& _position)							{ object.setPosition(_position); return *this; }
		GraphicsObjectModifier<T>& scale(const glm::vec3& _scale)								{ object.setScale(_scale); return *this; }
		GraphicsObjectModifier<T>& rotation(const glm::quat& _rotation)							{ object.setRotation(_rotation); return *this; }
		GraphicsObjectModifier<T>& color(const se::Color& _color)								{ object.setColor(_color); return *this; }
		GraphicsObjectModifier<T>& material(std::shared_ptr<se::graphics::Material> _material)	{ object.setMaterial(_material); return *this; }
		GraphicsObjectModifier<T>& primitiveType(se::graphics::PrimitiveType _primitiveType)	{ object.setPrimitiveType(_primitiveType); return *this; }
		GraphicsObjectModifier<T>& renderFlags(se::graphics::RenderFlagsType _renderFlags)		{ object.setRenderFlags(_renderFlags); return *this; }

	private:
		T& object;
	};

	GraphicsObjectModifier<se::graphics::Shape>	shape(se::graphics::ShapeType _type, se::graphics::ShapeParameters _params = {});
	GraphicsObjectModifier<se::graphics::Shape>	icon(std::string_view _texture);
	GraphicsObjectModifier<se::graphics::Line>	line(const glm::vec3 _start, const glm::vec3 _end);
	GraphicsObjectModifier<se::graphics::Line>	linePoint(const glm::vec3 _point, std::string_view _lineId);
	GraphicsObjectModifier<se::graphics::Text>	text(const std::string& _text, std::string_view _font = "");
	GraphicsObjectModifier<se::graphics::Model>	model(std::string_view _model);

private:

	template <typename T>
	struct FrameVector
	{
		FrameVector(se::graphics::View& _view) : view(_view) {}
		T& find(std::string_view _name)
		{
			auto it = std::find_if(vector.begin(), vector.end(), [_name](std::unique_ptr<T>& _e){ return _e->getName() == _name; });
			if (it != vector.end())
			{
				it->get()->setRenderState(true);
				return **it;
			}
			T& result = next();
			result.setName(_name);
			return result;
		}
		T& next()
		{
			se_assert(vector.size() >= currentIndex);
			if (vector.size() <= currentIndex)
			{
				vector.push_back(std::make_unique<T>());
				view.getScene().add(*vector.back());
			}
			T* result = vector[currentIndex++].get();
			se_assert(result);
			result->setRenderState(true);
			return *result;
		}
		void endFrame()
		{
			for (auto&& element : vector)
			{
				element->setRenderState(false);
			}
			currentIndex = 0;
		}
		void clear()
		{
			vector.clear();
			currentIndex = 0;
		}

		se::graphics::View& view;
		std::vector<std::unique_ptr<T>> vector;
		size_t currentIndex = 0;
	};

	se::graphics::View& view;
	se::graphics::ShaderManager& shaderManager;
	se::graphics::TextureManager& textureManager;
	se::graphics::FontManager& fontManager;
	se::graphics::ModelDataManager& modelDataManager;
	se::graphics::ShapeGenerator& shapeGenerator;

	std::shared_ptr<se::graphics::Material> defaultFlatMaterial;
	std::shared_ptr<se::graphics::Material> defaultTextMaterial;

	FrameVector<se::graphics::Shape> shapes;
	FrameVector<se::graphics::Line> lines;
	FrameVector<se::graphics::Text> texts;
	FrameVector<se::graphics::Model> models;
};


// Global interface
namespace ImGfx
{
	void init(ImmediateModeGraphics& _instance);
	void deinit();

	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Shape>	shape(se::graphics::ShapeType _type, se::graphics::ShapeParameters _params = {});
	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Shape>	icon(std::string_view _texture);
	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Line>	line(const glm::vec3 _start, const glm::vec3 _end);
	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Line>	linePoint(const glm::vec3 _point, std::string_view _lineId);
	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Text>	text(const std::string& _text, std::string_view _font = "");
	ImmediateModeGraphics::GraphicsObjectModifier<se::graphics::Model>	model(std::string_view _model);
}
