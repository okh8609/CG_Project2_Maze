#include "Helper.h"

// 4X4矩陣 乘 4X4矩陣 
// 輸出為一個 4X4矩陣
void Helper::matrix44_X_matrix44(float A[4][4], float B[4][4], float RR[4][4])
{
	const int size = 4;

	float** R = new float*[4]; // result: float R[4][4]
	for (int i = 0; i < 4; ++i)
		R[i] = new float[4];

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			float num = 0;
			for (int k = 0; k < size; ++k)
			{
				num += A[i][k] * B[k][j];
			}
			R[i][j] = num;
		}
	}
	//return R;
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			RR[i][j] = R[i][j];
		}
	}
}

// 4X4矩陣 乘 4維向量
// 輸出為一個四維的向量
//float* matrix44_X_vector4(float M[4][4], float V[4])
float * Helper::matrix44_X_vector4(float M[4][4], float V[4])
{
	float *R = new float[4]; //result
	R[0] = V[0] * M[0][0] + V[1] * M[0][1] + V[2] * M[0][2] + V[3] * M[0][3];
	R[1] = V[0] * M[1][0] + V[1] * M[1][1] + V[2] * M[1][2] + V[3] * M[1][3];
	R[2] = V[0] * M[2][0] + V[1] * M[2][1] + V[2] * M[2][2] + V[3] * M[2][3];
	R[3] = V[0] * M[3][0] + V[1] * M[3][1] + V[2] * M[3][2] + V[3] * M[3][3];
	return R;
}

// 4維向量 Normalize
//  input: 四維向量
// output: 三維向量
float * Helper::norm_vector4(float V[4])
{
	float *R = new float[3]; //result
	R[0] = V[0] / V[3];
	R[1] = V[1] / V[3];
	R[2] = V[2] / V[3];
	return R;
}

Helper::Helper()
{
}


Helper::~Helper()
{
}
