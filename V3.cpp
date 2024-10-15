
#include "V3.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>
#include "m33.h"

V3::V3(float x, float y, float z) {

	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;

}
//addition
V3 V3::operator+(V3 v1) {

	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] + v1[0];
	ret[1] = v0[1] + v1[1];
	ret[2] = v0[2] + v1[2];
	return ret;

}
//subtraction
V3 V3::operator-(V3 v1) {
	
	V3 ret;
	V3& v0 = *this;
	ret[0] = v0[0] - v1[0];
	ret[1] = v0[1] - v1[1];
	ret[2] = v0[2] - v1[2];
	return ret;

}

//dot product
float V3::operator*(V3 v1) {

	V3 &v0 = *this;
	float ret = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
	return ret;

}

//cross product
V3 V3::operator^(V3 v1) {

	V3& v0 = *this;
	return V3(
		v0[1] * v1[2] - v0[2] * v1[1],
		v0[2] * v1[0] - v0[0] * v1[2],
		v0[0] * v1[1] - v0[1] * v1[0]
	);

}
//scalar multiplication
V3 V3::operator*(float scalar) {
	
	V3& v0 = *this;
	return V3(v0[0] * scalar, v0[1] * scalar, v0[2] * scalar);

}
//scalar division
V3 V3::operator/(float scalar) {

	V3& v0 = *this;
	return V3(v0[0] / scalar, v0[1] / scalar, v0[2] / scalar);

}

float& V3::operator[](int i) {

	return xyz[i];

}

ostream& operator<<(ostream& ostr, V3 v) {

	return ostr << v[0] << " " << v[1] << " " << v[2] << endl;

}

V3 V3::Normalize(V3 v) {
	return v / sqrt(v * v);
}

V3 V3::RotatePointAroundAxis(V3 point, V3 axis, float angle) {
	M33 rotationMatrix;
	rotationMatrix.SetRotationAxisAngle(axis, angle);
	return rotationMatrix * point;
}

V3 V3::RotateThisPointdAboutAxis(V3 aO, V3 aD, float theta) {

	// build coordinate system with aO as origin and aD as one axis
	M33 cs;
	cs[1] = aD.UnitVector();
	V3 aux(1.0f, 0.0f, 0.0f);
	if (fabsf(aD[0]) > fabsf(aD[1]))
		aux = V3(0.0f, 1.0f, 0.0f);
	cs[2] = (aD ^ aux).UnitVector();
	cs[0] = cs[1] ^ cs[2];


	V3& P = *this;
	// convert to cs
	V3 Pl = cs * (P - aO);

	// rotate in cs
	M33 roty; roty.SetRotY(theta);
	V3 PlR = roty * Pl;

	M33 cs_1; cs_1 = cs.Inverted();
	// come back to world
	V3 ret = cs_1 * PlR + aO;
	return ret;

}



float V3::Length() {

	V3& v = *this;
	float ret;
	ret = sqrtf(v * v);
	return ret;

}

V3 V3::UnitVector() {

	V3& v = *this;
	V3 ret = v * (1.0f / Length());
	return ret;

}


void V3::SetFromColor(unsigned int color) {

	unsigned char* rgb = (unsigned char*) &color;
	V3& v = *this;
	v[0] = ((float) rgb[0]) / 255.0f;
	v[1] = ((float)rgb[1]) / 255.0f;
	v[2] = ((float)rgb[2]) / 255.0f;

}

unsigned int V3::GetColor() {

	unsigned int ret;
	unsigned int rgb[3];
	V3& v = *this;
	for (int ci = 0; ci < 3; ci++) {
		float fc = v[ci];
		if (fc < 0.0f)
			fc = 0.0f;
		if (fc > 1.0f)
			fc = 1.0f;
		rgb[ci] = (unsigned int)(255.0f * fc);
	}
	ret = 0xFF000000 + rgb[2] * 256 * 256 + rgb[1] * 256 + rgb[0];

	return ret;
}

V3 V3::LightThisColor(V3 lv, float ka, V3 normal) {

	V3& colorv = *this;

	float kd = lv * normal;
	kd = (kd < 0.0f) ? 0.0f : kd;
	V3 ret = colorv * (ka + (1.0f - ka) * kd);
	return ret;
}