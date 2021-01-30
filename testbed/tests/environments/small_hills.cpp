#include "small_hills.hpp"
#include <vector>

SmallHills::SmallHills(bool blocked) : CarEnvironment(blocked) {
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

    std::vector<b2Vec2> points {
        b2Vec2(-200.0f, 0.0f), 
        b2Vec2(30.0f, 0.0f), 
        b2Vec2(100.f, 30.0f),
        b2Vec2(200.f, 50.0f),
        b2Vec2(300.f, -10.0f),
        b2Vec2(350.f, 20.0f),
        b2Vec2(400.f, 25.0f),
        b2Vec2(600.f, 15.0f),
        b2Vec2(900.f, 30.0f),
        b2Vec2(m_map_width, 50.f),
        b2Vec2(m_map_width + 20.f, 50.f)
    };
    
    for(int i = 1; i < points.size(); i++) {
        shape.SetTwoSided(points[i - 1], points[i]);
        ground->CreateFixture(&fd);
    }
}

std::vector<float> SmallHills::evaluate_function(std::vector<Chromosome>& chromosomes)
{
    Settings s_settings;

    Lock();

    CreateCars(chromosomes, 50);

    if(not m_blocked) 
    {
        DeleteCars();
    }

    Unlock();
    
    auto stepsBefore = m_stepCount;
    if(m_blocked) 
    {
        while(m_stepCount - stepsBefore < 60 * 20) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } 
    else 
    {
        while(m_stepCount - stepsBefore < 60 * 20) 
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
