#ifndef __F1_STRAIGHT_LINE_HPP
#define __F1_STRAIGHT_LINE_HPP

#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "environments/straight_line.hpp"
#include <pthread.h>

class F1StraightLine : public Test
{
public:
	F1StraightLine();

    void Step(Settings& settings) override;

	static Test* Create();

    ~F1StraightLine();

    StraightLine *blocked_environment;
    
    pthread_t pthread;
};

#endif