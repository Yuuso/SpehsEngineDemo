#pragma once

#include "Demo/Demos/DemoApplication.h"
#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/GUI/GUIShape.h"


class GUIPlayground final : public DemoApplication
{
public:
	GUIPlayground(DemoContext& _context);
	~GUIPlayground();

	void init() override;
	bool update() override;

private:
	se::gui::GUIView view;
	se::gui::GUIShape root;
};
