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

	//Frame 1

	int u0 = 20;
	int v0 = 40;
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("SW Framebuffer");
	fb->show();
	fb->redraw();
	float hfov = 80.0f;
	ppc = new PPC(hfov, fb->w, fb->h);

	

	//Frame 2
	/*int u1 = 320;
	int v1 = 440;
	int h1 = 200;
	int w1 = 300;
	fb1 = new FrameBuffer(u1, v1, w1, h1);
	fb1->position(u1, v1);
	fb1->label("Framebuffer 2");
	fb1->show();
	fb1->redraw();
	float hfov = 60.0f;
	ppc = new PPC(hfov, fb1->w, fb1->h);*/


	gui = new GUI();
	gui->show();
	gui->uiw->position(u0, v0 + fb->h + v0);

	tmsN = 1;
	tms = new TM[tmsN];

	tms[0].LoadBin("geometry/teapot1K.bin");
	//tms[0].LoadBin("geometry/teapot57K.bin");
	tms[0].onFlag = 1;
	V3 centroid = tms[0].Centroid();

	tms[0].Translate(V3(0.0f, 0.0f, -150.0f) - centroid);
	centroid = V3(0.0f, 0.0f, -150.0f);
	sm = 1;
	ka = 0;
	lv = V3(0, 0, 1);
	tms[0].Light(ka, lv);
	Render();
	fb->redraw();
}


void Scene::SM1() {
	cerr << endl;

	{
		sm = 1;
		lv = V3(0, 0, 1);
		tms[0].Light(ka, lv);
		Render();
		fb->redraw();
		
		return;
	}

	{
		cerr << "INFO: pressed DBG" << endl;
		fb->Set(0xFF0000FF);
		fb->redraw();
	}
}

void Scene::LightTop() {

	lv = V3(0.0f, 1.0f, 0.0f);
	tms[0].Light(ka, lv);
	Render();
	fb->redraw();

}

void Scene::LightSide() {
	lv = V3(1.0f, 0.0f, 0.0f);
	tms[0].Light(ka, lv);
	Render();
	fb->redraw();
}

void Scene::LightRotate() {
	V3 lv(0.0f, 0.0f, 1.0f);
	for (int fi = 0; fi < 100; fi++) {

		tms[0].Light(ka, lv);
		if (sm == 1 || sm == 0) {
			Render();
		}
		else if (sm == 2) {
			RenderSM2();
		}
		else {
			RenderSM3();
		}
		fb->redraw();
		Fl::check();

		lv = lv.RotateThisPointdAboutAxis(V3(0.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f), 1.0f);

	}

	Render();
	return;
}

void Scene::LightBrighten() {
	ka += .1;
}

void Scene::LightDim() {
	ka -= .1;
}

void Scene::Color() {
	
	tms[0].SetAllColors(V3(1.0f, 0.0f, 1.0f));
	tms[0].Light(ka, lv);
	Render();
	fb->redraw();
}

void Scene::SM2() {
	sm = 1;
	lv = V3(0, 0, 1);
	tms[0].Light(ka, lv);
	RenderSM2();
	fb->redraw();
}

void Scene::SM3() {
	sm = 1;
	lv = V3(0, 0, 1);
	tms[0].Light(ka, lv);
	RenderSM3();
	fb->redraw();
}


void Scene::Render() {

	if (!fb)
		return;

	fb->Set(0xFFFFFFFF);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderFilled(fb, ppc);
	}
	fb->redraw();

}

void Scene::RenderSM2() {

	if (!fb)
		return;

	fb->Set(0xFFFFFFFF);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderFilledSM2(fb, ppc, lv, ka);
	}
	fb->redraw();

}

void Scene::RenderSM3() {

	if (!fb)
		return;

	fb->Set(0xFFFFFFFF);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderFilledSM3(fb, ppc, lv, ka);
	}
	fb->redraw();

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