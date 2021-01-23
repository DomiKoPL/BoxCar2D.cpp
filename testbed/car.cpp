#include "car.h"

Car::Car() {

}

Car::Car(const Chromosome& chromosome, b2World* world, float init_speed) {
    b2PolygonShape chassis;
    b2Vec2 vertices[8];

    for(int i = 0; i < 8; i++) {
        float r = chromosome.GetVertexMagnitude(i);
        float angle = chromosome.GetVertexAngle(i);
        vertices[i].Set(sin(angle) * r, cos(angle) * r);
    }
    chassis.Set(vertices, 8);

    b2FixtureDef bodyDef;
    bodyDef.shape = &chassis;
    bodyDef.density = 1.0f;

    short bits = 0x0001; 
    bodyDef.filter.groupIndex = -1;

    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(0.0f, 0.0f);
    m_body = world->CreateBody(&bd);
    m_body->CreateFixture(&bodyDef);
    
    b2FixtureDef fd;
    fd.filter.groupIndex = -1;

    b2CircleShape circle;
    fd.shape = &circle;
    fd.density = 1.0f;
    fd.friction = 0.9f;
   
    b2WheelJointDef jd;
    b2Vec2 axis(0.0f, 1.f);

    float hertz = 4.0f;
    float dampingRatio = 0.7f;
    float omega = 2.0f * b2_pi * hertz;

    for(int i = 0; i < 8; i++) {
        if(chromosome.GetVertexWhell(i)) {
            circle.m_radius = chromosome.GetVertexWhellRadius(i);
            bd.position.Set(vertices[i].x, vertices[i].y);
            auto& wheel{m_wheels[i]};
            wheel = world->CreateBody(&bd);
            wheel->CreateFixture(&fd);

            float mass = wheel->GetMass();

			jd.Initialize(m_body, wheel, wheel->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 20.0f;
			jd.enableMotor = true;
			jd.stiffness = mass * omega * omega;
			jd.damping = 2.0f * mass * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;
			m_springs[i] = (b2WheelJoint*)world->CreateJoint(&jd);
            m_springs[i]->SetMotorSpeed(init_speed);
        }
    }
}