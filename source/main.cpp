#include "coordinator.h"
#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

GameData G{};

void updateDrawFrame()
{
	//UPDATE
	update(G);
	//DRAW
	BeginDrawing();

	ClearBackground(PURPLE);
	draw(G);

	EndDrawing();
}

int main()
{
	SetWindowState(FLAG_MSAA_4X_HINT);
	InitWindow(256, 256, "WELCOMD TO THE INTERWEBS");
    InitAudioDevice();
	initialize(G);

	SetTargetFPS(144);

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(updateDrawFrame, 0, 1);
#else
	while (!WindowShouldClose())
	{
		updateDrawFrame();
	}
#endif
	return 0;
}