#include "testcase.hpp"

TestCase::TestCase() 
{
    m_mapWidth = 1000.f;

    b2Vec2 gravity;
	gravity.Set(0.0f, -20.0f);
    m_world->SetGravity(gravity);

    b2Body* ground = NULL;
    {
        b2BodyDef bd;
        ground = m_world->CreateBody(&bd);

        b2EdgeShape shape;

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 0.0f;
        fd.friction = 0.6f;

        shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(20.0f, 0.0f));
        ground->CreateFixture(&fd);

        float hs[10] = {0.25f, 1.0f, 4.0f, 0.0f, 0.0f, -1.0f, -2.0f, -2.0f, -1.25f, 0.0f};

        float x = 20.0f, y1 = 0.0f, dx = 5.0f;

        for (int32 i = 0; i < 10; ++i)
        {
            float y2 = hs[i];
            shape.SetTwoSided(b2Vec2(x, y1), b2Vec2(x + dx, y2));
            ground->CreateFixture(&fd);
            y1 = y2;
            x += dx;
        }

        for (int32 i = 0; i < 10; ++i)
        {
            float y2 = hs[i];
            shape.SetTwoSided(b2Vec2(x, y1), b2Vec2(x + dx, y2));
            ground->CreateFixture(&fd);
            y1 = y2;
            x += dx;
        }

        shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
        ground->CreateFixture(&fd);

        x += 80.0f;
        shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
        ground->CreateFixture(&fd);

        x += 40.0f;
        shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 10.0f, 5.0f));
        ground->CreateFixture(&fd);

        x += 20.0f;
        shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
        ground->CreateFixture(&fd);

        x += 40.0f;
        shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x, 20.0f));
        ground->CreateFixture(&fd);
    }

    // // Teeter
    // {
    //     b2BodyDef bd;
    //     bd.position.Set(140.0f, 1.0f);
    //     bd.type = b2_dynamicBody;
    //     b2Body* body = m_world->CreateBody(&bd);

    //     b2PolygonShape box;
    //     box.SetAsBox(10.0f, 0.25f);
    //     body->CreateFixture(&box, 1.0f);

    //     b2RevoluteJointDef jd;
    //     jd.Initialize(ground, body, body->GetPosition());
    //     jd.lowerAngle = -8.0f * b2_pi / 180.0f;
    //     jd.upperAngle = 8.0f * b2_pi / 180.0f;
    //     jd.enableLimit = true;
    //     m_world->CreateJoint(&jd);

    //     body->ApplyAngularImpulse(100.0f, true);
    // }

    // Bridge
    {
        int32 N = 20;
        b2PolygonShape shape;
        shape.SetAsBox(1.0f, 0.125f);

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 1.0f;
        fd.friction = 0.6f;

        b2RevoluteJointDef jd;

        b2Body* prevBody = ground;
        for (int32 i = 0; i < N; ++i)
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(161.0f + 2.0f * i, -0.125f);
            b2Body* body = m_world->CreateBody(&bd);
            body->CreateFixture(&fd);

            b2Vec2 anchor(160.0f + 2.0f * i, -0.125f);
            jd.Initialize(prevBody, body, anchor);
            m_world->CreateJoint(&jd);

            prevBody = body;
        }

        b2Vec2 anchor(160.0f + 2.0f * N, -0.125f);
        jd.Initialize(prevBody, ground, anchor);
        m_world->CreateJoint(&jd);
    }

    // Boxes
    {
        b2PolygonShape box;
        box.SetAsBox(0.5f, 0.5f);

        b2Body* body = NULL;
        b2BodyDef bd;
        bd.type = b2_dynamicBody;

        bd.position.Set(230.0f, 0.5f);
        body = m_world->CreateBody(&bd);
        body->CreateFixture(&box, 0.5f);

        bd.position.Set(230.0f, 1.5f);
        body = m_world->CreateBody(&bd);
        body->CreateFixture(&box, 0.5f);

        bd.position.Set(230.0f, 2.5f);
        body = m_world->CreateBody(&bd);
        body->CreateFixture(&box, 0.5f);

        bd.position.Set(230.0f, 3.5f);
        body = m_world->CreateBody(&bd);
        body->CreateFixture(&box, 0.5f);

        bd.position.Set(230.0f, 4.5f);
        body = m_world->CreateBody(&bd);
        body->CreateFixture(&box, 0.5f);
    }
	

    // b2BodyDef groundBody;
    // groundBody.position.Set(0.0f, -10.0f);

    // b2Body* ground = m_world->CreateBody(&groundBody);

    // b2EdgeShape shape;

    // b2FixtureDef fd;
    // fd.shape = &shape;
    // fd.density = 0.0f;
    // fd.friction = 0.6f;

    // shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(m_mapWidth + 20.f, 0.0f));
    // ground->CreateFixture(&fd);

    // shape.SetTwoSided(b2Vec2(m_mapWidth + 20.f, 0.0f), b2Vec2(m_mapWidth + 20.f, 50.0f));
    // ground->CreateFixture(&fd);

    for(auto& car : m_cars) {
        Chromosome chromosome;
        chromosome.Random();
        car = new Car(chromosome, m_world, -100);
    }
};

