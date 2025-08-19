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
    InitWindow(300, 200, "RayGUI Layout - Login Form Demo");
    SetTargetFPS(60);

    // Load GUI style - try multiple possible paths
    GuiLoadStyle("vendor/raygui/styles/cyber/style_cyber.rgs");

    // GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    Color bg_color = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

    // State
    char username[32] = {0};
    char password[32] = {0};
    bool username_edit = false;
    bool password_edit = false;

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
        Rectangle screen_rect = {0, 0, screen_w, screen_h};

        // Toplevel Column: Header(50px), Content(stretch)
        RGLSetDefaultPadAll(10);
        GuiBeginColumn(screen_rect, NULL);
        RGLSetDefaultPadAll(0);

            // Username row
            GuiBeginRow(GuiLayoutRec(50, -1), NULL);
                GuiLabel(GuiLayoutRec(75, -1), "Username:");
                if (GuiTextBox(GuiLayoutRec(-1, -1), username, 32, username_edit)) username_edit = !username_edit;
            GuiLayoutEnd();

            // Password Row
            GuiBeginRow(GuiLayoutRec(50, -1), NULL);
                GuiLabel(GuiLayoutRec(75, -1), "Password:");
                if (GuiTextBox(GuiLayoutRec(-1, -1), password, 32, password_edit)) password_edit = !password_edit;
            GuiLayoutEnd();

            // Centered Login button
            RGLPlan login_row_plan = GuiPlanCreate((float[]){-1, 100, -1}, 3); // space, 100px, space
            GuiBeginRow(GuiLayoutRec(50, -1), &login_row_plan);
                GuiLayoutRec(-1, -1); // take next rect as Space
                DebugButton(GuiLayoutRec(-1, -1), "Login");
                GuiLayoutRec(-1, -1); // Take final space (not reall needed)
            GuiLayoutEnd();

        GuiLayoutEnd(); // Toplevel Column

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context
    return 0;
}
