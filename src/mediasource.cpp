#include "vela.hpp"
#include "osdialog.h"

/**
 	A module for MediaSource
 **/
struct MediaSource : Module {
	enum ParamIds {
		CLOCK_PARAM,
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
		IMAGE_LIGHT,
		VIDEO_LIGHT,
		CAMERA_LIGHT,
		NUM_LIGHTS
	};
	enum class Media { 
		IMAGE = 0,
		VIDEO,
		CAMERA
	};

	Media active_media = Media::IMAGE;
	float phase = 0.0;

	std::shared_ptr<Image> image = Image::load(assetPlugin(plugin, "res/blank.png")); // empty image base
	int	 texture = image->handle;
	cv::VideoCapture cap; // null video object
	cv::Mat img_frame;

	MediaSource() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	void useImage(char *filename){
		if( active_media == Media::CAMERA){cap.release();};
		if( active_media != Media::IMAGE){
			active_media = Media::IMAGE;
		}
		image = Image::load(filename);
		texture	= image->handle;
	}

	void useVideo(char *filename){
		if( active_media == Media::CAMERA){cap.release();};
		if( active_media != Media::VIDEO){
			cap.open(filename); // open the default camera
			if(cap.isOpened()){active_media = Media::VIDEO;};
		}
	}

	void useCamera(){
		if( active_media != Media::CAMERA){
			active_media = Media::CAMERA;
			cap.open(0); // open the default camera
			if(cap.isOpened()){active_media = Media::CAMERA;};
		}
	}

	void updateFrame(){
		if (active_media == Media::VIDEO  || active_media == Media::CAMERA) {
			info("Next video frame");
			cap.read(img_frame);

			cv::flip(img_frame, img_frame, 0);
			glGenTextures(1, (GLuint *) &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// Set texture clamping method
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


			glTexImage2D(GL_TEXTURE_2D,     // Type of texture
							0,                 // Pyramid level (for mip-mapping) - 0 is the top level
							GL_RGB,            // Internal colour format to convert to
							img_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
							img_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
							0,                 // Border width in pixels (can either be 1 or 0)
							GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
							GL_UNSIGNED_BYTE,  // Image data type
							img_frame.ptr());        // The actual image data itself

			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	void toggleLights(Media ty){
		lights[IMAGE_LIGHT].value = (ty == Media::IMAGE) ? 1.0f : 0.0f;
		lights[VIDEO_LIGHT].value = (ty == Media::VIDEO) ? 1.0f : 0.0f;
		lights[CAMERA_LIGHT].value = (ty == Media::CAMERA) ? 1.0f : 0.0f;
	}
};

void MediaSource::step() {
	// update output ref to texture handle
	outputs[REF_OUTPUT].value = (float) texture;

	float clockTime = powf(2.0, params[CLOCK_PARAM].value);
	phase += clockTime / engineGetSampleRate();
	if (phase >= 1.0) {
		info("Frame");
		phase -= 1.0;
		updateFrame();
	}

	toggleLights(active_media);
};

struct ImageLoader : Button {
	MediaSource *module;

	ImageLoader() {};

	void onAction(EventAction &e) override {
		info("file open\n");
		char *filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
		if (filename) {
			module->useImage(filename);
			free(filename);
		}
		else {
			info("\tCanceled\n");
		}
	};
};

struct VideoLoader : Button {
	MediaSource *module;

	VideoLoader() {};

	void onAction(EventAction &e) override {
		info("file open\n");
		char *filename = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
		if (filename) {
			module->useVideo(filename);
			free(filename);
		}
		else {
			info("\tCanceled\n");
		}
	};
};

struct CameraLoader : Button {
	MediaSource *module;

	CameraLoader() {};

	void onAction(EventAction &e) override {
		module->useCamera();
	};
};


struct Drawer : Button {
	MediaSource *module;
	void draw(NVGcontext *vg) override {
		module->updateFrame();
	}
};


struct MediaSourceWidget : ModuleWidget {
	MediaSourceWidget(MediaSource *module);
};

MediaSourceWidget::MediaSourceWidget(MediaSource *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/imageinput.svg")));

	box.size = Vec(20 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		ImageLoader* imgLoader = new ImageLoader();
		imgLoader->module = module;
		imgLoader->box.size = Vec(180, 38);
		imgLoader->box.pos = Vec(58, 100);
		imgLoader->text = "\n LOAD IMAGE";
		addChild(imgLoader);
		addChild(ModuleLightWidget::create<MediumLight<BlueLight>>(Vec(38, 113), module, MediaSource::IMAGE_LIGHT));

		VideoLoader* vidLoader = new VideoLoader();
		vidLoader->module = module;
		vidLoader->box.size = Vec(180, 38);
		vidLoader->box.pos = Vec(58, 148);
		vidLoader->text = "\n LOAD VIDEO";
		addChild(vidLoader);
		addChild(ModuleLightWidget::create<MediumLight<BlueLight>>(Vec(38, 161), module, MediaSource::VIDEO_LIGHT));

		CameraLoader* camLoader = new CameraLoader();
		camLoader->module = module;
		camLoader->box.size = Vec(180, 38);
		camLoader->box.pos = Vec(58, 196);
		camLoader->text = "\n CAMERA";
		addChild(camLoader);
		addChild(ModuleLightWidget::create<MediumLight<BlueLight>>(Vec(38, 209), module, MediaSource::CAMERA_LIGHT));

		Drawer* ddr = new Drawer();
		ddr->module = module;
		ddr->box.size = Vec(1, 1);
		ddr->box.pos = Vec(1, 1);
		addChild(ddr);
	}

	addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(58, 295), module, MediaSource::CLOCK_PARAM, -2.0, 6.0, 1.0));
	addOutput(Port::create<PJ301MPort>(Vec(190, 300), Port::OUTPUT, module, MediaSource::REF_OUTPUT));
};

Model *modelMediaSource = Model::create<MediaSource, MediaSourceWidget>("Vela", "MediaSource", "Image,Video,Camera Media", VISUAL_TAG);