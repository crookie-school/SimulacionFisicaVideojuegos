#if defined(_DEBUG) && defined(NDEBUG)
#pragma message("⚠️ ¡ALERTA! Ambas macros (_DEBUG y NDEBUG) están definidas a la vez.")
#elif defined(_DEBUG)
#pragma message("ℹ️ Solo _DEBUG está definida.")
#elif defined(NDEBUG)
#pragma message("ℹ️ Solo NDEBUG está definida.")
#else
#pragma message("⚠️ ¡ALERTA! Ninguna macro está definida.")
#endif

#include "App/SIMApp.h"

int main(int, const char*const*)
{
	ApplicationDesc appDesc;
	appDesc.fixedTimestep = 1.f / 60.f;

	SIMApp app(appDesc);

	app.run();

	return 0;
}