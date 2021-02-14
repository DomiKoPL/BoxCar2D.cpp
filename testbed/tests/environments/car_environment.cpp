#include "car_environment.hpp"
#include <algorithm>
#include <string>

CarEnvironment::CarEnvironment(bool blocked) : m_blocked{blocked} 
{
    
};

void CarEnvironment::Step(Settings& settings)
{
    float timeStep = settings.m_hertz > 0.0f ? 1.0f / settings.m_hertz : float(0.0f);
    
    if(m_blocked) 
    {
        m_textLine = 26;
    
        if (settings.m_pause)
        {
            if (settings.m_singleStep)
            {
                settings.m_singleStep = 0;
            }
            else
            {
                timeStep = 0.0f;
            }

            g_debugDraw.DrawString(5, m_textLine, "****PAUSED****");
            m_textLine += m_textIncrement;
        }

        uint32 flags = 0;
        flags += settings.m_drawShapes * b2Draw::e_shapeBit;
        flags += settings.m_drawJoints * b2Draw::e_jointBit;
        flags += settings.m_drawAABBs * b2Draw::e_aabbBit;
        flags += settings.m_drawCOMs * b2Draw::e_centerOfMassBit;
        g_debugDraw.SetFlags(flags);

        if(settings.m_showBest and m_cars.size() > 0u) 
        {
            auto best_car = *std::max_element(m_cars.begin(), m_cars.end(), [&](auto& a, auto& b) {
                return a->GetBody()->GetPosition().x < b->GetBody()->GetPosition().x;
            });

            g_camera.m_center.x = best_car->GetBody()->GetPosition().x;
            g_camera.m_center.y = best_car->GetBody()->GetPosition().y;
        }
        
        if(settings.m_debugInfo and m_cars.size() > 0u) 
        {
            auto best_car = *std::max_element(m_cars.begin(), m_cars.end(), [&](auto& a, auto& b) {
                return a->GetBody()->GetPosition().x < b->GetBody()->GetPosition().x;
            });
            
            std::string x_string =   "X  \t= " + std::to_string(best_car->GetBody()->GetPosition().x);
            g_debugDraw.DrawString(5, m_textLine, x_string.c_str());
            m_textLine += m_textIncrement;

            std::string vel_string = "body velocity\t= " + std::to_string(best_car->GetBody()->GetAngularVelocity());
            g_debugDraw.DrawString(5, m_textLine, vel_string.c_str());
            m_textLine += m_textIncrement;

            g_debugDraw.DrawString(5, m_textLine, "wheel linV\tAngV\tMotorS\tradius\ttorque");
            m_textLine += m_textIncrement;

            for(int i = 0; i < 8; i++) {
                const b2Body* wheel{best_car->GetWheel(i)};
                std::string vel_string = "      ";
                if(wheel != nullptr) {
                    vel_string += std::to_string(wheel->GetLinearVelocity().x);
                    vel_string += "\t" + std::to_string(wheel->GetAngularVelocity());
                    vel_string += "\t" + std::to_string(best_car->GetWheelJoint(i)->GetMotorSpeed());
                    vel_string += "\t" + std::to_string(wheel->GetFixtureList()->GetShape()->m_radius);
                    vel_string += "\t" + std::to_string(best_car->GetWheelJoint(i)->GetMaxMotorTorque());
                } else {
                    vel_string += "none";
                }
                g_debugDraw.DrawString(5, m_textLine, vel_string.c_str());
                m_textLine += m_textIncrement;
            }
        }


    }

    m_world->SetAllowSleeping(settings.m_enableSleep);
    m_world->SetWarmStarting(settings.m_enableWarmStarting);
    m_world->SetContinuousPhysics(settings.m_enableContinuous);
    m_world->SetSubStepping(settings.m_enableSubStepping);

    m_pointCount = 0;

    m_world->Step(timeStep, settings.m_velocityIterations, settings.m_positionIterations);

    if(m_blocked) 
    {
        m_world->DebugDraw();
        g_debugDraw.Flush();
    }

    if (timeStep > 0.0f)
    {
        ++m_stepCount;
    }

    if(m_blocked) {

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

        if (settings.m_drawProfile)
        {
            const b2Profile& p = m_world->GetProfile();

            b2Profile aveProfile;
            memset(&aveProfile, 0, sizeof(b2Profile));
            if (m_stepCount > 0)
            {
                float scale = 1.0f / m_stepCount;
                aveProfile.step = scale * m_totalProfile.step;
                aveProfile.collide = scale * m_totalProfile.collide;
                aveProfile.solve = scale * m_totalProfile.solve;
                aveProfile.solveInit = scale * m_totalProfile.solveInit;
                aveProfile.solveVelocity = scale * m_totalProfile.solveVelocity;
                aveProfile.solvePosition = scale * m_totalProfile.solvePosition;
                aveProfile.solveTOI = scale * m_totalProfile.solveTOI;
                aveProfile.broadphase = scale * m_totalProfile.broadphase;
            }

            g_debugDraw.DrawString(5, m_textLine, "step [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.step, aveProfile.step, m_maxProfile.step);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "collide [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.collide, aveProfile.collide, m_maxProfile.collide);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "solve [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solve, aveProfile.solve, m_maxProfile.solve);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "solve init [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveInit, aveProfile.solveInit, m_maxProfile.solveInit);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "solve velocity [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveVelocity, aveProfile.solveVelocity, m_maxProfile.solveVelocity);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "solve position [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solvePosition, aveProfile.solvePosition, m_maxProfile.solvePosition);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "solveTOI [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveTOI, aveProfile.solveTOI, m_maxProfile.solveTOI);
            m_textLine += m_textIncrement;
            g_debugDraw.DrawString(5, m_textLine, "broad-phase [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.broadphase, aveProfile.broadphase, m_maxProfile.broadphase);
            m_textLine += m_textIncrement;
        }

        if (m_bombSpawning)
        {
            b2Color c;
            c.Set(0.0f, 0.0f, 1.0f);
            g_debugDraw.DrawPoint(m_bombSpawnPoint, 4.0f, c);

            c.Set(0.8f, 0.8f, 0.8f);
            g_debugDraw.DrawSegment(m_mouseWorld, m_bombSpawnPoint, c);
        }

        if (settings.m_drawContactPoints)
        {
            const float k_impulseScale = 0.1f;
            const float k_axisScale = 0.3f;

            for (int32 i = 0; i < m_pointCount; ++i)
            {
                ContactPoint* point = m_points + i;

                if (point->state == b2_addState)
                {
                    // Add
                    g_debugDraw.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
                }
                else if (point->state == b2_persistState)
                {
                    // Persist
                    g_debugDraw.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
                }

                if (settings.m_drawContactNormals == 1)
                {
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_axisScale * point->normal;
                    g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
                }
                else if (settings.m_drawContactImpulse == 1)
                {
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_impulseScale * point->normalImpulse * point->normal;
                    g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
                }

                if (settings.m_drawFrictionImpulse == 1)
                {
                    b2Vec2 tangent = b2Cross(point->normal, 1.0f);
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_impulseScale * point->tangentImpulse * tangent;
                    g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
                }
            }
        }
    }
}

bool CarEnvironment::IsBlocked() const {
    return m_blocked;
}

void CarEnvironment::Lock() {
    m_mutex.lock();
}

void CarEnvironment::Unlock() {
    m_mutex.unlock();
}

Test* CarEnvironment::Create()
{
    return nullptr;
}

void CarEnvironment::CreateCars(std::vector<Chromosome>& chromosomes, float init_speed)
{
    m_stepCount = 0;
    
    for(auto& car : m_cars) 
    {
        m_cars_to_delete.push_back(car);
    }

    m_cars = std::vector<Car*>(chromosomes.size());

    for (int i = 0; i < chromosomes.size(); ++i)
    {
        m_cars.at(i) = new Car(chromosomes.at(i), m_world, init_speed);
    }
}

void CarEnvironment::DeleteCars() 
{
    if(m_cars_to_delete.size() == 0u) 
    {
        return;
    }

    for(auto& car : m_cars_to_delete) 
    {
        delete car;
    }

    m_cars_to_delete.clear();
}