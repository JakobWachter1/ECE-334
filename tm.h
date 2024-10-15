#pragma once

#include "v3.h"
#include "framebuffer.h"
#include "ppc.h"

class TM {
public:
	V3* verts, * colors, * normals;
	int vertsN;
	int onFlag;
	unsigned int* tris;
	int trisN;
	TM() : verts(0), vertsN(0), tris(0), trisN(0), colors(0), normals(0) {};
	void SetRectangle(float rw, float rh);
	void AllocateMemory();
	void Translate(V3 tv);
	void RenderWF(FrameBuffer* fb, PPC* ppc);
	V3 Centroid();
	void Position(V3& newCenter);
	void LoadBin(char* fname);
	void ScaleInPlace(float scf);
	void Rotate(V3 aO, V3 aD, float theta);
	void SetAllColors(V3 c);
	void Light(float ka, V3 lv);
	void RenderFilled(FrameBuffer* fb, PPC* ppc);
	void RenderFilledSM2(FrameBuffer* fb, PPC* ppc, V3 lv, float ka);
	void RenderFilledSM3(FrameBuffer* fb, PPC* ppc, V3 lv, float ka);
};
