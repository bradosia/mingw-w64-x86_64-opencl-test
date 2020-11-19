#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220

#include <filesystem>
#include <iostream>
#ifdef __APPLE__
#include <OpenCL/cl2.hpp>
#else
#include <CL/cl2.hpp>
#endif

std::string readFile(std::string path) {
  std::string ret;
  if (auto const fd = std::fopen(path.c_str(), "rb")) {
    auto const bytes = std::filesystem::file_size(path);
    ret.resize(bytes);
    std::fread(ret.data(), 1, bytes, fd);
    std::fclose(fd);
  }
  return ret;
}

int main() {
  cl_int *err = 0;
  // get all platforms (drivers), e.g. NVIDIA
  std::vector<cl::Platform> platformList;
  cl::Platform::get(&platformList);

  if (platformList.size() == 0) {
    std::cout << " No platforms found. Check OpenCL installation!\n";
    return -1;
  }
  cl::Platform default_platform = platformList[0];
  std::cout << "Using platform: "
            << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

  // Select the default platform and create a context using this platform and
  // the GPU
  cl_context_properties cps[3] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
  cl::Context context(CL_DEVICE_TYPE_GPU, cps, 0, 0, err);
  std::cout << "cl_int: " << err << std::endl;

  // get default device (CPUs, GPUs) of the default platform
  std::vector<cl::Device> all_devices;
  default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
  if (all_devices.size() == 0) {
    std::cout << " No devices found. Check OpenCL installation!\n";
    return -1;
  }

  // device[0] is the GPU
  cl::Device default_device = all_devices[0];
  std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>()
            << "\n";

  // create the program that we want to execute on the device
  cl::Program::Sources sources;

  // calculates for each element; C = A + B

  std::string kernel_code = readFile("resources/example00.c");
  sources.push_back({kernel_code.c_str(), kernel_code.length()});

  cl::Program program(context, sources);
  if (program.build({default_device}) != CL_SUCCESS) {
    std::cout << "Error building: "
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)
              << std::endl;
    return -1;
  }

  int n = 50000;
  int n2 = 100;

  // create buffers on device (allocate space on GPU)
  cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * n);
  cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * n);
  cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * n);

  // create things on here (CPU)
  int A[n], B[n], C[n];
  int *A1 = new int[n];
  int *B1 = new int[n];
  for (int i = 0; i < n; i++) {
    A[i] = i;
    B[i] = 1;
    A1[i] = i;
    B1[i] = 1;
  }

  std::cout << "A: ";
  for (int i = 0; i < n2; i++) {
    std::cout << A[i] << " ";
  }
  std::cout << std::endl;

  // We create a command queue for the GPU
  cl::QueueProperties props = cl::QueueProperties::Profiling;
  cl::CommandQueue queue(context, default_device, props, 0);
  // push write commands to queue
  queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * n, A1);
  queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * n, B1);

  // run the kernel
  cl::Kernel simple_add(program, "simple_add");
  simple_add.setArg(0, buffer_A);
  simple_add.setArg(1, buffer_B);
  simple_add.setArg(2, buffer_C);
  simple_add.setArg(3, n);
  cl::Event profiling_evt;
  queue.enqueueNDRangeKernel(simple_add, cl::NullRange, cl::NDRange(n),
                             cl::NDRange(1), NULL, &profiling_evt);
  try {
    profiling_evt.wait();
  } catch (cl::Error error) {
    std::cout << error.what() << ", code: " << error.err() << std::endl;
  }

  // read result from GPU to here
  int *C1 = new int[n];
  try {
    queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * n, C1);
  } catch (cl::Error error) {
    std::cout << error.what() << ", code: " << error.err() << std::endl;
  }

  try {
    queue.finish();
  } catch (cl::Error error) {
    std::cout << error.what() << ", code: " << error.err() << std::endl;
  }

  try {
    cl_ulong time_start =
        profiling_evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    cl_ulong time_end =
        profiling_evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    cl_ulong total_time = time_end - time_start;
    std::cout << "Total time: " << total_time * 0.001 * 0.001 << " ms \n";
  } catch (cl::Error error) {
    std::cout << error.what() << ", code: " << error.err() << std::endl;
  }

  for (int i = 0; i < n2; i++) {
    std::cout << A1[i] << " + " << B1[i] << " = " << C1[i] << std::endl;
  }

  return 0;
}
