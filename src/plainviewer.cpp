#include "vsynth.hpp"

/**
 	A Module for viewing things
 **/
struct PlainViewer : Module {
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
	
	int texture; // empty image base

	PlainViewer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {};
	void step() override;
};

void PlainViewer::step() {
	// Keep textures in sync
	texture = (int) inputs[SRC_INPUT].value;
	outputs[REF_OUTPUT].value = (float) texture;
};


struct ViewerDisplay : SpriteWidget {
	PlainViewer *module;

	ViewerDisplay() {};

	void draw(NVGcontext *vg)  override;
};

void ViewerDisplay::draw(NVGcontext *vg) {
	int width, height;
	nvgImageSize(vg, module->texture, &width, &height);
	int stride = width / spriteSize.x;
	if (stride == 0) {
		warn("Size of SpriteWidget is %d, %d but spriteSize is %f, %f", width, height, spriteSize.x, spriteSize.y);
		return;
	}
	Vec offset = Vec((index % stride) * spriteSize.x, (index / stride) * spriteSize.y);
	NVGpaint paint = nvgImagePattern(vg, spriteOffset.x - offset.x, spriteOffset.y - offset.y, width, height, 0.0,  module->texture, 1.0);
	nvgFillPaint(vg, paint);
	nvgBeginPath(vg);
	nvgRect(vg, spriteOffset.x, spriteOffset.y, spriteSize.x, spriteSize.y);
	nvgFill(vg);
};

struct PlainViewerWidget : ModuleWidget {
	PlainViewerWidget(PlainViewer *module);
};

PlainViewerWidget::PlainViewerWidget(PlainViewer *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/plainviewer.svg")));

	box.size = Vec(40 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		ViewerDisplay* view = new ViewerDisplay();
		view->module = module;
		view->spriteSize = Vec(500, 380);
		view->spriteOffset = Vec(120,0);
		addChild(view);
	}

	addInput(Port::create<PJ301MPort>(Vec(50, 160), Port::INPUT, module, PlainViewer::SRC_INPUT));
	addOutput(Port::create<PJ301MPort>(Vec(50, 280), Port::OUTPUT, module, PlainViewer::REF_OUTPUT));
};

Model *modelPlainViewer = Model::create<PlainViewer, PlainViewerWidget>("VSynth", "PlainViewer", "Plain Viewer", VISUAL_TAG);