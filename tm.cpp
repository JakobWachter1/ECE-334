#include "tm.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <strstream>

#include <math.h>
#include "scene.h"


void TM::AllocateMemory() {

	verts = new V3[vertsN];
	colors = new V3[vertsN];
	normals = new V3[vertsN];
	texCoords = new V3[vertsN];
	tris = new unsigned int[3 * trisN];

}

void TM::SetRectangle(float rw, float rh) {

	vertsN = 4;
	trisN = 2;
	AllocateMemory();

	verts[0] = V3(-rw / 2.0f, +rh / 2.0f, 0.0f);
	verts[1] = V3(-rw / 2.0f, -rh / 2.0f, 0.0f);
	verts[2] = V3(+rw / 2.0f, -rh / 2.0f, 0.0f);
	verts[3] = V3(+rw / 2.0f, +rh / 2.0f, 0.0f);

	texCoords[0] = V3(0.f, 0.f, 0.0f);
	texCoords[1] = V3(0.f, 1.f, 0.0f);
	texCoords[2] = V3(1.f, 1.f, 0.0f);
	texCoords[3] = V3(1.f, 0.f, 0.0f);

	int tri = 0;
	tris[tri * 3 + 0] = 0;
	tris[tri * 3 + 1] = 1;
	tris[tri * 3 + 2] = 2;
	tri++;
	tris[tri * 3 + 0] = 2;
	tris[tri * 3 + 1] = 3;
	tris[tri * 3 + 2] = 0;
	tri++;

}

void TM::Translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}

}

V3 TM::Centroid() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++) {
		ret = ret + verts[vi];
	}
	ret = ret * (1.0f / (float)vertsN);
	return ret;

}

void TM::RenderWF(FrameBuffer* fb, PPC* ppc) {
	if (!onFlag)
		return;

	// go over all triangles
	for (int tri = 0; tri < trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		for (int vi = 0; vi < 3; vi++) {


			vinds[vi] = tris[3 * tri + vi];
			tvs[vi] = verts[vinds[vi]];
			if (!ppc->Project(tvs[vi], pvs[vi]))
				continue;
			//			fb->RasterizeCircle(pvs[vi], 3, 0xFF000000);
		}
		for (int ei = 0; ei < 3; ei++) {
			fb->Rasterize2DSegment(pvs[ei], pvs[(ei + 1) % 3],
				colors[vinds[ei]], colors[vinds[(ei + 1) % 3]]);
		}

	}
	// for current triangle go over all three edges
	// for each edge, project 3D vertices defining it (endpoints)
	// draw 2D segment connecting endpoint projections

}

