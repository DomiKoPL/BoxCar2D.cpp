#ifndef __ES_RANDOM_TRACK_HPP
#define __ES_RANDOM_TRACK_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/random_track.hpp"
#include "es_solver.cpp"

class ESRandomTrack : public Test
{
public:
    inline static constexpr int POPULATION_SIZE{50};

	ESRandomTrack();

    void Step(Settings& settings) override;

	static Test* Create();

    RandomTrack *environment;
    RandomTrack *blocked_environment;

    ES_solver<32, POPULATION_SIZE, POPULATION_SIZE> *es_solver;
    std::thread thread;

    ~ESRandomTrack();
};

#endif