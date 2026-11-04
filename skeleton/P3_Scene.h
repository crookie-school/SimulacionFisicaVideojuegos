#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "Particle.h"
#include "ForceGenerator.h"
#include "ParticleGenerator.h"
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
        genConfig.shape = GeneratorShape::Box;
        genConfig.meanPose = AOEorigin.p;
        genConfig.desvPose = AOEsize/2.f;

        genConfig.velType = GeneratorVelType::Radial;
        genConfig.meanVel = Vector3D{ 0.f, 0.f, 0.f };
        genConfig.desvVel = Vector3D{ 0.f, 0.f, 0.f };
        genConfig.meanSpeed = 0.f;
        genConfig.desvSpeed = 0.f;

        genConfig.meanAliveTime = 5.f;
        genConfig.desvAliveTime = 0.f;

        genConfig.meanMass = 1.f;
        genConfig.desvMass = 0.f;

        genConfig.meanDamping = 0.99f;
        genConfig.desvDamping = 0.f;

        genConfig.meanColor = { 0.8f, 0.6f, 0.3f, 1.f };
        genConfig.desvColor = { 0.1f, 0.1f, 0.1f, 0.f };

        gen = new ParticleGenerator(200.f, genConfig);
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

    void drawGui() override
    {
        // Titulos
        ImGui::Begin("Practica 3");
        ImGui::Text("Settings for scene 3");
        ImGui::Spacing();

        // Configuracion del generador
        if (ImGui::CollapsingHeader("Generator settings"))
        {
            gen->showUI(true);
        }

        if (ImGui::CollapsingHeader("Gravity settings"))
        {
            ImGui::PushID(0);
            ImGui::Checkbox("Enabled", &enableGravity);
            gravityFG->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Wind settings"))
        {
            ImGui::PushID(1);
            ImGui::Checkbox("Enabled", &enableWind);
            windFG->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Tornado settings"))
        {
            ImGui::PushID(2);
            ImGui::Checkbox("Enabled", &enableTornado);
            tornadoFG->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Explosion settings"))
        {
            ImGui::PushID(3);
            if (ImGui::Button("Start explosion"))
            {
                explosionFG->beginExplosion();
            }
            explosionFG->showUI();
            ImGui::PopID();
        }


        ImGui::End();
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
    ParticleGenerationConfig genConfig;
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

