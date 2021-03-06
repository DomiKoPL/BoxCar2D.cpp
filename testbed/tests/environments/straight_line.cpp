#include "straight_line.hpp"

StraightLine::StraightLine(bool blocked) : CarEnvironment(blocked) {
    m_map_width = 1000.f;

    b2Vec2 gravity;
	gravity.Set(0.0f, -10.0f);
    m_world->SetGravity(gravity);

    b2BodyDef groundBody;
    groundBody.position.Set(0.0f, 0.0f);

    b2Body* ground = m_world->CreateBody(&groundBody);

    b2EdgeShape shape;

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.friction = 0.5f;

    shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(m_map_width + 20.f, 0.0f));
    ground->CreateFixture(&fd);

    shape.SetTwoSided(b2Vec2(m_map_width + 20.f, 0.0f), b2Vec2(m_map_width + 20.f, 50.0f));
    ground->CreateFixture(&fd);
}

std::vector<float> StraightLine::evaluate_function(std::vector<Chromosome>& chromosomes)
{
    Settings s_settings;

    Lock();

    CreateCars(chromosomes, 20);

    if(not m_blocked) 
    {
        DeleteCars();
    }

    Unlock();
    
    auto stepsBefore = m_stepCount;
    if(m_blocked) 
    {
        while(m_cars_done < m_cars.size() and m_stepCount < 30 * 60) 
        {
            if (m_stepCount % 600 < 10) std::clog << "BLOCKED\t" << m_cars_done << " " << m_cars.size() << "\t\t" << m_stepCount << "\n";
            assert(m_stepCount >= stepsBefore);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } 
    else 
    {
        while(m_cars_done < m_cars.size() and m_stepCount < 30 * 60) 
        {
            std::cerr << "STEP" << "\n";
            Lock();
            if (m_stepCount % 600 < 1) std::cerr << "UNBLOCKED???\t" << m_cars_done << " " << m_cars.size() << "\t\t" << m_stepCount << "\n";
            assert(m_stepCount >= stepsBefore);
            assert(m_world != nullptr);
            assert(m_cars.size() > 0u);
            Step(s_settings);
            Unlock();
            if(m_stepCount % 100 == 0) 
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            }
        }
    }

    Lock();
    std::vector<float> dists;

    for(int i = 0; i < chromosomes.size(); ++i)
    {
        assert(m_cars.size() > i);
        dists.push_back(m_cars.at(i)->eval(m_map_width, 30 * 60));
    }

    for(auto& car : m_cars) 
    {
        m_cars_to_delete.push_back(car);
    }

    m_cars.clear();

    DeleteCars();

    Unlock();
    return dists;
}
