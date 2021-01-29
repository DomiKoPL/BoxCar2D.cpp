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

        if(settings.m_showBest) 
        {
            auto best_car = *std::max_element(m_cars.begin(), m_cars.end(), [&](auto& a, auto& b) {
                return a->GetBody()->GetPosition().x < b->GetBody()->GetPosition().x;
            });

            g_camera.m_center.x = best_car->GetBody()->GetPosition().x;
            g_camera.m_center.y = best_car->GetBody()->GetPosition().y;
        }
        
        if(settings.m_debugInfo) 
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

            g_debugDraw.DrawString(5, m_textLine, "wheel linV\tAngV\tMotorS\tradius");
            m_textLine += m_textIncrement;

            for(int i = 0; i < 8; i++) {
                const b2Body* wheel{best_car->GetWheel(i)};
                std::string vel_string = "      ";
                if(wheel != nullptr) {
                    vel_string += std::to_string(wheel->GetLinearVelocity().x);
                    vel_string += "\t" + std::to_string(wheel->GetAngularVelocity());
                    vel_string += "\t" + std::to_string(best_car->GetWheelJoint(i)->GetMotorSpeed());
                    vel_string += "\t" + std::to_string(wheel->GetFixtureList()->GetShape()->m_radius);
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
    }

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

void CarEnvironment::CreateCars(std::vector<Chromosome>& chromosomes)
{
    for(auto& car : m_cars) 
    {
        m_cars_to_delete.push_back(car);
    }

    m_cars = std::vector<Car*>(chromosomes.size());

    for (int i = 0; i < chromosomes.size(); ++i)
    {
        m_cars.at(i) = new Car(chromosomes.at(i), m_world, -50);
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