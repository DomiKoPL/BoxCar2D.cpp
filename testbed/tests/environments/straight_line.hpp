#ifndef __STRAIGHT_LINE_HPP
#define __STRAIGHT_LINE_HPP

#include "car_environment.hpp"

class StraightLine : public CarEnvironment
{
public:
	StraightLine(bool blocked);

    std::vector<float> evaluate_function(std::vector<Chromosome>& chromosomes) override;
};

#endif
