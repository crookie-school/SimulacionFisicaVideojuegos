#pragma once
#include "Scene.h"
#include <vector>
#include <iostream>

class P4_Scene :
    public Scene
{
public:
    explicit P4_Scene(std::string name) : Scene(std::move(name))
    {
    }

    void init() override
    {
        // First one standard spring uniting 2 particles
        p1 = new Particle({ -10.0,10.0,0.0 }, 1, 99999, {1.f, 0.2f, 0.3f, 1.f});
        p2 = new Particle({ 10.0,10.0,0.0 }, 1, 99999, { 0.2f, 0.1f, 0.3f, 1.f });
        p2->setMass(2.0);
        f1 = new SpringForceGenerator(5, 10, p2);
        f2 = new SpringForceGenerator(5, 10, p1);

        // Then one spring with one fixed side
        p3 = new Particle({ -10.0,20.0,0.0 }, 1, 99999);
        f3 = new AnchoredSpringFG(5, 10, { 10.0,20.0,0.0 });

        p4 = new Particle({ 0.f, -5.f, 0.f }, 500.f, 999999, {0.8f, 0.6f, 0.2f, 1.f}, CreateShape(physx::PxBoxGeometry(1.f, 2.f, 1.f)));
        f4 = new BouyancyForceGenerator(0.f, 1.f, 1000);

        g = new GravityForceGenerator({ 0.f, -10.f, 0.f });
    }

    void update(double dt) override
    {
        //g->updateForce(p1, dt);
        //g->updateForce(p2, dt);
        g->updateForce(p3, dt);
        g->updateForce(p4, dt);

        f1->updateForce(p1, dt);
        f2->updateForce(p2, dt);
        f3->updateForce(p3, dt);
        f4->updateForce(p4, dt);

        p1->integrateEulerSemi(dt);
        p2->integrateEulerSemi(dt);
        p3->integrateEulerSemi(dt);
        p4->integrateEulerSemi(dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override
    {
        switch (key)
        {
        case 'f': 
            std::cout << "Aplicando fuerza a particula con muelle estático\n";
            p3->addForce({ 500.f, 0.f, 0.f });
            break;
        case 'k': f3->setK(f3->getK() + 5.); std::cout << "Constante del muelle: " << f3->getK() << '\n'; break;
        case 'j': f3->setK(f3->getK() - 5.); std::cout << "Constante del muelle: " << f3->getK() << '\n'; break;
        }

    }

    void cleanup() override
    {
        delete p1;
        delete p2;
        delete p3;
        delete p4;

        delete f1;
        delete f2;
        delete f3;
        delete f4;

        delete g;
    }

private:
    Particle* p1;
    Particle* p2;
    SpringForceGenerator* f1;
    SpringForceGenerator* f2;

    Particle* p3;
    AnchoredSpringFG* f3;

    GravityForceGenerator* g;

    Particle* p4;
    BouyancyForceGenerator* f4;
};

