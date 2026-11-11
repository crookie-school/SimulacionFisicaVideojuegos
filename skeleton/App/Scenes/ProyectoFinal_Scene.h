#pragma once
#include "../../Framework/Scene/Scene.h"
#include "../../Framework/Render/Renderer.h"
#include "../../Framework/Physics.h"
#include "../../ImGui/imgui.h"
#include <vector>
#include <iostream>

#include "../Vehicle/Car.h"

static constexpr int FR_WHEEL = 0;
static constexpr int FL_WHEEL = 1;
static constexpr int BR_WHEEL = 2;
static constexpr int BL_WHEEL = 3;

class ProyectoFinal_Scene :
    public Scene
{
public:
    using Scene::Scene;

    void init() override
    {
        // suelo
        floorMat = Physics::get().createMaterial(0.2, 0.2, 0.2);
        floor = Physics::get().createStaticRigidbody(physx::PxTransform(physx::PxVec3(0.f, -0.501f, 0.f)));
        physx::PxShape* plane = Physics::get().createShape(physx::PxBoxGeometry(50000.f, 0.5f, 50000.f), floorMat);
        plane->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        floor->attachShape(*plane);
        plane->release();

        // Vehiculo
        WheelDesc wheelDesc;
        wheelDesc.wheelMountPoint = { 0.5f, -0.3f, 0.8f };
        wheelDesc.forwardDir = { 0.f, 0.f, -1.f };
        wheelDesc.wheelRadius = 0.3f;
        wheelDesc.suspRestLength = 0.5f;
        wheelDesc.suspStiffness = 20000;
        wheelDesc.suspDamping = 1000;
        wheelDesc.tiresFrictionCoeff = 0.5f;
        wheelDesc.rollResistance = 0.01f;

        CarDesc carDesc;
        carDesc.carSize = { 1.f, 1.f, 2.f };
        carDesc.mass = 1500.f;
        car = new Car({ 0.f, 5.f, 0.f }, carDesc, wheelDesc);

        // Escenario
        physx::PxRigidStatic* elevacion = Physics::get().createStaticRigidbody(physx::PxTransform(physx::PxVec3(0.f, 0.f, -50.f)));
        physx::PxShape* elevacionShape = Physics::get().createShape(physx::PxBoxGeometry(10.f, 0.3f, 10.f), floorMat);
        elevacionShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        elevacion->attachShape(*elevacionShape);
        elevacionShape->release();
        escenario.push_back(elevacion);

        physx::PxRigidStatic* rampa = Physics::get().createStaticRigidbody(physx::PxTransform(physx::PxVec3(7.5f, -2.f, -70.f), physx::PxQuat(0.3f, {1.f, 0.f, 0.f})));
        physx::PxShape* rampaShape = Physics::get().createShape(physx::PxBoxGeometry(10.f, 0.3f, 10.f), floorMat);
        rampaShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        rampa->attachShape(*rampaShape);
        rampaShape->release();
        escenario.push_back(rampa);
    }

    void update(double dt) override
    {
        physx::PxVec3 camPos = car->getPosition() + physx::PxVec3(0.f, 10.f, 20.f);
        camera->setCameraPosition(camPos, car->getPosition() - camPos);

        car->update(dt);
    }

    void drawGui() override
    {
        ImGui::Begin("Proyecto Final");
        ImGui::Text("Settings for final project");

        car->drawGui();

        ImGui::End();
    }

    void render() override
    {
        Renderer::get().renderActor(floor, false, { 1.f, 1.f, 1.f, 1.f });
        
        car->render();

        for (auto a : escenario)
            Renderer::get().renderActor(a, true, { 0.8f, 0.8f, 0.8f, 1.f });
    }

    void keyPress(unsigned char key) override
    {

    }

    void cleanup() override
    {
        delete car;
    }

private:
    Car* car;

    physx::PxMaterial* floorMat;
    physx::PxRigidStatic* floor;
    std::vector<physx::PxRigidActor*> escenario;
};

