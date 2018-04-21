#pragma once
#include "rack.hpp"
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include "opencv2/opencv.hpp"
#include "shader.hpp"
// Hack to get framebuffer objects working on OpenGL 2 (we blindly assume the extension is supported)
#define NANOVG_FBO_VALID 1
#include "nanovg_gl_utils.h"

using namespace rack;

extern Plugin *plugin;

////////////////////
// module widgets
////////////////////

////////////////////////////////////////////// WIDGETS //////////////////////////////////////////////

extern Model *modelMediaSource;
extern Model *modelPlainViewer;
extern Model *modelTintFilter;