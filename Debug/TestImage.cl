//__constant sampler_t sampler = CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;

__kernel void ImageTest (__write_only image2d_t img, const float somevalue)
{
	int2 coord;
	coord.x = get_global_id(0);
	coord.y = get_global_id(1);
	float4 color;
	color.x = ((float)coord.x)/get_global_size(0);
	color.y = ((float)coord.y)/get_global_size(1);
	color.z = 0;//
	color.w = 1;
	
	write_imagef(img,coord,color);
}