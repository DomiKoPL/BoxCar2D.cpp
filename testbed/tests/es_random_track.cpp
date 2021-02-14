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
	srand(time(0));
    int seed = rand();
	environment = new RandomTrack(false, seed);
	blocked_environment = new RandomTrack(true, seed);	

	std::thread thread = std::thread([&]() {
		es_solver = new ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, 1, true);
		while(1) {
			// es_solver->run(4, environment);
			try {
				es_solver->run(100, blocked_environment);
			} catch(std::exception e) {
				std::cerr << "GOWNO\n";
				std::cerr << e.what() << "\n";
			}
		}
	});

	pthread = thread.native_handle();
	thread.detach();
}

void ESRandomTrack::Step(Settings& settings)
{	
	blocked_environment->Lock();

	blocked_environment->Step(settings);
	// Test::Step(settings);
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
	delete blocked_environment;
}

static int testIndex = RegisterTest("RandomTrack", "ES", ESRandomTrack::Create);