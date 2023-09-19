#pragma once

#include "Demo/Demos/DemoApplication.h"
#include "SpehsEngine/GUI/Canvas.h"
#include "SpehsEngine/GUI/Mirror.h"


class ViewModel;

class GUIPlayground final : public DemoApplication
{
public:
	GUIPlayground(DemoContext& _context);
	~GUIPlayground();

	void init() override;
	bool update() override;

	struct AppData
	{
		int value = 4;
	};

private:

	se::gui::Mirror mirror;
	se::gui::Canvas canvas;
	std::shared_ptr<ViewModel> viewModel;
	AppData appData;
};
