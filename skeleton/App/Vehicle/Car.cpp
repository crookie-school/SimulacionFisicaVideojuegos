#include "Car.h"
#include "../../Framework/ResourceLoader/ModelLibrary.h"

Car::Car(const physx::PxVec3& spawnPosition, const std::string& meshPath, const CarDesc& carDesc, const EngineDesc& engineDesc, const WheelDesc& wheelsDesc) :
    carBody(Physics::get().createDynamicRigidbody(physx::PxTransform(spawnPosition))),
    engine(engineDesc),
    frW(carBody, wheelsDesc),
    flW(carBody, wheelsDesc),
    brW(carBody, wheelsDesc),
    blW(carBody, wheelsDesc),
    carFrontArea(carDesc.carSize.x* carDesc.carSize.z),
    carMeshName(meshPath)
{
    frW.setMountPoint({ -wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, wheelsDesc.wheelMountPoint.z }); 
    flW.setMountPoint({ wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, wheelsDesc.wheelMountPoint.z }); frW.setFlipMesh(true);
    brW.setMountPoint({ -wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, -wheelsDesc.wheelMountPoint.z });
    blW.setMountPoint({ wheelsDesc.wheelMountPoint.x, wheelsDesc.wheelMountPoint.y, -wheelsDesc.wheelMountPoint.z }); brW.setFlipMesh(true);

    engine.attachWheel(&brW);
    engine.attachWheel(&blW);

    physx::PxMaterial* carMat = Physics::get().createMaterial(0.5, 0.5, 0.5);
    physx::PxShape* cube = Physics::get().createShape(physx::PxBoxGeometry(carDesc.carSize / 2.f), carMat);
    cube->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
    carBody->attachShape(*cube);
    cube->release();

    physx::PxRigidBodyExt::setMassAndUpdateInertia(*carBody, carDesc.mass);
}

void Car::update(float dt)
{
    // Calculamos la resistencia con el aire
    const physx::PxVec3 carVel = carBody->getLinearVelocity();
    const float airRestistance = 0.5 * 0.3f * carFrontArea * 1.29f;
    carBody->addForce(-airRestistance * carVel * carVel.magnitude());

    engine.update(dt);

    frW.update(dt);
    flW.update(dt);
    brW.update(dt);
    blW.update(dt);
}

void Car::render()
{
    if(drawBodyCollider) Renderer::get().renderActor(carBody, true, {color.x, color.y, color.z, 0.1f});
    frW.render();
    flW.render();
    brW.render();
    blW.render();
    
    Renderer::get().renderMeshModel(ModelLibrary::get().getModel(carMeshName), "body", carBody->getGlobalPose() * physx::PxTransform(0, -0.7f, 0), {1.f, 1.f, 1.f});
}

void Car::drawGui()
{
    ImGui::BeginGroup();
    ImGui::PushID(this);

    ImGui::ColorEdit3("Car body color", &color.x);
    ImGui::Checkbox("Draw body collider", &drawBodyCollider);

    if (ImGui::Button("Reset car pos"))
    {
        carBody->setGlobalPose(physx::PxTransform(physx::PxVec3(0.f, 5.f, 0.f)));
        carBody->setLinearVelocity({ 0.f, 0.f, 0.f });
        carBody->setAngularVelocity({ 0.f, 0.f, 0.f });
    }

    float mass = carBody->getMass();
    if (ImGui::SliderFloat("Mass", &mass, 200.f, 10000))
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*carBody, mass);

    float throttle = engine.getThrottlePosition();
    if (ImGui::VSliderFloat("##throttle", { 80, 150 }, &throttle, 0, 1, "%.2f\nThrottle"))
        engine.setThrottlePosition(throttle);
    ImGui::SameLine();

    static float brakes;
    if (ImGui::VSliderFloat("##brake", { 80, 150 }, &brakes, 0, 1, "%.2f\nBrake"))
    {
        frW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        flW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        brW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
        blW.applyBrake(brakes, ImGui::GetIO().DeltaTime);
    }
    ImGui::SameLine();

    int gear = engine.getCurrentGear();
    if (ImGui::VSliderInt("##gears", { 80, 150 }, &gear, 0, 6, "%d\nGear"))
        engine.changeGear(gear);

    float steering = frW.getSteer();
    if (ImGui::SliderFloat("Steering", &steering, -physx::PxPi / 4, physx::PxPi / 4))
    {
        frW.setSteer(steering);
        flW.setSteer(steering);
    }
        
    ImGui::Begin("Car info");
    float linVel = carBody->getLinearVelocity().magnitude() * 3.6f;
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
    sprintf(overlay, "linear velocity %f (km/s)", linVel);
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
    
    ImGui::End();

    ImGui::PopID();
    ImGui::EndGroup();
}
