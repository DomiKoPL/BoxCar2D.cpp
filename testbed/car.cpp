#include "car.h"
#include <exception>
#include <iostream>

Car::Car() {

}

Car::Car(const Chromosome& chromosome, b2World* world, float init_speed) {
    m_world = world;

    b2PolygonShape chassis;
    b2Vec2 vertices[8];

    for(int i = 0; i < 8; i++) {
        float r = chromosome.GetVertexMagnitude(i);
        float angle = chromosome.GetVertexAngle(i);
        vertices[i].Set(sin(angle) * r, cos(angle) * r);
    }
    chassis.Set(vertices, 8);

    b2CircleShape circle;

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(0.0f, 5.0f);
    m_body = world->CreateBody(&bd);
    
    b2FixtureDef bodyDef;
    bodyDef.shape = &chassis;
    bodyDef.density = 1.0f;
    bodyDef.filter.groupIndex = -1;
    m_body->CreateFixture(&bodyDef);

    b2FixtureDef fd;
    fd.filter.groupIndex = -1;
    fd.shape = &circle;
    fd.density = 1.0f;
    fd.friction = 0.9f;

    b2WheelJointDef jd;
    b2Vec2 axis(0.0f, 1.0f);

    float hertz = 4.0f;
    float dampingRatio = 0.7f;
    float omega = 2.0f * b2_pi * hertz;

    m_wheels.fill(nullptr);
    m_springs.fill(nullptr);

    for(int i = 0; i < 8; i++) {
        if(chromosome.GetVertexWhell(i)) {
            circle.m_radius = chromosome.GetVertexWhellRadius(i);
            bd.position.Set(vertices[i].x, 5 + vertices[i].y);
            m_wheels[i] = world->CreateBody(&bd);
            m_wheels[i]->CreateFixture(&fd);

            float mass1 = m_wheels[i]->GetMass();

            jd.Initialize(m_body, m_wheels[i], m_wheels[i]->GetPosition(), axis);
            jd.motorSpeed = 0.0f;
            jd.maxMotorTorque = 20.0f;
            jd.enableMotor = true;
            jd.stiffness = mass1 * omega * omega;
            jd.damping = 2.0f * mass1 * dampingRatio * omega;
            jd.lowerTranslation = -0.25f;
            jd.upperTranslation = 0.25f;
            jd.enableLimit = true;
            m_springs[i] = (b2WheelJoint*)world->CreateJoint(&jd);

            m_springs[i]->SetMotorSpeed(init_speed);
        }
    }
}

Car::~Car() {
    m_world->DestroyBody(m_body);
    // std::cerr << "BODY DESTROYED\n";

    for(auto& whell : m_wheels) {
        if(whell != nullptr) {
            m_world->DestroyBody(whell);
        }
    }
    // std::cerr << "WHELLS DESTROYED\n";

    m_body = nullptr;
    for(auto& whell : m_wheels) {
        whell = nullptr;
    }
    // std::cerr << "CHANGED TO NULLPTR\n";

    for(auto& spring : m_springs) {
        spring = nullptr;
    }
}