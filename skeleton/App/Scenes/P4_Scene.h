#pragma once
#include "../../Framework/Scene/Scene.h"
#include <vector>
#include <iostream>

class P4_Scene :
    public Scene
{
public:
    using Scene::Scene;

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

        p4 = new Particle({ 0.f, -5.f, 0.f }, 500.f, 999999, {0.8f, 0.6f, 0.2f, 1.f}, physx::PxBoxGeometry(1.f, 2.f, 1.f));
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

    void drawGui() override
    {
        // Titulos
        ImGui::Begin("Practica 4");
        ImGui::Text("Settings for scene 4");
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Gravity settings"))
        {
            ImGui::PushID(0);
            g->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Spring 1 settings"))
        {
            ImGui::PushID(1);
            f1->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Spring 2 settings"))
        {
            ImGui::PushID(2);
            f2->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Anchored Spring settings"))
        {
            ImGui::PushID(2);
            if(ImGui::Button("Apply force")) p3->addForce({ 500.f, 0.f, 0.f });
            f3->showUI();
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Bouyancy settings"))
        {
            ImGui::PushID(3);
            f4->showUI();
            ImGui::PopID();
        }


        ImGui::End();
    }

    void render() override
    {
        p1->render();
        p2->render();
        p3->render();
        p4->render();
        f3->renderAnchor();
        f4->renderWater();
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

