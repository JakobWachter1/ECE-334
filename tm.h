#pragma once

#include "v3.h"
#include "framebuffer.h"
#include "ppc.h"
#include "m33.h"

class TM {
public:
	V3* verts, * colors, * normals, * texCoords;
	int onFlag;
	int vertsN;
	unsigned int* tris;
	int trisN;
	FrameBuffer* tfb;
	TM() : onFlag(1), verts(0), vertsN(0), tris(0), trisN(0),
		colors(0), normals(0), texCoords(0), tfb(0) {};
	void SetRectangle(float rw, float rh);
	void AllocateMemory();
	void Translate(V3 tv);
	void RenderWF(FrameBuffer* fb, PPC* ppc);
	void RenderFilledMode(FrameBuffer* fb, PPC* ppc);
	void RenderHW();
	V3 Centroid();
	void LoadBin(char* fname);
	void ScaleInPlace(float scf);
	void Rotate(V3 aO, V3 aD, float theta);
	void SetAllColors(V3 c);
	void Light(float ka, V3 lv, V3 ev);
	M33 SetEEQS(V3* pvs);
	M33 SetSSI(V3* pvs);
	AABB GetAABB();
};
