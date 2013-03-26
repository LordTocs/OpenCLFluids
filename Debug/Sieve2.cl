__constant unsigned int majorsize = sizeof(int) * 8;
__kernel void Sieve (__global char *NumberField, unsigned int base, unsigned int max)
{
	const int index = get_global_id (0)+2;
	unsigned int result = (index) * base;
	NumberField[result] = 0;
}