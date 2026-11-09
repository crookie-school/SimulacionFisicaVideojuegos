#pragma once

#include "Scene.h"
#include "../Window.h"
#include "../Render/Camera.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

// Gestor global de escenas (singleton).
// Permite registrar fábricas de escenas por nombre, solicitar cambios de
// escena de forma diferida y delegar llamadas de actualización y teclado.
class SceneManager : public IInputEventListener {
public:
    SceneManager(Camera* cam) : camera(cam) {}

    // Registra una escena concreta T con un identificador de nombre.
    // T debe derivar de Scene y exponer un constructor T(const std::string&).
    template <typename T>
    void registerScene(const std::string& name) {
        m_factories[name] = [name, this]() { return std::make_unique<T>(name, camera); };
    }

    // Solicita el cambio de escena. El cambio se aplicará de forma diferida
    // durante la próxima llamada a update() para evitar modificar la escena
    // actual mientras se itera sobre ella.
    void changeScene(const std::string& name);

    // Actualiza la escena actual (y aplica cambios pendientes antes de ello).
    // dt es el tiempo en segundos desde la última actualización.
    void update(double dt);

    void drawGui();

    void render();

    // Inherited via IInputEventListener
    void onKeyDown(unsigned char k) override;
    void onKeyUp(unsigned char k) override;
    void onMouseButton(int b, int s, int x, int y) override;
    void onMouseMove(int x, int y) override;

    // Devuelve un puntero a la escena actual (no toma propiedad).
    [[nodiscard]] Scene* getCurrentScene() const { return m_currentScene.get(); }

private:
    // Map de fábricas que crean instancias de Scene identificadas por nombre.
    std::unordered_map<std::string, std::function<std::unique_ptr<Scene>()>> m_factories;
    // Escena actualmente activa.
    std::unique_ptr<Scene> m_currentScene{ nullptr };
    // Nombre de la escena pendiente de cargar y flag para indicar cambio.
    std::string m_pendingSceneName;
    bool m_hasPendingChange{ false };

    // Aplica el cambio pendiente (limpia la escena actual y crea la nueva).
    void applyPendingSceneChange();

    Camera* camera;
};
