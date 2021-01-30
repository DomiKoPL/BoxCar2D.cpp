#ifndef __RANDOM_TRACK_HPP
#define __RANDOM_TRACK_HPP

#include "car_environment.hpp"

class RandomTrack : public CarEnvironment
{
public:
	RandomTrack(bool blocked, int seed);

    std::vector<float> evaluate_function(std::vector<Chromosome>& chromosomes) override;
};

#endif
