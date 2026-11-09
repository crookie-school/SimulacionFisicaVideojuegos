#pragma once

#include "../../Framework/Scene/Scene.h"

class EmptyScene : public Scene {
public:
    using Scene::Scene;

    void init() override {
    }

    void update(double dt) override {
        // Lógica/Integración del alumno (por ejemplo, movimiento simple)
        //m_transform.p.y -= static_cast<float>(9.8 * dt);
    }

    void render() override {}

    void cleanup() override {
    }

private:
};