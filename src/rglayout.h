#ifndef RGLAYOUT_H
#define RGLAYOUT_H

// #include "raylib.h"
#include <stdbool.h>

// Function specifiers in case library is build/used as a shared library
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
// NOTE: visibility("default") attribute makes symbols "visible" when compiled with -fvisibility=hidden
#if defined(_WIN32)
#if defined(__TINYC__)
#define __declspec(x) __attribute__((x))
#endif
#if defined(BUILD_LIBTYPE_SHARED)
#define RGLAYOUTAPI __declspec(dllexport) // We are building the library as a Win32 shared library (.dll)
#elif defined(USE_LIBTYPE_SHARED)
#define RGLAYOUTAPI __declspec(dllimport) // We are using the library as a Win32 shared library (.dll)
#endif
#else
#if defined(BUILD_LIBTYPE_SHARED)
#define RGLAYOUTAPI __attribute__((visibility("default"))) // We are building as a Unix shared library (.so/.dylib)
#endif
#endif

#ifndef RGLAYOUTAPI
    #define RGLAYOUTAPI // Functions defined as 'extern' by default (implicit specifiers)
#endif

// ===== CONFIGURATION =====

// Maximum nesting depth for containers
#define RGL_MAX_STACK 16
#define RGL_PLAN_MAX 32

// ===== TYPES & ENUMS =====

// Standalone Support: TODO
#if defined(RGLAYOUT_STANDALONE)
typedef struct Rectangle
{
    float x;      // Rectangle top-left corner position x
    float y;      // Rectangle top-left corner position y
    float width;  // Rectangle width
    float height; // Rectangle height
} Rectangle;

#endif

// Layout direction - either horizontal (ROW) or vertical (COLUMN)
typedef enum
{
    RGL_AXIS_ROW,   // Horizontal layout (left to right)
    RGL_AXIS_COLUMN // Vertical layout (top to bottom)
} RGLAxis;

// Vertical alignment options
typedef enum
{
    RGL_VALIGN_TOP,
    RGL_VALIGN_BOTTOM,
    RGL_VALIGN_CENTER,
    RGL_VALIGN_NONE
} RGLVAlign;

// Padding for all four sides of a container
typedef struct
{
    float left;
    float top;
    float right;
    float bottom;
} RGLPad;

// Plan struct
typedef struct
{
    float sizes[RGL_PLAN_MAX]; // Can you allocate a fixed-size array
    int length;      // Length of sizes array
    float gap;       // Gap between elements
    RGLPad pad;      // Container padding
    bool has_gap;    // Whether gap was explicitly set
    bool has_pad;    // Whether padding was explicitly set
} RGLPlan;

// Internal container state tracking layout progress
typedef struct
{
    // Layout bounds
    Rectangle outer; // Original bounds including padding
    Rectangle inner; // Available space after padding

    // Layout configuration
    RGLAxis axis; // Row or Column direction
    float gap;    // Space between children
    RGLPad pad;   // Container padding

    // Dynamic state during layout
    float cursor;    // Current position along main axis
    float remaining; // Remaining space in main axis
    int placed;      // Number of children placed so far

    // Layout plan (optional)
    RGLPlan plan;          // Plan specification
    int plan_index;        // Current position in plan
    float plan_sum_fixed;  // Total fixed pixels in plan
    float plan_sum_weight; // Total flex weight in plan
    float plan_gap_total;  // Total gap space required by plan
} RGLContainer;

// Global layout state - stack of nested containers
typedef struct
{
    RGLContainer stack[RGL_MAX_STACK];
    int top; // Current stack depth
} RGLState;

// ===== PADDING HELPERS =====

#define RGL_PAD_0 (RGLPad){0, 0, 0, 0}
#define RGL_PAD_10 (RGLPad){10, 10, 10, 10}

// ===== FUNCTION DECLARATIONS =====

// Configuration functions
RGLAYOUTAPI void RGLSetDefaultPadAll(float p);
RGLAYOUTAPI void RGLSetDefaultPad(RGLPad p);
RGLAYOUTAPI void RGLSetDefaultVAlign(RGLVAlign align);
RGLAYOUTAPI void RGLSetDefaultGap(float g);

