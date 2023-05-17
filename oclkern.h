//
// Created by alx on 5/15/23.
//

#ifndef OPENCL0_OCLKERN_H
#define OPENCL0_OCLKERN_H

const char *kernel_randomize = R"(
inline float random(ulong seed) {
    unsigned int state = seed * (get_global_id(1) * get_global_size(0) + get_global_id(0));
    unsigned int x = state * 1234567 + 8901234;
    float r = as_float((x & 0x007fffff) | 0x3f800000) - 1.0f;
    return r;
}

inline bool getCell(__read_write image2d_t mat, int x, int y) {
    uint4 pixel = read_imageui(mat, (int2)(x, y));
    bool status = pixel.s0 == 255 ? true : false;
    return status;
}

inline void setCell(__read_write image2d_t mat, int x, int y, bool state) {
    uint4 pixelValue = (state == true) ? (uint4)(255, 0, 0, 255) : (uint4)(0, 0, 0, 255);
    write_imageui(mat, (int2)(x, y), pixelValue);
}

__kernel void randomizeMatrix(__read_write image2d_t mat, ulong seed) {
    int coordx = get_global_id(0);
    int coordy = get_global_id(1);
    if (coordx >= get_image_width(mat) || coordy >= get_image_height(mat))
        return;

    float r = random(seed);
    bool cell_state;
    if (r < 0.5f) {
        cell_state = false;
    } else {
        cell_state = true;
    }
    setCell(mat, coordx, coordy, cell_state);
}

__kernel void stepMatrix(__read_write image2d_t mat) {
    int coordx = get_global_id(0);
    int coordy = get_global_id(1);
    int mat_width = get_image_width(mat);
    int mat_height = get_image_height(mat);
    if (coordx >= mat_width || coordy >= mat_height)
        return;

    //get status of current cell and count the neighbours
    bool current_cell_status = getCell(mat, coordx, coordy);
    int neighbors = 0;
    for (int i = coordx - 1; i < coordx + 2; i++) {
        for (int j = coordy - 1; j < coordy + 2; j++) {
            if (i == coordx && j == coordy) continue;                              // don't count itself as neighbour
            if (i < 0 || j < 0 || i >= mat_width || j >= mat_height) continue;   // count cells outside matrix as dead
            bool current_neighbour_status = getCell(mat, i, j);
            if (current_neighbour_status) neighbors++;
        }
    }
    barrier(CLK_GLOBAL_MEM_FENCE); //sync threads globally
    bool next_cell_status;
    if (current_cell_status) {
        // Cell is currently alive
        if (neighbors < 2 || neighbors > 3) {
            // Underpopulation or overpopulation, cell dies
            next_cell_status = false;
        } else {
            // Cell survives
            next_cell_status = true;
        }
    } else {
        // Cell is currently dead
        if (neighbors == 3) {
            // Reproduction, dead cell becomes alive
            next_cell_status = true;
        } else {
            // Cell remains dead
            next_cell_status = false;
        }
    }
    setCell(mat, coordx, coordy, next_cell_status);
}
)";
#endif //OPENCL0_OCLKERN_H
