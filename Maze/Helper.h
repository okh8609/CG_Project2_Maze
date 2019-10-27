#pragma once
class Helper
{
public:
	static void matrix44_X_matrix44(float A[4][4], float B[4][4], float R[4][4]);
	static float* matrix44_X_vector4(float M[4][4], float V[4]);
	static void norm_vector4(float V[4]); //have functional sied effect

	Helper();
	~Helper();
};

