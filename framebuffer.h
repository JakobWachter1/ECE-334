#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int* pix; // pixel array // 0xAABBGGRR; // red 0xFF0000FF
	int w, h;
	float *zb;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
	void Set(unsigned int bgr);
	void Set(int u, int v, unsigned int col);
	void SetGuarded(int u, int v, unsigned int col);
	void RasterizeRectangle(int u0, int v0, int l, int h, unsigned int col);
	int ClipRectangle(int& u0, int& v0, int& rw, int& rh);
	void RasterizeCircle(V3 center, float radius, unsigned int color);
	void Rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1);
	void Render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC* ppc);
	void ClearZB();
	int IsFarther(int u, int v, float z);
	void SetZB(int u, int v, float z);
	void RasterizeTriangle(V3 p0, V3 p1, V3 p2, V3 c0, V3 c1, V3 c2);
	float EdgeFunction(V3& a, V3& b, V3& c);
	void Rasterize3DSphere(V3 center, float radius, V3 color, PPC* ppc);
	void RasterizeTriangleWithNormals(V3 p0, V3 p1, V3 p2, V3 n0, V3 n1, V3 n2, V3 lv, float ka);
	float GetZB(int u, int v);
};