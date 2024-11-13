#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tm.h"
#include "CGInterface.h"


class Scene {
public:

	GUI* gui;
	FrameBuffer* fb, * hwfb;
	FrameBuffer* texfb;
	// SHADERS
	CGInterface* cgi;
	ShaderOneInterface* soi;
	int needHWInit;
	// END SHADERS
	V3 L;
	float exp;
	PPC* ppc;
	TM* tms;
	int tmsN;
	Scene();
	void Button1();
	void Button2();
	void Button3();
	void Button4();
	void Button5();
	void Button6();
	void Button7();
	void Button8();
	void Button9();
	void Render();
	void RenderHW();
	void AppendValuesToFile(V3& a, V3& b, V3& c, V3& C, int w, int h, const std::string& filename);
};

extern Scene* scene;
