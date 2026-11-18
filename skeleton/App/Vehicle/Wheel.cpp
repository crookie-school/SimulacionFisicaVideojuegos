#include "Wheel.h"
#include "../../ImGui/imgui.h"
#include <vector>

Wheel::Wheel(PxRigidDynamic* vehicleBody, const WheelDesc& desc) :
	vehicleBody(vehicleBody),
	mountPoint(desc.wheelMountPoint),
	forward(desc.forwardDir.getNormalized()),
	rollResistance(desc.rollResistance),
	tiresFrictionCoeff(desc.tiresFrictionCoeff),
	restLength(desc.suspRestLength),
	stiffness(desc.suspStiffness),
	damping(desc.suspDamping),
	brakeCoeff(desc.brakeCoeff),
	suspensionLength(desc.suspRestLength),
	tireMeshName(desc.meshName),
	flipMesh(desc.flipMesh)
{
	setWheelRadius(desc.wheelRadius);
}

// Deberia hacer que el torque se aplique en el update para controlar el consume de agarre
void Wheel::applyTorque(float torque)
{
	if (!isGrounded) return;

	const physx::PxVec3 finalDir = physx::PxMat33(physx::PxQuat(steer, { 0.f, 1.f, 0.f })) * forward;

	float driveStrength = torque / wheelRadius;
	const float maxStrength = tiresFrictionCoeff * wheelLoad;
	driveStrength = physx::PxClamp(driveStrength, -maxStrength, maxStrength);
	longitudinalForce += driveStrength;
}

// Deberia hacer que la frenada se aplique en el update para controlar el consume de agarre
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
	float brakeStrength = physx::PxClamp(stopForce, -brakeLimit, brakeLimit);
	longitudinalForce += brakeStrength;
}

void Wheel::update(float dt)
{
	// Reset wheel grip budget
	wheelGripBudget = 0;

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

	// Calculamos las fuerzas longitudinales y transversales a la rueda
	const physx::PxVec3 forwardGlobal = vehicleBody->getGlobalPose().rotate(physx::PxQuat(steer, {0, 1, 0}).rotate(forward)); // Cogemos la direccion en coordenadas globales de la rueda
	const physx::PxVec3 sideGlobal = vehicleBody->getGlobalPose().rotate({0.f, 1.f, 0.f}).cross(forwardGlobal); // Cogemos el vector perpendicular de la direccion de la rueda, tambien en global
	const physx::PxVec3 vel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, origin); // Cogemos la velocidad actual del cuerpo en la posicion de la rueda

	// Calculamos la cantidad de velocidad que hay en los ejes de cada rueda
	const float vForward = vel.dot(forwardGlobal);
	const float vSide = vel.dot(sideGlobal);

	// Calculamos la resistencia al rodamiento (resistencia al movimiento de frente)
	float rollResStrength = -rollResistance * wheelLoad * physx::PxSign(vForward);
	longitudinalForce += rollResStrength;

	// Calculamos la fuerza total longitudinal
	longitudinalForce = consumeGrip(longitudinalForce);

	// Calculamos la resistencia al derrape (resistencia al movimiento lateral)
	const float massOnTheWheel = wheelLoad / 9.81f;
	float sideSlideResStrength = physx::PxClamp(- vSide * massOnTheWheel / dt, -tiresFrictionCoeff * wheelLoad, tiresFrictionCoeff * wheelLoad);
	sideSlideResStrength = consumeGrip(sideSlideResStrength);

	// Aplicamos la fuerza de resistencia
	const physx::PxVec3 forwAxisForce = forwardGlobal * longitudinalForce;
	const physx::PxVec3 sideAxisForce = sideGlobal * sideSlideResStrength;
	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, forwAxisForce + sideAxisForce, origin);

	longitudinalForce = 0;


	// Integramos la rotacion de la rueda (para el render)
	totalRotation += (vForward / wheelRadius) * dt;
	totalRotation = std::fmod(totalRotation, physx::PxTwoPi);
}

