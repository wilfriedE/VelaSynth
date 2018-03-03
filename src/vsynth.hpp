#pragma once
#include "rack.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include "opencv2/imgproc/imgproc.hpp"
#include <unordered_map>
#include "viewer.cpp"
#include "mediasource.cpp"

using namespace rack;

extern Plugin *plugin;

extern std::unordered_map<float, Module*> output_refs;

////////////////////
// module widgets
////////////////////

struct ImageSourceWidget : ModuleWidget {
	ImageSourceWidget();
};

struct PlainViewerWidget : ModuleWidget {
	PlainViewerWidget();
};
