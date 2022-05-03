#include "word_learner.h"
#include "words.h"

#define FASTUI_UPDATE_DELAY 150
#define MODE_COUNT 2

struct regmenu_ screen_data = {55, 1, 0, touch_handler, keypress_handler, update_screen, 0, init_app, 0, 0};

struct context** ctx_p;
struct context* ctx;

int main(int return_screen, char** argv)
{
    init_app((void*)return_screen);
}

void init_app(void* return_screen)
{
#ifdef BipEmulator
    ctx_p = (context**)get_ptr_temp_buf_2();
    _srand(0);
#else
    ctx_p = get_ptr_temp_buf_2();
#endif

    if ((return_screen == *ctx_p) && get_var_menu_overlay())
    {
        ctx = *ctx_p;
#ifdef BipEmulator
        *ctx_p = (context*)NULL;
#else
        *ctx_p = NULL;
#endif
        reg_menu(&screen_data, 0);
        *ctx_p = ctx;
    }
    else
    {
        reg_menu(&screen_data, 0);

        *ctx_p = (struct context*)pvPortMalloc(sizeof(struct context));
        ctx = *ctx_p;
        _memclr(ctx, sizeof(struct context));

#ifdef BipEmulator
        ctx->proc = (Elf_proc_*)return_screen;
#else
        ctx->proc = return_screen;
#endif

        if (return_screen && ctx->proc->elf_finish)
        {
            ctx->ret_f = ctx->proc->elf_finish;
        }
        else
        {
            ctx->ret_f = show_watchface;
        }

        initial_values();
    }

    show_screen();
}

void initial_values() {}

void show_screen()
{
    load_font();

    set_display_state_value(8, 1);

    draw_screen();
}

void keypress_handler()
{
    set_display_state_value(8, 0);
    show_menu_animate(ctx->ret_f, (unsigned int)init_app, ANIMATE_RIGHT);
};

void update_screen()
{
    draw_screen();

    repaint_screen_lines(1, 176);

    set_update_period(1, FASTUI_UPDATE_DELAY);
}

void next_state(int* word_index)
{
    _srand(get_tick_count());
    int r = (*word_index);
    while (r == (*word_index))
    {
        r = _rand() % WORD_COUNT;
    }
    (*word_index) = r;
}

int touch_handler(void* param)
{
#ifdef BipEmulator
    struct gesture_* gest = (gesture_*)param;
#else
    struct gesture_* gest = param;
#endif
    int result = 0;

    switch (gest->gesture)
    {
        case GESTURE_CLICK: {
            if (ctx->mode < MODE_COUNT)
                ++ctx->mode;
            else
                ctx->mode = 0;
            draw_screen();
            repaint_screen_lines(1, 176);
            break;
        };
        case GESTURE_SWIPE_RIGHT: {
            break;
        };
        case GESTURE_SWIPE_LEFT: {
            break;
        };
        case GESTURE_SWIPE_UP: {
            next_state(&ctx->currentScreen);
            show_menu_animate(draw_screen, 0, ANIMATE_UP);
#ifdef BipEmulator
            draw_screen();
            repaint_screen_lines(1, 176);
#endif
            break;
        };
        case GESTURE_SWIPE_DOWN: {
            next_state(&ctx->currentScreen);
            show_menu_animate(draw_screen, 0, ANIMATE_DOWN);

#ifdef BipEmulator
            draw_screen();
            repaint_screen_lines(1, 176);
#endif
            break;
        };
        default:
            break;
    }
    return result;
};

void draw_screen()
{
    set_bg_color(COLOR_BLACK);
    set_fg_color(COLOR_WHITE);
    fill_screen_bg();

    if (ctx->mode == 0)
    {
        text_out_center(words[ctx->currentScreen * 3], VIDEO_X / 2, VIDEO_Y / 8);
        text_out_center(words[ctx->currentScreen * 3 + 1], VIDEO_X / 2, VIDEO_Y / 4);
        draw_horizontal_line(VIDEO_Y / 2, 0, VIDEO_X);
        text_out_center(words[ctx->currentScreen * 3 + 2], VIDEO_X / 2, VIDEO_Y * 3 / 4);
    }
    else if (ctx->mode == 1)
    {
        text_out_center(words[ctx->currentScreen * 3], VIDEO_X / 2, VIDEO_Y / 3);
        text_out_center(words[ctx->currentScreen * 3 + 1], VIDEO_X / 2, VIDEO_Y / 3 + 22);
    }
    else if (ctx->mode == 2)
    {
        text_out_center(words[ctx->currentScreen * 3], VIDEO_X / 2, VIDEO_Y / 2 - 15);
    }

    repaint_screen_lines(0, 176);
};