#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "es_straight_line.hpp"
#include <iostream>
#include "settings.h"

ESStraightLine::ESStraightLine()
{
	srand(time(0));
	environment = new StraightLine(false);
	blocked_environment = new StraightLine(true);	

	std::thread thread = std::thread([&]() {
		es_solver = new ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, 1, true);
		while(1) {
			es_solver->run(4, environment);
			es_solver->run(1, blocked_environment);
		}
	});

	pthread = thread.native_handle();
	thread.detach();
}

void ESStraightLine::Step(Settings& settings)
{	
	blocked_environment->Lock();

	blocked_environment->Step(settings);
	// Test::Step(settings);
	blocked_environment->DeleteCars();

	blocked_environment->Unlock();
}

Test* ESStraightLine::Create()
{
	return new ESStraightLine;
}

ESStraightLine::~ESStraightLine() {
	pthread_cancel(pthread);
	
	delete environment;
	delete blocked_environment;
}

// static int testIndex = RegisterTest("StraightLine", "ESStraightLine", ESStraightLine::Create);