#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "Particle.h"
#include "ForceGenerator.h"
#include <random>

class P3_Scene :
    public Scene
{
public:
    explicit P3_Scene(std::string name) : Scene(std::move(name)),
        AOEorigin({0.f, 0.f, 0.f})
    {
    }

    void init() override
    {
        sys = new ParticleSystem();

        // Configuraci'on del generador que usamos para crear particulas en el AOE
        ParticleGenerationConfig config;
        config.shape = GeneratorShape::Box;
        config.meanPose = AOEorigin.p;
        config.desvPose = AOEsize/2.f;

        config.velType = GeneratorVelType::Radial;
        config.meanVel = Vector3D{ 0.f, 0.f, 0.f };
        config.desvVel = Vector3D{ 0.f, 0.f, 0.f };
        config.meanSpeed = 0.f;
        config.desvSpeed = 0.f;

        config.meanAliveTime = 5.f;
        config.desvAliveTime = 0.f;

        config.meanMass = 1.f;
        config.desvMass = 0.f;

        config.meanDamping = 0.99f;
        config.desvDamping = 0.f;

        config.meanColor = { 0.8f, 0.6f, 0.3f, 1.f };
        config.desvColor = { 0.1f, 0.1f, 0.1f, 0.f };

        gen = new ParticleGenerator(200.f, config);
        gen->setEmitting(true);


        // Crea una caja con lineas para ver el area de efecto claramente (me lo enseñó un compañero)
        physx::PxShape* shape = CreateShape(physx::PxBoxGeometry(AOEsize / 2.f));
        shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
        shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
        AOEDebugBox = new RenderItem(shape, &AOEorigin, { 0.f, 0.f, 0.f, 1.f });


        // Creamos los generadores
        gravityFG = new GravityForceGenerator({0.f, -10.f, 0.f});
        windFG = new WindForceGenerator(0.4f, { 100.f, 0.f, 0.f }, AOEorigin.p - AOEsize / 2.f, AOEsize);
        tornadoFG = new TornadoForceGenerator(10.f, 2.f, 0.4f, AOEorigin.p - AOEsize / 2.f, AOEsize);
        explosionFG = new ExplosionForceGenerator(AOEorigin.p, 100000.f, AOEsize.magnitudeSquared(), 0.05f);
    }

    void update(double dt) override
    {
        if (enableGravity) gravityFG->updateForce(*sys, dt);
        if (enableWind) windFG->updateForce(*sys, dt);
        if (enableTornado) tornadoFG->updateForce(*sys, dt);
        explosionFG->updateForce(*sys, dt);

        gen->update(*sys, dt);
        sys->update(dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override
    {
        switch (key)
        {
        case 'e': gen->setEmitting(!gen->isEmitting()); std::cout << "Emission: " << (gen->isEmitting() ? "Activated" : "Deactivated") << '\n'; break;
        case '+': gen->setEmissionRate(gen->getEmissionRate() + 50); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;
        case '-': gen->setEmissionRate(gen->getEmissionRate() - 50); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;

        case 'g': enableGravity = !enableGravity; std::cout << "Gravity: " << (enableGravity ? "Activated" : "Deactivated") << '\n'; break;
        case 'r': enableWind = !enableWind; std::cout << "Wind: " << (enableWind ? "Activated" : "Deactivated") << '\n'; break;
        case 't': enableTornado = !enableTornado; std::cout << "Tornado: " << (enableTornado ? "Activated" : "Deactivated") << '\n'; break;
        case 'x': explosionFG->beginExplosion(); std::cout << "Explosion started\n"; break;
        }
    }

    void cleanup() override
    {
        delete sys;
        delete gen;
        AOEDebugBox->release();

        delete gravityFG;
        delete windFG;
        delete tornadoFG;
        delete explosionFG;
    }

private:
    ParticleSystem* sys;
    ParticleGenerator* gen;

    // Caja para visualizar el AOE (Area of effect)
    physx::PxTransform AOEorigin;
    Vector3 AOEsize = { 80, 80, 80 };
    RenderItem* AOEDebugBox;

    // Generadores de fuerzas
    bool enableGravity = false;
    GravityForceGenerator* gravityFG;
    bool enableWind = false;
    WindForceGenerator* windFG;
    bool enableTornado = false;
    TornadoForceGenerator* tornadoFG;
    // La explosion solo se activa con la funci'on beginExplosion()
    ExplosionForceGenerator* explosionFG;
};

