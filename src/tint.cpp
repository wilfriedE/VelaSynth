#include "vela.hpp"

/**
 	A module for TintFilter
 **/
struct TintFilter : Module {
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

	Shader* shader;
    GLuint texture;
	int	og_texture, tex_w, tex_h;
	TintFilter() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
        shader  = new Shader(assetPlugin(plugin, "res/shaders/v_shader.glsl"), assetPlugin(plugin, "res/shaders/f_shader.glsl"));
        Image baseImg = Image(assetPlugin(plugin, "res/blank.png"));
        updateTexture(baseImg.handle); 
    }

	void step() override;

	void updateTexture(int new_texture){
		if(og_texture == new_texture){ return; } // skip update if original texture is same as new
        og_texture = new_texture;
        glGetTexLevelParameteriv(new_texture, 0, GL_TEXTURE_WIDTH, &tex_w);
        glGetTexLevelParameteriv(new_texture, 0, GL_TEXTURE_HEIGHT, &tex_h);

        texture = (GLuint) og_texture; // let texture = new_texture before applying filters
	}

    void applyFilters(NVGcontext *vg, NVGLUframebuffer* fb){
        int pxRatio = 1.0;
        double t = glfwGetTime();
        ////sdkjsjkdn
        int winWidth, winHeight;
        int fboWidth, fboHeight;
        int pw, ph, x, y;
        float s = 20.0f;
        float sr = (cosf(t)+1)*0.5f;
        float r = s * 0.6f * (0.2f + 0.8f * sr);

        if (fb == NULL) {
            info("FBO is null");
            return;
        }
        nvgImageSize(vg, fb->image, &fboWidth, &fboHeight);
        winWidth = (int)(fboWidth / pxRatio);
        winHeight = (int)(fboHeight / pxRatio);

        // Draw some stuff to an FBO as a test
        nvgluBindFramebuffer(fb);
        glViewport(0, 0, fboWidth, fboHeight);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

        pw = (int)ceilf(winWidth / s);
        ph = (int)ceilf(winHeight / s);

        nvgBeginPath(vg);
        for (y = 0; y < ph; y++) {
            for (x = 0; x < pw; x++) {
                float cx = (x+0.5f) * s;
                float cy = (y+0.5f) * s;
                nvgCircle(vg, cx,cy, r);
            }
        }
        nvgFillColor(vg, nvgRGBA(220,160,0,200));
        nvgFill(vg);

        nvgEndFrame(vg);
        nvgluBindFramebuffer(NULL);

	}
};

void TintFilter::step() {
    updateTexture((int) inputs[SRC_INPUT].value);
	// update output ref to texture handle
	outputs[REF_OUTPUT].value = (float) texture;
};


struct TintFilterWidget : ModuleWidget {
	TintFilterWidget(TintFilter *module);
    void draw(NVGcontext *vg) override;
};

void TintFilterWidget::draw(NVGcontext *vg){
    TintFilter* mdl = (TintFilter*) module;

    NVGLUframebuffer* fb = nvgluCreateFramebuffer(vg, mdl->tex_w, mdl->tex_h, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
    mdl->applyFilters(vg, fb);
    ModuleWidget::draw(vg);
};

TintFilterWidget::TintFilterWidget(TintFilter *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/imageinput.svg")));

	box.size = Vec(20 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    addInput(Port::create<PJ301MPort>(Vec(130, 300), Port::INPUT, module, TintFilter::SRC_INPUT));
	addOutput(Port::create<PJ301MPort>(Vec(190, 300), Port::OUTPUT, module, TintFilter::REF_OUTPUT));
};

Model *modelTintFilter = Model::create<TintFilter, TintFilterWidget>("Vela", "TintFilter", "Tint Filter", VISUAL_TAG);