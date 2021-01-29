#ifndef __ES_SMALL_HILLS_HPP
#define __ES_SMALL_HILLS_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/small_hills.hpp"
#include "es_solver.cpp"

class ESSmallHills: public Test
{
public:
    inline static constexpr int POPULATION_SIZE{100};

	ESSmallHills();

    void Step(Settings& settings) override;

	static Test* Create();

    SmallHills *environment;
    SmallHills *blocked_environment;

    ES_solver<32, POPULATION_SIZE, POPULATION_SIZE> *es_solver;
    std::thread thread;

    ~ESSmallHills();
};

#endif