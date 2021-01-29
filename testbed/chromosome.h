#ifndef CHROMOSOME_H
#define CHROMOSOME_H

#include <array>

struct Chromosome {
    std::array<float, 32> genes;
    /*
        vertexAngle
        vertexMagnitude
        vertexWhell
        vertexWhellRadius
    */

    Chromosome();
    Chromosome(std::array<float, 32> chromosome);
    void Random();
    float GetVertexAngle(int idx);
    float GetVertexMagnitude(int idx);
    bool GetVertexWhell(int idx);
    float GetVertexWhellRadius(int idx);
};

#endif