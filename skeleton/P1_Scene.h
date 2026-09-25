#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <array>
#include "Particle.h"

class P1_Scene :
    public Scene
{
public:
    explicit P1_Scene(std::string name) : Scene(std::move(name)),
        p({0, 0, 0}, {0, 1000, 0}, 0.99f)
    {}

    void init() override {
        
    }

    void update(double dt) override {
        p.integrateEulerSemi(dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
    }

    void cleanup() override {
        
    }

private:
    Particle p;
};

