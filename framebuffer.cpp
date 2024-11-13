
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) :
	Fl_Gl_Window(u0, v0, _w, _h, 0) {

	ishw = 0;
	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
	zb = new float[w * h];
}

void FrameBuffer::draw() {

	if (!ishw) {
		glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	}
	else {
		scene->RenderHW();
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	}

}

int FrameBuffer::handle(int event) {

	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			return 0;
		V3 color; color.SetFromColor(Get(u, v));
		if (this == scene->hwfb) {
			color = (color - V3(0.5f, 0.5f, 0.5f)) * 128.0f;
		}
		cerr << u << " " << v << " " << color << "         \r";
		return 0;
	}
	default:
		return 0;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle() {

	int key = Fl::event_key();
	switch (key) {
	case FL_Left: {
		cerr << "INFO: pressed left arrow key";
		break;
	}
	default:
		cerr << "INFO: do not understand keypress" << endl;
		return;
	}

}

// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w * h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char* fname) {

	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

void FrameBuffer::Set(unsigned int bgr) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			pix[(h - 1 - v) * w + u] = bgr;
		}
	}

}

void FrameBuffer::SetGuarded(int u, int v, unsigned int col) {

	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return;

	Set(u, v, col);

}

void FrameBuffer::Set(int u, int v, unsigned int col) {

	pix[(h - 1 - v) * w + u] = col;

}


void FrameBuffer::RasterizeRectangle(int u0, int v0, int rw, int rh,
	unsigned int col) {

	if (!ClipRectangle(u0, v0, rw, rh))
		return;

	for (int v = v0; v < v0 + rh; v++) {
		for (int u = u0; u < u0 + rw; u++) {
			Set(u, v, col);
			//			SetGuarded(u, v, col);
		}
	}

}

// return 0 if entire rectangle is off screen
int FrameBuffer::ClipRectangle(int& u0, int& v0, int& rw, int& rh) {

	if (u0 > w - 1 || u0 + rw - 1 < 0 ||
		v0 > h - 1 || v0 + rh - 1 < 0)
		return 0;

	if (u0 < 0) {
		rw = rw - (-u0);
		u0 = 0;
	}

	if (v0 < 0) {
		rh = rh - (-v0);
		v0 = 0;
	}

	if (u0 + rw - 1 > w - 1) {
		rw = w - u0;
	}

	if (v0 + rh - 1 > h - 1) {
		rh = h - v0;
	}


	return 1;
}

void FrameBuffer::RasterizeCircle(V3 center, float radius,
	unsigned int color) {

	int u0, v0, rw, rh;
	u0 = (int)(center[0] - radius + 0.5f);
	v0 = (int)(center[1] - radius);
	rh = rw = (int)(2.0f * radius - 0.5f);

	if (!ClipRectangle(u0, v0, rw, rh))
		return;

	center[2] = 0.0f;
	float r2 = radius * radius;
	for (int v = v0; v <= v0 + rh; v++) {
		for (int u = u0; u <= u0 + rw; u++) {
			V3 pixCenter(.5f + (float)u, .5f + (float)v, 0.0f);
			float d2 = (pixCenter - center) * (pixCenter - center);
			if (d2 > r2)
				continue;
			Set(u, v, color);
		}
	}

}


// p = p0 + (p1-p0)*t
void FrameBuffer::Rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1) {

	// find the largest span (horizontal or vertical)
	float hspan = fabsf(p0[0] - p1[0]);
	float vspan = fabsf(p0[1] - p1[1]);
	int stepsN = (hspan < vspan) ? (int)(vspan) : (int)hspan;
	stepsN += 1;
	// that's how many steps n on the segment
  // walk on segment and take n steps
	for (int stepi = 0; stepi <= stepsN; stepi++) {
		// for each step, set current pixel
		float t = (float)stepi / (float)stepsN;
		V3 p = p0 + (p1 - p0) * t;
		V3 cc = c0 + (c1 - c0) * t;
		int u = (int)p[0];
		int v = (int)p[1];
		if (u < 0 || v < 0 || u > w - 1 || v > h - 1)
			continue;
		if (IsFarther(u, v, p[2]))
			continue;
		SetZB(u, v, p[2]);
		SetGuarded(u, v, cc.GetColor());
	}

}


void FrameBuffer::Render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC* ppc) {

	V3 pp0;
	if (!ppc->Project(p0, pp0))
		return;
	V3 pp1;
	if (!ppc->Project(p1, pp1))
		return;

	Rasterize2DSegment(pp0, pp1, c0, c1);
	return;
}


