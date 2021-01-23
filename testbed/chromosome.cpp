#include "chromosome.h"
#include "test.h"

Chromosome::Chromosome() {
    genes.fill(0.f);
}

void Chromosome::Random() {
    for(int i = 0; i < 8; i++) {
        genes[i * 4] = RandomFloat(0, 3.14 * 2);
        genes[i * 4 + 1] = RandomFloat(0.4f, 3.f);
        genes[i * 4 + 2] = RandomFloat(0, 1);
        genes[i * 4 + 3] = RandomFloat(0.2, 0.5);
    }
}

float Chromosome::GetVertexAngle(int idx) const {
    return genes.at(idx * 4);
}

float Chromosome::GetVertexMagnitude(int idx) const {
    return genes.at(idx * 4 + 1);
}

float Chromosome::GetVertexWhell(int idx) const {
    return genes.at(idx * 4 + 2) <= 0.3f;
}

float Chromosome::GetVertexWhellRadius(int idx) const {
    return genes.at(idx * 4 + 3);
}