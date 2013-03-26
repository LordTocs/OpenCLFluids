__constant unsigned int majorsize = sizeof(int) * 8;
__kernel void Sieve (__global int *NumberField, unsigned int base, unsigned int window, unsigned int max)
{
	const int index = get_global_id (0)+2;
	unsigned int result = (index) * base;
	unsigned int majorindex = result / majorsize;
	unsigned int minorindex = result % majorsize;
	atomic_and (&NumberField[majorindex], ~(1 << minorindex));
}