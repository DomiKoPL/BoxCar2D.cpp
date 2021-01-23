#ifndef __STRAIGHT_LINE_HPP
#define __STRAIGHT_LINE_hPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>

class StraightLine : public Test
{
public:

	StraightLine();

    void Step(Settings& settings) override;

	static Test* Create();

	std::array<Car*, 20> m_cars;
    void CreateCars(std::array<Chromosome, 20> chromosomes);
    float m_mapWidth;
    bool testbedMode = true;
    
};


#endif