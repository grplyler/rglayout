#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define RGLAYOUT_IMPLEMENTATION
#include "../src/rglayout.h"

#include <string.h>
#include <stdio.h>

// Helper function to draw debug buttons (similar to Odin's DebugButtonEx)
static void DebugButton(Rectangle rect, const char* label) {
    // Draw button with distinctive colors for debugging layout
    static int button_counter = 0;
    char button_text[64];

    if (label != NULL) {
        strncpy(button_text, label, sizeof(button_text) - 1);
        button_text[sizeof(button_text) - 1] = '\0';
    } else {
        snprintf(button_text, sizeof(button_text), "Button %d", button_counter++);
    }

    GuiButton(rect, button_text);
}

//------------------------------------------------------------------------------------
// Sidebar Settings Demo
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(720, 480, "RayGUI Layout - 3Panel Demo");
    SetTargetFPS(60);

    // Load GUI style - try multiple possible paths
    GuiLoadStyle("vendor/raygui/styles/cyber/style_cyber.rgs");

    // GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

    // Layout Settings
    RGLSetDefaultGap(10.0f);
    RGLSetDefaultPadAll(0.0f);

    // Main game loop
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q)) break;

        // Update (nothing yet)

        // Draw
        BeginDrawing();
        ClearBackground(bg_color);

        float screen_w = (float)GetScreenWidth();
        float screen_h = (float)GetScreenHeight();
        Rectangle panel = {0, 0, screen_w, screen_h};

        // Toplevel Column: Header(50px), Content(stretch), Footer(50px)
        RGLPlan column_plan = GuiPlanCreate((float[]){50, -1}, 2);
        GuiPlanSetPadAll(&column_plan, 10);
        GuiBeginColumn(panel, &column_plan);

            // Header Row
             DebugButton(GuiLayoutRec(-1, -1), "Header");

            // Content Row with 3 columns: Left(flex 1), Center(flex 3), Right(flex 1)
            RGLPlan content_plan = GuiPlanCreate((float[]){1, 3, 1}, 3);
            GuiBeginRow(GuiLayoutRec(-1, -1), &content_plan);
                DebugButton(GuiLayoutRec(-1, -1), "Left\nSidebar");      // Left Sidebar
                DebugButton(GuiLayoutRec(-1, -1), "Main\nContent");      // Center Content
                DebugButton(GuiLayoutRec(50, -1), "Right\nSidebar");;
            GuiLayoutEnd();


        GuiLayoutEnd(); // Toplevel Column


        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    return 0;
}