void TM::LoadBin(char* fname) {

	ifstream ifs(fname, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}

	ifs.read((char*)&vertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (verts)
		delete verts;
	verts = new V3[vertsN];

	ifs.read(&yn, 1); // cols 3 floats
	if (colors) {
		delete colors;
		colors = 0;
	}
	if (yn == 'y') {
		colors = new V3[vertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete normals;
	normals = 0;
	if (yn == 'y') {
		normals = new V3[vertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float* tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[vertsN * 2];
	}


	ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

	if (colors) {
		ifs.read((char*)colors, vertsN * 3 * sizeof(float)); // load cols
	}

	if (normals)
		ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}


void TM::ScaleInPlace(float scf) {
	V3 oldCenter = Centroid();
	Translate(oldCenter * -1.0f);
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] * scf;
	Translate(oldCenter);

}


void TM::Rotate(V3 aO, V3 aD, float theta) {

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi].RotateThisPointdAboutAxis(aO, aD, theta);


}


void TM::SetAllColors(V3 c) {

	if (!colors)
		return;

	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = c;

}

void TM::Light(float ka, V3 lv, V3 ev) {

	if (!colors)
		return;

	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = colors[vi].LightThisColor(lv, ka, normals[vi], ev);

}


void TM::RenderFilledMode(FrameBuffer* fb, PPC* ppc) {

	if (!onFlag)
		return;

	V3 centroid = Centroid();
	V3 L = scene->L;

	//	SetAllColors(V3(1.0f, 0.0f, 0.0f));

	M33 colorM, normalM, texCoordsM;
	// go over all triangles
	for (int tri = 0; tri < trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		AABB aabb;
		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tris[3 * tri + vi];
			tvs[vi] = verts[vinds[vi]];
			colorM[vi] = colors[vinds[vi]];
			normalM[vi] = normals[vinds[vi]];
			if (texCoords) {
				texCoordsM[vi] = texCoords[vinds[vi]];
			}
			if (!ppc->Project(tvs[vi], pvs[vi]))
				continue;
			aabb.AddPoint(pvs[vi]);
			//#define SM2
#ifdef SM2
			V3 lv = (L - tvs[vi]).UnitVector();
			colors[vinds[vi]] = V3(1.0f, 0.0f, 0.0f);
			colors[vinds[vi]] = colors[vinds[vi]].LightThisColor(lv, 0.2f, normals[vinds[vi]]);
#endif
		}

		if (!aabb.Clip(fb->w, fb->h))
			continue;
		int left = (int)aabb.minC[0];
		int right = (int)aabb.maxC[0];
		int top = (int)aabb.minC[1];
		int bottom = (int)aabb.maxC[1];
		M33 eeqs;
		eeqs = SetEEQS(pvs);

		M33 ssim = SetSSI(pvs);
		M33 colorABC = (ssim * colorM).Transpose();
		M33 normalABC = (ssim * normalM).Transpose();
		M33 texCoordABC = (ssim * texCoordsM).Transpose();

		M33 VC;
		VC[0] = tvs[0] - ppc->C;
		VC[1] = tvs[1] - ppc->C;
		VC[2] = tvs[2] - ppc->C;
		VC = VC.Transpose();
		M33 camM;
		camM.SetColumn(0, ppc->a);
		camM.SetColumn(1, ppc->b);
		camM.SetColumn(2, ppc->c);
		M33 Q = VC.Inverted() * camM;
		V3 msDenABC = Q[0] + Q[1] + Q[2];
		V3 numsABC = Q.Transpose() * texCoordsM.GetColumn(0);
		V3 numtABC = Q.Transpose() * texCoordsM.GetColumn(1);

		V3 wABC = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);

		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				// if not inside triangle continue
				V3 pixc(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 sid = eeqs * pixc;
				if (sid[0] < 0.0f || sid[1] < 0.0f || sid[2] < 0.0f)
					continue;

				// PIXEL PROGRAM (SHADER)

								// if not visible continue
				float curr1_w = wABC * pixc;
				// fb->IsFartherElseSet(u, v, curr1_w)
				if (fb->IsFarther(u, v, curr1_w))
					continue;
				fb->SetZB(u, v, curr1_w);
				V3 currNormal = normalABC * pixc;
				currNormal = currNormal.UnitVector();
				V3 P = ppc->C +
					(ppc->a * pixc[0] + ppc->b * pixc[1] + ppc->c) * (1.0f / curr1_w);
				V3 currColorv = colorABC * pixc;
				V3 e = (ppc->C - P).UnitVector();
				V3 r = currNormal.ReflectRayAboutThisNormal(e);
				// LOOKUP reflected ray into cubemap
// Visualization of reflected rays
#if 0
				int visstep = 50;
				if (u % visstep == 0 && v % visstep == 0) {
					fb->Render3DPoint(P, V3(0.0f, 0.0f, 0.0f), ppc, 5);
					V3 R;
					R = P + r.UnitVector() * 5.0f;
					fb->Render3DSegment(P, R,
						V3(0.0f, 0.0f, 0.0f), V3(1.0f, 0.0f, 0.0f), ppc);
					fb->Render3DPoint(R, V3(1.0f, 0.0f, 0.0f), ppc, 5);
					R = P + currNormal * 5.0f;
					fb->Render3DSegment(P, R,
						V3(0.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 1.0f), ppc);
					fb->Render3DPoint(R, V3(0.0f, 0.0f, 1.0f), ppc, 5);
				}
#endif
				// end visualization
				if (tfb) {
					V3 currTexCoords;
					if (0) {
						V3 currTexCoords = texCoordABC * pixc;
					}
					else {
						currTexCoords[0] = (numsABC * pixc) / (msDenABC * pixc);
						currTexCoords[1] = (numtABC * pixc) / (msDenABC * pixc);
					}
					float s = currTexCoords[0] - (float)((int)currTexCoords[0]);
					float t = currTexCoords[1] - (float)((int)currTexCoords[1]);
					int tu = (int)(s * (float)tfb->w);
					int tv = (int)(t * (float)tfb->h);
					currColorv.SetFromColor(tfb->Get(tu, tv));
				}

				//				V3 lv = (L - P).UnitVector();
				//				currColorv = currColorv.LightThisColor(lv, 0.2f, currNormal, e);

				// END PIXEL PROGRAM (SHADER)

								// set color
				fb->Set(u, v, currColorv.GetColor());
			}
		}

	}


}

/*
x-x0      y-y0
----  =   -------
x1-x0     y1-y0

x(y1 - y0) + y(-x1 + x0) - x0(y1 - y0) + y0(x1 - x0) = 0
*/

M33 TM::SetEEQS(V3* pvs) {

	M33 ret;
	for (int ei = 0; ei < 3; ei++) {
		float x0 = pvs[ei][0];
		float y0 = pvs[ei][1];
		float x1 = pvs[(ei + 1) % 3][0];
		float y1 = pvs[(ei + 1) % 3][1];
		float x2 = pvs[(ei + 2) % 3][0];
		float y2 = pvs[(ei + 2) % 3][1];
		ret[ei] = V3(y1 - y0, -x1 + x0, -x0 * (y1 - y0) + y0 * (x1 - x0));
		if (ret[ei] * V3(x2, y2, 1.0f) < 0.0f)
			ret[ei] = ret[ei] * -1.0f;
	}
	return ret;
}

// r(u, v) = Au+Bv+C
// r0 = Au0+Bv0+C*1
// r1 = Au1+Bv1+C
// r2 = Au2+Bv2+C
// [ABC]^T = M^-1 [r0 r1 r2]^T
M33 TM::SetSSI(V3* pvs) {

	M33 trim;
	trim[0] = pvs[0];
	trim[1] = pvs[1];
	trim[2] = pvs[2];
	trim.SetColumn(2, V3(1.0f, 1.0f, 1.0f));
	return trim.Inverted();

}



void TM::RenderHW() {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// vertices will be provided in an array
	glEnableClientState(GL_VERTEX_ARRAY);
	// here are the vertices
	glVertexPointer(3, GL_FLOAT, 0, (float*)verts);

	// vertex colors will be provided in an array
	glEnableClientState(GL_COLOR_ARRAY);
	// here are the vertices
	glColorPointer(3, GL_FLOAT, 0, (float*)colors);


	// draw the triangle mesh
	glDrawElements(GL_TRIANGLES, 3 * trisN, GL_UNSIGNED_INT, tris);


	// done using color array
	glDisableClientState(GL_COLOR_ARRAY);

	// done using array of vertices
	glDisableClientState(GL_VERTEX_ARRAY);

}

AABB TM::GetAABB() {

	AABB aabb;
	for (int vi = 0; vi < vertsN; vi++)
		aabb.AddPoint(verts[vi]);

	return aabb;

}
