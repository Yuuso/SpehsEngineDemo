#include "stdafx.h"
#include "Demo/Demos/GUIPlayground.h"

#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/GUI/GUIStack.h"
#include "SpehsEngine/GUI/GUIText.h"

using namespace se::gui;
using namespace se::gui::unit_literals;

constexpr GUIVec2 centerAlign{0.5_parent, 0.5_parent};
constexpr GUIVec2 centerAnchor{0.5_self, 0.5_self};

GUIPlayground::GUIPlayground(DemoContext& _context)
	: DemoApplication(_context)
	, view(_context.shaderManager, _context.textureManager, _context.fontManager, _context.eventSignaler, 465)
{
}
GUIPlayground::~GUIPlayground()
{
	root.clearChildren();
	view.remove(root);
}

void GUIPlayground::init()
{
	demoContext.mainWindow.add(view.getView());

	root.setSize(1.0_view);
	root.setColor(se::Color(se::Black));
	view.add(root);

	createGUI();

	demoContext.showWindowDefault();
}
bool GUIPlayground::update()
{
	return true;
}
template <typename T>
static std::shared_ptr<T> makeElement(GUIElement& parent)
{
	auto result = std::make_shared<T>();
	parent.addChild(result);
	return result;
}
void GUIPlayground::createGUI()
{
	auto bg = makeElement<GUIShape>(root);
	bg->setSize(2000_px);
	bg->setAnchor(centerAnchor);
	bg->setAlignment(centerAlign);
	bg->setTexture("test_color.png");
	bg->setColor(se::Color().withAlpha(0.1f));

	auto stack = makeElement<GUIStack>(*bg);
	stack->setAnchor(centerAnchor);
	stack->setAlignment(centerAlign);
	stack->setOrientation(StackOrientation::Vertical);
	stack->setPadding(4_px);

	auto createTester =
		[]
		{
			auto bg = std::make_shared<GUIShape>();
			bg->setSize(150_px);
			bg->setDataContext(int{});
			bg->setTexture("stone_color.png");
			{
				GUIElementProperties props;
				props.color = se::Color(se::Blue);
				bg->setPressedProperties(props);
			}

			auto tex = makeElement<GUIShape>(*bg);
			tex->setSize(0.9_parent);
			tex->setAnchor(centerAnchor);
			tex->setAlignment(centerAlign);
			tex->setTexture("balldemon.png");

			auto fx = makeElement<GUIShape>(*tex);
			fx->setSize(1.5_parent);
			fx->setAnchor(centerAnchor);
			fx->setAlignment(centerAlign);
			fx->setTexture("wonky_smoke_atlas.png");

			return bg;
		};

	auto tester = createTester();
	stack->addChild(tester);
	stack->addChild(tester->clone());
	stack->addChild(tester->clone());
	stack->addChild(createTester());
	stack->addChild(createTester());
}