// Plan Builder Functions
RGLAYOUTAPI RGLPlan GuiPlanCreate(const float *sizes, int length);
RGLAYOUTAPI void GuiPlanAdd(RGLPlan *plan, float size);
RGLAYOUTAPI void GuiPlanSetGap(RGLPlan *plan, float gap);
RGLAYOUTAPI void GuiPlanSetPad(RGLPlan *plan, RGLPad pad);
RGLAYOUTAPI void GuiPlanSetPadAll(RGLPlan *plan, float pad);

// Padding helper functions
RGLAYOUTAPI RGLPad RGLPadAll(float p);
RGLAYOUTAPI RGLPad RGLPadX(float x);
RGLAYOUTAPI RGLPad RGLPadY(float y);
RGLAYOUTAPI RGLPad RGLPadL(float x);
RGLAYOUTAPI RGLPad RGLPadR(float x);

// Core layout functions
RGLAYOUTAPI void GuiBeginRow(Rectangle bounds, RGLPlan *plan);                                         // Uses defaults, plan optional
RGLAYOUTAPI void GuiBeginRowEx(Rectangle bounds, RGLPad pad, float gap, RGLPlan *plan);               // Expert version, plan optional
RGLAYOUTAPI void GuiBeginColumn(Rectangle bounds, RGLPlan *plan);                                      // Uses defaults, plan optional
RGLAYOUTAPI void GuiBeginColumnEx(Rectangle bounds, RGLPad pad, float gap, RGLPlan *plan);            // Expert version, plan optional

// Layout state functions
RGLAYOUTAPI void GuiLayoutEnd(void);

RGLAYOUTAPI Rectangle GuiLayoutRec(float main, float cross);                                                             // Uses defaults
RGLAYOUTAPI Rectangle GuiLayoutRecAlign(float main, float cross, RGLVAlign valign);                                                             // Uses defaults
RGLAYOUTAPI Rectangle GuiLayoutRecEx(float main, float cross, float pl, float pr, float pt, float pb, RGLVAlign valign); // Expert version

// New: return the last rectangle produced by GuiLayoutRec / GuiLayoutRecEx
RGLAYOUTAPI Rectangle GuiLayoutRecLast(void);

// Convenience functions
RGLAYOUTAPI Rectangle GuiLayoutPanel(float main, float cross, float pad_top, float pad_other, RGLPad *out_pad);

#endif // RGLAYOUT_H

// ===== IMPLEMENTATION =====

#ifdef RGLAYOUT_IMPLEMENTATION

#include <string.h>

// ===== GLOBAL DEFAULTS & STATE =====

// Private default values
static RGLPad g_default_pad = {10, 10, 10, 10};
static RGLVAlign g_default_valign = RGL_VALIGN_CENTER;
static float g_default_gap = 10.0f;

// The global layout state
static RGLState g_rgl = {0};

// New: store last returned rectangle
static Rectangle g_last_rect = {0};

// ===== CONFIGURATION FUNCTIONS =====

RGLAYOUTAPI void RGLSetDefaultPadAll(float p)
{
    g_default_pad = (RGLPad){p, p, p, p};
}

RGLAYOUTAPI void RGLSetDefaultPad(RGLPad p)
{
    g_default_pad = p;
}


RGLAYOUTAPI void RGLSetDefaultVAlign(RGLVAlign align)
{
    g_default_valign = align;
}

RGLAYOUTAPI void RGLSetDefaultGap(float g)
{
    g_default_gap = g;
}

// ===== PADDING HELPERS =====

RGLAYOUTAPI RGLPad RGLPadAll(float p)
{
    return (RGLPad){p, p, p, p};
}

RGLAYOUTAPI RGLPad RGLPadX(float x)
{
    return (RGLPad){x, 0, x, 0};
}

RGLAYOUTAPI RGLPad RGLPadY(float y)
{
    return (RGLPad){0, y, 0, y};
}

RGLAYOUTAPI RGLPad RGLPadL(float x)
{
    return (RGLPad){x, 0, 0, 0};
}

RGLAYOUTAPI RGLPad RGLPadR(float x)
{
    return (RGLPad){0, 0, x, 0};
}

