#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "Projectile.h"

class P1_Scene :
    public Scene
{
public:
    explicit P1_Scene(std::string name) : Scene(std::move(name))
    {}

    void init() override {
        
    }

    void update(double dt) override {
        for (Projectile* p : particles) {
            p->integrateEulerSemi(dt);
        }
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        Vector3D shootDir = GetCamera()->getDir().getNormalized();
        float realVel;
        float simVel;
        float mass = 0.f;
        switch (key) {
        case 'c': // Bala de cañon
            realVel = 250.f;
            simVel = 50.f;
            mass = 15.f;
            particles.push_back(new Projectile(camera.p, shootDir, realVel + offsetVelocidad, simVel, mass + offsetMasa));
            break;
        case 'v': // Bala de tanque
            realVel = 1800.f;
            simVel = 50.f;
            mass = 40.f;
            particles.push_back(new Projectile(camera.p, shootDir, realVel + offsetVelocidad, simVel, mass + offsetMasa));
            break;
        case 'b': // Pistola
            realVel = 330.f;
            simVel = 50.f;
            mass = .1f;
            particles.push_back(new Projectile(camera.p, shootDir, realVel + offsetVelocidad, simVel, mass + offsetMasa));
            break;

        case 'y':
            offsetMasa += 10.f;
            std::cout << "Offset Masa: " << offsetMasa << '\n';
            break;
        case 'h':
            offsetMasa -= 10.f;
            std::cout << "Offset Masa: " << offsetMasa << '\n';
            break;
        case 'u':
            offsetVelocidad += 10.f;
            std::cout << "Offset Vel: " << offsetVelocidad << '\n';
            break;
        case 'j':
            offsetVelocidad -= 10.f;
            std::cout << "Offset Vel: " << offsetVelocidad << '\n';
            break;
        }
        
    }

    void cleanup() override {
        for(Projectile* p : particles) {
            delete p;
        }
    }

private:
    std::vector<Projectile*> particles;

    float offsetMasa = 0.f;
    float offsetVelocidad = 0.f;
};

