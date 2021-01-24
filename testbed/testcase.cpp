#include "testcase.hpp"

TestCase::TestCase() 
{
    m_mapWidth = 1000.f;

    b2BodyDef groundBody;
    groundBody.position.Set(0.0f, -10.0f);

    b2Body* ground = m_world->CreateBody(&groundBody);

    b2EdgeShape shape;

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.0f;
    fd.friction = 0.6f;

    shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(m_mapWidth + 20.f, 0.0f));
    ground->CreateFixture(&fd);

    shape.SetTwoSided(b2Vec2(m_mapWidth + 20.f, 0.0f), b2Vec2(m_mapWidth + 20.f, 50.0f));
    ground->CreateFixture(&fd);

    for(auto& car : m_cars) {
        Chromosome chromosome;
        chromosome.Random();
        car = new Car(chromosome, m_world, -100);
    }
};

void TestCase::Step(Settings& settings)
{
    std::lock_guard<std::mutex> lock(mutex);
    
    if(blocked) {
        return;
    }

    float timeStep = settings.m_hertz > 0.0f ? 1.0f / settings.m_hertz : float(0.0f);

    m_world->SetAllowSleeping(settings.m_enableSleep);
    m_world->SetWarmStarting(settings.m_enableWarmStarting);
    m_world->SetContinuousPhysics(settings.m_enableContinuous);
    m_world->SetSubStepping(settings.m_enableSubStepping);

    m_pointCount = 0;

    m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations);

    if(draw) {
        m_world->DebugDraw();
    }

    if (timeStep > 0.0f)
    {
        ++m_stepCount;
    }
    // Track maximum profile times
    {
        const b2Profile& p = m_world->GetProfile();
        m_maxProfile.step = b2Max(m_maxProfile.step, p.step);
        m_maxProfile.collide = b2Max(m_maxProfile.collide, p.collide);
        m_maxProfile.solve = b2Max(m_maxProfile.solve, p.solve);
        m_maxProfile.solveInit = b2Max(m_maxProfile.solveInit, p.solveInit);
        m_maxProfile.solveVelocity = b2Max(m_maxProfile.solveVelocity, p.solveVelocity);
        m_maxProfile.solvePosition = b2Max(m_maxProfile.solvePosition, p.solvePosition);
        m_maxProfile.solveTOI = b2Max(m_maxProfile.solveTOI, p.solveTOI);
        m_maxProfile.broadphase = b2Max(m_maxProfile.broadphase, p.broadphase);

        m_totalProfile.step += p.step;
        m_totalProfile.collide += p.collide;
        m_totalProfile.solve += p.solve;
        m_totalProfile.solveInit += p.solveInit;
        m_totalProfile.solveVelocity += p.solveVelocity;
        m_totalProfile.solvePosition += p.solvePosition;
        m_totalProfile.solveTOI += p.solveTOI;
        m_totalProfile.broadphase += p.broadphase;
    }
}

int TestCase::GetStepCount() { 
    std::lock_guard<std::mutex> lock(mutex);
    return m_stepCount; 
}

void  TestCase::SetBlocked(bool newBlocked) {
    std::lock_guard<std::mutex> lock(mutex);
    blocked = newBlocked;
}

void  TestCase::SetDraw(bool newDraw) {
    std::lock_guard<std::mutex> lock(mutex);
    draw = newDraw;
}

Test* TestCase::Create()
{
    return new TestCase;
}

std::vector<float> TestCase::evaluate_function(const std::vector<Chromosome>& chromosomes, TestCase *test_case)
{
    std::cout << "Evaluating cars\n";
    // Trzeba wiedzieć jaki test
    // create_fnc
    // s_test->Step(s_settings);
    Settings s_settings;
    s_settings.m_hertz = 60;

    // std::array<Chromosome, 20> chromosomes;
    // for(auto &p : chromosomes) p.Random();
    test_case->CreateCars(chromosomes);

    // auto tp_ = std::chrono::high_resolution_clock::now();
    // auto tp = std::chrono::time_point(tp_);
    // auto end_tp = std::chrono::time_point(tp_ + std::chrono::seconds(1));
    
    auto stepsBefore = test_case->GetStepCount();
    if(test_case->blocked) {
        while(test_case->GetStepCount() - stepsBefore < 60 * 5) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "WAITING " << test_case->GetStepCount() - stepsBefore << "\n";
        }
    } else {
        while(test_case->GetStepCount() - stepsBefore < 60 * 5) {
            test_case->Step(s_settings);
        }
    }

    // while(std::chrono::high_resolution_clock::now() < end_tp)
    // {
    //     testCase->Step(s_settings);
        
        // for (int j = 0; j < 20; ++j)
        // {
        //     std::cout << "Car " << j << "\tx: " << 
        //         testCase->m_cars.at(j)->GetBody()->GetPosition().x << "\ty: " << 
        //         testCase->m_cars.at(j)->GetBody()->GetPosition().y << "\n";
        // }

        // std::cout << "\n\n";
    // }

    std::vector<float> dists;

    for(int i = 0; i < chromosomes.size(); ++i)
    {
        dists.push_back(1000.0f - test_case->m_cars.at(i)->GetBody()->GetPosition().x);
        std::cout << dists.back() << "\n";
    }
    
    std::cout << "Evaluating cars DONE\n";

    return dists;
}

void TestCase::CreateCars(const std::vector<Chromosome>& chromosomes)
{
    std::lock_guard<std::mutex> lock(mutex);

    std::cerr << "DESTROY CARS\n";
    for(auto& car : m_cars) {
        if(car != nullptr) {
            std::cerr << "PUUP\n";
            // car->DetroyBody();
            std::cerr << "PUUP DONE\n";
            delete car;
        }
    }
    std::cerr << "DESTROY DONE\n";

    m_cars = std::vector<Car*>(chromosomes.size());

    for (int i = 0; i < chromosomes.size(); ++i)
    {
        m_cars.at(i) = new Car(chromosomes.at(i), m_world, -100);
    }
}