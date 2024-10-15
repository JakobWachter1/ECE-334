#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tm.h"

class Scene {
public:

	GUI* gui;
	FrameBuffer* fb;
	FrameBuffer* fb1;
	PPC* ppc;
	PPC* ppc3;
	TM* tms;
	V3 centroid;
	int sm;
	float ka;
	V3 lv;
	int tmsN;
	Scene();
	void SM1();
	void SM2();
	void SM3();
	void LightTop();
	void LightSide();
	void LightRotate();
	void LightDim();
	void Color();
	void LightBrighten();
	void Render();
	void RenderSM2();
	void RenderSM3();
	void AppendValuesToFile(V3& a, V3& b, V3& c, V3& C, int w, int h, const std::string& filename);
};

extern Scene* scene;