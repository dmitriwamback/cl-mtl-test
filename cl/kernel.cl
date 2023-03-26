__kernel void sqr(__global float* i, __global float* o, const unsigned int count) {

    int id = get_global_id(0);
    if (id < count) o[id] = pow(i[id], 2);
}