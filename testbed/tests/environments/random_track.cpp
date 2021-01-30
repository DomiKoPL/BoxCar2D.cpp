#include "random_track.hpp"
#include <vector>
#include <chrono>
#include <random>

template <typename T>T random(std::mt19937& gen, T min, T max) {
    using dist = std::conditional_t<
        std::is_integral<T>::value, std::uniform_int_distribution<T>,
        std::uniform_real_distribution<T>>;
    return dist{min, max}(gen);
}

RandomTrack::RandomTrack(bool blocked, int seed) : CarEnvironment(blocked) {
    auto gen = std::mt19937(seed);
    
    int number_of_points = random(gen, 50, 100);
    
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
    
    b2Vec2 start(30.f, 0.f);
    shape.SetTwoSided(b2Vec2(-200.f, 0.f), start);
    ground->CreateFixture(&fd);
    float cur_angle = 0.f;

    for(int i = 0; i < number_of_points; i++) {
        float width = random(gen, 2.f, 40.f);
        cur_angle += random(gen, -b2_pi / 10, b2_pi / 10);
        cur_angle = std::clamp(cur_angle, -b2_pi / 4, b2_pi / 4);
        b2Vec2 cur(start + b2Vec2(cos(cur_angle) * width, sin(cur_angle) * width));
        shape.SetTwoSided(start, cur);
        ground->CreateFixture(&fd);
        start = cur;
    }

    m_map_width = start.x;
    b2Vec2 end(start.x + 50.f, start.y);
    shape.SetTwoSided(start, end);
    ground->CreateFixture(&fd);
    
    shape.SetTwoSided(end, end + b2Vec2(0.f, 50.f));
    ground->CreateFixture(&fd);
}

std::vector<float> RandomTrack::evaluate_function(std::vector<Chromosome>& chromosomes)
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
