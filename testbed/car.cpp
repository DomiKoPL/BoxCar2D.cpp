#include "car.h"
#include <exception>
#include <iostream>

Car::Car() {

}

Car::Car(Chromosome& chromosome, b2World* world, float init_speed) {
    m_world = world;
    chrom = chromosome;
    
    m_done = false;
    prev_step_count = -1;
    best_x = -1e9;

    b2PolygonShape chassis;
    b2Vec2 vertices[8];

    for(int i = 0; i < 8; i++) 
    {
        float r = chromosome.GetVertexMagnitude(i);
        float angle = chromosome.GetVertexAngle(i);
        vertices[i].Set(cos(angle) * r, sin(angle) * r);
    }
    chassis.Set(vertices, 8);
    
    float min_y = vertices[0].y;
    for(int i = 1; i < 8; i++) {
        min_y = std::min(min_y, vertices[i].y);
    }

    int number_of_wheels{0};
    for(int i = 0; i < 8; i++) 
    {
        if(chromosome.GetVertexWhell(i)) 
        {
            const float r{chromosome.GetVertexWhellRadius(i)};
            min_y = std::min(min_y, vertices[i].y - r);
            number_of_wheels++;
        }
    }

    b2CircleShape circle;

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(0.0f, -min_y);
    m_body = world->CreateBody(&bd);
    
    b2FixtureDef bodyDef;
    bodyDef.shape = &chassis;
    bodyDef.density = 30.0; //TODO: into chromosome
    bodyDef.restitution = 0.2;
    bodyDef.friction = 10.0;
    bodyDef.filter.groupIndex = -1;
    m_body->CreateFixture(&bodyDef);
    
    b2FixtureDef fd;
    fd.shape = &circle;
    fd.density = 100.f; // TODO: into chromosome
    fd.restitution = 0.2;
    fd.friction = 1.f;
    fd.filter.groupIndex = -1;

    b2WheelJointDef jd;
    b2Vec2 axis(0.0f, 1.0f);

    float hertz = 4.0f;
    float dampingRatio = 0.7f;
    float omega = 2.0f * b2_pi * hertz;

    m_wheels.fill(nullptr);
    m_springs.fill(nullptr);

    float car_mass = m_body->GetMass();

    int whells_cnt = 0;
    for(int i = 0; i < 8; i++) {
        if(chromosome.GetVertexWhell(i)) {
            whells_cnt++;
        }
    }

    if(whells_cnt >= 2) {
        for(int i = 0; i < 8; i++) {
            if(chromosome.GetVertexWhell(i)) {
                circle.m_radius = chromosome.GetVertexWhellRadius(i);
                bd.position.Set(vertices[i].x, vertices[i].y - min_y);

                m_wheels[i] = world->CreateBody(&bd);
                m_wheels[i]->CreateFixture(&fd);

                car_mass += m_wheels[i]->GetMass();
            }
        }
    }

    for(int i = 0; i < 8; i++) 
    {
        if(m_wheels[i] != nullptr) 
        {
            float torque = car_mass * -m_world->GetGravity().y / chromosome.GetVertexWhellRadius(i);

            float mass = m_wheels[i]->GetMass();
            jd.Initialize(m_body, m_wheels[i], m_wheels[i]->GetPosition(), axis);
            jd.motorSpeed = -init_speed;
            jd.maxMotorTorque = torque;
            jd.enableMotor = true;
            jd.stiffness = mass * omega * omega;
            jd.damping = 2.0f * mass * dampingRatio * omega;
            jd.lowerTranslation = -0.25f;
            jd.upperTranslation = 0.25f;
            jd.enableLimit = true;
            m_springs[i] = (b2WheelJoint*)world->CreateJoint(&jd);
        }
    }
}

void Car::update(int step_count) {
    if(m_done) return;

    float x = m_body->GetPosition().x;
    // std::cerr << "DONE " << x << " " << best_x << " " << step_count << " " << prev_step_count << "\n";
    float delta_x = std::max(5.f, 20.f - step_count / 60.f);

    if(prev_step_count >= 0) {
        if(x < best_x - delta_x) {
            m_done = true;
        }

        if(x <= best_x + 10.f and step_count - prev_step_count > 3 * 60) {
            m_done = true;
        }
    }
    
    if(prev_step_count == -1 or best_x < x) {
        best_x = x;
        prev_step_count = step_count;
    }
}

bool Car::is_done() const {
    return m_done;
}

void Car::set_done(bool done) {
    m_done = done;
}

float Car::get_best_x() const {
    return best_x;
}

float Car::eval(float map_width, int max_time) const {
    if(best_x > map_width) {
        return -(100 + max_time - prev_step_count);
    }

    return map_width - best_x;
}

Car::~Car() 
{
    m_world->DestroyBody(m_body);

    for(auto& whell : m_wheels) 
    {
        if(whell != nullptr)
        {
            m_world->DestroyBody(whell);
        }
    }

    m_body = nullptr;
    for(auto& whell : m_wheels) 
    {
        whell = nullptr;
    }

    for(auto& spring : m_springs) 
    {
        spring = nullptr;
    }
}