#ifndef __ES_STRAIGHT_LINE_HPP
#define __ES_STRAIGHT_LINE_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/straight_line.hpp"
#include "es_solver.cpp"
#include <pthread.h>

class ESStraightLine : public Test
{
public:
    inline static constexpr int POPULATION_SIZE{100};

    void Keyboard(int key) override
	{
		blocked_environment->Keyboard(key);
	}

	ESStraightLine();

    void Step(Settings& settings) override;

	static Test* Create();

    StraightLine *environment;
    StraightLine *blocked_environment;

    ES_solver<32, POPULATION_SIZE, POPULATION_SIZE> *es_solver;
    pthread_t pthread;

    ~ESStraightLine();
};

#endif