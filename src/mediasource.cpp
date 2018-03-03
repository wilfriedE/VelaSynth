#pragma once
#include "rack.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <unordered_map>

using namespace rack;

extern std::unordered_map<float, Module*> output_refs;

/**
 	A Module that has all the base component of a MediaSource
	Base for all Input Sources: CamSource, VideoSource, ImageSource
 **/
struct MediaSource : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		SRC_INPUT, // media source id
		NUM_INPUTS
	};
	enum OutputIds {
		REF_OUTPUT,
		NUM_OUTPUTS
	};
	
	float R_FLO = 1.0; // offset number - doesn't really mean much

	float output_refs_id;
 	std::vector<void*> viewers;
	cv::Mat imgFrame = cv::Mat(280, 380, CV_8UC3, cv::Scalar(0,0,0));
	std::shared_ptr<Image> image = Image::load(""); // empty image base


	MediaSource() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS)  {
		output_refs_id = (float) output_refs.size()+R_FLO; //
		outputs[REF_OUTPUT].value = output_refs_id;
		output_refs.insert({output_refs_id, this});
		
		// initialize blank image
		std::vector<uchar> buf;
    	cv::imencode(".jpg", imgFrame, buf, std::vector<int>() );
		// encoded image is now in buf (a vector)
		image->handle = nvgCreateImageMem(gVg, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY,&buf[0], buf.size());
	};

	/**
	 * Keep track of viewers
	 * */
	template <class Viewer>
	void registerViewer(Viewer *v) { 
		Viewer *c = dynamic_cast<Viewer*>(v);
		viewers.push_back(c);
	};

	template <class Viewer>
	void unregisterViewer(Viewer *v) { 
		Viewer *viewer = dynamic_cast<Viewer*>(v);

		// Check that the viewer actually exists
		auto viewer_f = std::find(viewers.begin(), viewers.end(), viewer);
		if(viewer_f != viewers.end()) {
			// Remove it
			viewers.erase(viewer_f);
		}
	};

	cv::Mat* getFrame(){
		return &imgFrame;
	};

	std::shared_ptr<Image> getImage(){
		return image;
	}

	/**
	 * Tells all viewers except itself that there is a change to frame or imageand they should update.
	 **/
	void updateViewers(){
			// TBD
	}

	virtual SpriteWidget *preview(Vec pos) {
		SpriteWidget *view = new SpriteWidget();
		view->spriteOffset = pos;
		view->spriteSize = Vec(380, 280);
		view->spriteImage = image;
		return view;
	}

	/**
	 * Remove MediaSource from output_refs 
	 **/
	 ~MediaSource()
    {
        output_refs.erase(output_refs_id);
    };
	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};
