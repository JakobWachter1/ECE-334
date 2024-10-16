#include "scene.h"
#include "V3.h"
#include "m33.h"
#include "ppc.h"
#include "tm.h"
#include <sstream>
#include <iomanip>

Scene* scene;

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
	float hfov = 60.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	V3 startingC = ppc->C;
	V3 startingVD = (ppc->a ^ ppc->b).UnitVector();


	gui = new GUI();
	gui->show();
	gui->uiw->position(u0, v0 + fb->h + v0);

	tmsN = 1;
	tms = new TM[tmsN];
	
	tmsN1 = 1;
	tms1 = new TM[tmsN1];

	tms[0].LoadBin("geometry/tree0.bin");
	tms[0].onFlag = 1;
	tms1[0].LoadBin("geometry/tree1.bin");
	tms1[0].onFlag = 1;
	V3 centroid = tms[0].Centroid();

	tms[0].Translate(V3(0.0f, 0.0f, -150.0f) - centroid);
	tms1[0].Translate(V3(0.0f, 0.0f, -150.0f) - centroid);
	tms1[0].Translate(V3(5.0f, 5.0f, 0.0f));
	centroid = V3(0.0f, 0.0f, -150.0f);

	sm = 1;
	ka = .5;
	lv = V3(0, 0, 1);
	float bottomY = tms[0].GetBottomBoundingAxis();

	TM plane;
	plane.CreatePlane(V3(-25.0f, bottomY - 1.0f, -200.0f), 50.0f, 50.0f);
	plane.onFlag = 1;
	tms[1] = plane;
	tmsN++;
	tms[1].SetAllColors(V3(1.0f, 0.5f, .5f));

	tms[0].Light(ka, lv);
	tms1[0].Light(ka, lv);
	tms[1].Light(ka, lv);
	ppc->Translate(V3(0.0f, 0.0f, -120.0f));
	// Ensure it renders along with the teapot
	Render();

}


void Scene::SM1() {
	Render();
	Fl::check();
	ppc->Translate(V3(0.0f, 0.0f, -5.0f));
}

void Scene::LightTop() {

	lv = V3(0.0f, 1.0f, 0.0f);
	tms[0].Light(ka, lv);
	Render();
	fb->redraw();

}

void Scene::LightSide() {
	Render();
	Fl::check();
	ppc->Translate(V3(0.0f, 0.0f, 5.0f));
}

void Scene::LightRotate() {
	V3 lv(0.0f, 0.0f, 1.0f);
	for (int fi = 0; fi < 100; fi++) {

		tms[0].Light(ka, lv);
		tms1[0].Light(ka, lv);
		tms[1].Light(ka, lv);
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

		lv = lv.RotateThisPointdAboutAxis(V3(0.0f, 0.0f, 0.0f), V3(0.0f, 0.1f, 0.0f), 0.1f);

	}

	Render();
	return;
}

void Scene::LightBrighten() {
	ka += .1;
	tms[0].Light(ka, lv);
	tms1[0].Light(ka, lv);
	tms[1].Light(ka, lv);
	Render();
}

void Scene::LightDim() {
	ka -= .1;
	tms[0].Light(ka, lv);
	tms1[0].Light(ka, lv);
	tms[1].Light(ka, lv);
	Render();
}

void Scene::SM2() {
	for (int fi = 0; fi < 10; fi++) {

		Render();  // Re-render the scene after moving the camera
		Fl::check();  // Handle any UI updates
	}
}

// Modify SM3 function to use Tilt for vertical movement
void Scene::SM3() {
	for (int fi = 0; fi < 10; fi++) {
		// Tilt the camera vertically, simulating looking up and down
		ppc->Tilt(0.5f);  // Adjust the tilting speed

		Render();  // Re-render the scene after tilting the camera
		Fl::check();  // Handle any UI updates
	}
}

// Modify Color function to use Roll for rotating the camera
void Scene::Color() {
	for (int fi = 0; fi < 10; fi++) {
		// Roll the camera around its axis to simulate rotation
		ppc->Roll(0.5f);  // Adjust the rolling speed here

		Render();  // Re-render the scene after rolling the camera
		fb->redraw();
		Fl::check();  // Handle any UI updates
	}
}

void Scene::Render() {

	if (!fb)
		return;
	fb->Set(0xFFFFFFFF);
	for (int tmi = tmsN - 1; tmi >= 0; tmi--) {
		tms[tmi].RenderFilled(fb, ppc);
	}
	for (int tmi = tmsN1 - 1; tmi >= 0; tmi--) {
		tms1[tmi].RenderFilled(fb, ppc);
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

void Scene::CenterCameraOnCentroid() {
	// Get the centroid of the object (assuming tms[0] is the teapot)
	V3 centroid = tms[0].Centroid();

	// Define a new camera position (you can adjust this based on how far the camera should be)
	V3 cameraPosition = centroid + V3(0.0f, 0.0f, 200.0f);  // Adjust the distance as needed

	// Reorient the camera to look at the centroid of the teapot
	ppc->SetPose(cameraPosition, centroid, V3(0.0f, 1.0f, 0.0f));  // Up vector is Y-axis

	// Render the scene after re-centering the camera
	Render();
	fb->redraw();
}

