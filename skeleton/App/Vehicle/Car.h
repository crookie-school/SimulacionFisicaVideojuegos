#pragma once

#include "Wheel.h"
#include "Engine.h"
#include "../../Framework/Physics.h"
#include "../../Framework/Render/Renderer.h"
#include "../../ImGui/imgui.h"
#include <vector>

struct CarDesc
{
    physx::PxVec3 carSize;
    float mass;
};

class Car
{
public:
	Car(const physx::PxVec3& spawnPosition, const CarDesc& carDesc, const EngineDesc& engineDesc, const WheelDesc& wheelsDesc) :
        carBody(Physics::get().createDynamicRigidbody(physx::PxTransform(spawnPosition))),
        engine(engineDesc),
        frW(carBody, wheelsDesc),
        flW(carBody, wheelsDesc),
        brW(carBody, wheelsDesc),
        blW(carBody, wheelsDesc),
        carFrontArea(carDesc.carSize.x * carDesc.carSize.z)
    {
        frW.setMountPoint({  wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, -wheelsDesc.wheelMountPoint.z });
        flW.setMountPoint({ -wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, -wheelsDesc.wheelMountPoint.z });
        brW.setMountPoint({  wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y,  wheelsDesc.wheelMountPoint.z });
        blW.setMountPoint({ -wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y,  wheelsDesc.wheelMountPoint.z });

        engine.attachWheel(&frW);
        engine.attachWheel(&flW);
        engine.attachWheel(&brW);
        engine.attachWheel(&blW);
        
        physx::PxMaterial* carMat = Physics::get().createMaterial(0.5, 0.5, 0.5);
        physx::PxShape* cube = Physics::get().createShape(physx::PxBoxGeometry(carDesc.carSize/2.f), carMat);
        cube->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
        carBody->attachShape(*cube);
        cube->release();

        physx::PxRigidBodyExt::setMassAndUpdateInertia(*carBody, carDesc.mass);
	}

    void update(float dt)
    {
        // Calculamos la resistencia con el aire
        const physx::PxVec3 carVel = carBody->getLinearVelocity();
        const float airRestistance = 0.5 * 0.3f * carFrontArea * 1.29f;
        carBody->addForce(-airRestistance * carVel * carVel.magnitude());

        frW.update(dt);
        flW.update(dt);
        brW.update(dt);
        blW.update(dt);

        engine.update(dt);
    }

    void render()
    {
        Renderer::get().renderActor(carBody, true, color);
        frW.render();
        flW.render();
        brW.render();
        blW.render();
    }

    void drawGui()
    {
        ImGui::BeginGroup();
        ImGui::PushID(this);

        ImGui::ColorEdit4("Car body color", &color.x);

        if (ImGui::Button("Reset car pos"))
        {
            carBody->setGlobalPose(physx::PxTransform(physx::PxVec3(0.f, 5.f, 0.f)));
            carBody->setLinearVelocity({ 0.f, 0.f, 0.f });
            carBody->setAngularVelocity({ 0.f, 0.f, 0.f });
        }

        float mass = carBody->getMass();
        ImGui::SliderFloat("Mass", &mass, 200.f, 10000);
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*carBody, mass);

        static float throttle;
        ImGui::SliderFloat("throttle", &throttle, 0, 1);
        engine.setThrottlePosition(throttle);

        static int gear;
        ImGui::DragInt("Gear", &gear, 1, 0, 6);
        engine.changeGear(gear);

        static float steering;
        ImGui::SliderFloat("Steering", &steering, -physx::PxPi / 4, physx::PxPi / 4);
        frW.setSteer(steering);
        flW.setSteer(steering);

        static float brakes;
        ImGui::SliderFloat("Brakes", &brakes, 0, 1);
        frW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        flW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        brW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        blW.applyBrake(brakes, ImGui::GetIO().DeltaTime);

        if(ImGui::CollapsingHeader("Car Data"))
        {
            float linVel = carBody->getLinearVelocity().magnitude();
            float angVel = carBody->getAngularVelocity().magnitude();
            static std::vector<float> linVelocitySamples;
            static std::vector<float> angVelocitySamples;
            static double refresh_time = 0.0;
            while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
            {
                linVelocitySamples.push_back(linVel);
                if (linVelocitySamples.size() > 50) linVelocitySamples.erase(linVelocitySamples.begin());
                angVelocitySamples.push_back(angVel);
                if (angVelocitySamples.size() > 50) angVelocitySamples.erase(angVelocitySamples.begin());
                refresh_time += 0.5f;
            }
            
            char overlay[64];
            sprintf(overlay, "linear velocity %f (m/s)", linVel);
            ImGui::PlotLines("##0",
                linVelocitySamples.data(),
                linVelocitySamples.size(),
                0, overlay, 0.f, 200.f, { 500.f, 100.f });

            sprintf(overlay, "angular velocity %f (rad/s)", angVel);
            ImGui::PlotLines("##1",
                angVelocitySamples.data(),
                angVelocitySamples.size(),
                0, overlay, 0.f, 50.f, { 500.f, 100.f });


            ImGui::SeparatorText("Wheel data");
            flW.drawGui();
            ImGui::SameLine();
            frW.drawGui();
            blW.drawGui();
            ImGui::SameLine();
            brW.drawGui();

            ImGui::SeparatorText("Engine data");
            engine.drawGui();
        }

        ImGui::PopID();
        ImGui::EndGroup();
    }

    physx::PxVec3 getPosition() { return carBody->getGlobalPose().p; }

private:
	physx::PxRigidDynamic* carBody;
	Wheel frW, flW, brW, blW;
    Engine engine;
    physx::PxVec4 color = { 0.8f, 0.3f, 0.2f, 1.f };
    float carFrontArea;
};

