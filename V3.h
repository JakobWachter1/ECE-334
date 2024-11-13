
#pragma once

#include <ostream>
#include <istream>

using namespace std;

class V3 {
public:
	float xyz[3];
	V3() {};
	V3(float x, float y, float z);
	float& operator[](int i);
	V3 Normalize(V3 v);
	V3 operator+(V3 v1);
	V3 operator-(V3 v1);
	float operator*(V3 v1);
	V3 operator^(V3 v1);
	V3 operator*(float scalar);  //scalar multiplication
	V3 operator/(float scalar);  //scalar division
	V3 RotatePointAroundAxis(V3 point, V3 axis, float angle);
	V3 RotateThisPointdAboutAxis(V3 aO, V3 aD, float theta);
	float Length();
	V3 UnitVector();
	// cerr << v0 << "; v2: " << v2 << endl;
	friend ostream& operator<<(ostream& ostr, V3 v);
	void SetFromColor(unsigned int color);
	unsigned int GetColor();
	V3 LightThisColor(V3 lv, float ka, V3 normal, V3 ev);
	V3 ReflectRayAboutThisNormal(V3 ray);
};

class AABB {
public:
	V3 minC, maxC;
	AABB() : minC(FLT_MAX, FLT_MAX, FLT_MAX),
		maxC(-FLT_MAX, -FLT_MAX, -FLT_MAX) {};
	void AddPoint(V3 newPoint);
	int Clip(int w, int h);
};
