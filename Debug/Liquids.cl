#pragma OPENCL EXTENSION cl_khr_3d_image_writes : require

__constant const sampler_t texsampler = CLK_NORMALIZED_COORDS_FALSE |
								     CLK_ADDRESS_CLAMP_TO_EDGE  |
								     CLK_FILTER_NEAREST;

__constant const sampler_t smoothsampler = CLK_NORMALIZED_COORDS_FALSE |
								     CLK_ADDRESS_CLAMP_TO_EDGE  |
								     CLK_FILTER_LINEAR;									 
							 

__kernel void Jacobi3D (__write_only image3d_t result, __read_only image3d_t prev, __read_only image3d_t b, const float alpha, const float beta)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int z = get_global_id(2);

	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	const unsigned int sz = get_global_size(2);
	
	float4 valueup = read_imagef    (prev, texsampler, ((float4)(x,y + 1,z,0)));
	float4 valuedown = read_imagef  (prev, texsampler, ((float4)(x,y - 1,z,0)));
	float4 valueleft = read_imagef  (prev, texsampler, ((float4)(x - 1,y,z,0)));
	float4 valueright = read_imagef (prev, texsampler, ((float4)(x + 1,y,z,0)));
	float4 valuefront = read_imagef (prev, texsampler, ((float4)(x,y,z + 1,0)));
	float4 valueback = read_imagef  (prev, texsampler, ((float4)(x,y,z - 1,0)));
	
	float4 bC = read_imagef (b, texsampler, ((float4)(x,y,z,0)));
	
	float4 resultvalue = (valueup + valuedown + valueleft + valueright + valuefront + valueback + alpha * bC) / beta;
	write_imagef (result, ((int4)(x,y,z,0)), resultvalue);
}

__kernel void Advect3D (__write_only image3d_t Result, __read_only image3d_t Prev, float dt, __read_only image3d_t Vel, float dissipation)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int z = get_global_id(2);

	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	const unsigned int sz = get_global_size(2);
	
	float3 vel = read_imagef (Vel,texsampler,((float4)(x,y,z,0))).xyz;
	
	float3 coord = ((float3)(x,y,z)) - vel * dt;
	
	float4 result = dissipation * read_imagef (Prev,smoothsampler,((float4)(coord,0)) + ((float4)(0.5,0.5,0.5,0.5)));
	
	write_imagef (Result,((int4)(x,y,z,0)),result);
}


__kernel void Divergence3D (__read_only image3d_t Velocity, __write_only image3d_t Divergence, const float halfinvdx)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int z = get_global_id(2);

	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	const unsigned int sz = get_global_size(2);
	
	float3 velup =    read_imagef (Velocity, texsampler, ((float4)(x, y + 1, z, 0))).xyz;
	float3 veldown =  read_imagef (Velocity, texsampler, ((float4)(x, y - 1, z, 0))).xyz;
	float3 velleft =  read_imagef (Velocity, texsampler, ((float4)(x - 1, y, z, 0))).xyz;
	float3 velright = read_imagef (Velocity, texsampler, ((float4)(x + 1, y, z, 0))).xyz;
	float3 velfront = read_imagef (Velocity, texsampler, ((float4)(x, y, z + 1, 0))).xyz;
	float3 velback =  read_imagef (Velocity, texsampler, ((float4)(x, y, z - 1, 0))).xyz;
	
	const float div = halfinvdx*(velup.y - veldown.y + velright.x - velleft.x + velfront.z - velback.z);
	write_imagef (Divergence,(int4)(x,y,z,0),((float4)(div,div,div,div)));	
}
__kernel void Reset (__write_only image3d_t zerome)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int z = get_global_id(1);
	
	float4 result = ((float)(0,0,0,0));
	write_imagef (zerome,((int4)(x,y,z,0)),result);
}

__kernel void VelSet (__write_only image3d_t veltex, int3 pos, float3 vel)
{
	const unsigned int px = get_global_id(0) - get_global_size(0)/2 + pos.x;
	const unsigned int py = get_global_id(1) - get_global_size(1)/2 + pos.y;
	const unsigned int pz = get_global_id(2) - get_global_size(2)/2 + pos.z;
	
	float dist = (px - pos.x)*(px - pos.x) + (py - pos.y)*(py - pos.y) + (pz - pos.z)*(pz - pos.z);
	
	if (dist < (get_global_size(0)/2)*(get_global_size(0)/2)*(get_global_size(0)/2))
	{
		write_imagef (veltex,((int4)(px,py,pz,0)),((float4)(vel,0)));
	}
}

__kernel void InkSet (__write_only image3d_t dens, int3 pos, uchar4 color)
{
	const unsigned int px = get_global_id(0) - get_global_size(0)/2 + pos.x;
	const unsigned int py = get_global_id(1) - get_global_size(1)/2 + pos.y;
	const unsigned int pz = get_global_id(2) - get_global_size(2)/2 + pos.z;
	
	float dist = (px - x)*(px - x) + (py - y)*(py - y) + (pz - z)*(pz - z);
	
	if (dist < (get_global_size(0)/2)*(get_global_size(0)/2)*(get_global_size(0)/2))
	{
		write_imagef (dens,((int4)(px,py,pz,0)),((float4)(color.x/255.0f, color.y/255.0f, color.z/255.0f,color.w/255.0f)));
	}
}

__kernel void Gradient3D (__read_only image3d_t Pressure, __read_only image3d_t Velocity, __write_only image3d_t VelocityResult, const float halfinvdx)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int z = get_global_id(2);

	const unsigned int sx = get_global_size(0);
	const unsigned int sy = get_global_size(1);
	const unsigned int sz = get_global_size(2);

	float pup    = read_imagef (Pressure, texsampler, ((float4)(x,y + 1,z))).w;
	float pdown  = read_imagef (Pressure, texsampler, ((float4)(x,y - 1,z))).w;
	float pleft  = read_imagef (Pressure, texsampler, ((float4)(x - 1,y,z))).w;
	float pright = read_imagef (Pressure, texsampler, ((float4)(x + 1,y,z))).w;
	float pfront = read_imagef (Pressure, texsampler, ((float4)(x,y,z + 1))).w;
	float pback  = read_imagef (Pressure, texsampler, ((float4)(x,y,z - 1))).w;
	
	float4 result = read_imagef (Velocity, texsampler, ((float4)(x,y,z,0)));
	result.xyz -= halfinvdx * ((float3)(pright - pleft, pup - pdown, pfront - pback));
	write_imagef (VelocityResult,((int4)(x,y,z,0)),result);
}
