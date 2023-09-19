#include "stdafx.h"
#include "Demo/Demos/GUIPlayground.h"

#include "SpehsEngine/GUI/Element.h"
#include "SpehsEngine/GUI/LogOutputAction.h"


class ViewModel : public se::gui::IPropertyHost
{
	GUI_REFLECT(ViewModel, se::gui::IPropertyHost)
	{
		GUI_REGISTER_DATA_PROPERTY(DataValue)
	}

	GUI_DATA_PROPERTY(DataValue, int, data->value)


public:
	ViewModel() = default; // TODO: Do ViewModel classes have to be default constructible?
	ViewModel(GUIPlayground::AppData& _data)
		: data(&_data) {}

private:
	GUIPlayground::AppData* data = nullptr;
};


using namespace se::gui;


GUIPlayground::GUIPlayground(DemoContext& _context)
	: DemoApplication(_context)
	, canvas(_context.eventSignaler, 123)
{
	mirror.registerPropertyHost<ViewModel>();
}
GUIPlayground::~GUIPlayground()
{}

void GUIPlayground::init()
{
	viewModel = std::make_shared<ViewModel>(appData);
	{
		Element& element = *canvas.add<Element>();

		element.onMouseMotion([]{ se::log::info("boo"); return false; });
		element.onMouseLeftPress([](const MouseButtonArgs& args){ se::log::info("hoo " + toString(args.position)); return false; });
		element.addEventTrigger(EventTrigger("MouseRightRelease", { std::make_shared<const LogOutputAction>("goo") }));
		element.addPropertyTrigger(PropertyTrigger("Example", 5, {}, { std::make_shared<const LogOutputAction>("Example is 5") }, { std::make_shared<const LogOutputAction>("Example is not 5") }));
		element.addPropertyTrigger(PropertyTrigger("Example", 6, {}, { std::make_shared<const LogOutputAction>("Example is 6") }, { std::make_shared<const LogOutputAction>("Example is not 6") }));

		element.addResource("TestResource", 10);
		element.addResource(std::make_shared<ValueResource<int>>(
			ResourceTargetType{ "Element" }, ResourceTargetPropertyName{ "Example" }, 11));

		se_assert(element.getExample() == 0);

		element.init();
		se_assert(element.getExample() == 11);

		element.setExample(3);
		se_assert(element.getExample() == 3);

		element.setExample(Binding{ "DataValue", BindingMode::TwoWay });
		se_assert(element.getExample() == 0);

		element.setDataContext(viewModel);
		se_assert(element.getExample() == 4);

		viewModel->setDataValue(5);
		se_assert(element.getExample() == 5);

		element.setExample(6);
		se_assert(viewModel->getDataValue() == 6);

		element.setDataContext(nullptr);
		se_assert(element.getExample() == 0);

		element.setExample(Binding{ "DataValue", BindingMode::OneWayToSource });
		se_assert(element.getExample() == 0);

		element.setDataContext(viewModel);
		se_assert(element.getExample() == 0);

		element.setExample(6);
		se_assert(viewModel->getDataValue() == 6);

		viewModel->setDataValue(7);
		se_assert(element.getExample() == 6);

		element.setDataContext(nullptr);
		se_assert(element.getExample() == 6);

		element.setExample(ResourceKey{ "TestResource" });
		se_assert(element.getExample() == 10);
	}
	{
		Element& element = *canvas.add<Element>();

		Style style;
		style.setters.push_back(PropertySetter{ "Example", 1 });
		element.setStyle(style);
		se_assert(element.getExample() == 0);

		element.init();
		se_assert(element.getExample() == 1);
	}
	{
		Element& element = *canvas.add<Element>();

		Style baseStyle;
		baseStyle.setters.push_back(PropertySetter{ "Example", 1 });
		element.addResource("BaseStyleResource", baseStyle);

		Style style;
		style.basedOn = ResourceKey{ "BaseStyleResource" };
		element.addResource("StyleResource", style);

		element.setStyle(ResourceKey{ "StyleResource" });
		se_assert(element.getExample() == 0);

		element.init();
		se_assert(element.getExample() == 1);
	}

	demoContext.showWindowDefault();
}
bool GUIPlayground::update()
{
	return true;
}