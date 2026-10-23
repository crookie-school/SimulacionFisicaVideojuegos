#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "Particle.h"
#include "ForceGenerator.h"
#include <random>

static const Vector3D WIND_VOL_POS = { 0.f, 0.f, -100.f };
static const Vector3D WIND_VOL_SZE = { 50.f, 50.f, 50.f};

class P3_Scene :
    public Scene
{
public:
    explicit P3_Scene(std::string name) : Scene(std::move(name)),
        gravity({ 0.f, -10.f, 0.f }), bouyancy({ 0.f, 5.f, 0.f }), pGrav({ 10.f, 20.f, 0.f }, 1.f, 20.f), pGravManual({ 15.f, 20.f, 0.f }, 1.f, 20.f), pGravPesada({ 20.f, 20.f, 0.f }, 20.f, 20.f), pBouy({ -10.f, 20.f, 0.f }, 2.f, 20.f),
        wind({ -100.f, 0.f, 0.f }, WIND_VOL_POS, WIND_VOL_SZE), pWind(WIND_VOL_POS + WIND_VOL_SZE * 0.5f, 5.f, 100.f, { 1.f, 1.f, 0.f, 1.f }),
        windVolShapeMarkers1(WIND_VOL_POS, 1.f, 9999.f), windVolShapeMarkers2(WIND_VOL_POS + WIND_VOL_SZE, 1.f, 9999.f),
        tornado(20.f, 0.f, WIND_VOL_POS, WIND_VOL_SZE), // La misma forma que la del volumen del viento para reusar los marcadores
        explosion({100.f, 0.f, 0.f}, 2500.f, 25.f, 1.5f)
    {
    }

    void init() override
    {
        pGravManual.setAcc({ 0.f, -10.f, 0.f });

        sys = new ParticleSystem(Vector3D(0.f, 0.f, 0.f));


        ParticleGenerationConfig config;
        config.shape = GeneratorShape::Box;
        config.meanPose = Vector3D{ 100.f, 0.f, 0.f };
        config.desvPose = Vector3D{ 7.f, 7.f, 7.f };

        config.velType = GeneratorVelType::Radial;
        config.meanVel = Vector3D{ 0.f, 0.f, 0.f };
        config.desvVel = Vector3D{ 0.f, 0.f, 0.f };
        config.meanSpeed = 0.f;
        config.desvSpeed = 0.f;

        config.meanAliveTime = 20.f;
        config.desvAliveTime = 0.f;

        config.meanMass = 1.f;
        config.desvMass = 0.f;

        config.meanDamping = 0.99f;
        config.desvDamping = 0.f;

        config.meanColor = { 0.8f, 0.6f, 0.3f, 1.f };
        config.desvColor = { 0.1f, 0.1f, 0.1f, 0.f };

        gen = new ParticleGenerator(5.f, config);
        gen->setEmitting(false);
    }

    void update(double dt) override
    {

        gravity.updateForce(&pGrav, dt);
        gravity.updateForce(&pGravPesada, dt);
        bouyancy.updateForce(&pBouy, dt);

        //wind.updateForce(&pWind, dt);
        tornado.updateForce(&pWind, dt);

        pGrav.integrateEulerSemi(dt);
        pGravManual.integrateEulerSemi(dt);
        pGravPesada.integrateEulerSemi(dt);
        pBouy.integrateEulerSemi(dt);
        pWind.integrateEulerSemi(dt);

        explosion.updateForce(*sys, dt);
        sys->update(dt);
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override
    {
        switch (key)
        {
            case 'j': pWind.addForce({ 0.f, -800.f, 0.f }); break; // Push down
            case 'k': pWind.addForce({ 0.f, 800.f, 0.f }); break; // Push up
            case 'h': pWind.addForce({-800.f, 0.f, 0.f}); break; // Push to the left
            case 'l': pWind.addForce({ 800.f, 0.f, 0.f}); break; // Push to the right

            case 'g': gen->generate(*sys, 1000); std::cout << "Generating particles\n"; break;
            case 'e': explosion.beginExplosion(); std::cout << "Explosion started\n"; break;
        }
    }

    void cleanup() override
    {
        delete sys;
        delete gen;
    }

private:
    GravityForceGenerator gravity;
    GravityForceGenerator bouyancy;
    Particle pGrav;
    Particle pGravManual;
    Particle pGravPesada;
    Particle pBouy;

    WindForceGenerator wind;
    TornadoForceGenerator tornado;
    Particle pWind;
    Particle windVolShapeMarkers1;
    Particle windVolShapeMarkers2;


    ParticleSystem* sys;
    ParticleGenerator* gen;
    ExplosionForceGenerator explosion;
};

