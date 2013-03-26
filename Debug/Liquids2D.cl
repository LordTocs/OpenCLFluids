__constant const sampler_t texsampler = CLK_NORMALIZED_COORDS_FALSE |
								     CLK_ADDRESS_CLAMP_TO_EDGE  |
								     CLK_FILTER_NEAREST;

__constant const sampler_t smoothsampler = CLK_NORMALIZED_COORDS_FALSE |
								     CLK_ADDRESS_CLAMP_TO_EDGE  |
								     CLK_FILTER_LINEAR;									 
							 

__kernel void Jacobi2D (__write_only image2d_t result, __read_only image2d_t prev, __read_only image2d_t b, const float alpha, const float beta)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	
	float4 valueup = read_imagef    (prev, texsampler, ((float2)(x,y + 1)));
	float4 valuedown = read_imagef  (prev, texsampler, ((float2)(x,y - 1)));
	
	float4 valueleft = read_imagef  (prev, texsampler, ((float2)(x - 1,y)));
	float4 valueright = read_imagef (prev, texsampler, ((float2)(x + 1,y)));
	
	float4 bC = read_imagef (b, texsampler, ((float2)(x,y)));
	
	float4 resultvalue = (valueup + valuedown + valueleft + valueright + alpha * bC) / beta;
	write_imagef (result, ((int2)(x,y)), resultvalue);
}

__kernel void Advect2D (__write_only image2d_t Result, __read_only image2d_t Prev, float dt, __read_only image2d_t Vel, float dissipation)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	
	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	
	float4 vel = read_imagef (Vel,texsampler,((float2)(x,y)));
	
	float2 coord = ((float2)(x,y)) - vel.xy * dt;
	
	float4 result = dissipation * read_imagef (Prev,smoothsampler,coord + ((float2)(0.5,0.5)));
	
	write_imagef (Result,((int2)(x,y)),result);
}


__kernel void Divergence2D (__read_only image2d_t Velocity, __write_only image2d_t Divergence, const float halfinvdx)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	
	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	
	float2 velup =    read_imagef (Velocity, texsampler, ((float2)(x, y + 1))).xy;
	float2 veldown =  read_imagef (Velocity, texsampler, ((float2)(x, y - 1))).xy;
	float2 velleft =  read_imagef (Velocity, texsampler, ((float2)(x - 1, y))).xy;
	float2 velright = read_imagef (Velocity, texsampler, ((float2)(x + 1, y))).xy;
	
	const float div = halfinvdx*(velup.y - veldown.y + velright.x - velleft.x);
	write_imagef (Divergence,(int2)(x,y),((float4)(div,div,div,div)));	
}

__kernel void Gradient2D (__read_only image2d_t Pressure, __read_only image2d_t Velocity, __write_only image2d_t VelocityResult, const float halfinvdx)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	
	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);

	float pup    = read_imagef (Pressure, texsampler, ((float2)(x,y + 1))).w;
	float pdown  = read_imagef (Pressure, texsampler, ((float2)(x,y - 1))).w;
	float pleft  = read_imagef (Pressure, texsampler, ((float2)(x - 1,y))).w;
	float pright = read_imagef (Pressure, texsampler, ((float2)(x + 1,y))).w;
	
	float4 result = read_imagef (Velocity, texsampler, ((float2)(x,y)));
	result.xy -= halfinvdx * ((float2)(pright - pleft, pup - pdown));
	write_imagef (VelocityResult,((int2)(x,y)),result);
}

__kernel void Reset (__write_only image2d_t zerome)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	
	float4 result = ((float)(0,0,0,0));
	write_imagef (zerome,((int2)(x,y)),result);
}

__kernel void VelSet (__write_only image2d_t vel, unsigned int x, unsigned int y, float dx, float dy)
{
	const unsigned int px = get_global_id(0) - get_global_size(0)/2 + x;
	const unsigned int py = get_global_id(1) - get_global_size(1)/2 + y;
	
	float dist = (px - x)*(px - x) + (py - y)*(py - y);
	
	if (dist < (get_global_size(0)/2)*(get_global_size(0)/2))
	{
		write_imagef (vel,((int2)(px,py)),((float4)(dx,dy,0,1)));
	}
}

__kernel void InkSet (__write_only image2d_t dens, unsigned int x, unsigned int y, uchar4 color)
{
	const unsigned int px = get_global_id(0) - get_global_size(0)/2 + x;
	const unsigned int py = get_global_id(1) - get_global_size(1)/2 + y;
	
	float dist = (px - x)*(px - x) + (py - y)*(py - y);
	
	if (dist < (get_global_size(0)/2)*(get_global_size(0)/2))
	{
		write_imagef (dens,((int2)(px,py)),((float4)(color.x/255.0f, color.y/255.0f, color.z/255.0f,color.w/255.0f)));
	}
}


