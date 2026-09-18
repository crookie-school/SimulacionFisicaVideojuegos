#pragma once
#include "Scene.h"
#include "RenderUtils.hpp"
#include <array>
#include "Vector3D.h"

constexpr int N_ESFERAS = 10;

class P0_Scene :
    public Scene
{
public:
    explicit P0_Scene(std::string name) : Scene(std::move(name)) {}

    void init() override {
        // Ejemplo: Creación de una esfera usando las utilidades de render existentes
        physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(1.0f));

        // Reto A
        m_centroTr = physx::PxTransform(Vector3D(0.0f, 0.0f, 0.0f));
        Vector3D u(3.f, 1.f, 0.f);
        Vector3D v(0.f, 4.f, 0.f);
        Vector3D w = u.cross(v);
        m_uTr = physx::PxTransform(u.normalized() * 5.f);
        m_vTr = physx::PxTransform(v.normalized() * 5.f);
        m_wTr = physx::PxTransform(w.normalized() * 5.f);

        //m_centroEscena = new RenderItem(shape, &m_centroTr, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        //m_vecU = new RenderItem(shape, &m_uTr, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
        //m_vecV = new RenderItem(shape, &m_vTr, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        //m_vecW = new RenderItem(shape, &m_wTr, Vector4(0.0f, 0.0f, 1.0f, 1.0f));


        // Reto B
        D = Vector3D(0.f, 0.f, 1.f);
        P_1 = physx::PxTransform(Vector3D(2.f, 0.f, 3.f));
        P_2 = physx::PxTransform(Vector3D(-4.f, 0.f, 1.f));
        P_3 = physx::PxTransform(Vector3D(0.f, 0.f, -5.f));
        P_4 = physx::PxTransform(Vector3D(3.f, 0.f, 0.f));

        // He hecho esta pequeña lambda para el reto B
        auto colorFromDot = [](float dotResult) {
            if (dotResult > 0) {
                return Vector4(1.f, 0.f, 0.f, 1.f);
            }
            else if (dotResult < 0) {
                return Vector4(0.f, 1.0f, 0.f, 1.f);
            }
            else {
                return Vector4(1.f, 1.f, 0.f, 1.f);
            }
            };

        //m_P1Esfera = new RenderItem(shape, &P_1, colorFromDot(D.dot(P_1.p)));
        //m_P2Esfera = new RenderItem(shape, &P_2, colorFromDot(D.dot(P_2.p)));
        //m_P3Esfera = new RenderItem(shape, &P_3, colorFromDot(D.dot(P_3.p)));
        //m_P4Esfera = new RenderItem(shape, &P_4, colorFromDot(D.dot(P_4.p)));


        // Reto C
        Vector3D vecA(-8.f, 1.f, -8.f);
        Vector3D vecB(8.f, 8.f, 8.f);
        A = physx::PxTransform(vecA);
        B = physx::PxTransform(vecB);
        AEsfera = new RenderItem(shape, &A, Vector4(0.f, 1.f, 0.f, 1.f));
        BEsfera = new RenderItem(shape, &B, Vector4(1.f, 0.f, 0.f, 1.f));

        // Generar puntos intermedios
        Vector3D vecAtoB = vecB - vecA;
        Vector3D vecStep = vecAtoB * (1.f / (float)(N_ESFERAS));
        for (int i = 0; i < N_ESFERAS; i++) {
            interPoints[i] = physx::PxTransform(vecA + vecStep * i);
            interEsferas[i] = new RenderItem(shape, &interPoints[i], Vector4(1.f, 1.f, 1.f, 1.f));
        }
    }

    void update(double dt) override {
        // Lógica/Integración del alumno (por ejemplo, movimiento simple)
    }

    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
    }

    void cleanup() override {
        if (m_centroEscena) {
            m_centroEscena->release(); // Deregistra y destruye el item
            m_centroEscena = nullptr;
        }
        if (m_vecU) {
            m_vecU->release(); // Deregistra y destruye el item
            m_vecU = nullptr;
        }
        if (m_vecV) {
            m_vecV->release(); // Deregistra y destruye el item
            m_vecV = nullptr;
        }
        if (m_vecW) {
            m_vecW->release(); // Deregistra y destruye el item
            m_vecW = nullptr;
        }

        if (m_P1Esfera) {
            m_P1Esfera->release(); // Deregistra y destruye el item
            m_P1Esfera = nullptr;
        }

        if (m_P2Esfera) {
            m_P2Esfera->release(); // Deregistra y destruye el item
            m_P2Esfera = nullptr;
        }

        if (m_P3Esfera) {
            m_P3Esfera->release(); // Deregistra y destruye el item
            m_P3Esfera = nullptr;
        }

        if (m_P4Esfera) {
            m_P4Esfera->release(); // Deregistra y destruye el item
            m_P4Esfera = nullptr;
        }

        if (AEsfera) {
            AEsfera->release(); // Deregistra y destruye el item
            AEsfera = nullptr;
        }

        if (BEsfera) {
            BEsfera->release(); // Deregistra y destruye el item
            BEsfera = nullptr;
        }

        for (RenderItem* item : interEsferas) {
            item->release();
            item = nullptr;
        }
    }

private:
    // Reto A
    physx::PxTransform m_centroTr;
    physx::PxTransform m_uTr;
    physx::PxTransform m_vTr;
    physx::PxTransform m_wTr;
    RenderItem* m_centroEscena{ nullptr };
    RenderItem* m_vecU{ nullptr };
    RenderItem* m_vecV{ nullptr };
    RenderItem* m_vecW{ nullptr };

    // Reto B
    Vector3D D;
    physx::PxTransform P_1;
    physx::PxTransform P_2;
    physx::PxTransform P_3;
    physx::PxTransform P_4;
    RenderItem* m_P1Esfera{ nullptr };
    RenderItem* m_P2Esfera{ nullptr };
    RenderItem* m_P3Esfera{ nullptr };
    RenderItem* m_P4Esfera{ nullptr };

    // Reto C
    physx::PxTransform A;
    physx::PxTransform B;
    std::array<physx::PxTransform, N_ESFERAS> interPoints;
    RenderItem* AEsfera, *BEsfera;
    std::array<RenderItem*, N_ESFERAS> interEsferas;
};

