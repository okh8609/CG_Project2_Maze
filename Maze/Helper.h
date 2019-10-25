#pragma once
class Helper
{
public:
	static float** matrix44_X_matrix44(float A[4][4], float B[4][4]);
	static float* matrix44_X_vector4(float **M, float V[4]);
	static float* norm_vector4(float V[4]);

	Helper();
	~Helper();
};

