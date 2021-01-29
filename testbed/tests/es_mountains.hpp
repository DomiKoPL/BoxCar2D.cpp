#ifndef __ES_MOUNTAINS_HPP
#define __ES_MOUNTAINS_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/mountains.hpp"
#include "es_solver.cpp"

class ESMountains: public Test
{
public:
    inline static constexpr int POPULATION_SIZE{100};

	ESMountains();

    void Step(Settings& settings) override;

	static Test* Create();

    Mountains *environment;
    Mountains *blocked_environment;

    ES_solver<32, POPULATION_SIZE, POPULATION_SIZE> *es_solver;
    std::thread thread;

    ~ESMountains();
};

#endif