#define _GNU_SOURCE
#include <cuda.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


static void *lib_handle_cuda = 0;
static CUresult (*lib_cuMemAllocManaged) (CUdeviceptr * dptr, size_t bitesize, int flags) = 0;
static CUresult (*lib_cuMemFree_v2) (CUdeviceptr dptr) = 0;

static void *lib_handle_dl = 0;
static void *(*lib_dlsym) (void *, const char *) = 0;

static void __attribute__((constructor (101))) cuda_override_init ()
{
	lib_handle_dl = dlopen ("libdl.so.2", RTLD_LAZY);

	if (lib_handle_dl)
		lib_dlsym = dlvsym (lib_handle_dl, "dlsym", "GLIBC_2.2.5");

	if (!lib_dlsym || !lib_handle_dl) {
		printf ("================== CUDA HACK FAILED TO PREPARE ======================\n");
		exit (1);
	}

	putenv ("TF_GPU_ALLOCATOR=cuda_malloc_async");
	printf ("================== CUDA HACK PREPARED ===============================\n");
}

void *dlsym (void *handle, const char *name)
{
	void *ret;

	if (!lib_dlsym)
		cuda_override_init ();

	if (!strcmp (name, "cuMemAlloc_v2"))
		ret = cuMemAlloc_v2;
	else if (!strcmp (name, "cuMemAllocFromPoolAsync"))
		ret = cuMemAllocFromPoolAsync;
	else if (!strcmp (name, "cuMemFreeAsync"))
		ret = cuMemFreeAsync;
	else
		ret = lib_dlsym (handle, name);

	return ret;
}

static void init ()
{
	if (lib_cuMemAllocManaged)
		return;

	lib_handle_cuda = dlopen ("libcuda.so.1", RTLD_LAZY);
	if (lib_handle_cuda) {
		lib_cuMemAllocManaged = lib_dlsym (lib_handle_cuda, "cuMemAllocManaged");
		lib_cuMemFree_v2 = lib_dlsym (lib_handle_cuda, "cuMemFree_v2");
	}

	if (!lib_cuMemAllocManaged || !lib_cuMemFree_v2) {
		printf ("================== CUDA HACK FAILED TO INITIALIZE ======================\n");
		exit (1);
	}
	printf ("================== CUDA HACK INITIALIZED ===============================\n");

	return;
}

CUresult cuMemFreeAsync (CUdeviceptr dptr, CUstream hStream)
{
	init ();

	CUresult r;
	r = lib_cuMemFree_v2 (dptr);

	return r;
}

CUresult cuMemAlloc_v2 (CUdeviceptr * dptr, size_t bitesize)
{
	init ();

	CUresult r;
	r = lib_cuMemAllocManaged (dptr, bitesize, CU_MEM_ATTACH_GLOBAL);

	return r;
}

CUresult cuMemAllocFromPoolAsync (CUdeviceptr * dptr, size_t bitesize, CUmemoryPool pool, CUstream hStream)
{
	init ();

	CUresult r;
	r = lib_cuMemAllocManaged (dptr, bitesize, CU_MEM_ATTACH_GLOBAL);

	return r;
}
