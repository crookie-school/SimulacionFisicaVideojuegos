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
	brakeCoeff(desc.brakeCoeff),
	suspensionLength(desc.suspRestLength)
{
}

void Wheel::applyTorque(float torque)
{
	if (!isGrounded) return;

	const physx::PxVec3 finalDir = physx::PxMat33(physx::PxQuat(steer, { 0.f, 1.f, 0.f })) * forward;

	float driveForce = torque / wheelRadius;
	const float maxForce = tiresFrictionCoeff * wheelLoad;
	driveForce = physx::PxClamp(driveForce, -maxForce, maxForce);

	// Pasamos de N/m a N dividiendo entre el radio de la rueda
	applyForceToCarInMountPoint(finalDir * driveForce);
}

void Wheel::applyBrake(float brakeAmount, float dt)
{
	if (!isGrounded) return;

	const physx::PxVec3 mountPointGlobal = getMountPointGlobal();

	// Obtenemos la velocidad del vehiculo (sist. ref. global) y la direcci'on en la que mira la rueda (tambien en sist. ref. global)
	const physx::PxVec3 forwardGlobal = vehicleBody->getGlobalPose().rotate(physx::PxQuat(steer, { 0, 1, 0 }).rotate(forward)); // Cogemos la direccion en coordenadas globales de la rueda
	const physx::PxVec3 vel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, mountPointGlobal); // Cogemos la velocidad actual del cuerpo en la posicion de la rueda
	const float vForward = vel.dot(forwardGlobal);

	// Calculamos una fuerza que intenta detener el coche rapidamente
	const float massOnWheel = wheelLoad / 9.81f;
	const float stopForce = -vForward * massOnWheel / dt;

	// Fuerza maxima que pueden frenar los frenos
	const float brakeLimit = tiresFrictionCoeff * wheelLoad * brakeAmount;

	// Calculamos la fuerza de frenado producida
	const float brakeStrength = physx::PxClamp(stopForce, -brakeLimit, brakeLimit);

	const physx::PxVec3 brakeForce = forwardGlobal * brakeStrength;
	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, brakeForce, mountPointGlobal);
}

void Wheel::update(float dt)
{
	// Posicion de origen de la rueda (global)
	const physx::PxVec3 origin = getMountPointGlobal();

	// Lanzamos un rallo para ver el punto de contacto de la rueda con el suelo
	const float raycastLenth = wheelRadius + restLength;
	physx::PxRaycastBuffer hitData;
	isGrounded = Physics::get().raycast(origin, { 0.f, -1.f, 0.f }, raycastLenth, hitData);

	// Si el rayo no ha contactado con el suelo la rueda esta en el aire no hacemos nada
	if (!isGrounded)
	{
		suspensionLength = restLength;
		return;
	}

	// Calculamos cuanto se comprime la suspension dada la nueva posicion de la rueda
	const float compression = raycastLenth - hitData.block.distance;
	suspensionLength = std::fmin(std::fmax(hitData.block.distance - wheelRadius, 0.f), restLength);

	// Calculamos la velocidad a la que se ha comprimido la suspension
	const float compressionVel = (compression - prevCompression) / dt;
	prevCompression = compression;

	// Aplicamos una fuerza correspondiente a la compresion en el eje Y del vehiculo
	// F = k*x + c*v
	wheelLoad = physx::PxMax(0.f, stiffness * compression + damping * compressionVel);
	applyForceToCarInMountPoint({ 0.f, wheelLoad, 0.f });

	// Calculamos la resistencia de la rueda al moviemiento actual
	const physx::PxVec3 forwardGlobal = vehicleBody->getGlobalPose().rotate(physx::PxQuat(steer, {0, 1, 0}).rotate(forward)); // Cogemos la direccion en coordenadas globales de la rueda
	const physx::PxVec3 sideGlobal = vehicleBody->getGlobalPose().rotate({0.f, 1.f, 0.f}).cross(forwardGlobal); // Cogemos el vector perpendicular de la direccion de la rueda, tambien en global
	const physx::PxVec3 vel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, origin); // Cogemos la velocidad actual del cuerpo en la posicion de la rueda

	// Calculamos la cantidad de velocidad que hay en los ejes de cada rueda
	const float vForward = vel.dot(forwardGlobal);
	const float vSide = vel.dot(sideGlobal);

	// Calculamos la resistencia al rodamiento (resistencia al movimiento de frente)
	const float rollResStrength = -rollResistance * wheelLoad * physx::PxSign(vForward);

	// Calculamos la resistencia al derrape (resistencia al movimiento lateral)
	const float massOnTheWheel = wheelLoad / 9.81f;
	const float sideSlideResStrength = physx::PxClamp(- vSide * massOnTheWheel / dt, -tiresFrictionCoeff * wheelLoad, tiresFrictionCoeff * wheelLoad);

	// Aplicamos la fuerza de resistencia
	const physx::PxVec3 movementResistanceForce = forwardGlobal * rollResStrength + sideGlobal * sideSlideResStrength;
	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, movementResistanceForce, origin);
}

void Wheel::render()
{
	const physx::PxVec3 wheelLocalMountPoint = mountPoint + physx::PxVec3(0, -suspensionLength, 0);
	const physx::PxTransform wheelGlobPos = vehicleBody->getGlobalPose() *
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

float Wheel::getWheelAngularVelocity() const
{
	const physx::PxVec3 linVel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, getMountPointGlobal());

	return linVel.magnitude() / wheelRadius;
}

physx::PxVec3 Wheel::getMountPointGlobal() const
{
	return vehicleBody->getGlobalPose().transform(mountPoint);;
}

void Wheel::applyForceToCarInMountPoint(const physx::PxVec3& localForce)
{
	const physx::PxVec3 forceGlobal = vehicleBody->getGlobalPose().rotate(localForce);

	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, forceGlobal, getMountPointGlobal());
}
