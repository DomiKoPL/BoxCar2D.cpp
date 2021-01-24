#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "straight_line.hpp"
#include <iostream>
#include "settings.h"

StraightLine::StraightLine()
{
	testCase = new TestCase();
	testCase->SetBlocked(false);
	blockedTestCase = new TestCase();
	blockedTestCase->SetDraw(true);

	es_solver = new ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>(TestCase::evaluate_function, testCase, 1, true);

	thread = std::thread(&ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>::run, &*es_solver, 30, blockedTestCase);
}

void StraightLine::Step(Settings& settings)
{
	blockedTestCase->SetBlocked(false);
	blockedTestCase->Step(settings);
	blockedTestCase->SetBlocked(true);
	std::lock_guard<std::mutex> lock(blockedTestCase->mutex);
	Test::Step(settings);
}

Test* StraightLine::Create()
{
	return new StraightLine;
}

static int testIndex = RegisterTest("StraightLine", "StraightLine", StraightLine::Create);