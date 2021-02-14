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

	thread = std::thread([&]() {
		es_solver = new ES_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, 1, true);
		while(1) {
			es_solver->run(4, environment);
			es_solver->run(1, blocked_environment);
		}
	});
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
	thread.detach();
}

static int testIndex = RegisterTest("RandomTrack", "ES", ESRandomTrack::Create);