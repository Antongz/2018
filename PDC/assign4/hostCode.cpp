#ifdef __APPLE__
#include <OpenCL/opencl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <climits>

#define DEFAULT_PROBLEM_SIZE 5000

// Checks whether an array is sorted.
// inputs: int *array - Pointer to the array of numbers.
//         int length - length of array.
// output: True if the array was sorted. False otherwise.
bool sorted(int *array, int length) {
    int i=0;
    for (i=0; i<length-1; i++) {
        if (array[i] > array[i+1]){
            return false;
        }
    }
    return true;
}

// Load kernel code from a file into a string.
// inputs: const char* filename - filename of kernel code.
// output: string - kernel code.
std::string loadKernel(const char* filename) {
    std::ifstream f(filename);
    // Read the file a string stream buffer and convert it to a string.
    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    return buffer.str();
}

// Inputs: N - length of array to sort.
int main(int argc, char **argv) {
    // Set default size.
    int N = DEFAULT_PROBLEM_SIZE;
    // Parse array size N from input if provided.
    if (argc == 2) {
        std::istringstream(argv[1]) >> N;
    } else {
        std::cout << "Program requires one input: N - size of array to sort." << std::endl;
        exit(1);
    }

    // Generate a random array with the given length.
    int *array = new int[N];
    for (int i=0; i<N; i++) {
        srand(time(NULL));
        for (int i=0; i<N; i++) {
            array[i] = rand() + rand() - RAND_MAX;
        }
    }

    // Error code variable for checking success of each openCL function.
    cl_int err;

    // ************ Get platform information to all_platforms *************
    std::vector<cl::Platform> all_platforms;

    // Identify all platforms in all_platforms.
    err = cl::Platform::get(&all_platforms);
    if (all_platforms.size()==0 && err != CL_SUCCESS) {
        std::cout << "No platforms found! Error code: " << err << std::endl;
        exit(1);
    }
    cl::Platform default_platform=all_platforms[0];

    // ************ Get device(GPU) information associated with the all_platforms to all_devices *************
    std::vector<cl::Device> all_devices;
    err = default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size()==0 && err != CL_SUCCESS) {
        std::cout << "No devices found! Error code: " << err << std::endl;
    }

    // Set the default_device to the first device in all_devices.
    cl::Device default_device = all_devices[0];

    // ************ Create an OpenCL context for the GPU device *************
    cl::Context context(default_device);

    // ************ Create a command-queue queue for default_device with profiling enabled *************
    cl::CommandQueue queue(context, default_device, CL_QUEUE_PROFILING_ENABLE, &err);
    if (err != CL_SUCCESS) {
        std::cout << "Could not make command queue, error: " << err << std::endl;
        exit(1);
    }

    // ************ Create memory buffers(buffer_A for array and buffer_B for the size of array) on the host and device *************
    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int)*N, array, &err);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int), &N, &err);


    // ************ Load and Setup the kernel path *************
    cl::Program::Sources sources;
    std::string kernel_code = loadKernel("kernel_code.cl");
    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    // ************ Create a program from the kernel sources *************
    cl::Program program(context, sources);

    // ************ Build the program from the file called kernel_code.cl and compile it *************
    err = program.build({default_device});
    if (err != CL_SUCCESS) {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << " Error code: " << err << std::endl;
        exit(1);
    }

    // Calculate the optimal global workgroup size for the given problem size.
    // Ensures that the global workgroup size doesn't default to 1 (sequential program) for specific problem sizes by padding extra processors.
    // Original code by Marco13 from: https://stackoverflow.com/questions/22968369/get-optimum-local-global-workgroup-size-in-opencl/22969485#22969485
    // Get the maximum work group size for the default device.
    size_t workgroup_size;
    err = clGetDeviceInfo(default_device(), CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    if (err != CL_SUCCESS) {
        std::cout << workgroup_size << std::endl;
        std::cout << "Error: " << err << std::endl;
        exit(1);
    }
    int maxWorkGroupSize = workgroup_size;
    int numWorkGroups = (N-1) / maxWorkGroupSize + 1;
    int globalSizePadded = numWorkGroups * maxWorkGroupSize;

    // ************ Create the OpenCL kernel: kernel_even_step and kernel_odd_step *************
    cl::Kernel kernel_even_step = cl::Kernel(program, "even_step");
    cl::Kernel kernel_odd_step = cl::Kernel(program, "odd_step");

    // ************ Create kernel arguments *************
    // Reads the buffer_A for the first argument on both kernel_even_step and kernel_odd_step.
    // Reads the buffer_B for the second argument on both kernel_even_step and kernel_odd_step.
    kernel_even_step.setArg(0,buffer_A);
    kernel_even_step.setArg(1,buffer_B);
    kernel_odd_step.setArg(0,buffer_A);
    kernel_odd_step.setArg(1,buffer_B);

    /* Perform parallel bubble sort. */
    cl_ulong time_start = 0, time_end = 0;
    cl_ulong total_time = 0;
    cl::Event event;

    // ************ Iterating k to perform odd-even bubble sort *************
    for (int k=0; k<N; k++) {
        // ************ If k is even number, then we launch kernel_even_step linked to an event *************
        // ************ Otherwis, we launch kernel_odd_step linked to an event *************
        if (k % 2 == 0) {
            queue.enqueueNDRangeKernel(kernel_even_step, cl::NullRange, globalSizePadded, cl::NullRange, NULL, &event);
        } else {
            queue.enqueueNDRangeKernel(kernel_odd_step, cl::NullRange, globalSizePadded, cl::NullRange, NULL, &event);
        }

        // Wait for queued kernel to finish execution.
        event.wait();
        // Get execution time for the kernel and add it to the total time.
        time_start = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
        time_end = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
        total_time += time_end - time_start;
    }

    // ************ Read the memory buffer(buffer_A) on the device to the local array res *************
    int *res = new int[N];
    queue.enqueueReadBuffer(buffer_A,CL_TRUE,0,sizeof(int)*N,res);

    // If the array is sorted, output the execution time.
    if (sorted(res, N)) {
        std::cout << 1.0*total_time /1000000000 << std::endl;
    } else {
        std::cout << "Error: array not sorted after kernel execution." << std::endl;
        for(int k=0;k<N;k++)
            std::cout << res[k] << " ";
        exit(1);
    }

    // Clean up.
    delete array;
    delete res;

    return 0;
}
