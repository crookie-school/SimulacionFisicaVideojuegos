#pragma once
#include "../../Framework/Scene/Scene.h"
#include "../../Framework/Render/Renderer.h"
#include "../../Framework/Physics.h"
#include "../../Framework/Input.h"
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
        physx::PxShape* plane = Physics::get().createShape(physx::PxBoxGeometry(500.f, 0.5f, 500.f), floorMat);
        plane->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        floor->attachShape(*plane);
        plane->release();

        ModelLibrary::get().addModel("Assets/ambulance.obj");
        ModelLibrary::get().addModel("Assets/wheel-default.obj");

        // Vehiculo
        WheelDesc wheelDesc;
        wheelDesc.wheelMountPoint = { 0.5f, -0.3f, 1.f };
        wheelDesc.forwardDir = { 0.f, 0.f, 1.f };
        wheelDesc.wheelRadius = 0.3f;
        wheelDesc.suspRestLength = 0.3f;
        wheelDesc.suspStiffness = 30000;
        wheelDesc.suspDamping = 1000;
        wheelDesc.tiresFrictionCoeff = 0.7f;
        wheelDesc.rollResistance = 0.05f;
        wheelDesc.brakeCoeff = 0.3;
        wheelDesc.meshName = "Assets/wheel-default.obj";

        EngineDesc engineDesc;
        engineDesc.differentialRatio = 3.42f;
        engineDesc.transmissionEfficieny = 0.7f;
        engineDesc.gears = { -2.9, 2.66, 1.78, 1.3, 1.0, 0.74, 0.5 };
        engineDesc.RPMtoTorqueCurve = { {1000, 275}, {1500, 425}, { 2000, 500 }, {3000, 550}, {6000, 550}, {7000, 450} };
        engineDesc.idleRPM = 1000.f;

        CarDesc carDesc;
        carDesc.carSize = { 1.5f, 0.8f, 3.2f };
        carDesc.mass = 1500.f;
        car = new Car({ 0.f, 5.f, 0.f }, "Assets/ambulance.obj", carDesc, engineDesc, wheelDesc);
        car->changeGear(1);

        // Escenario
        physx::PxRigidStatic* elevacion = Physics::get().createStaticRigidbody(physx::PxTransform(physx::PxVec3(0.f, 0.f, -50.f)));
        physx::PxShape* elevacionShape = Physics::get().createShape(physx::PxBoxGeometry(10.f, 0.1f, 10.f), floorMat);
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
        physx::PxVec3 camPos = car->getPosition() + physx::PxVec3(0.f, 5.f, 10.f);
        camera->setCameraPosition(camPos, car->getPosition() - camPos);

        car->update(dt);

        if (Input::get().isKeyDown('i')) car->setThrottlePos(1.f);
        else car->setThrottlePos(0.f);

        if (Input::get().isKeyDown('k')) car->brake(0.5f, dt);

        if (Input::get().isKeyDown('j')) car->setSteer(physx::PxPi / 7);
        else if (Input::get().isKeyDown('l')) car->setSteer(-physx::PxPi / 7);
        else car->setSteer(0);

        static int gear = 1;
        if (Input::get().wasKeyJustPressed('u'))
        {
            gear = std::max(--gear, 0);
            car->changeGear(gear);
        }
        else if(Input::get().wasKeyJustPressed('o'))
        {
            gear = std::min(++gear, 6);
            car->changeGear(gear);
        }

        if (Input::get().isKeyDown(' ')) car->pullHandbrake(1.f, dt);
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
            Renderer::get().renderActor(a, false, { 0.8f, 0.8f, 0.8f, 1.f });
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

