#include "SceneManager.h"
#include <iostream>
#include "ImGui/imgui.h"

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
    // Mostrar menu principal (barra de arriba)
    if (ImGui::BeginMainMenuBar())
    {
        // Item para cambiar de escenas
        if (ImGui::BeginMenu("Scenes"))
        {
            if (ImGui::MenuItem("Empty"))  SceneManager::instance().changeScene("EscenaVacia");
            if (ImGui::MenuItem("P0"))     SceneManager::instance().changeScene("P0_Scene");
            if (ImGui::MenuItem("P1"))     SceneManager::instance().changeScene("P1_Scene");
            if (ImGui::MenuItem("P2"))     SceneManager::instance().changeScene("P2_Scene");
            if (ImGui::MenuItem("P3"))     SceneManager::instance().changeScene("P3_Scene");
            if (ImGui::MenuItem("P4"))     SceneManager::instance().changeScene("P4_Scene");
            ImGui::EndMenu();
        }

        // Muestra la escena actual
        if (m_currentScene)
        {
            const char* sceneName = m_currentScene.get()->getName().c_str();
            ImGui::SameLine(ImGui::GetWindowWidth()/2.f - ImGui::CalcTextSize(sceneName).x, 0);
            ImGui::Text(sceneName);
        }

        // Muestra los fps
        char fps[32];
        snprintf(fps, sizeof(fps), "%.1f FPS", ImGui::GetIO().Framerate);
        float w = ImGui::CalcTextSize(fps).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - w - 10);
        ImGui::TextUnformatted(fps);

        ImGui::EndMainMenuBar();
    }

    if (m_currentScene) m_currentScene->drawGui(); 
}

// Reenvía eventos de teclado a la escena actual. Mantener este método simple
// permite centralizar atajos globales si se desea en el futuro.
void SceneManager::keyPress(unsigned char key, const physx::PxTransform& cameraTransform) {
    // Teclas globales de navegación entre prácticas
    if (key == 'V') {
        changeScene("EscenaVacia");
        return; // Consumimos el evento para que no interfiera con la escena
    }
    if (key == '0') {
        changeScene("P0_Scene");
        return; // Consumimos el evento para que no interfiera con la escena
    }
    if (key == '1') {
        changeScene("P1_Scene");
        return;
    }
    if (key == '2') {
        changeScene("P2_Scene");
        return;
    }
    if (key == '3') {
        changeScene("P3_Scene");
        return;
    }
    if (key == '4') {
        changeScene("P4_Scene");
        return;
    }

    // Si no es una tecla de navegación global, se la pasamos a la escena activa
    if (m_currentScene != nullptr) {
        m_currentScene->keyPress(key, cameraTransform);
    }
}