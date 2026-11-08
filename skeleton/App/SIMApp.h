#pragma once

#include "../Framework/BaseApplication.h"

#include "Scenes/EmptyScene.h"
#include "Scenes/P1_Scene.h"
#include "Scenes/P2_Scene.h"
#include "Scenes/P3_Scene.h"
#include "Scenes/P4_Scene.h"

class SIMApp : public BaseApplication
{
public:
	SIMApp(const ApplicationDesc& desc) : BaseApplication(desc)
	{
		sceneManager.registerScene<EmptyScene>("EmptyScene");
		sceneManager.registerScene<P1_Scene>("P1_Scene");
		sceneManager.registerScene<P2_Scene>("P2_Scene");
		sceneManager.registerScene<P3_Scene>("P3_Scene");
		sceneManager.registerScene<P4_Scene>("P4_Scene");

        sceneManager.changeScene("EmptyScene");
	}

    ~SIMApp() = default;

    void drawAppUI() override
    {
        // Mostrar menu principal (barra de arriba)
        if (ImGui::BeginMainMenuBar())
        {
            // Item para cambiar de escenas
            if (ImGui::BeginMenu("Scenes"))
            {
                if (ImGui::MenuItem("Empty"))  sceneManager.changeScene("EscenaVacia");
                if (ImGui::MenuItem("P1"))     sceneManager.changeScene("P1_Scene");
                if (ImGui::MenuItem("P2"))     sceneManager.changeScene("P2_Scene");
                if (ImGui::MenuItem("P3"))     sceneManager.changeScene("P3_Scene");
                if (ImGui::MenuItem("P4"))     sceneManager.changeScene("P4_Scene");
                ImGui::EndMenu();
            }

            // Muestra la escena actual
            Scene* currScene = sceneManager.getCurrentScene();
            if (currScene)
            {
                const char* sceneName = currScene->getName().c_str();
                ImGui::SameLine(ImGui::GetWindowWidth() / 2.f - ImGui::CalcTextSize(sceneName).x, 0);
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
    }

	void onKeyDown(unsigned char key) override
	{
        BaseApplication::onKeyDown(key);

        // Teclas globales de navegación entre prácticas
        if (key == 'V')
        {
            sceneManager.changeScene("EscenaVacia");
            return; // Consumimos el evento para que no interfiera con la escena
        }
        if (key == '1')
        {
            sceneManager.changeScene("P1_Scene");
            return;
        }
        if (key == '2')
        {
            sceneManager.changeScene("P2_Scene");
            return;
        }
        if (key == '3')
        {
            sceneManager.changeScene("P3_Scene");
            return;
        }
        if (key == '4')
        {
            sceneManager.changeScene("P4_Scene");
            return;
        }
	}
};