void FrameBuffer::ClearZB() {

	for (int uv = 0; uv < w * h; uv++)
		zb[uv] = 0.0f;

}




int FrameBuffer::IsFarther(int u, int v, float z) {

	if (GetZB(u, v) > z)
		return 1;

	return 0;

}

float FrameBuffer::GetZB(int u, int v) {

	return zb[(h - 1 - v) * w + u];

}


void FrameBuffer::SetZB(int u, int v, float z) {

	zb[(h - 1 - v) * w + u] = z;

}


void FrameBuffer::SetChecker(int csize, V3 c0, V3 c1) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			int cv = v / csize;
			int cu = u / csize;
			if ((cu + cv) % 2)
				Set(u, v, c0.GetColor());
			else
				Set(u, v, c1.GetColor());
		}
	}

}


unsigned int FrameBuffer::Get(int u, int v) {

	return pix[(h - 1 - v) * w + u];

}


void FrameBuffer::Render3DPoint(V3 p, V3 c, PPC* ppc, int psize) {

	V3 pp;
	if (!ppc->Project(p, pp))
		return;

	for (int v = (int)pp[1] - psize / 2; v < (int)pp[1] + psize / 2; v++) {
		for (int u = (int)pp[0] - psize / 2; u < (int)pp[0] + psize / 2; u++) {
			if (IsFarther(u, v, pp[2]))
				continue;
			SetZB(u, v, pp[2]);
			Set(u, v, c.GetColor());
		}
	}

}

void FrameBuffer::SetBackgroundEM(/*CM *cm, */PPC* ppc) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			V3 eyeRay = ppc->GetRay(u, v);
			unsigned int col = 0xFF0000FF;
			// col = cm->LookUp(eyeRay);
			Set(u, v, col);
		}
	}

}



//My Code

void FrameBuffer::RasterizeTriangle(V3 p0, V3 p1, V3 p2, V3 c0, V3 c1, V3 c2) {
	int minX = std::max(0, (int)std::floor(std::min({ p0[0], p1[0], p2[0] })));
	int maxX = std::min(w - 1, (int)std::ceil(std::max({ p0[0], p1[0], p2[0] })));
	int minY = std::max(0, (int)std::floor(std::min({ p0[1], p1[1], p2[1] })));
	int maxY = std::min(h - 1, (int)std::ceil(std::max({ p0[1], p1[1], p2[1] })));

	for (int u = minX; u <= maxX; ++u) {
		for (int v = minY; v <= maxY; ++v) {
			V3 p(u + 0.5f, v + 0.5f, 0.0f);
			float area = EdgeFunction(p0, p1, p2);
			float w0 = EdgeFunction(p1, p2, p) / area;
			float w1 = EdgeFunction(p2, p0, p) / area;
			float w2 = EdgeFunction(p0, p1, p) / area;

			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				V3 color = c0 * w0 + c1 * w1 + c2 * w2;

				Set(u, v, color.GetColor());
			}
		}
	}
}

float FrameBuffer::EdgeFunction(V3& a, V3& b, V3& c) {
	return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
}



void FrameBuffer::RasterizeTriangleWithNormals(V3 p0, V3 p1, V3 p2, V3 n0, V3 n1, V3 n2, V3 lv, float ka) {
	int minX = std::max(0, (int)std::floor(std::min({ p0[0], p1[0], p2[0] })));
	int maxX = std::min(w - 1, (int)std::ceil(std::max({ p0[0], p1[0], p2[0] })));
	int minY = std::max(0, (int)std::floor(std::min({ p0[1], p1[1], p2[1] })));
	int maxY = std::min(h - 1, (int)std::ceil(std::max({ p0[1], p1[1], p2[1] })));

	for (int u = minX; u <= maxX; ++u) {
		for (int v = minY; v <= maxY; ++v) {
			V3 p(u + 0.5f, v + 0.5f, 0.0f);
			float area = EdgeFunction(p0, p1, p2);
			float w0 = EdgeFunction(p1, p2, p) / area;
			float w1 = EdgeFunction(p2, p0, p) / area;
			float w2 = EdgeFunction(p0, p1, p) / area;

			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				V3 interpolatedNormal = (n0 * w0 + n1 * w1 + n2 * w2).UnitVector();

				float kd = std::max(0.0f, interpolatedNormal * lv);
				V3 color = V3(1.0f, 1.0f, 1.0f) * (ka + (1.0f - ka) * kd);

				Set(u, v, color.GetColor());
			}
		}
	}
}

