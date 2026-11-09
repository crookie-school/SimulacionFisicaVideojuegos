#include "SceneManager.h"
#include <iostream>
#include "../../ImGui/imgui.h"

// Solicita el cambio de escena. Si el nombre no está registrado, se escribe
// un error en stderr y la solicitud se ignora.
void SceneManager::changeScene(const std::string& name) {
    if (m_factories.find(name) == m_factories.end()) {
        std::cerr << "[SceneManager Error] Escena no registrada: " << name << "\n";
        return;
    }
    m_pendingSceneName = name;
    m_hasPendingChange = true;
}

// Realiza la transición a la escena pendiente: limpia la escena actual,
// crea la nueva usando la fábrica y la inicializa.
void SceneManager::applyPendingSceneChange() {
    if (!m_hasPendingChange) return;

    if (m_currentScene) {
        m_currentScene->cleanup();
    }

    m_currentScene = m_factories[m_pendingSceneName]();
    if (m_currentScene) {
        m_currentScene->init();
        std::cout << "[SceneManager] Escena cargada: " << m_pendingSceneName << "\n";
    }

    m_hasPendingChange = false;
    m_pendingSceneName.clear();
}

void SceneManager::onKeyDown(unsigned char key)
{
    // Si no es una tecla de navegación global, se la pasamos a la escena activa
    if (m_currentScene != nullptr)
    {
        m_currentScene->keyPress(key);
    }
}

// Por ahora estas no se usan
void SceneManager::onKeyUp(unsigned char k){}
void SceneManager::onMouseButton(int b, int s, int x, int y){}
void SceneManager::onMouseMove(int x, int y){}

// Llamada por frame: aplica el cambio pendiente antes de actualizar la escena
// para evitar modificar la escena mientras está siendo usada.
void SceneManager::update(double dt) {
    if (m_hasPendingChange) {
        applyPendingSceneChange();
    }

    if (m_currentScene) {
        m_currentScene->update(dt);
    }
}

void SceneManager::drawGui() { 
    

    if (m_currentScene) m_currentScene->drawGui(); 
}

void SceneManager::render()
{
    if (m_currentScene) m_currentScene->render();
}
