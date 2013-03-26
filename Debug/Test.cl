 __kernel void Test (__global float *test_data,const int somevalue)
 {
	const int idx = get_global_id(0);
	test_data[idx] = idx * somevalue;
 }