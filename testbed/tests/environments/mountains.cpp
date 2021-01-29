#include "mountains.hpp"
#include <vector>

Mountains::Mountains(bool blocked) : CarEnvironment(blocked) {

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

    m_map_width = 1000.f;
    std::vector<b2Vec2> points {
        b2Vec2(-200.0f, 0.0f), 
        b2Vec2(100.0f, 0.0f), 
        b2Vec2(110.f, 5.f),
        b2Vec2(115.f, 0.f),
        b2Vec2(130.f, 20.f),
        b2Vec2(135.f, 25.f),
        b2Vec2(140.f, 35.f),
        b2Vec2(160.f, 50.f),
        b2Vec2(180.f, 40.f),
        b2Vec2(200.f, 55.f),
        b2Vec2(220.f, 45.f),
        b2Vec2(240.f, 60.f),
        b2Vec2(280.f, 80.f),
        b2Vec2(300.f, 100.f)
    };
    
    for(int i = 1; i < points.size(); i++) {
        shape.SetTwoSided(points[i - 1], points[i]);
        ground->CreateFixture(&fd);
    }
}

std::vector<float> Mountains::evaluate_function(std::vector<Chromosome>& chromosomes)
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