// ===== UTILITY FUNCTIONS =====

// Shrink rectangle by padding on all sides
static Rectangle shrink_rect(Rectangle r, RGLPad p)
{
    Rectangle res = r;
    res.x += p.left;
    res.y += p.top;
    res.width -= (p.left + p.right);
    res.height -= (p.top + p.bottom);

    // Ensure non-negative dimensions
    if (res.width < 0)
        res.width = 0;
    if (res.height < 0)
        res.height = 0;
    return res;
}

// Check if container flows horizontally
static bool is_row(const RGLContainer *c)
{
    return c->axis == RGL_AXIS_ROW;
}

// Get the main axis size (width for rows, height for columns)
static float main_size(Rectangle r, bool row)
{
    return row ? r.width : r.height;
}

// Get the cross axis size (height for rows, width for columns)
static float cross_size(Rectangle r, bool row)
{
    return row ? r.height : r.width;
}

// ===== CORE LAYOUT ALGORITHM =====

// Initialize a container with layout parameters
static void init_common(RGLContainer *c, Rectangle bounds, RGLAxis axis, RGLPad pad, float gap, RGLPlan plan)
{
    // Set up container bounds and configuration
    c->outer = bounds;

    // Use plan's padding if set, otherwise use provided padding
    c->pad = plan.has_pad ? plan.pad : pad;
    c->inner = shrink_rect(bounds, c->pad);
    c->axis = axis;

    // Use plan's gap if set, otherwise use provided gap (which may be default)
    c->gap = plan.has_gap ? plan.gap : gap;

    // Initialize dynamic state
    c->placed = 0;
    c->cursor = (axis == RGL_AXIS_ROW) ? c->inner.x : c->inner.y;
    c->remaining = (axis == RGL_AXIS_ROW) ? c->inner.width : c->inner.height;

    // Set up layout plan (if provided)
    c->plan = plan;
    c->plan_index = 0;
    c->plan_sum_fixed = 0;
    c->plan_sum_weight = 0;
    c->plan_gap_total = 0;

    // Pre-calculate plan totals for flex distribution
    if (plan.length > 0)
    {
        float fixed_px = 0;
        float weight_sum = 0;

        for (int i = 0; i < plan.length; i++)
        {
            float v = plan.sizes[i];
            if (v >= 20)
            {
                // Values >= 20 are pixel sizes
                fixed_px += v;
            }
            else if (v == -1)
            {
                // -1 means "fill" (weight of 1)
                weight_sum += 1;
            }
            else if (v > 0 && v < 20)
            {
                // Values 0-19 are flex weights
                weight_sum += v;
            }
        }

        // Pre-calculate gap total - use the actual gap that will be used
        float gaps = (plan.length > 1) ? (plan.length - 1) * c->gap : 0;

        c->plan_sum_fixed = fixed_px;
        c->plan_sum_weight = weight_sum;
        c->plan_gap_total = gaps;
    }
}

// Allocate the next rectangle in the current container
static Rectangle take_rect(RGLContainer *c, float main, float cross)
{
    bool row = is_row(c);

    // Add gap before this element (except for the first one)
    if (c->placed > 0 && c->gap > 0)
    {
        c->cursor += c->gap;
        // Only consume gap from 'remaining' in ad-hoc mode
        if (c->plan.length == 0)
        {
            c->remaining -= c->gap;
        }
    }

    // Clamp main axis size to available space
    float m = main;
    if (m > c->remaining)
        m = c->remaining;
    if (m < 0)
        m = 0;

    // Handle cross axis sizing
    float cross_fill = cross_size(c->inner, row);
    float cr = cross;
    if (cr < 0)
        cr = cross_fill; // Negative = fill cross axis
    if (cr > cross_fill)
        cr = cross_fill; // Clamp to available space

    // Create the rectangle based on layout direction
    Rectangle r;
    if (row)
    {
        // Horizontal layout
        r = (Rectangle){
            .x = c->cursor,
            .y = c->inner.y,
            .width = m,
            .height = cr};
        c->cursor += m;
        c->remaining -= m;
    }
    else
    {
        // Vertical layout
        r = (Rectangle){
            .x = c->inner.x,
            .y = c->cursor,
            .width = cr,
            .height = m};
        c->cursor += m;
        c->remaining -= m;
    }

    c->placed++;
    return r;
}

