#ifndef __SGA_RANDOM_TRACK_HPP
#define __SGA_RANDOM_TRACK_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/random_track.hpp"
#include "sga_solver.cpp"
#include <pthread.h>

class SGARandomTrack : public Test
{
public:
    inline static constexpr int POPULATION_SIZE{50};

	SGARandomTrack();

    void Step(Settings& settings) override;

	static Test* Create();

    RandomTrack *environment;
    RandomTrack *blocked_environment;

    SGA_solver<32, POPULATION_SIZE, POPULATION_SIZE> *sga_solver;
    pthread_t pthread;

    ~SGARandomTrack();
};

#endif