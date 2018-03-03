#pragma once
#include "rack.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <unordered_map>
#include "mediasource.cpp"

using namespace rack;

extern std::unordered_map<float, Module*> output_refs;

/**
 	A Module that has all the base component of a Viewer
	Base for all Viewer types: Shaders, PlainViewer, StreamViewer
 **/
struct Viewer :  public MediaSource {
	enum InputIds {
		SRC_INPUT, // media source id
		NUM_INPUTS
	};

	float media_source_id;
	MediaSource* media_source = NULL;

	Viewer() : MediaSource() {};

	void updateSrcInput(float src_id) {
		if(src_id == media_source_id){ return; } // different media source?
		if(media_source != NULL){ // unregister current media source
			media_source_id = -1;
			media_source->unregisterViewer(this);
			media_source = NULL;
		}
		if(src_id >= R_FLO){ // valid media source?
			auto m = output_refs.find(src_id);
			if(m != output_refs.end()) {
				media_source = dynamic_cast<MediaSource*>(m->second);
				media_source_id = src_id;
				media_source->registerViewer(this);
			}
		}
	}

	virtual void update() {updateViewers();} // calls update on it's viewers

	void frameToImage() {
		nvgDeleteImage(gVg, image->handle);
		// update image
		std::vector<uchar> buf;
    	cv::imencode(".jpg", imgFrame, buf, std::vector<int>() );
		// encoded image is now in buf (a vector)
		image->handle = nvgCreateImageMem(gVg, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY,&buf[0], buf.size());
	}
};