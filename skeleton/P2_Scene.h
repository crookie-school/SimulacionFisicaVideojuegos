#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "ParticleSystem.h"
#include "ParticleGenerator.h"
#include <random>

class P2_Scene :
    public Scene
{
public:
    explicit P2_Scene(std::string name) : Scene(std::move(name)), sys()
    {
    }

    void init() override {
        sys = new ParticleSystem(Vector3D(0.f, -20.f, 0.f));


        ParticleGenerationConfig config;
        config.shape = GeneratorShape::Sphere;
        config.meanPose = Vector3D{ 0.f, 0.f, 0.f };
        config.desvPose = Vector3D{ 10.f, 10.f, 10.f };

        config.velType = GeneratorVelType::Independent;
        config.meanVel = Vector3D{ 0.f, 0.f, 0.f };
        config.desvVel = Vector3D{ 0.f, 0.f, 0.f };
        config.meanSpeed = 10.f;
        config.desvSpeed = 0.f;

        config.meanAliveTime = 2.f;
        config.desvAliveTime = 0.f;

        config.meanMass = 1.f;
        config.desvMass = 0.f;

        config.meanDamping = 0.99f;
        config.desvDamping = 0.f;

        gen = new ParticleGenerator(1000.f, config);
    }

    void update(double dt) override {
        sys->update(dt);
        gen->update(*sys, dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        switch (key)
        {
        case 'g': gen->generate(*sys, 500); std::cout << "Generated 500 particles\n"; break;

        case 'i': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,10.f)); break;
        case 'k': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,-10.f)); break;
        case 'j': gen->setPosition(gen->getPosition() + Vector3D(10.f,0.f,0.f)); break;
        case 'l': gen->setPosition(gen->getPosition() + Vector3D(-10.f,0.f,0.f)); break;

        case 'e': gen->setEmitting(!gen->isEmitting()); std::cout << "Is emitting: " << (gen->isEmitting() ? "True" : "False") << '\n'; break;

        case 'c': gen->setShape(GeneratorShape::Point); std::cout << "Emission Shape: Point\n"; break;
        case 'v': gen->setShape(GeneratorShape::Sphere); std::cout << "Emission Shape: Sphere\n"; break;
        case 'b': gen->setShape(GeneratorShape::Box); std::cout << "Emission Shape: Box\n"; break;

        case '+': gen->setEmissionRate(gen->getEmissionRate() + 100); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;
        case '-': gen->setEmissionRate(gen->getEmissionRate() - 100); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;

        case 'r': gen->setVelType(GeneratorVelType::Radial); break;
        case 't': gen->setVelType(GeneratorVelType::Independent); break;
        case 'y': gen->setVelocity(Vector3D(0.f, 10.f, 0.f)); break;
        case 'u': gen->setVelocity(Vector3D(0.f, 0.f, 0.f)); break;
        default:  break;
        }
    }

    void cleanup() override {
        delete sys;
        delete gen;
    }

private:
    ParticleSystem* sys;
    ParticleGenerator* gen;
};

