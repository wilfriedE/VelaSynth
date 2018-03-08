#include "vsynth.hpp"
#include "../ext/osdialog/osdialog.h"

/**
 	A module for ImageSource
 **/
struct ImageSource : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		SRC_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		REF_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	std::shared_ptr<Image> image = Image::load(assetPlugin(plugin, "res/blank.png")); // empty image base
	int	 texture = image->handle;
	ImageSource() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	void replaceImage(char *filename){
		image = Image::load(filename);
		texture	= image->handle;
	}
};

void ImageSource::step() {
	// update output ref to texture handle
	outputs[REF_OUTPUT].value = (float) texture;
};

struct ImageLoader : Button {
	ImageSource *module;

	ImageLoader() {};

	void onAction(EventAction &e) override;
};

void ImageLoader::onAction(EventAction &e) {
	info("file open\n");
	char *filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
	if (filename) {
		module->replaceImage(filename);
		free(filename);
	}
	else {
		info("\tCanceled\n");
	}
};

struct ImageSourceWidget : ModuleWidget {
	ImageSourceWidget(ImageSource *module);
};

ImageSourceWidget::ImageSourceWidget(ImageSource *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/imageinput.svg")));

	box.size = Vec(20 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		ImageLoader* imgLoader = new ImageLoader();
		imgLoader->module = module;
		imgLoader->box.size = Vec(180, 38);
		imgLoader->box.pos = Vec(58, 100);
		imgLoader->text = "\n LOAD IMAGE";
		addChild(imgLoader);
	}

	addOutput(Port::create<PJ301MPort>(Vec(190, 300), Port::OUTPUT, module, ImageSource::REF_OUTPUT));
};

Model *modelImageSource = Model::create<ImageSource, ImageSourceWidget>("VSynth", "ImageSource", "Image Media", VISUAL_TAG);