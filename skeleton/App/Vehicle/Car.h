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
	Car(const physx::PxVec3& spawnPosition, const CarDesc& carDesc, const EngineDesc& engineDesc, const WheelDesc& wheelsDesc);

    void update(float dt);

    void render();

    void drawGui();

    void changeGear(int gear) { engine.changeGear(gear); }
    void setSteer(float rad) { frW.setSteer(rad); flW.setSteer(rad); }
    void setThrottlePos(float amount) { engine.setThrottlePosition(amount); }
    void brake(float amount, float dt) { frW.applyBrake(amount, dt); flW.applyBrake(amount, dt); brW.applyBrake(amount, dt); blW.applyBrake(amount, dt); }
    physx::PxVec3 getPosition() { return carBody->getGlobalPose().p; }

private:
	physx::PxRigidDynamic* carBody;
	Wheel frW, flW, brW, blW;
    Engine engine;
    physx::PxVec4 color = { 0.8f, 0.3f, 0.2f, 1.f };
    float carFrontArea;
};