// ===== PUBLIC API =====

RGLAYOUTAPI void GuiBeginRow(Rectangle bounds, RGLPlan *plan)
{
    RGLPlan empty_plan = {0};
    RGLPlan actual_plan = plan ? *plan : empty_plan;
    GuiBeginRowEx(bounds, g_default_pad, g_default_gap, &actual_plan);
}

RGLAYOUTAPI void GuiBeginRowEx(Rectangle bounds, RGLPad pad, float gap, RGLPlan *plan)
{
    RGLContainer c;
    RGLPlan empty_plan = {0};
    RGLPlan actual_plan = plan ? *plan : empty_plan;
    init_common(&c, bounds, RGL_AXIS_ROW, pad, gap, actual_plan);
    if (g_rgl.top < RGL_MAX_STACK)
    {
        g_rgl.stack[g_rgl.top] = c;
        g_rgl.top++;
    }
}

RGLAYOUTAPI void GuiBeginColumn(Rectangle bounds, RGLPlan *plan)
{
    RGLPlan empty_plan = {0};
    RGLPlan actual_plan = plan ? *plan : empty_plan;
    GuiBeginColumnEx(bounds, g_default_pad, g_default_gap, &actual_plan);
}

RGLAYOUTAPI void GuiBeginColumnEx(Rectangle bounds, RGLPad pad, float gap, RGLPlan *plan)
{
    RGLContainer c;
    RGLPlan empty_plan = {0};
    RGLPlan actual_plan = plan ? *plan : empty_plan;
    init_common(&c, bounds, RGL_AXIS_COLUMN, pad, gap, actual_plan);
    if (g_rgl.top < RGL_MAX_STACK)
    {
        g_rgl.stack[g_rgl.top] = c;
        g_rgl.top++;
    }
}

RGLAYOUTAPI void GuiLayoutEnd(void)
{
    if (g_rgl.top > 0)
    {
        g_rgl.top--;
    }
}

RGLAYOUTAPI Rectangle GuiLayoutRec(float main, float cross)
{
    return GuiLayoutRecEx(main, cross, 0.0f, 0.0f, 0.0f, 0.0f, g_default_valign);
}

RGLAYOUTAPI Rectangle GuiLayoutRecAlign(float main, float cross, RGLVAlign valign)
{
    return GuiLayoutRecEx(main, cross, 0.0f, 0.0f, 0.0f, 0.0f, valign);
}


