#ifndef __SMALL_HILLS_HPP
#define __SMALL_HILLS_HPP

#include "car_environment.hpp"

class SmallHills : public CarEnvironment
{
public:
	SmallHills(bool blocked);

    std::vector<float> evaluate_function(std::vector<Chromosome>& chromosomes) override;
};

#endif
