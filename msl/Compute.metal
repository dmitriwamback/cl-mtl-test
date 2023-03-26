#include <metal_stdlib>
using namespace metal;

kernel void add(constant float* iArray1 [[buffer(0)]],
                constant float* iArray2 [[buffer(1)]],
                device float* result [[buffer(2)]],
                uint index [[thread_position_in_grid]]) {

    result[index] = iArray1[index] + iArray2[index];
}