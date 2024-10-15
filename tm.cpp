#include "tm.h"


using namespace std;

#include <iostream>
#include <fstream>
#include <strstream>
#include <math.h>
#include <FL/math.h>



void TM::AllocateMemory() {

	verts = new V3[vertsN];
	colors = new V3[vertsN];
	normals = new V3[vertsN];
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

void TM::Position(V3& newCenter) {
	V3 currentCenter = Centroid();
	V3 translation = newCenter - currentCenter;
	for (int i = 0; i < vertsN; i++) {
		verts[i] = verts[i] + translation;
	}
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

void TM::Light(float ka, V3 lv) {

	if (!colors)
		return;

	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = colors[vi].LightThisColor(lv, ka, normals[vi]);
		
}

void TM::RenderFilled(FrameBuffer* fb, PPC* ppc) {
	if (!onFlag)
		return;

	for (int tri = 0; tri < trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		V3 colors3[3];
		unsigned int vinds[3];

		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tris[3 * tri + vi];
			tvs[vi] = verts[vinds[vi]];
			colors3[vi] = colors[vinds[vi]];

			if (!ppc->Project(tvs[vi], pvs[vi])) {
				continue;
			}

		}

		fb->RasterizeTriangle(pvs[0], pvs[1], pvs[2], colors3[0], colors3[1], colors3[2]);
	}
}

void TM::RenderFilledSM2(FrameBuffer* fb, PPC* ppc, V3 lv, float ka) {
	if (!onFlag)
		return;

	for (int tri = 0; tri < trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		V3 colors3[3];
		unsigned int vinds[3];

		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tris[3 * tri + vi];
			tvs[vi] = verts[vinds[vi]];
			colors3[vi] = colors[vinds[vi]];

			if (!ppc->Project(tvs[vi], pvs[vi])) {
				continue;
			}

			colors3[vi] = colors[vinds[vi]].LightThisColor(lv, ka, normals[vinds[vi]]);
		}

		fb->RasterizeTriangle(pvs[0], pvs[1], pvs[2], colors3[0], colors3[1], colors3[2]);
	}
}

void TM::RenderFilledSM3(FrameBuffer* fb, PPC* ppc, V3 lv, float ka) {
	if (!onFlag)
		return;

	for (int tri = 0; tri < trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		V3 normals3[3];
		unsigned int vinds[3]; 

		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tris[3 * tri + vi];
			tvs[vi] = verts[vinds[vi]];

			if (!ppc->Project(tvs[vi], pvs[vi])) {
				continue;
			}

			normals3[vi] = normals[vinds[vi]];
		}
		fb->RasterizeTriangleWithNormals(pvs[0], pvs[1], pvs[2], normals3[0], normals3[1], normals3[2], lv, ka);
	}
}