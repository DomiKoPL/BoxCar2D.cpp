#ifndef CAR_H
#define CAR_H

#include "test.h"
#include "chromosome.h"
#include <array>
#include <cassert>

class Car {
public:
    Car();
    Car(const Chromosome& chromosome, b2World* world, float init_speed);
    const b2Body* GetBody() const { return m_body; }
    ~Car();
private:
	b2World* m_world;
    b2Body* m_body;
	std::array<b2Body*, 8> m_wheels;
	std::array<b2WheelJoint*, 8> m_springs;
};

#endif