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
        sys = new ParticleSystem();


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
        config.desvAliveTime = 1.f;

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
        case 'g': gen->generate(*sys, 500); break;
        case 'i': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,10.f)); break;
        case 'k': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,-10.f)); break;
        case 'j': gen->setPosition(gen->getPosition() + Vector3D(10.f,0.f,0.f)); break;
        case 'l': gen->setPosition(gen->getPosition() + Vector3D(-10.f,0.f,0.f)); break;
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

