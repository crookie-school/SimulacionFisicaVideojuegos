#pragma once
#include "../../Framework/Scene/Scene.h"
#include <vector>
#include <iostream>
#include "ParticleSystem.h"
#include "ParticleGenerator.h"
#include "ForceGenerator.h"
#include "../../ImGui/imgui.h"
#include <random>

class P2_Scene :
    public Scene
{
public:
    using Scene::Scene;

    struct ParticleGenUIConfig
    {
        char name[32];
        ParticleGenerationConfig config;
    };

    void init() override {
        sys = new ParticleSystem();

        // Configuraciones por defecto
        ParticleGenerationConfig config1;
        config1.shape = GeneratorShape::Cylinder;
        config1.meanPose = Vector3D{ 0.f, 0.f, 0.f };
        config1.desvPose = Vector3D{ 10.f, 10.f, 10.f };

        config1.velType = GeneratorVelType::Independent;
        config1.meanVel = Vector3D{ 0.f, 0.f, 0.f };
        config1.desvVel = Vector3D{ 0.f, 0.f, 0.f };
        config1.meanSpeed = 10.f;
        config1.desvSpeed = 0.f;

        config1.meanAliveTime = 2.f;
        config1.desvAliveTime = 0.f;

        config1.meanMass = 1.f;
        config1.desvMass = 0.f;

        config1.meanDamping = 0.99f;
        config1.desvDamping = 0.f;

        config1.meanColor = { 0.8f, 0.2f, 0.3f, 1.f };
        config1.desvColor = { 0.1f, 0.1f, 0.1f, 0.f };
        configs.push_back({
            "Config 1",
            config1 
            });

        // Parte opcional - Cuando mueren las particulas originales, salen 3 azules con velocidad radial desde la posicion donde murio la anterior
        ParticleGenerationConfig config2;
        config2.shape = GeneratorShape::Sphere;
        config2.desvPose = Vector3D(1.f, 1.f, 1.f);

        config2.velType = GeneratorVelType::Radial;
        config2.meanSpeed = 5.f;
        config2.desvSpeed = 2.f;

        config2.meanAliveTime = 1.f;

        config2.meanColor = { 0.2f, 0.2f, 0.8f, 1.f };
        config2.desvColor = { 0.1f, 0.1f, 0.1f, 0.f };
        configs.push_back({
            "Config 2",
            config2
            });

        gen = new ParticleGenerator(5.f, config1);
        childGen = new ParticleGenerator(0.f, config2);
    }

    void update(double dt) override {
        if(enableGravity) gravityFG.updateForce(*sys, dt);

        sys->update(dt);
        gen->update(*sys, dt);
    }

    void showGenerationConfig(int which)
    {
        ImGui::PushID(which);
        ParticleGenUIConfig& conf = configs[which];

        // Cambiar el nombre
        ImGui::InputText("Name", conf.name, 32);

        conf.config.showUI();

        ImGui::PopID();
    }

    void drawGui() override
    {
        // Titulos
        ImGui::Begin("Practica 2");
        ImGui::Text("Settings for scene 2");
        ImGui::Spacing();

        // --------- UI especificos de la practica 2 ---------
        // Boton para generar particulas
        static int amount = 500;
        ImGui::DragInt("Amount", &amount, 1, 1000);
        ImGui::SameLine();
        if (ImGui::Button("Generate"))
            gen->generate(*sys, amount);
        // Gravedad
        ImGui::Checkbox("Enable gravity", &enableGravity);

        // --------- UI del generador principal ---------
        gen->showUI();

        std::vector<char*> names(configs.size());
        for (int i = 0; i < configs.size(); ++i) names[i] = configs[i].name;
        
        // --------- UI de la configuracion del generador principal ---------
        ImGui::Spacing();
        if(ImGui::CollapsingHeader("Main generation settings"))
        {
            ImGui::Combo("Preset", &selectedConf, names.data(), names.size());

            // botones para crear nuevas configuraciones
            ImGui::SameLine();
            if (ImGui::Button("New Preset"))
            {
                configs.push_back({
                    "New Config",
                    {}
                    });
                selectedConf = configs.size() - 1;
            }

            showGenerationConfig(selectedConf);
        }

        // --------- UI de la configuracion del generador de particulas hijas -----------
        ImGui::Spacing();
        if(ImGui::CollapsingHeader("Children generation settings"))
        {
            static int childAmountOnDeath = 2; // Cuantas generar al morir
            static bool spawnChildren = false;
            if (ImGui::Checkbox("Spawn children", &spawnChildren))
            {
                std::function<void(Particle*)> onDeath;
                if (spawnChildren)
                    onDeath = [this](Particle* p)
                    {
                        childGen->setPosition(p->getPos()); childGen->generate(*sys, childAmountOnDeath);
                    };
                configs[selectedConf].config.onDeathCallback = onDeath;
            }

            // Mostramos la configuracion de las hijas para editar comodamente
            if (spawnChildren)
            {
                ImGui::Combo("Children config", &childrenConf, names.data(), names.size());
                ImGui::DragInt("Children amount", &childAmountOnDeath, 1.f, 1.f, 50.f);
                showGenerationConfig(childrenConf);
            }
        }

        gen->setConfig(configs[selectedConf].config);
        childGen->setConfig(configs[childrenConf].config);

        ImGui::End();
    }

    void render() override
    {
        for (auto it = sys->begin(); it != sys->end(); it++)
            (*it)->render();
    }

    // Por si alguna vez quito lo de ImGui
    /*void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        switch (key)
        {
        case 'g': gen->generate(*sys, 500); std::cout << "Generated 500 particles\n"; break;

        case 'i': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,10.f)); break;
        case 'k': gen->setPosition(gen->getPosition() + Vector3D(0.f,0.f,-10.f)); break;
        case 'j': gen->setPosition(gen->getPosition() + Vector3D(10.f,0.f,0.f)); break;
        case 'l': gen->setPosition(gen->getPosition() + Vector3D(-10.f,0.f,0.f)); break;

        case 'e': gen->setEmitting(!gen->isEmitting()); std::cout << "Is emitting: " << (gen->isEmitting() ? "True" : "False") << '\n'; break;

        case 'c': gen->setShape(GeneratorShape::Point); std::cout << "Emission Shape: Point\n"; break;
        case 'v': gen->setShape(GeneratorShape::Sphere); std::cout << "Emission Shape: Sphere\n"; break;
        case 'b': gen->setShape(GeneratorShape::Box); std::cout << "Emission Shape: Box\n"; break;
        case 'n': gen->setShape(GeneratorShape::Cylinder); std::cout << "Emission Shape: Cylinder\n"; break;

        case '+': gen->setEmissionRate(gen->getEmissionRate() + 100); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;
        case '-': gen->setEmissionRate(gen->getEmissionRate() - 100); std::cout << "Emission Rate: " << gen->getEmissionRate() << '\n'; break;

        case 'r': gen->setVelType(GeneratorVelType::Radial); std::cout << "Velocity type: Radial\n"; break;
        case 't': gen->setVelType(GeneratorVelType::Independent); std::cout << "Velocity type: Independent\n"; break;
        case 'y': gen->setVelocity(Vector3D(0.f, 10.f, 0.f)); std::cout << "Particle Velocity: 0.0, 10.0, 0.0\n"; break;
        case 'u': gen->setVelocity(Vector3D(0.f, 0.f, 0.f)); std::cout << "Particle Velocity: 0.0, 0.0, 0.0\n"; break;

        case 'z': enableGravity = !enableGravity; std::cout << "Gravity: " << (enableGravity ? "Activated" : "Deactivated") << '\n'; break;

        case 'h': std::cout << "Generator modifiable properties:\n"
            "\t- g: Generate 500 particles\n"
            "\t- i/j/k/l: Move Generator position\n"
            "\t- e: toggle emission\n"
            "\t- c/v/b/n: Change generator shape (Point / Sphere / Box / Cylinder)\n"
            "\t- +/-: Increment/Decrement emission rate by 100\n"
            "\t- r/t: Change velocity type (Radial / Independent)\n"
            "\t- y/u: Change independent particle velocity (y is upwards, u is none)\n"
            "\t- z/x: Deactivate/Activate gravity\n"; break;
        default:  break;
        }
    }*/

    void cleanup() override {
        delete sys;
        delete gen;
        delete childGen;
    }

private:
    ParticleSystem* sys;
    ParticleGenerator* gen;
    ParticleGenerator* childGen;

    std::vector<ParticleGenUIConfig> configs;
    int selectedConf = 0;
    int childrenConf = 1;

    bool enableGravity = false;
    GravityForceGenerator gravityFG = GravityForceGenerator({0.f, -10.f, 0.f});
};

