#pragma once
#include "../../Framework/Scene/Scene.h"
#include <vector>
#include <iostream>
#include "../Particle.h"
#include "../ForceGenerator.h"
#include "../ParticleGenerator.h"
#include "../../Framework/Render/Renderer.h"
#include <random>

class P3_Scene :
    public Scene
{
public:
    using Scene::Scene;

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


        // Creamos los generadores
        gravityFG = new GravityForceGenerator({0.f, -10.f, 0.f});
        windFG = new WindForceGenerator(0.4f, { 100.f, 0.f, 0.f }, AOEorigin.p - AOEsize / 2.f, AOEsize);
        tornadoFG = new TornadoForceGenerator(10.f, 2.f, 0.4f, AOEorigin.p - AOEsize / 2.f, AOEsize);
        explosionFG = new ExplosionForceGenerator(AOEorigin.p, 100000.f, AOEsize.magnitudeSqr(), 0.05f);
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

    void render() override
    {
        for (auto it = sys->begin(); it != sys->end(); it++)
            (*it)->render();

        // Draw AOE Box
        Renderer::get().renderShape(physx::PxBoxGeometry(AOEsize / 2.f), physx::PxTransform(AOEorigin), {0.f, 0.f, 0.f, 0.1f}); // alpha < 1.f se pone con wireframe (es asi como estaba el codigo de render antes)
    }

    void cleanup() override
    {
        delete sys;
        delete gen;

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
    physx::PxTransform AOEorigin = physx::PxTransform({ 0.f, 0.f, 0.f });
    Vector3D AOEsize = { 80, 80, 80 };

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