void TestCase::Step(Settings& settings)
{
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
    return m_stepCount; 
}

void  TestCase::SetBlocked(bool newBlocked) {
    blocked = newBlocked;
}

void  TestCase::SetDraw(bool newDraw) {
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

    test_case->mutex.lock();

    if(test_case->m_world->IsLocked()) {
        std::cerr << "WORLD IS LOCKED\n";
    }

    while(test_case->m_world->IsLocked()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    test_case->CreateCars(chromosomes);

    if(not test_case->blocked) {
        test_case->DeleteCars();
    }

    test_case->mutex.unlock();

    // auto tp_ = std::chrono::high_resolution_clock::now();
    // auto tp = std::chrono::time_point(tp_);
    // auto end_tp = std::chrono::time_point(tp_ + std::chrono::seconds(1));
    
    auto stepsBefore = test_case->GetStepCount();
    if(test_case->blocked) 
    {
        while(test_case->GetStepCount() - stepsBefore < 60 * 20) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "WAITING " << test_case->GetStepCount() - stepsBefore << "\n";
        }
    } 
    else 
    {
        while(test_case->GetStepCount() - stepsBefore < 60 * 20) {
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

    test_case->mutex.lock();
    std::vector<float> dists;

    for(int i = 0; i < chromosomes.size(); ++i)
    {
        assert(test_case->m_cars.size() > i);
        assert(test_case->m_cars.at(i)->GetBody() != nullptr);
        dists.push_back(1000.0f - test_case->m_cars.at(i)->GetBody()->GetPosition().x);
        std::cout << dists.back() << "\n";
    }
    
    std::cout << "Evaluating cars DONE\n";

    test_case->mutex.unlock();
    return dists;
}

void TestCase::CreateCars(const std::vector<Chromosome>& chromosomes)
{
    for(auto& car : m_cars) {
        m_cars_to_delete.push_back(car);
    }

    m_cars = std::vector<Car*>(chromosomes.size());

    for (int i = 0; i < chromosomes.size(); ++i)
    {
        m_cars.at(i) = new Car(chromosomes.at(i), m_world, -100);
    }
}

void TestCase::DeleteCars() 
{
    if(m_cars_to_delete.size() == 0u) {
        return;
    }

    if(m_world->IsLocked()) {
        std::cerr << "CANT DELETE WORLD IS LOCKED\n";
        return;
    }

    std::cerr << "DELETE CARS\n";
    for(auto& car : m_cars_to_delete) 
    {
        delete car;
    }

    std::cerr << "DELETE CARS DONE\n";
    m_cars_to_delete.clear();
}