#include "chromosome.h"
#include "test.h"
#include <algorithm>

Chromosome::Chromosome() {
    genes.fill(0.f);
}

Chromosome::Chromosome(std::array<float, 32> chromosome)
{
    genes = chromosome;
}

void Chromosome::Random() 
{
    for(int i = 0; i < 8; i++) 
    {
        genes[i * 4    ] = RandomFloat(0.f, 3.14f * 2);
        genes[i * 4 + 1] = RandomFloat(0.f, 20.f);
        genes[i * 4 + 2] = RandomFloat(0.f, 1.f);
        genes[i * 4 + 3] = RandomFloat(0.f, 10.f);
    }
}

float Chromosome::GetVertexAngle(int idx) 
{
    return genes.at(idx * 4);
}

float Chromosome::GetVertexMagnitude(int idx) 
{
    float& r{genes.at(idx * 4 + 1)};
    r = std::min(std::abs(r), 20.f);
    return r;
}

bool Chromosome::GetVertexWhell(int idx) 
{
    float& r{genes.at(idx * 4 + 2)};
    r = std::clamp(r, 0.f, 1.f);

    if(GetVertexWhellRadius(idx) < 0.1f) 
        return false;

    return r >= 0.5f;
}

float Chromosome::GetVertexWhellRadius(int idx) 
{
    float& r{genes.at(idx * 4 + 3)};
    r = std::clamp(r, 0.0f, 10.f);
    return r;
}