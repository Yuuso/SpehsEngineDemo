#include "stdafx.h"
#include "Demo/Demos/MusicManager.h"

#include "SpehsEngine/Audio/Group.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/GUI/GUIStack.h"

using namespace se::legacygui;
using namespace se::gui::unit_literals;


MusicManager::MusicManager(DemoContext& _demoContext)
	: DemoApplication(_demoContext)
	, view(_demoContext.shaderManager, _demoContext.textureManager, _demoContext.fontManager, _demoContext.eventSignaler, 678)
{
}
MusicManager::~MusicManager()
{
	root.clearChildren();
	view.remove(root);
}

void MusicManager::init()
{
	auto themeResource = demoContext.audioManager.stream("MusicTheme", "music_manager_theme_001.ogg");
	auto commonResource = demoContext.audioManager.stream("MusicCommon", "music_manager_common_001.ogg");
	auto actionResource = demoContext.audioManager.stream("MusicAction", "music_manager_action_001.ogg");

	themeResource->waitUntilReady();
	commonResource->waitUntilReady();
	actionResource->waitUntilReady();
	lastBar = timeToBars(commonResource->getLength() - se::time::fromSeconds(0.1f));

	themeLayer.setResource(themeResource);
	commonLayer.setResource(commonResource);
	actionLayer.setResource(actionResource);

	outputBus.connect(demoContext.audioEngine.getMasterBus());
	outputBus.setVolume(1.0f);

	themeLayer.setOutput(outputBus);
	commonLayer.setOutput(outputBus);
	actionLayer.setOutput(outputBus);


	commonLayer.play(true);
	commonLayer.setLooping(true);
	commonLayer.setVolume(1.0f);
	commonLayer.setInaudibleBehavior(se::audio::InaudibleBehavior::Nothing);

	themeLayer.play(true);
	themeLayer.setLooping(true);
	themeLayer.setVolume(1.0f);
	themeLayer.setInaudibleBehavior(se::audio::InaudibleBehavior::Nothing);

	actionLayer.play(true);
	actionLayer.setLooping(true);
	actionLayer.setVolume(0.0f);
	actionLayer.setInaudibleBehavior(se::audio::InaudibleBehavior::Nothing);

	assert(commonLayer.getLoopPoint() == themeLayer.getLoopPoint());
	assert(commonLayer.getLoopPoint() == actionLayer.getLoopPoint());


	demoContext.mainWindow.add(view.getView());

	root.setSize({ 1.0_view, 1.0_view });
	root.setColor(se::Color(se::DimGray));
	view.add(root);

	auto makeButton =
		[](const std::string& _text, const se::Color _color) -> std::shared_ptr<GUIShape>
		{
			auto shape = std::make_shared<GUIShape>();
			shape->setSize({150_px, 50_px});
			shape->setColor(_color);
			shape->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
			{
				GUIElementProperties hoverProps;
				hoverProps.color = se::brightnessAddNewColor(_color, 0.1f, true);
				shape->setHoverProperties(hoverProps);
				GUIElementProperties pressedProps;
				pressedProps.color = se::brightnessAddNewColor(_color, -0.1f, true);
				pressedProps.scale = glm::vec2(0.98f);
				shape->setPressedProperties(pressedProps);
			}
			shape->setPadding(16_px);

			auto text = std::make_shared<GUIText>();
			text->setFont("OpenSans-Regular");
			text->insert(_text);
			text->setHeight(1_parent);
			text->setInheritInputStatus(true);
			text->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
			text->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
			shape->addChild(text);

			return shape;
		};

	auto playButton = makeButton("PLAY", se::Color(se::DarkSeaGreen));
	playButton->onClick([this](auto&){togglePause();});
	playButton->setPosition({ 0.5_parent, 0.8_parent });
	root.addChild(playButton);
	playButtonText = std::dynamic_pointer_cast<GUIText>(playButton->getChild(0));

	auto queueButton = makeButton("QUEUE", se::Color(se::Gray));
	queueButton->onClick([this](auto&){queueChange();});
	queueButton->setPosition({ 0.35_parent, 0.5_parent });
	root.addChild(queueButton);

	auto fadeButton = makeButton("FADE", se::Color(se::Gray));
	fadeButton->onClick([this](auto&){fadeChange(se::time::fromSeconds(1.0f));});
	fadeButton->setPosition({ 0.65_parent, 0.5_parent });
	root.addChild(fadeButton);

	timerText = std::make_shared<GUIText>();
	timerText->insert("00:00");
	timerText->setHeight(28_px);
	timerText->setPosition({ 0.49_parent, 0.3_parent });
	timerText->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Right);
	root.addChild(timerText);

	auto layerDisplay = makeButton("THEME", se::Color(se::DarkGray));
	layerDisplay->setPosition({ 0.5_parent, 0.3_parent });
	layerDisplay->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Left);
	layerDisplay->setHoverProperties({});
	layerDisplay->setPressedProperties({});
	root.addChild(layerDisplay);
	layerDisplayText = std::dynamic_pointer_cast<GUIText>(layerDisplay->getChild(0));


	demoContext.showWindowDefault();
	demoContext.mainWindow.setWidth(800);
	demoContext.mainWindow.setHeight(400);
}
bool MusicManager::update()
{
	if (!playing)
	{
		return true;
	}

	currentBar = timeToBars(commonLayer.getStreamPosition()) % (lastBar + 1);
	if (queuedLayerChangeOnBar == currentBar)
	{
		queuedLayerChangeOnBar = -1;
		fadeChange(se::time::Time::zero);
	}
	const auto beats = (timeToBeats(commonLayer.getStreamPosition()) % beatsPerBar) + 1;
	timerText->clear();
	timerText->insert((currentBar > 9 ? "" : "0") + std::to_string(currentBar) + ":0" + std::to_string(beats));

	return true;
}
int MusicManager::timeToBeats(se::time::Time _time)
{
	return static_cast<int>(_time.asSeconds() * (beatsPerMinute / 60.0f));
}
int MusicManager::timeToBars(se::time::Time _time)
{
	return timeToBeats(_time) / beatsPerBar;
}
void MusicManager::togglePause()
{
	se::audio::Group group;
	group.addVoice(commonLayer);
	group.addVoice(themeLayer);
	group.addVoice(actionLayer);
	group.setPause(playing);
	playing = !playing;

	if (startTime == se::time::Time::zero)
	{
		startTime = se::time::now();
	}

	playButtonText->clear();
	playButtonText->insert(playing ? "PAUSE" : "PLAY");
}
void MusicManager::queueChange()
{
	if (!playing)
	{
		return;
	}

	queuedLayerChangeOnBar = currentBar + 1;
	if (queuedLayerChangeOnBar > lastBar)
	{
		queuedLayerChangeOnBar = 0;
	}
}
void MusicManager::fadeChange(se::time::Time _fade)
{
	if (!playing)
	{
		return;
	}

	const bool isActionLayerActive = actionLayer.getVolume() == 1.0f;
	themeLayer.setVolume(isActionLayerActive ? 1.0f : 0.0f, _fade);
	actionLayer.setVolume(isActionLayerActive ? 0.0f : 1.0f, _fade);
	layerDisplayText->clear();
	layerDisplayText->insert(isActionLayerActive ? "THEME" : "ACTION");
}
