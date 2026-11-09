#pragma once
#include "../../Framework/Scene/Scene.h"
#include "../../Framework/Render/Renderer.h"
#include "../../Framework/Physics.h"
#include "../../ImGui/imgui.h"
#include <vector>
#include <iostream>

#include "../Vehicle/Wheel.h"

class ProyectoFinal_Scene :
    public Scene
{
public:
    using Scene::Scene;

    void init() override
    {
        camera->setCameraPosition({ -50.f, 5.f, 0.f }, { 1.f, 0.f, 0.f });

        floorMat = Physics::get().createMaterial(0.2, 0.2, 0.2);
        floor = Physics::get().createStaticRigidbody(physx::PxTransform(physx::PxVec3(0.f, 1.f, 0.f)));
        physx::PxShape* plane = Physics::get().createShape(physx::PxBoxGeometry(500.f, 0.5f, 500.f), floorMat);
        plane->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        floor->attachShape(*plane);
        plane->release();
        actors.push_back(floor);

        physx::PxMaterial* carMat = Physics::get().createMaterial(0.5, 0.5, 0.5);
        carBody = Physics::get().createDynamicRigidbody(physx::PxTransform(physx::PxVec3(0.f, 5.f, 0.f)));
        physx::PxShape* cube = Physics::get().createShape(physx::PxBoxGeometry(0.5, 0.5, 1), carMat);
        cube->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
        carBody->attachShape(*cube);
        cube->release();

        physx::PxRigidBodyExt::setMassAndUpdateInertia(*carBody, 800.f);

        actors.push_back(carBody);
        
        Wheel frWheel(carBody, { 0.5f, -0.5f, 0.8f }, 0.3, 0.5, 10000.f, 500.f);
        wheels.push_back(frWheel);
        Wheel flWheel = frWheel; flWheel.setMountPoint({ -0.5f, -0.5f, 0.8f });
        wheels.push_back(flWheel);
        Wheel brWheel = frWheel; brWheel.setMountPoint({ -0.5f, -0.5f, -0.8f });
        wheels.push_back(brWheel);
        Wheel blWheel = frWheel; blWheel.setMountPoint({ 0.5f, -0.5f, -0.8f });
        wheels.push_back(blWheel);
    }

    void update(double dt) override
    {
        for (Wheel& w : wheels)
            w.update(dt);
    }

    void drawGui() override
    {
        ImGui::Begin("Proyecto Final");
        ImGui::Text("Settings for final project");

        if(ImGui::Button("Reset car pos"))
            carBody->setGlobalPose(physx::PxTransform(physx::PxVec3(0.f, 5.f, 0.f)));
        
        ImGui::End();
    }

    void render() override
    {
        Renderer::get().renderActor(floor, true, { 1.f, 1.f, 1.f, 1.f });
        Renderer::get().renderActor(carBody, true, { 1.f, 0.4f, 0.3f, 1.f });

        for (Wheel& w : wheels)
            w.render();
    }

    void cleanup() override
    {
    }

private:
    std::vector<Wheel> wheels;
    physx::PxMaterial* floorMat;
    physx::PxRigidStatic* floor;

    physx::PxRigidDynamic* carBody;

    std::vector<physx::PxRigidActor*> actors;
};

