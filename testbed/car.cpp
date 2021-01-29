#include "car.h"
#include <exception>
#include <iostream>

Car::Car() {

}

Car::Car(Chromosome& chromosome, b2World* world, float init_speed) {
    m_world = world;

    // for(int i = 0; i < 8; i++) {
    //     float r = chromosome.GetVertexMagnitude(i);
    //     float angle = chromosome.GetVertexAngle(i);
    //     float x = cos(angle) * r;
    //     float y = sin(angle) * r;
    //     std::cerr << x << " " << y << "\t";
    //     std::cerr << chromosome.GetVertexWhell(i) << " r=" << chromosome.GetVertexWhellRadius(i) << "\n";
    // }

    b2PolygonShape chassis;
    b2Vec2 vertices[8];

    for(int i = 0; i < 8; i++) 
    {
        float r = chromosome.GetVertexMagnitude(i);
        float angle = chromosome.GetVertexAngle(i);
        vertices[i].Set(cos(angle) * r, sin(angle) * r);
    }
    chassis.Set(vertices, 8);

    for(int i = 0; i < 8; i++) 
    {
        float r = chromosome.GetVertexMagnitude(i);
        float angle = chromosome.GetVertexAngle(i);
        float x = cos(angle) * r;
        float y = sin(angle) * r;
        assert(std::abs(x - vertices[i].x) <= 0.0001);
        assert(std::abs(y - vertices[i].y) <= 0.0001);
    }

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
    bodyDef.friction = 5.0f;
    bodyDef.density = 1.0f;
    bodyDef.filter.groupIndex = -1;
    m_body->CreateFixture(&bodyDef);

    b2FixtureDef fd;
    fd.filter.groupIndex = -1;
    fd.shape = &circle;
    fd.density = 1.f;
    fd.friction = 0.9f;

    b2WheelJointDef jd;
    b2Vec2 axis(0.0f, 1.0f);

    float hertz = 4.0f;
    float dampingRatio = 0.7f;
    float omega = 2.0f * b2_pi * hertz;

    m_wheels.fill(nullptr);
    m_springs.fill(nullptr);

    for(int i = 0; i < 8; i++) 
    {
        if(chromosome.GetVertexWhell(i)) 
        {
            circle.m_radius = chromosome.GetVertexWhellRadius(i);
            bd.position.Set(vertices[i].x, vertices[i].y - min_y);

            m_wheels[i] = world->CreateBody(&bd);
            m_wheels[i]->CreateFixture(&fd);
            // m_wheels[i]->SetAngularDamping(0.7f);
            float mass1 = m_wheels[i]->GetMass();

            jd.Initialize(m_body, m_wheels[i], m_wheels[i]->GetPosition(), axis);
            jd.motorSpeed = init_speed;
            jd.maxMotorTorque = 20.f;
            jd.enableMotor = true;
            jd.stiffness = mass1 * omega * omega;
            jd.damping = 2.0f * mass1 * dampingRatio * omega;
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