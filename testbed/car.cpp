#include "car.h"
#include <exception>
#include <iostream>

Car::Car() {

}

Car::Car(Chromosome& chromosome, b2World* world, float init_speed) {
    m_world = world;

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
    
    for(int i = 0; i < 8; i++) {
        if(chromosome.GetVertexWhell(i)) {
            circle.m_radius = chromosome.GetVertexWhellRadius(i);
            bd.position.Set(vertices[i].x, vertices[i].y - min_y);

            m_wheels[i] = world->CreateBody(&bd);
            m_wheels[i]->CreateFixture(&fd);

            car_mass += m_wheels[i]->GetMass();
        }
    }

    for(int i = 0; i < 8; i++) 
    {
        if(chromosome.GetVertexWhell(i)) 
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