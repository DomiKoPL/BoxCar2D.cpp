// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "test.h"
#include <string>

class DomiKo : public Test
{
public:

	DomiKo()
	{
		m_speed = 50.0f;
        m_mapWidth = 100.f;
		
        b2BodyDef groundBody;
        groundBody.position.Set(0.0f, 0.0f);

        b2Body* ground = m_world->CreateBody(&groundBody);

        b2EdgeShape shape;

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 0.0f;
        fd.friction = 0.6f;

        shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(m_mapWidth + 20.f, 0.0f));
        ground->CreateFixture(&fd);

        shape.SetTwoSided(b2Vec2(m_mapWidth + 20.f, 0.0f), b2Vec2(m_mapWidth + 20.f, 50.0f));
        ground->CreateFixture(&fd);

		// Car
		{
			b2PolygonShape chassis;
			b2Vec2 vertices[8];
            
            float lox = -3.f;
            float hix = 3.f;
            float loy = 0.f;
            float hiy = 2.f;

            for(int i = 0; i < 8; i++) {
                vertices[i].Set(RandomFloat(lox, hix), RandomFloat(loy, hiy));
            }
            
			chassis.Set(vertices, 8);

			b2CircleShape circle;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 5.0f);
			m_car = m_world->CreateBody(&bd);
			m_car->CreateFixture(&chassis, 1.0f);

			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 1.0f;
			fd.friction = 0.9f;

			circle.m_radius = RandomFloat(0.1f, 0.6f);
			bd.position.Set(vertices[3].x, vertices[3].y + 5.f);
			m_wheel1 = m_world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);

			circle.m_radius = RandomFloat(0.1f, 0.6f);
			bd.position.Set(vertices[4].x, vertices[4].y + 5.f);
			m_wheel2 = m_world->CreateBody(&bd);
			m_wheel2->CreateFixture(&fd);

			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.f);

			float mass1 = m_wheel1->GetMass();
			float mass2 = m_wheel2->GetMass();

			float hertz = 4.0f;
			float dampingRatio = 0.7f;
			float omega = 2.0f * b2_pi * hertz;

			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 20.0f;
			jd.enableMotor = true;
			jd.stiffness = mass1 * omega * omega;
			jd.damping = 2.0f * mass1 * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;
			m_spring1 = (b2WheelJoint*)m_world->CreateJoint(&jd);

			jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 20.0f;
			jd.enableMotor = true;
			jd.stiffness = mass2 * omega * omega;
			jd.damping = 2.0f * mass2 * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;
			m_spring2 = (b2WheelJoint*)m_world->CreateJoint(&jd);
		}

        m_spring1->SetMotorSpeed(-m_speed);
        m_spring2->SetMotorSpeed(-m_speed);
	}

    void Step(Settings& settings) override
	{
		m_textLine += m_textIncrement;

		g_camera.m_center.x = m_car->GetPosition().x;
        if(m_car->GetPosition().x > m_mapWidth) {
		    g_debugDraw.DrawString(5, m_textLine, "WICTORY");
        } else {
		    g_debugDraw.DrawString(5, m_textLine, std::to_string(m_car->GetPosition().x).c_str());
        }
		Test::Step(settings);
	}

	static Test* Create()
	{
		return new DomiKo;
	}

	b2Body* m_car;
	b2Body* m_wheel1;
	b2Body* m_wheel2;

	float m_speed;
	b2WheelJoint* m_spring1;
	b2WheelJoint* m_spring2;

    float m_mapWidth;
};

static int testIndex = RegisterTest("Benchmark", "DomiKo", DomiKo::Create);
