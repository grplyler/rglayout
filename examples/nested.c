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
    InitWindow(1024, 512, "RayGUI Layout - Advanced Nested Demo");
    SetTargetFPS(60);

    // Load GUI style - try multiple possible paths
    GuiLoadStyle("vendor/raygui/styles/cyber/style_cyber.rgs");

    // GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

    // Layout Settings
    RGLSetDefaultGap(5.0f);
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
        Rectangle screen_rect = {0, 0, screen_w, screen_h};

        // Toplevel Column: Header(50px), Content(stretch)
        RGLPlan column_plan = GuiPlanCreate((float[]){50, -1}, 2);
        GuiPlanSetPadAll(&column_plan, 10);
        GuiBeginColumn(screen_rect, &column_plan);

            // Headers
            RGLPlan header_plan = GuiPlanCreate((float[]){1, 4}, 2);
            GuiBeginRow(GuiLayoutRec(-1, -1), &header_plan);
                DebugButton(GuiLayoutRec(-1, -1), "Logo");
                DebugButton(GuiLayoutRec(-1, -1), "Header");
            GuiLayoutEnd();

            RGLPlan content_plan = GuiPlanCreate((float[]){1, 3, 1}, 3);
            GuiBeginRow(GuiLayoutRec(-1, -1), &content_plan);

                // Left Sidebar
                DebugButton(GuiLayoutRec(-1, -1), "Left\nSidebar");      // Left Sidebar

                // Center Content
                GuiBeginColumn(GuiLayoutRec(-1, -1), NULL);

                    // Center 60px tall roolbar
                    Rectangle tb_rect = GuiLayoutRec(60, -1);
                    GuiBeginRow(tb_rect, NULL);
                        DebugButton(GuiLayoutRec(-1, -1), "Toolbar");
                    GuiLayoutEnd();

                    // Nested Toolbar: resuse the same rect as toolbar wrapper
                    RGLPlan toolbar_plan = GuiPlanCreate(NULL, 0);
                    GuiPlanAdd(&toolbar_plan, -1); // Add a Flex Fill Item
                    GuiPlanAddRepeat(&toolbar_plan, 40, 10); // Add 10 30px Items
                    GuiPlanSetPadAll(&toolbar_plan, 10); // Add 5px Padding
                    GuiBeginRow(tb_rect, &toolbar_plan);
                        DebugButton(GuiLayoutRec(-1, -1), "Toolbar"); // Uses plan[0] which is flex

                        // Add 10 Toolbar button 20px wide
                        for (int i = 0; i < 10; i++)
                        {
                            DebugButton(GuiLayoutRec(-1, -1), TextFormat("%d", i)); // Uses plan[1..] which are fixed sizes
                        }
                    GuiLayoutEnd();

                    // Main Content
                    GuiBeginRow(GuiLayoutRec(-1, -1), NULL);
                        RGLPlan main_plan = GuiPlanCreate((float[]){-1, 50}, 2);
                        GuiBeginColumn(GuiLayoutRec(-1, -1), &main_plan);
                            DebugButton(GuiLayoutRec(-1, -1), "Content");
                            DebugButton(GuiLayoutRec(-1, -1), "Footer");
                        GuiLayoutEnd();
                    GuiLayoutEnd();


                GuiLayoutEnd();

                // Right Sidebar (Nested)
                RGLPlan sidebar_plan = GuiPlanCreate((float[]){-1, 100, 50}, 3);
                GuiPlanSetPadAll(&sidebar_plan, 10);
                GuiBeginColumn(GuiLayoutRec(-1, -1), &sidebar_plan);
                    DebugButton(GuiLayoutRecLast(), "Right Sidebar"); // Reuse the sidebar_rect but this time it has padding
                    DebugButton(GuiLayoutRec(-1, -1), "Sidebar Nested");
                    DebugButton(GuiLayoutRec(100, 100), "Custom Size"); // Note the main axis (first param) is set in plan
                    DebugButton(GuiLayoutRec(-1, -1), "Right\n Nested Footer");
                GuiLayoutEnd();

            GuiLayoutEnd();

        GuiLayoutEnd(); // Toplevel Column


        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    return 0;
}
