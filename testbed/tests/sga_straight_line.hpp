#ifndef __SGA_STRAIGHT_LINE_HPP
#define __SGA_STRAIGHT_LINE_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/straight_line.hpp"
#include "sga_solver.cpp"
#include <pthread.h>

class SGAStraightLine : public Test
{
public:
    inline static constexpr int POPULATION_SIZE{50};

	SGAStraightLine();

    void Step(Settings& settings) override;

	static Test* Create();

    StraightLine *environment;
    StraightLine *blocked_environment;

    SGA_solver<32, POPULATION_SIZE, POPULATION_SIZE> *sga_solver;
    pthread_t pthread;

    ~SGAStraightLine();
};

#endif