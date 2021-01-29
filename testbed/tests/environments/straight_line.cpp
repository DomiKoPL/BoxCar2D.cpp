#include "straight_line.hpp"

StraightLine::StraightLine(bool blocked) : CarEnvironment(blocked) {
    m_map_width = 1000.f;

    b2Vec2 gravity;
	gravity.Set(0.0f, -20.0f);
    m_world->SetGravity(gravity);

    b2BodyDef groundBody;
    groundBody.position.Set(0.0f, 0.0f);

    b2Body* ground = m_world->CreateBody(&groundBody);

    b2EdgeShape shape;

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.0f;
    fd.friction = 0.6f;

    shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(m_map_width + 20.f, 0.0f));
    ground->CreateFixture(&fd);

    shape.SetTwoSided(b2Vec2(m_map_width + 20.f, 0.0f), b2Vec2(m_map_width + 20.f, 50.0f));
    ground->CreateFixture(&fd);
}

std::vector<float> StraightLine::evaluate_function(std::vector<Chromosome>& chromosomes)
{
    Settings s_settings;

    Lock();

    CreateCars(chromosomes);

    if(not m_blocked) 
    {
        DeleteCars();
    }

    Unlock();
    
    auto stepsBefore = m_stepCount;
    if(m_blocked) 
    {
        while(m_stepCount - stepsBefore < 60 * 10) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } 
    else 
    {
        while(m_stepCount - stepsBefore < 60 * 10) 
        {
            Step(s_settings);
        }
    }

    Lock();
    std::vector<float> dists;

    for(int i = 0; i < chromosomes.size(); ++i)
    {
        dists.push_back(m_map_width - m_cars.at(i)->GetBody()->GetPosition().x);
    }

    Unlock();
    return dists;
}
