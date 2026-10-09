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

        Particle p1;
        p1.setPos({ 0.f, 0.f, 0.f });
        p1.setVel({ 0.f, 100.f, 0.f });
        p1.setLifeTime(10.f);
        Particle p2;
        p2.setPos({ 0.f, 0.f, 0.f });
        p2.setVel({ 10.f, 5.f, 10.f });
        p2.setLifeTime(5.f);
        p2.setMass(0.f);
        p2.setDamping(0.f);
        

        gen = new ParticleGenerator<std::uniform_real_distribution<float>>(ParticleGenerationModel{ p1, p2 });
    }

    void update(double dt) override {
        sys->update(dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if (key == 'g') {
            gen->generate(*sys, 1);
        }
    }

    void cleanup() override {
        delete sys;
        delete gen;
    }

private:
    ParticleSystem* sys;
    ParticleGenerator<std::uniform_real_distribution<float>>* gen;
};

