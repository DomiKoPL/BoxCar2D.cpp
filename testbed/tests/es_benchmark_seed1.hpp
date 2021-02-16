#ifndef __ES_BENCHMARK_SEED1_HPP
#define __ES_BENCHMARK_SEED1_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/random_track.hpp"
#include <pthread.h>

class ESBenchmarkSeed1 : public Test
{
public:
    inline static constexpr int POPULATION_SIZE{50};

	ESBenchmarkSeed1();

    void Step(Settings& settings) override;

	static Test* Create();

    RandomTrack *blocked_environment;
    
    pthread_t pthread;

    ~ESBenchmarkSeed1();
};

#endif