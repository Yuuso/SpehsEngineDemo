#pragma once

#include "Demo/DemoContext.h"
#include "Demo/Demos/DemoApplication.h"
#include "SpehsEngine/Audio/AudioSource.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/GUI/GUIText.h"


class MusicManager final : public DemoApplication
{
public:

	MusicManager(DemoContext& _demoContext);
	~MusicManager();

	void init() override;
	bool update() override;

private:

	int timeToBeats(se::time::Time _time);
	int timeToBars(se::time::Time _time);
	void togglePause();
	void queueChange();
	void fadeChange(se::time::Time _fade);

	se::legacygui::GUIView view;
	se::legacygui::GUIShape root;
	std::shared_ptr<se::legacygui::GUIText> playButtonText;
	std::shared_ptr<se::legacygui::GUIText> timerText;
	std::shared_ptr<se::legacygui::GUIText> layerDisplayText;

	const float beatsPerMinute = 120.0;
	const int beatsPerBar = 4;

	se::time::Time startTime;
	bool playing = false;
	int lastBar = -1;
	int currentBar = -1;
	int queuedLayerChangeOnBar = -1;

	se::audio::AudioSource themeLayer;
	se::audio::AudioSource commonLayer;
	se::audio::AudioSource actionLayer;
	se::audio::Bus outputBus;
};
