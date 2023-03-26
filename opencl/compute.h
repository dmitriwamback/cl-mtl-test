#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
    #include <OpenCL/opencl.h>
#else
    #error "This OpenCL project is not currently available for your OS"
#endif

cl_context          ctx;
cl_device_id        id;
cl_command_queue    cmdq;
cl_program          program;
cl_kernel           kernel;
size_t              local;
size_t              global;

cl_mem input, output;

int compute(const char* filePath, const char* kernelMainFunctionName) {

    int count = 1000000;
    float data[count], results[count];
    for(int i = 0; i < count; i++) {
        data[i] = 1.01f * rand() / 920137.0f;
    }

    global = count;

    if (clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &id, NULL) != CL_SUCCESS) {
        printf("Failed to get device IDs!");
        return -1;
    }

    FILE* file_ptr = fopen(filePath, "r");
    int strSize, readSize;
    char* buffer = NULL;

    if (file_ptr == NULL) {
        printf("The file cannot be opened!");
        return -1;
    }

    fseek(file_ptr, 0, SEEK_END);
    strSize = ftell(file_ptr);
    rewind(file_ptr);
    buffer = (char*)malloc(sizeof(char) * (strSize + 1));
    readSize = fread(buffer, sizeof(char), strSize, file_ptr);

    buffer[strSize] = '\0';
    if (strSize != readSize) { free(buffer); }
    fclose(file_ptr);

    int errc;
    ctx = clCreateContext(0, 1, &id, NULL, NULL, &errc);
    if (!ctx) {
        printf("Failed to create a context!");
        return -1;
    }

    cmdq = clCreateCommandQueue(ctx, id, 0, &errc);
    if (!cmdq) {
        printf("Failed to create a command queue!");
        return -1;
    }
    program = clCreateProgramWithSource(ctx, 1, (const char**)&buffer, NULL, &errc);
    if (!program) {
        printf("Failed to create a program!");
        return -1;
    }
    if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS) {
        printf("Failed to build the program!");
        return -1;
    }

    kernel = clCreateKernel(program, kernelMainFunctionName, &errc);

    input = clCreateBuffer(ctx, CL_MEM_READ_ONLY, sizeof(float) * count, NULL, NULL);
    output = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, NULL);

    if (clEnqueueWriteBuffer(cmdq, input, CL_TRUE, 0, sizeof(float) * count, data, 0, NULL, NULL) != CL_SUCCESS) {
        printf("Failed to write mem!");
        return -1;
    }

    errc    = 0;
    errc    = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    errc   |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output); 
    errc   |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &count); 
    if (errc != CL_SUCCESS) {
        printf("Failed to set kernel arguments!");
        return -1;
    }
    errc = clGetKernelWorkGroupInfo(kernel, id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    errc = clEnqueueNDRangeKernel(cmdq, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    errc = clEnqueueReadBuffer(cmdq, output, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL);
    for (int i = 0; i < count; i++) {
        printf("%f\n", results[i]);
    }

    return 0;
}