#define Pos(r,c) c+r*size

__kernel void MatrixMult (__global float *A, __global float *B, __global float *Result)
{
	const unsigned int c = get_global_id(0);
	const unsigned int r = get_global_id(1);
	const unsigned int size = get_global_size(0);
	
	Result[Pos(r,c)] = 0;
	
	
	for (int k = 0; k < size; ++k)
	{
		Result[Pos(r,c)] += A[Pos(r,k)] * B[Pos(k,c)];
	}
}