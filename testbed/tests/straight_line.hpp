#ifndef __STRAIGHT_LINE_HPP
#define __STRAIGHT_LINE_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "testcase.hpp"
#include "es_solver.cpp"

constexpr int POPULATION_SIZE{20};

class StraightLine : public Test
{
public:
	StraightLine();

    void Step(Settings& settings) override;

	static Test* Create();

    TestCase *testCase;
    TestCase *blockedTestCase;
    ES_solver<32, POPULATION_SIZE, POPULATION_SIZE> *es_solver;
    std::thread thread;
};

#endif