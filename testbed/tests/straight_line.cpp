#include "test.h"
#include <string>
#include "car.h"
#include "chromosome.h"
#include <algorithm>
#include "straight_line.hpp"
#include <iostream>
#include "settings.h"

	StraightLine::StraightLine()
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


    void StraightLine::Step(Settings& settings)
	{
		// std::cout << "eo1\n";
		m_textLine += m_textIncrement;
		
		std::array<float, 20> x;
		for(int i = 0; i < 20; i++) {
			x[i] = m_cars[i]->GetBody()->GetPosition().x;
		}
		sort(x.rbegin(), x.rend());
		// std::cout << "eo21\n";
		g_camera.m_center.x = x[0];
		// for(int i = 0; i < 5; i++) {
		// 	g_debugDraw.DrawString(5, m_textLine + i * m_textIncrement, std::to_string(x[i]).c_str());
		// }
		// std::cout << "e3\n";
		if (testbedMode) Test::Step(settings);
		else 
		{
			float timeStep = settings.m_hertz > 0.0f ? 1.0f / settings.m_hertz : float(0.0f);

			m_world->SetAllowSleeping(settings.m_enableSleep);
			m_world->SetWarmStarting(settings.m_enableWarmStarting);
			m_world->SetContinuousPhysics(settings.m_enableContinuous);
			m_world->SetSubStepping(settings.m_enableSubStepping);

			m_pointCount = 0;

			m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations);

			m_world->DebugDraw();

			if (timeStep > 0.0f)
			{
				++m_stepCount;
			}
			// Track maximum profile times
			{
				const b2Profile& p = m_world->GetProfile();
				m_maxProfile.step = b2Max(m_maxProfile.step, p.step);
				m_maxProfile.collide = b2Max(m_maxProfile.collide, p.collide);
				m_maxProfile.solve = b2Max(m_maxProfile.solve, p.solve);
				m_maxProfile.solveInit = b2Max(m_maxProfile.solveInit, p.solveInit);
				m_maxProfile.solveVelocity = b2Max(m_maxProfile.solveVelocity, p.solveVelocity);
				m_maxProfile.solvePosition = b2Max(m_maxProfile.solvePosition, p.solvePosition);
				m_maxProfile.solveTOI = b2Max(m_maxProfile.solveTOI, p.solveTOI);
				m_maxProfile.broadphase = b2Max(m_maxProfile.broadphase, p.broadphase);

				m_totalProfile.step += p.step;
				m_totalProfile.collide += p.collide;
				m_totalProfile.solve += p.solve;
				m_totalProfile.solveInit += p.solveInit;
				m_totalProfile.solveVelocity += p.solveVelocity;
				m_totalProfile.solvePosition += p.solvePosition;
				m_totalProfile.solveTOI += p.solveTOI;
				m_totalProfile.broadphase += p.broadphase;
			}
		}
	}

	Test* StraightLine::Create()
	{
		return new StraightLine;
	}

	void StraightLine::CreateCars(std::array<Chromosome, 20> chromosomes)
	{
		for (int i = 0; i < chromosomes.size(); ++i)
		{
			delete m_cars.at(i);
			m_cars.at(i) = new Car(chromosomes.at(i), m_world, -100);
		}
	}

	static int testIndex = RegisterTest("StraightLine", "StraightLine", StraightLine::Create);