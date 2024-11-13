#include "scene.h"
#include "V3.h"
#include "m33.h"
#include "ppc.h"
#include "tm.h"
#include <sstream>
#include <iomanip>

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>
#include <strstream>

Scene::Scene() {

	int u0 = 20;
	int v0 = 40;
	int h = 400;
	int w = 600;
	texfb = 0;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("SW Framebuffer");
	fb->show();
	fb->redraw();

	hwfb = new FrameBuffer(u0, v0, w, h);
	hwfb->ishw = 1;
	hwfb->position(u0 + w + u0, v0);
	hwfb->label("HW Framebuffer");
	hwfb->show();
	hwfb->redraw();

	float hfov = 60.0f;
	ppc = new PPC(hfov, fb->w, fb->h);

	gui = new GUI();
	gui->show();
	gui->uiw->position(u0, v0 + fb->h + v0);

	tmsN = 1;
	tms = new TM[tmsN];

	// SHADERS
	cgi = 0;
	soi = 0;
	needHWInit = 1;
	// END SHADERS
}


void Scene::Button1() {
	cerr << endl;

	{
		V3 n(0.0f, 0.0f, 1.0f);
		V3 e(0.0f, 0.0f, 10.0f);
		V3 r = n.ReflectRayAboutThisNormal(e);
		//		cerr << r << endl;
		//		return;
		tms[0].LoadBin("geometry/teapot1K.bin");
		//`		tms[0].LoadBin("geometry/teapot57K.bin");
		tms[0].onFlag = 1;
		V3 centroid = tms[0].Centroid();

		//		tms[0].SetAllColors(V3(1.0f, 0.0f, 0.0f));

		float centroidz = -100.0f;
		tms[0].Translate(V3(0.0f, 0.0f, centroidz) - centroid);
		centroid = V3(0.0f, 0.0f, centroidz);

		ppc->SetPose(centroid + V3(0.0f, 100.0f, 0.0f), V3(0.0f, -1.0f, 0.0f),
			V3(0.0f, 0.0f, -1.0f));

		Render();
		return;
	}

}

void Scene::Button2() {

	float sidelength = 30.0f;
	float rw = 50.0f;
	//tms[0].SetCube(sidelength);
	tms[0].SetAllColors(V3(1.0f, 0.0f, 0.0f));
	tms[0].Translate(V3(0.0f, 0.0f, -150.0f));

	texfb = new FrameBuffer(50 + fb->w + 50, 50, 256, 256);
	texfb->label("texture");
	texfb->SetChecker(32, V3(0.7f, 0.7f, 0.7f), V3(0.0f, 0.0f, 0.0f));

	//texfb->LoadTiff("mydbg/image.tif");

	texfb->show();

	tms[0].tfb = texfb;

	for (int fi = 0; fi < 360; fi++) {
		Render();
		Fl::check();
		//tms[0].Rotate(tms[0].Centroid(), V3(0.0f, 1.0f, 0.0f), 1.0f);
	}
	return;

}

void Scene::Button3() {
	float sidelength = 40.0f;
	float rw = 50.0f;
	//tms[0].SetCube(sidelength);
	tms[0].SetAllColors(V3(1.0f, 0.0f, 0.0f));
	tms[0].Translate(V3(0.0f, 0.0f, -150.0f));

	texfb = new FrameBuffer(50 + fb->w + 50, 50, 256, 256);
	texfb->label("texture");
	texfb->SetChecker(32, V3(0.7f, 0.7f, 0.7f), V3(0.0f, 0.0f, 0.0f));

	texfb->LoadTiff("img2.tiff");

	texfb->show();

	tms[0].tfb = texfb;

	for (int fi = 0; fi < 360; fi++) {
		Render();
		Fl::check();
		//tms[0].Rotate(tms[0].Centroid(), V3(0.0f, 1.0f, 0.0f), 1.0f);
	}
	return;
}

void Scene::Button4() {
	tms[0].LoadBin("geometry/teapot1K.bin");
	tms[0].onFlag = 1;
	V3 centroid = tms[0].Centroid();
	float centroidz = -150.0f;
	tms[0].Translate(V3(0.0f, 0.0f, centroidz) - centroid);
	centroid = V3(0.0f, 0.0f, centroidz);

	AABB aabb = tms[0].GetAABB();
	cerr << "INFO: aabb diagonal: " << (aabb.minC - aabb.maxC).Length() << endl;
	cerr << "INFO: centroid: " << centroid << endl;
	//		L = ppc->C + (centroid - ppc->C)*0.81f;
	L = ppc->C + (centroid - ppc->C) * 0.5f;
	exp = 1.0f;

	cerr << endl;
	for (int fi = 0; fi < 720; fi++) {
		Render();
		//			fb->Render3DPoint(L, V3(1.0f, 1.0f, 0.0f), ppc, 7);
		fb->redraw();
		hwfb->redraw();
		Fl::check();
		break;
		//			L = L + V3(0.1f, 0.0f, 0.0f);
		tms[0].Rotate(centroid, V3(0.0f, 1.0f, 0.0f), 1.0f);
		exp += .1f;
		cerr << "Specular exponent: " << exp << "         \r";
	}
	return;
}

void Scene::Button5() {

}

void Scene::Button6() {

}

void Scene::Button7() {
}

void Scene::Button8() {
}

void Scene::Button9() {
}


void Scene::Render() {

	if (!fb)
		return;
	fb->Set(0xFFFFFFFF);
	//	fb->ClearZB();
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderFilledMode(fb, ppc);
	}
	fb->redraw();

}

void Scene::RenderHW() {

	if (!hwfb)
		return;

	// SHADERS
	if (needHWInit) {
		cerr << "INFO: per session initialization" << endl;
		cgi = new CGInterface;
		cgi->PerSessionInit();
		soi = new ShaderOneInterface;
		soi->PerSessionInit(cgi);
		needHWInit = 0;
	}

	soi->PerFrameInit();

	// END SHADERS

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 0.7f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ppc->SetViewHW();

	for (int tmi = 0; tmi < tmsN; tmi++) {
		if (!tms[tmi].onFlag)
			continue;
		// this mesh is to be rendered with shaders
		cgi->EnableProfiles();
		// more specifically, this mesh is to be rendered with ShaderOne
		soi->BindPrograms();
		tms[tmi].RenderHW();
		// disable shaders, maybe other meshes don't need shaders
		cgi->DisableProfiles();
	}

	// SHADERS
	soi->PerFrameDisable();
	// END SHADERS

}


void Scene::AppendValuesToFile(V3& a, V3& b, V3& c, V3& C, int w, int h, const std::string& filename) {
	std::ofstream outFile;
	outFile.open(filename, std::ios_base::app);

	if (!outFile) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return;
	}

	outFile << "a: " << a << "\n";
	outFile << "b: " << b << "\n";
	outFile << "c: " << c << "\n";
	outFile << "C: " << C << "\n";
	outFile << "w: " << w << "\n";
	outFile << "h: " << h << "\n";
	outFile << "-------------------\n"; 

	outFile.close();
}
