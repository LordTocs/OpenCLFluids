#pragma OPENCL EXTENSION cl_khr_fp64: enable

__constant double range = 2;

__constant int iterations = 256;

__constant const sampler_t gradient_sampler = CLK_NORMALIZED_COORDS_TRUE |
											  CLK_ADDRESS_CLAMP_TO_EDGE         |
											  CLK_FILTER_NEAREST;

inline double2 cmult(double2 a, double2 b)
{
	return (double2)(a.x * b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

__kernel void Julia (__write_only image2d_t img, double2 C, __read_only image2d_t gradient)
{
	int2 coord = (int2)(get_global_id(0), get_global_id(1));
	int2 halfsize = (int2)(get_global_size(0)/2, get_global_size(1)/2);
	int2 centeredcoord = coord - halfsize;
	double2 z = (double2)(((double)centeredcoord.x)/halfsize.x,
	                      ((double)centeredcoord.y)/halfsize.y) * range;
	
	int k = 0;
	double julialength = 0;
	while ((k < iterations) && (julialength < 100))
	{
		z = cmult(z,z) + C;
		julialength = length(z);
		++k;
	}
	
	float colorval = 0;
	
	colorval = clamp(((float)(k))/(iterations),0.0f,1.0f);
	
	float4 color = read_imagef (gradient,gradient_sampler,(float2)(colorval,0));

	write_imagef(img,coord,color);
}