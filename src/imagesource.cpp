#include "vsynth.hpp"
#include "../ext/osdialog/osdialog.h"

/**
 	A Module for ImageSource
 **/
struct  ImageSourceModule : MediaSource {
	void step() override;
};

void ImageSourceModule::step() {};

ImageSourceWidget::ImageSourceWidget() {
	ImageSourceModule *module = new ImageSourceModule();
	setModule(module);

	box.size = Vec(40 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/imageinput.svg")));
		addChild(panel);

		addChild(module->preview(Vec(50,65)));
	}
	


	{
		info("file open\n");
		char *filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
		if (filename) {
			info("\t%s\n", filename);
			free(filename);
		}
		else {
			info("\tCanceled\n");
		}
	}


	addOutput(createOutput<PJ301MPort>(Vec(560, 300), module, ImageSourceModule::REF_OUTPUT));
};
