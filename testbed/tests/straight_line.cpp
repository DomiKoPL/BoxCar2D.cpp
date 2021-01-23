#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>

class StraightLine : public Test
{
public:

	StraightLine()
	{
        m_mapWidth = 1000.f;
		
        b2BodyDef groundBody;
        groundBody.position.Set(0.0f, -10.0f);

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

		for(auto& car : m_cars) {
			Chromosome chromosome;
			chromosome.Random();
			car = new Car(chromosome, m_world, -100);
		}
	}

    void Step(Settings& settings) override
	{
		m_textLine += m_textIncrement;
		
		std::array<float, 20> x;
		for(int i = 0; i < 20; i++) {
			x[i] = m_cars[i]->GetBody()->GetPosition().x;
		}
		sort(x.rbegin(), x.rend());

		g_camera.m_center.x = x[0];
		for(int i = 0; i < 5; i++) {
			g_debugDraw.DrawString(5, m_textLine + i * m_textIncrement, std::to_string(x[i]).c_str());
		}
		Test::Step(settings);
	}

	static Test* Create()
	{
		return new StraightLine;
	}

	std::array<Car*, 20> m_cars;
    float m_mapWidth;
};

static int testIndex = RegisterTest("StraightLine", "StraightLine", StraightLine::Create);
