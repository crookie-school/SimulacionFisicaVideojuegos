#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>
#include <iostream>
#include "Projectile.h"
#include "ImGui/imgui.h"

class P1_Scene :
    public Scene
{
public:
    explicit P1_Scene(std::string name) : Scene(std::move(name))
    {}

    void init() override {
        // Creamos configuraciones default
        configs.push_back({
            "Cannon",
            250.f,
            50.f,
            15.f,
            {0.f, 0.f, 0.f, 1.f}
            });
        configs.push_back({
            "Tank",
            1800.f,
            50.f,
            40.f,
            {1.f, 0.f, 0.f, 1.f}
            });
        configs.push_back({
            "Gun",
            330.f,
            50.f,
            .1f,
            {0.f, 1.f, 0.f, 1.f}
            });
    }

    void update(double dt) override {
        for (Projectile* p : particles) {
            p->integrateEulerSemi(dt);
        }
    }

    struct ProjectileConfig
    {
        char name[32]; // (para que ImGui permita modificarlo con InputText
        float realVel;
        float simVel;
        float mass;
        physx::PxVec4 color;
    };

    void drawGui() override
    {
        // Titulos
        ImGui::Begin("Practica 1");
        ImGui::Text("Settings for scene 1");

        // Elegir proyectil
        std::vector<char*> names(configs.size());
        for (int i = 0; i < configs.size(); ++i) names[i] = configs[i].name;
        ImGui::Combo("Projectile", &selected, names.data(), names.size());

        // Ajustar propiedades
        ProjectileConfig& c = configs[selected];
        ImGui::InputText("Name", c.name, IM_ARRAYSIZE(c.name));
        ImGui::DragFloat("Real velocity (m/s)", &c.realVel, 1.f, 0.f, 5000.f);
        ImGui::DragFloat("Sim velocity (m/s)", &c.simVel, 1.f, 0.f, 5000.f);
        ImGui::DragFloat("Mass (kg)", &c.mass, 0.1f, 0.01f, 1000.f);
        ImGui::ColorEdit4("Color", &c.color.x);

        // Permitir crear o borrar configuraciones
        ImGui::SeparatorText("Create/Remove projectile");
        if (ImGui::Button("Create new projectile"))
        {
            configs.push_back({
                "New projectile",
                0.f,
                0.f,
                1.f,
                {0.f, 0.f, 0.f, 1.f}
                });
            selected = configs.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Projectile"))
        {
            configs.erase(configs.begin() + selected);
        }

        ImGui::Separator();
        if (ImGui::Button("Shoot!"))
        {
            shoot(c);
        }
        ImGui::End();
    }

    void shoot(const ProjectileConfig& conf)
    {
        Vector3D shootDir = GetCamera()->getDir().getNormalized();
        physx::PxTransform camera = GetCamera()->getTransform();
        particles.push_back(new Projectile(camera.p, shootDir, conf.realVel, conf.simVel, conf.mass, conf.color));
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override
    {
        if (key == ' ') shoot(configs[selected]);
    }

    void cleanup() override {
        for(Projectile* p : particles) {
            delete p;
        }
    }

private:
    std::vector<Projectile*> particles;
    std::vector<ProjectileConfig> configs;

    int selected = 0;
};

