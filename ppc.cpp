
#include "ppc.h"

#include "m33.h"

#include "framebuffer.h"

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovr = hfov / 180.0f * 3.1415926f;
	float f = (float)w / (2.0f * tanf(hfovr / 2.0f));
	c = V3(-(float)w / 2.0f, (float)h / 2.0f, -f);

}

int PPC::Project(V3 P, V3& Q) {

	M33 cam;
	cam.SetColumn(0, a);
	cam.SetColumn(1, b);
	cam.SetColumn(2, c);
	M33 cami = cam.Inverted();
	Q = cami * (P - C);
	if (Q[2] < 0.0f)
		return 0;
	Q[0] = Q[0] / Q[2];
	Q[1] = Q[1] / Q[2];
	Q[2] = 1.0f / Q[2];
	return 1;
}

void PPC::Translate(V3 tv) {

	C = C + tv;

}


void PPC::SetPose(V3 newC, V3 newVD, V3 newUpG) {

	float f = GetF();
	V3 newPP = newC + newVD * f;
	V3 newa = (newVD ^ newUpG).UnitVector();
	V3 newb = (newVD ^ newa).UnitVector();
	V3 newc = newPP - newa * ((float)w / 2.0f) - newb * ((float)h / 2.0f) - newC;

	a = newa;
	b = newb;
	c = newc;
	C = newC;

}


void PPC::RenderWF(FrameBuffer* fb, float visf, PPC* visppc) {

	float scf = visf / GetF();
	V3 bv(0.0f, 0.0f, 0.0f);
	fb->Render3DSegment(
		C,
		C + c * scf, bv, bv, visppc);
	fb->Render3DSegment(
		C + c * scf,
		C + (c + a * (float)w) * scf,
		bv, bv, visppc);
	fb->Render3DSegment(
		C + (c + a * (float)w) * scf,
		C + (c + a * (float)w + b * (float)h) * scf,
		bv, bv, visppc);
	fb->Render3DSegment(
		C + (c + a * (float)w + b * (float)h) * scf,
		C + (c + b * (float)h) * scf,
		bv, bv, visppc);
	fb->Render3DSegment(
		C + (c + b * (float)h) * scf,
		C + c * scf,
		bv, bv, visppc);

}


V3 PPC::GetVD() {

	V3 ret;
	ret = (a ^ b).UnitVector();
	return ret;
}

float PPC::GetF() { return c * GetVD(); }

V3 PPC::GetRay(int u, int v) {

	V3 ret = c + a * (.5f + (float)u) + b * (.5f + (float)v);
	ret = ret.UnitVector();
	return ret;

}

void PPC::SetViewHW() {

	SetIntrinsicsHW();
	SetExtrinsicsHW();

}

void PPC::SetIntrinsicsHW() {

	float nearz = 0.25f;
	float farz = 1000.0f;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float f = GetF();
	float scalef = nearz / f;
	glFrustum(
		-(float)w / 2.0f * scalef,
		+(float)w / 2.0f * scalef,
		-(float)h / 2.0f * scalef,
		+(float)h / 2.0f * scalef,
		nearz,
		farz);

}

void PPC::SetExtrinsicsHW() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	V3 LAP = C + GetVD() * 100.0f;
	gluLookAt(C[0], C[1], C[2],
		LAP[0], LAP[1], LAP[2],
		-b[0], -b[1], -b[2]);

}


//My code

void PPC::Pan(float theta) {
	M33 rotation;
	rotation.SetRotY(theta);
	a = rotation * a;
	b = rotation * b;
	c = rotation * c;
}

void PPC::Tilt(float theta) {
	M33 rotation;
	rotation.SetRotX(theta);
	a = rotation * a;
	b = rotation * b;
	c = rotation * c;
}

void PPC::Roll(float theta) {
	M33 rotation;
	rotation.SetRotZ(theta);
	a = rotation * a;
	b = rotation * b;
	c = rotation * c;
}

void PPC::Zoom(float zoomFactor) {
	c = c * zoomFactor;
}

//// Interpolation: Linear interpolation between two cameras
//PPC PPC::Interpolate(PPC& other, float t) {
//	PPC result = *this;
//	result.C = C + (t * (other.C - C));
//	result.a = a + (t * (other.a - a));
//	result.b = b + (t * (other.b - b));
//	result.c = c + (t * (other.c - c));
//	return result;
//}