void Wheel::render()
{
	const physx::PxVec3 wheelLocalMountPoint = mountPoint + physx::PxVec3(0, -suspensionLength, 0);
	const physx::PxTransform wheelGlobPos = vehicleBody->getGlobalPose() *
		physx::PxTransform(wheelLocalMountPoint) *
		physx::PxTransform(physx::PxQuat(steer, { 0.f, 1.f, 0.f })) * // Rotamos visualmente la rueda segun el giro
		physx::PxTransform(physx::PxQuat(totalRotation, { 1.f, 0.f, 0.f })); // Rotamos la rueda segun la distancia que se mueve

	Model& wheelMesh = ModelLibrary::get().getModel(tireMeshName);
	physx::PxVec3 scale;
	if (flipMesh) scale = wheelScale.multiply({ -1.f, 1.f, 1.f });
	else scale = wheelScale;

	Renderer::get().renderMesh(wheelMesh, wheelGlobPos, scale);
}

void Wheel::drawGui()
{
	ImGui::PushID(this);
	suspensionLengthSampels.push_back(prevCompression);
	if (suspensionLengthSampels.size() > 50) suspensionLengthSampels.erase(suspensionLengthSampels.begin());
	char overlay[32];
	sprintf(overlay, "compression %f", prevCompression);
	ImGui::PlotLines("##CompressionPlot", 
		suspensionLengthSampels.data(), 
		suspensionLengthSampels.size(), 
		0, overlay, -0.5f, 0.5f, {100.f, 100.f});

	ImGui::SameLine();
	if (ImGui::VSliderFloat("##Radius", { 25.f, 100.f}, &wheelRadius, 0.01f, 2.f, "%.2f m"))
	{
		setWheelRadius(wheelRadius); // to update the wheel scale 
	}

	ImGui::SameLine();
	ImGui::VSliderFloat("##TireFrictionCoeff", { 25.f, 100.f }, &tiresFrictionCoeff, 0.0f, 1.f, "%.2f tire grip");

	ImGui::SameLine();
	ImGui::VSliderFloat("##SuspRestLength", { 25.f, 100.f }, &restLength, 0.0f, 1.f, "%.2f m susp rest");

	ImGui::SameLine();
	ImGui::VSliderFloat("##SuspStiffness", { 25.f, 100.f }, &stiffness, 0.0f, 100000.f, "%.2f N/m susp stiff");

	ImGui::SameLine();
	ImGui::VSliderFloat("##SuspDamping", { 25.f, 100.f }, &damping, 0.0f, 10000.f, "%.2f N·s/m susp damp");

	ImGui::SameLine();
	sprintf(overlay, "%.2f/%.2f", wheelGripBudget, wheelLoad * tiresFrictionCoeff);
	const float maxForce = wheelLoad * tiresFrictionCoeff;
	ImGui::ProgressBar(maxForce > 0.f ? wheelGripBudget / maxForce : 0.f, { 100.f, 100.f }, overlay);
	ImGui::PopID();
}

float Wheel::getWheelAngularVelocity() const
{
	const physx::PxVec3 linVel = physx::PxRigidBodyExt::getVelocityAtPos(*vehicleBody, getMountPointGlobal());

	return linVel.magnitude() / wheelRadius;
}

void Wheel::setWheelRadius(float radius)
{
	wheelRadius = radius;

	Model& wheelMesh = ModelLibrary::get().getModel(tireMeshName);
	const physx::PxVec3 bounds = wheelMesh.getBoundingBox();
	physx::PxVec3 newScale(1, wheelRadius / bounds.y, wheelRadius / bounds.z);
	wheelScale = newScale;
}

physx::PxVec3 Wheel::getMountPointGlobal() const
{
	return vehicleBody->getGlobalPose().transform(mountPoint);
}

void Wheel::applyForceToCarInMountPoint(const physx::PxVec3& localForce)
{
	const physx::PxVec3 forceGlobal = vehicleBody->getGlobalPose().rotate(localForce);

	physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, forceGlobal, getMountPointGlobal());
}

float Wheel::consumeGrip(float forceDesired)
{
	const float gripRemaining = fmax(0.f, tiresFrictionCoeff * wheelLoad - wheelGripBudget);
	const float finalForce = physx::PxClamp(forceDesired, -gripRemaining, gripRemaining);
	wheelGripBudget += fabs(finalForce);
	return finalForce;
}
