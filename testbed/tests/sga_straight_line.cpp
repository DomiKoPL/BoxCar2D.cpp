#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "sga_straight_line.hpp"
#include <iostream>
#include "settings.h"

SGAStraightLine::SGAStraightLine()
{
	environment = nullptr;
	blocked_environment = nullptr;
	sga_solver = nullptr;
}

void SGAStraightLine::Step(Settings& settings)
{	
	if(environment == nullptr)
	{
		srand(time(0));
		environment = new StraightLine(false);
		blocked_environment = new StraightLine(true);	

		std::thread thread = std::thread([&]() {
			sga_solver = new SGA_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, settings.m_seed, settings.m_newPopulationSelection);
			while(1) {
				int per = settings.m_drawPerGenerations;
				if(per > 1) {
					sga_solver->run(per - 1, environment);
				}
				sga_solver->run(1, blocked_environment);
			}
		});

		pthread = thread.native_handle();
		thread.detach();
	}

	blocked_environment->Lock();

	blocked_environment->Step(settings);
	blocked_environment->DeleteCars();

	blocked_environment->Unlock();
}

Test* SGAStraightLine::Create()
{
	return new SGAStraightLine;
}

SGAStraightLine::~SGAStraightLine() {
	std::cerr << "WAITING FOR ~SGAStraightLine()\n";
	environment->Lock();
	std::cerr << "~SGAStraightLine()\n";
	pthread_cancel(pthread);
	
	delete sga_solver;
	sga_solver = nullptr;

	delete environment;
	environment = nullptr;
	
	delete blocked_environment;
	blocked_environment = nullptr;
}

// static int testIndex = RegisterTest("StraightLine", "SGA", SGAStraightLine::Create);