#ifndef __MOUNTAINS_HPP
#define __MOUNTAINS_HPP

#include "car_environment.hpp"

class Mountains : public CarEnvironment
{
public:
	Mountains(bool blocked);

    std::vector<float> evaluate_function(std::vector<Chromosome>& chromosomes) override;
};

#endif
