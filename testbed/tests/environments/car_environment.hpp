#ifndef __CAR_ENVIRONMENT_HPP
#define __CAR_ENVIRONMENT_HPP

#include "test.h"
#include "car.h"
#include "chromosome.h"
#include "settings.h"

#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <future>
#include <mutex>

class CarEnvironment : public Test
{
public:
	CarEnvironment(bool blocked);

    void Step(Settings& settings) override;

    bool IsBlocked() const;

    void Lock();
    void Unlock();

    virtual std::vector<float> evaluate_function(std::vector<Chromosome>& chromosomes) = 0;
    
    void CreateCars(std::vector<Chromosome>& chromosomes, float init_speed);
    void DeleteCars();

	static Test* Create();

    ~CarEnvironment();

protected:
    float m_map_width;
    int m_cars_done;
    std::vector<Car*> m_cars;
    std::vector<Car*> m_cars_to_delete;
    bool m_blocked;
    std::mutex m_mutex;
};

#endif