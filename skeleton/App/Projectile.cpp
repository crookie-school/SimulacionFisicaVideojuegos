#include "Projectile.h"
#include <iostream>

void Projectile::updateSimulatedMassAndGravity(float rV, float sV) {
    simMass = (rV * rV) / (inverseMass * sV * sV);
    simGravity = GRAVITY * ((sV * sV) / (rV * rV));


    std::cout << "Real Vel: " << rV << " | ";
    std::cout << "Sim Vel: " << sV << '\n';
    std::cout << "Real Mass: " << 1.f/inverseMass << "  |  ";
    std::cout << "Sim Mass :" << simMass << '\n';
}

void Projectile::integrateEulerSemi(double t)
{
	// Apply gravity
	acc = acc + Vector3D(0.f, simGravity, 0.f);

    Particle::integrateEulerSemi(t);
    acc = Vector3D( 0, 0, 0 );
}
