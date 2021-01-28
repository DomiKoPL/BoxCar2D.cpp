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
    float GetVertexAngle(int idx) const;
    float GetVertexMagnitude(int idx) const;
    bool GetVertexWhell(int idx) const;
    float GetVertexWhellRadius(int idx) const;
};

#endif