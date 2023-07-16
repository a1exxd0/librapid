#include <librapid/librapid.hpp>

#include <stdlib.h> // setenv

namespace librapid {
	namespace global {
		bool throwOnAssert				= false;
		size_t multithreadThreshold		= 5000;
		size_t gemmMultithreadThreshold = 100;
		size_t gemvMultithreadThreshold = 100;
		size_t numThreads				= 8;
		size_t randomSeed				= 0; // Set in PreMain
		bool reseed						= false;
		size_t cacheLineSize			= 64;
		size_t memoryAlignment			= LIBRAPID_DEFAULT_MEM_ALIGN;

#if defined(LIBRAPID_HAS_OPENCL)
		std::vector<cl::Device> openclDevices;
		cl::Context openCLContext;
		cl::Device openCLDevice;
		cl::CommandQueue openCLQueue;
		cl::Program::Sources openCLSources;
		cl::Program openCLProgram;
		bool openCLConfigured = false;
#endif // LIBRAPID_HAS_OPENCL

#if defined(LIBRAPID_HAS_CUDA)
		cudaStream_t cudaStream;
		cublasHandle_t cublasHandle;
		cublasLtHandle_t cublasLtHandle;
		uint64_t cublasLtWorkspaceSize = 1024 * 1024 * 4;
		void *cublasLtWorkspace;
		jitify::JitCache jitCache;
#endif // LIBRAPID_HAS_CUDA
	}  // namespace global

#if defined(_WIN32)
#	define SETENV(name, value) _putenv_s(name, value)
#else
#	define SETENV(name, value) setenv(name, value, 1)
#endif

	void setOpenBLASThreadsEnv(int num_threads) {
		char num_threads_str[20];
		sprintf(num_threads_str, "%d", num_threads);

		SETENV("OPENBLAS_NUM_THREADS", num_threads_str);
		SETENV("GOTO_NUM_THREADS", num_threads_str);
		SETENV("OMP_NUM_THREADS", num_threads_str);
	}

	void setNumThreads(size_t numThreads) {
		global::numThreads = numThreads;

		// OpenBLAS threading
#if defined(LIBRAPID_BLAS_OPENBLAS)
		openblas_set_num_threads(numThreads);
		omp_set_num_threads(numThreads);
		goto_set_num_threads(numThreads);

		setOpenBLASThreadsEnv(numThreads);

#endif // LIBRAPID_BLAS_OPENBLAS

		// MKL threading
#if defined(LIBRAPID_BLAS_MKL)
		mkl_set_num_threads(numThreads);
#endif // LIBRAPID_BLAS_MKL
	}

	size_t getNumThreads() { return global::numThreads; }

	void setSeed(size_t seed) {
		global::randomSeed = seed;
		global::reseed	   = true;
	}

	size_t getSeed() { return global::randomSeed; }
} // namespace librapid
