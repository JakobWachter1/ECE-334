#pragma once

#include <ostream>
#include <istream>

using namespace std;

#include "V3.h"

class M33 {
public:
	V3 rows[3];
	M33();
	M33(V3 r0, V3 r1, V3 r2);
	V3& operator[](int i);
	M33(bool identity);
	void SetRotX(float alpha);
	void SetRotY(float alpha);
	void SetRotZ(float alpha);
	V3 operator*(V3 v);
	M33 operator*(M33& m);
	friend ostream& operator<<(ostream& ostr, M33 m);
	V3 GetColumn(int i);
	void SetColumn(int i, V3 v);
	M33 Inverted();
	M33 Inverse();
	M33 Transpose();
	void SetRotationAxisAngle(V3 axis, float angle);
};
