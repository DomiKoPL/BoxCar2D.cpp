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
	srand(time(0));
    int seed = rand();
	environment = new RandomTrack(false, seed);
	blocked_environment = new RandomTrack(true, seed);	

	std::thread thread = std::thread([&]() {
		sga_solver = new SGA_solver<32, POPULATION_SIZE, POPULATION_SIZE>(blocked_environment, 1, false);
		while(1) {
			// SGA_solver->run(4, environment);
			try {
				sga_solver->run(100, blocked_environment);
			} catch(std::exception e) {
				std::cerr << "GOWNO\n";
				std::cerr << e.what() << "\n";
			}
		}
	});

	pthread = thread.native_handle();
	thread.detach();
}

void SGARandomTrack::Step(Settings& settings)
{	
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
	pthread_cancel(pthread);
	
	delete environment;
	delete blocked_environment;
}

static int testIndex = RegisterTest("RandomTrack", "SGA", SGARandomTrack::Create);