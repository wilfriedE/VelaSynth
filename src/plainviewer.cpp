#include "vsynth.hpp"

/**
 	A Module for viewing things
 **/
struct  PlainViewer : Viewer {
	void step() override;

	void update() override;

	SpriteWidget *preview(Vec pos) override;
};


void PlainViewer::step() {
	updateSrcInput(inputs[SRC_INPUT].value); // Check Source input and if source input is ref to new source then update
};

void PlainViewer::update() {
	if(media_source != NULL){
		cv::resize(media_source->imgFrame, imgFrame, cv::Size(480, 380));
		frameToImage();
	}
	updateViewers();
}

SpriteWidget *PlainViewer::preview(Vec pos) {
	SpriteWidget *view = new SpriteWidget();
	view->spriteOffset = pos;
	view->spriteSize = Vec(380, 480);
	view->spriteImage = image;
	return view;
};

PlainViewerWidget::PlainViewerWidget() {
	PlainViewer *module = new PlainViewer();
	setModule(module);

	box.size = Vec(40 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/plainviewer.svg")));
		addChild(panel);

		addChild(module->preview(Vec(120,0)));
	}

	addInput(createInput<PJ301MPort>(Vec(50, 160), module, PlainViewer::SRC_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(50, 280), module, PlainViewer::REF_OUTPUT));
};
