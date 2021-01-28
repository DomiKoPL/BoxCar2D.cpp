#ifndef __TESTCASE_HPP
#define __TESTCASE_HPP

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

class TestCase : public Test
{
public:

	TestCase();

    void Step(Settings& settings) override;

    int GetStepCount();
    void SetBlocked(bool newBlocked);
    void SetDraw(bool newDraw);

	static Test* Create();

    static std::vector<float> evaluate_function(const std::vector<Chromosome>& chromosomes, TestCase *test_case);

    void CreateCars(const std::vector<Chromosome>& chromosomes);
    void DeleteCars();

    float m_mapWidth;
    std::vector<Car*> m_cars;
    std::vector<Car*> m_cars_to_delete;
    bool blocked = true;
    bool draw = false;

    std::mutex mutex;
};

#endif