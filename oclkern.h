//
// Created by alx on 5/15/23.
//

#ifndef OPENCL0_OCLKERN_H
#define OPENCL0_OCLKERN_H

const char *kernel_randomize = "inline float random(ulong seed) {\n"
                               "    unsigned int state = seed * (get_global_id(1) * get_global_size(0) + get_global_id(0));\n"
                               "    unsigned int x = state * 1234567 + 8901234;\n"
                               "    float r = as_float((x & 0x007fffff) | 0x3f800000) - 1.0f;\n"
                               "    return r;\n"
                               "}\n"
                               "\n"
                               "inline bool getCell(__read_write image2d_t mat, int x, int y) {\n"
                               "    uint4 pixel = read_imageui(mat, (int2)(x, y));\n"
                               "    bool status = pixel.s0 == 255 ? true : false;\n"
                               "    return status;\n"
                               "}\n"
                               "\n"
                               "inline void setCell(__read_write image2d_t mat, int x, int y, bool state) {\n"
                               "    uint4 pixelValue = (state == true) ? (uint4)(255, 0, 0, 255) : (uint4)(0, 0, 0, 255);\n"
                               "    write_imageui(mat, (int2)(x, y), pixelValue);\n"
                               "}\n"
                               "\n"
                               "__kernel void randomizeMatrix(__read_write image2d_t mat, ulong seed) {\n"
                               "    int coordx = get_global_id(0);\n"
                               "    int coordy = get_global_id(1);\n"
                               "    if (coordx >= get_image_width(mat) || coordy >= get_image_height(mat))\n"
                               "        return;\n"
                               "\n"
                               "    float r = random(seed);\n"
                               "    bool cell_state;\n"
                               "    if (r < 0.5f) {\n"
                               "        cell_state = false;\n"
                               "    } else {\n"
                               "        cell_state = true;\n"
                               "    }\n"
                               "    setCell(mat, coordx, coordy, cell_state);\n"
                               "}\n"
                               "\n"
                               "__kernel void stepMatrix(__read_write image2d_t mat) {\n"
                               "    int coordx = get_global_id(0);\n"
                               "    int coordy = get_global_id(1);\n"
                               "    int mat_width = get_image_width(mat);\n"
                               "    int mat_height = get_image_height(mat);\n"
                               "    if (coordx >= mat_width || coordy >= mat_height)\n"
                               "        return;\n"
                               "\n"
                               "    //get status of current cell and count the neighbours\n"
                               "    bool current_cell_status = getCell(mat, coordx, coordy);\n"
                               "    int neighbors = 0;\n"
                               "    for (int i = coordx - 1; i < coordx + 2; i++) {\n"
                               "        for (int j = coordy - 1; j < coordy + 2; j++) {\n"
                               "            if (i == coordx && j == coordy) continue;                              // don't count itself as neighbour\n"
                               "            if (i < 0 || j < 0 || i >= mat_width || j >= mat_height) continue;   // count cells outside matrix as dead\n"
                               "            bool current_neighbour_status = getCell(mat, i, j);\n"
                               "            if (current_neighbour_status) neighbors++;\n"
                               "        }\n"
                               "    }\n"
                               "    barrier(CLK_GLOBAL_MEM_FENCE); //sync threads globally\n"
                               "    bool next_cell_status;\n"
                               "    if (current_cell_status) {\n"
                               "        // Cell is currently alive\n"
                               "        if (neighbors < 2 || neighbors > 3) {\n"
                               "            // Underpopulation or overpopulation, cell dies\n"
                               "            next_cell_status = false;\n"
                               "        } else {\n"
                               "            // Cell survives\n"
                               "            next_cell_status = true;\n"
                               "        }\n"
                               "    } else {\n"
                               "        // Cell is currently dead\n"
                               "        if (neighbors == 3) {\n"
                               "            // Reproduction, dead cell becomes alive\n"
                               "            next_cell_status = true;\n"
                               "        } else {\n"
                               "            // Cell remains dead\n"
                               "            next_cell_status = false;\n"
                               "        }\n"
                               "    }\n"
                               "    setCell(mat, coordx, coordy, next_cell_status);\n"
                               "}";
#endif //OPENCL0_OCLKERN_H
