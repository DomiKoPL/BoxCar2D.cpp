#ifndef CAR_H
#define CAR_H

#include "test.h"
#include "chromosome.h"
#include <array>

class Car {
public:
    Car();
    Car(Chromosome& chromosome, b2World* world, float init_speed);
    const b2Body* GetBody() const { return m_body; }
    const b2Body* GetWheel(int idx) const { return m_wheels.at(idx); }
    const b2WheelJoint* GetWheelJoint(int idx) const { return m_springs.at(idx); }
    void update(int step_count);
    bool is_done() const;
    void set_done(bool done);
    float get_best_x() const;
    float eval(float map_width, int max_time) const;
    ~Car();
    Chromosome chrom;
private:
    float best_x;
    int prev_step_count;
    bool m_done;
	b2World* m_world;
    b2Body* m_body;
	std::array<b2Body*, 8> m_wheels;
	std::array<b2WheelJoint*, 8> m_springs;
};

#endif