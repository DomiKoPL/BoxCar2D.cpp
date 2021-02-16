#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "sga_random_track.hpp"
#include <iostream>
#include "settings.h"

SGARandomTrack::SGARandomTrack()
{
	environment = nullptr;
	blocked_environment = nullptr;
	sga_solver = nullptr;
}

void SGARandomTrack::Step(Settings& settings)
{	
	if(environment == nullptr) 
	{
		environment = new RandomTrack(false, settings.m_seed);
		blocked_environment = new RandomTrack(true, settings.m_seed);	

		std::thread thread = std::thread([&]() {
			sga_solver = new SGA_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, settings.m_seed, settings.m_newPopulationSelection);
			// int per = settings.m_drawPerGenerations;
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
	// Test::Step(settings);
	blocked_environment->DeleteCars();

	blocked_environment->Unlock();
}

Test* SGARandomTrack::Create()
{
	return new SGARandomTrack;
}

SGARandomTrack::~SGARandomTrack() {
	std::cerr << "WAITING FOR ~SGARandomTrack()\n";
	environment->Lock();
	std::cerr << "~SGARandomTrack()\n";
	pthread_cancel(pthread);
	
	delete sga_solver;
	sga_solver = nullptr;

	delete environment;
	environment = nullptr;
	
	delete blocked_environment;
	blocked_environment = nullptr;
}

static int testIndex = RegisterTest("RandomTrack", "SGA", SGARandomTrack::Create);