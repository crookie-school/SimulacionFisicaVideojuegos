#pragma once
#include "Particle.h"

constexpr float GRAVITY = -9.81f;

class Projectile :
    public Particle
{
public:
    Projectile(const Vector3D& Pos, const Vector3D& dir, float realVel, float simVel, float mass)
        : Particle(Pos, mass, 99999.f), simMass(mass)
    {
        updateSimulatedMassAndGravity(realVel, simVel);

        vel = dir * simVel;
    }

    void updateSimulatedMassAndGravity(float realVel, float simVel);

    virtual void integrateEulerSemi(double t) override;
private:
    float simMass;
    float simGravity;
};