RGLAYOUTAPI Rectangle GuiLayoutRecEx(float main, float cross, float pl, float pr, float pt, float pb, RGLVAlign valign)
{
    if (g_rgl.top <= 0)
    {
        // No active container - clear last rect and return zero rect
        g_last_rect = (Rectangle){0, 0, 0, 0};
        return g_last_rect;
    }

    RGLContainer *c = &g_rgl.stack[g_rgl.top - 1];
    Rectangle result;

    // PLANNED MODE: Use the layout plan to determine sizing
    if (c->plan.length > 0 && c->plan_index < c->plan.length)
    {
        float v = c->plan.sizes[c->plan_index];
        c->plan_index++;

        float main_px;
        if (v >= 20)
        {
            // Fixed pixel size
            main_px = v;
        }
        else if (v == -1)
        {
            // Fill remaining space (flex weight of 1)
            bool row = (c->axis == RGL_AXIS_ROW);
            float avail = row ? c->inner.width : c->inner.height;

            float space_for_flex = avail - c->plan_sum_fixed - c->plan_gap_total;
            if (space_for_flex < 0)
                space_for_flex = 0;

            if (c->plan_sum_weight > 0)
            {
                float weight_ratio = 1.0f / c->plan_sum_weight;
                main_px = weight_ratio * space_for_flex;
            }
            else
            {
                main_px = space_for_flex;
            }
        }
        else if (v > 0 && v < 20)
        {
            // Flex weight (0-19)
            bool row = (c->axis == RGL_AXIS_ROW);
            float avail = row ? c->inner.width : c->inner.height;

            float space_for_flex = avail - c->plan_sum_fixed - c->plan_gap_total;
            if (space_for_flex < 0)
                space_for_flex = 0;

            if (c->plan_sum_weight > 0)
            {
                float weight_ratio = v / c->plan_sum_weight;
                main_px = weight_ratio * space_for_flex;
            }
            else
            {
                main_px = 0;
            }
        }
        else
        {
            // Values < -1 are ignored, treated as 0 size
            main_px = 0;
        }

        result = take_rect(c, main_px, cross);
    }
    else
    {
        // AD-HOC MODE
        float main_px = (main < 0) ? c->remaining : main;
        result = take_rect(c, main_px, cross);
    }

    // Apply vertical alignment before padding
    if (valign != RGL_VALIGN_NONE && c->axis == RGL_AXIS_ROW)
    {
        float container_height = c->inner.height;
        float element_height = result.height;

        if (element_height < container_height)
        {
            switch (valign)
            {
            case RGL_VALIGN_TOP:
                // Already at top (default behavior)
                break;
            case RGL_VALIGN_CENTER:
            {
                float offset = (container_height - element_height) / 2;
                result.y += offset;
                break;
            }
            case RGL_VALIGN_BOTTOM:
            {
                float offset = container_height - element_height;
                result.y += offset;
                break;
            }
            case RGL_VALIGN_NONE:
                // No alignment
                break;
            }
        }
    }

    // Apply padding if specified
    if (pl != 0 || pt != 0 || pr != 0 || pb != 0)
    {
        result.x += pl;
        result.y += pt;
        result.width -= (pl + pr);
        result.height -= (pt + pb);

        // Clamp to prevent negative sizes
        if (result.width < 0)
            result.width = 0;
        if (result.height < 0)
            result.height = 0;
    }

    // Save last produced rectangle
    g_last_rect = result;

    return result;
}

// New: return copy of last produced rectangle
RGLAYOUTAPI Rectangle GuiLayoutRecLast(void)
{
    return g_last_rect;
}

RGLAYOUTAPI Rectangle GuiLayoutPanel(float main, float cross, float pad_top, float pad_other, RGLPad *out_pad)
{
    Rectangle r = GuiLayoutRec(main, cross);
    if (out_pad != NULL)
    {
        *out_pad = (RGLPad){pad_other, pad_top, pad_other, pad_other};
    }
    return r;
}

// ===== PLAN BUILDER FUNCTIONS =====

RGLAYOUTAPI RGLPlan GuiPlanCreate(const float *sizes, int length)
{
    RGLPlan plan = {0};
    plan.length = 0;
    plan.gap = 0;
    plan.pad = (RGLPad){0, 0, 0, 0};
    plan.has_gap = false;
    plan.has_pad = false;

    if (sizes != NULL && length > 0 && length <= RGL_PLAN_MAX)
    {
        for (int i = 0; i < length; i++)
        {
            plan.sizes[i] = sizes[i];
        }
        plan.length = length;
    }

    return plan;
}

RGLAYOUTAPI void GuiPlanAdd(RGLPlan *plan, float size)
{
    if (plan != NULL && plan->length < RGL_PLAN_MAX)
    {
        plan->sizes[plan->length] = size;
        plan->length++;
    }
}

RGLAYOUTAPI void GuiPlanAddRepeat(RGLPlan *plan, float size, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (plan != NULL && plan->length < RGL_PLAN_MAX)
        {
            plan->sizes[plan->length] = size;
            plan->length++;
        }
    }
}


RGLAYOUTAPI void GuiPlanSetGap(RGLPlan *plan, float gap)
{
    if (plan != NULL)
    {
        plan->gap = gap;
        plan->has_gap = true;
    }
}

RGLAYOUTAPI void GuiPlanSetPad(RGLPlan *plan, RGLPad pad)
{
    if (plan != NULL)
    {
        plan->pad = pad;
        plan->has_pad = true;
    }
}

RGLAYOUTAPI void GuiPlanSetPadAll(RGLPlan *plan, float pad)
{
    if (plan != NULL)
    {
        plan->pad = (RGLPad){pad, pad, pad, pad};
        plan->has_pad = true;
    }
}

#endif // RGLAYOUT_IMPLEMENTATION
