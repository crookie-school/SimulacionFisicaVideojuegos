#include "Wheel.h"
#include "../../ImGui/imgui.h"
#include <vector>

Wheel::Wheel(PxRigidDynamic* vehicleBody, const WheelDesc& desc) :
	vehicleBody(vehicleBody),
	mountPoint(desc.wheelMountPoint),
	forward(desc.forwardDir.getNormalized()),
	wheelRadius(desc.wheelRadius),
	rollResistance(desc.rollResistance),
	tiresFrictionCoeff(desc.tiresFrictionCoeff),
	restLength(desc.suspRestLength),
	stiffness(desc.suspStiffness),
	damping(desc.suspDamping),
	suspensionLength(desc.suspRestLength)
{
}

void Wheel::applyTorque(float torque)
{
	if (!wasGrounded) return;

	physx::PxVec3 finalDir = physx::PxMat33(physx::PxQuat(steer, { 0.f, 1.f, 0.f })) * forward;
	// Pasamos de N/m a N dividiendo entre el radio de la rueda
	applyForceToCarInMountPoint(finalDir * (torque / wheelRadius));
}

void Wheel::update(float dt)
{
	// Posicion de origen de la rueda (global)
	physx::PxVec3 origin = vehicleBody->getGlobalPose().transform(mountPoint);

	// Lanzamos un rallo para ver el punto de contacto de la rueda con el suelo
	float raycastLenth = wheelRadius + restLength;
	physx::PxRaycastBuffer hitData;
	bool grounded = Physics::get().raycast(origin, { 0.f, -1.f, 0.f }, raycastLenth, hitData);

	// Si el rayo no ha contactado con el suelo la rueda esta en el aire no hacemos nada
	if (!grounded)
	{
		wasGrounded = false;
		suspensionLength = restLength;
		return;
	}

	// Calculamos cuanto se comprime la suspension dada la nueva posicion de la rueda
	float compression = raycastLenth - hitData.block.distance;
	suspensionLength = std::fmin(std::fmax(hitData.block.distance - wheelRadius, 0.f), restLength);

	// Calculamos la velocidad a la que se ha comprimido la suspension (Verlet)
	if (!wasGrounded) prevCompression = compression;
	float compressionVel = (compression - prevCompression) / dt;
	prevCompression = compression;
	wasGrounded = true;

	// Aplicamos una fuerza correspondiente a la compresion en el eje Y del vehiculo
	// F = k*x + c*v
	float suspensionForce = std::fmax(0, stiffness * compression + damping * compressionVel);
	applyForceToCarInMountPoint({ 0.f, suspensionForce, 0.f });

	// Calculamos la resistencia de la rueda al moviemiento actual
	physx::PxVec3 forwardGlobal = vehicleBody->getGlobalPose().rotate(physx::PxQuat(steer, {0, 1, 0}).rotate(forward)); // Cogemos la direccion en coordenadas globales de la rueda
	physx::PxVec3 sideGlobal = vehicleBody->getGlobalPose().rotate({0.f, 1.f, 0.f}).cross(forwardGlobal); // Cogemos el vector perpendicular de la direccion de la rueda, tambien en global
	physx::PxVec3 vel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, origin); // Cogemos la velocidad actual del cuerpo en la posicion de la rueda

	// Calculamos la cantidad de velocidad que hay en los ejes de cada rueda
	float vForward = vel.dot(forwardGlobal);
	float vSide = vel.dot(sideGlobal);

	// Calculamos la resistencia al rodamiento (resistencia al movimiento de frente)
	float rollResStrength = -rollResistance * suspensionForce * physx::PxSign(vForward);

	// Calculamos la resistencia al derrape (resistencia al movimiento lateral)
	float massOnTheWheel = suspensionForce / 9.81f;
	float sideSlideResStrength = physx::PxClamp(- vSide * massOnTheWheel / dt, -tiresFrictionCoeff * suspensionForce, tiresFrictionCoeff * suspensionForce);

	// Aplicamos la fuerza de resistencia
	physx::PxVec3 movementResistanceForce = forwardGlobal * rollResStrength + sideGlobal * sideSlideResStrength;
	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, movementResistanceForce, origin);
}

void Wheel::render()
{
	physx::PxVec3 wheelLocalMountPoint = mountPoint + physx::PxVec3(0, -suspensionLength, 0);
	physx::PxTransform wheelGlobPos = vehicleBody->getGlobalPose() *
		physx::PxTransform(wheelLocalMountPoint) *
		physx::PxTransform(physx::PxQuat(steer, { 0.f, 1.f, 0.f })); // Rotamos visualmente la rueda segun el giro
	Renderer::get().renderShape(physx::PxBoxGeometry(wheelRadius / 2.f, wheelRadius, wheelRadius), wheelGlobPos, { 0.f, 0.f, 0.f, 1.f });
}

void Wheel::drawGui()
{
	ImGui::PushID(this);
	suspensionLengthSampels.push_back(prevCompression);
	if (suspensionLengthSampels.size() > 50) suspensionLengthSampels.erase(suspensionLengthSampels.begin());
	char overlay[32];
	sprintf(overlay, "compression %f", prevCompression);
	ImGui::PlotLines("##", 
		suspensionLengthSampels.data(), 
		suspensionLengthSampels.size(), 
		0, overlay, -0.5f, 0.5f, {100.f, 100.f});
	ImGui::PopID();
}

void Wheel::applyForceToCarInMountPoint(const physx::PxVec3& localForce)
{
	physx::PxVec3 mountPointGlobal = vehicleBody->getGlobalPose().transform(mountPoint);
	physx::PxVec3 forceGlobal = vehicleBody->getGlobalPose().rotate(localForce);

	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, forceGlobal, mountPointGlobal);
}
