#pragma once

#include <ostream>
#include <istream>

using namespace std;

#include "V3.h"
#include "framebuffer.h"

class PPC {
public:
	V3 a, b, c, C;
	int w, h;
	PPC(float hfov, int w, int h);
	int Project(V3 P, V3& Q);
	void Translate(V3 tv);
	void SetPose(V3 newC, V3 newVD, V3 newUpG);
	void RenderWF(FrameBuffer* fb, float visf, PPC* visppc);
	V3 GetVD();
	float GetF();
	void Pan(float theta);
	void Tilt(float theta);
	void Roll(float theta);
	void Zoom(float zoomFactor);
	//PPC Interpolate(PPC& other, float t);
};
