#include <assert.h>
#include <cuda.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


int main (int argc, char *argv[])
{
	CUresult r;
	CUdevice dev;
	CUdeviceptr mem;
	int device_count;
	CUcontext ctx;

	printf ("Initializing...\n");
	r = cuInit (0);
	assert (r == CUDA_SUCCESS);

	r = cuDeviceGetCount (&device_count);
	assert (r == CUDA_SUCCESS);
	assert (device_count > 0);

	r = cuDeviceGet (&dev, 0);
	assert (r == CUDA_SUCCESS);

	r = cuCtxCreate (&ctx, 0, dev);
	assert (r == CUDA_SUCCESS);

	printf ("Init done.\n");


	printf ("Allocating...\n");
	r = cuMemAlloc (&mem, 1234);
	assert (r == CUDA_SUCCESS);
	printf ("Allocating done.\n");

	printf ("Freeing...\n");
	r = cuMemFree (mem);
	assert (r == CUDA_SUCCESS);
	printf ("Freeing done.\n");
}
