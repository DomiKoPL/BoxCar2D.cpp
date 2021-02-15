#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "es_random_track.hpp"
#include <iostream>
#include "settings.h"

ESRandomTrack::ESRandomTrack()
{
	environment = nullptr;
	blocked_environment = nullptr;
}

void ESRandomTrack::Step(Settings& settings)
{	
	if(environment == nullptr) 
	{
		environment = new RandomTrack(false, settings.m_seed);
		blocked_environment = new RandomTrack(true, settings.m_seed);	

		std::thread thread = std::thread([&]() {
			es_solver = new ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, 1, settings.m_seed, settings.m_newPopulationSelection, true);
			while(1) {
				int per = settings.m_drawPerGenerations;
				if(per > 1) {
					es_solver->run(per - 1, environment);
				}
				es_solver->run(1, blocked_environment);
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

Test* ESRandomTrack::Create()
{
	return new ESRandomTrack;
}

ESRandomTrack::~ESRandomTrack() {
	pthread_cancel(pthread);
	
	delete environment;
	environment = nullptr;
	delete blocked_environment;
	blocked_environment = nullptr;
}

static int testIndex = RegisterTest("RandomTrack", "ES", ESRandomTrack::Create);