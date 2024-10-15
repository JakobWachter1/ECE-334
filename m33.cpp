#include "m33.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

M33::M33() {}


M33::M33(V3 r0, V3 r1, V3 r2) {
	rows[0] = r0;
	rows[1] = r1;
	rows[2] = r2;
}

V3& M33::operator[](int i) {

	return rows[i];

}


M33::M33(bool identity) {
	if (identity) {
		rows[0] = V3(1.0f, 0.0f, 0.0f);
		rows[1] = V3(0.0f, 1.0f, 0.0f);
		rows[2] = V3(0.0f, 0.0f, 1.0f);
	}
}
//rotate around x
void M33::SetRotX(float alpha) {
	float rad = alpha * M_PI / 180.0f;
	rows[0] = V3(1.0f, 0.0f, 0.0f);
	rows[1] = V3(0.0f, cos(rad), -sin(rad));
	rows[2] = V3(0.0f, sin(rad), cos(rad));
}

//rotate around y
void M33::SetRotY(float alpha) {
	float rad = alpha * M_PI / 180.0f;
	rows[0] = V3(cos(rad), 0.0f, sin(rad));
	rows[1] = V3(0.0f, 1.0f, 0.0f);
	rows[2] = V3(-sin(rad), 0.0f, cos(rad));
}

//rotate around z
void M33::SetRotZ(float alpha) {
	float rad = alpha * M_PI / 180.0f;
	rows[0] = V3(cos(rad), -sin(rad), 0.0f);
	rows[1] = V3(sin(rad), cos(rad), 0.0f);
	rows[2] = V3(0.0f, 0.0f, 1.0f);
}

/*
m00 m01 m02   v0     (m00*v0 + m01*v1 + m02*v2)
m10 m11 m12   v1  =
m20 m21 m22   v2
*/

V3 M33::operator*(V3 v) {

	V3 ret;
	M33 &m = *this;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;
}


M33 M33::operator*(M33& m) {
	M33 transposed = m.Transpose();
	return M33(
		*this * transposed[0],
		*this * transposed[1],
		*this * transposed[2]
	).Transpose();
}

ostream& operator<<(ostream& ostr, M33 m) {

	return ostr << m[0] << m[1] << m[2];

}


V3 M33::GetColumn(int i) {

	return V3(rows[0][i], rows[1][i], rows[2][i]);

}



M33 M33::Inverted() {

	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a * (1.0f / (a * _a));
	V3 _b = c ^ a; _b = _b * (1.0f / (b * _b));
	V3 _c = a ^ b; _c = _c * (1.0f / (c * _c));
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

void M33::SetColumn(int i, V3 column) {

	M33& m = *this;
	m[0][i] = column[0];
	m[1][i] = column[1];
	m[2][i] = column[2];

}

M33 M33::Inverse() {
	V3 a = rows[0];
	V3 b = rows[1];
	V3 c = rows[2];

	V3 r0 = b^c;
	V3 r1 = c^a;
	V3 r2 = a^b;

	float invDet = 1.0f / (r2 * c);

	return M33(r0 * invDet, r1 * invDet, r2 * invDet).Transpose();
}


M33 M33::Transpose() {
	return M33(GetColumn(0), GetColumn(1), GetColumn(2));
}

void M33::SetRotationAxisAngle(V3 axis, float angle) {
	axis = axis.Normalize(axis);
	float cosA = cos(angle * M_PI / 180.0f);
	float sinA = sin(angle * M_PI / 180.0f);
	float oneMinusCosA = 1.0f - cosA;

	float x = axis[0];
	float y = axis[1];
	float z = axis[2];

	rows[0] = V3(cosA + x * x * oneMinusCosA, x * y * oneMinusCosA - z * sinA, x * z * oneMinusCosA + y * sinA);
	rows[1] = V3(y * x * oneMinusCosA + z * sinA, cosA + y * y * oneMinusCosA, y * z * oneMinusCosA - x * sinA);
	rows[2] = V3(z * x * oneMinusCosA - y * sinA, z * y * oneMinusCosA + x * sinA, cosA + z * z * oneMinusCosA);
}

