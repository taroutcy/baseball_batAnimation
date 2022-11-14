#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_X 800
#define WINDOW_Y 450

typedef struct // ボール用構造体
{
    int x; // 座標
    int y;
    int r;  // 半径
    int xp; // 移動量(座標をどれだけ変化させるか)
    int yp;
} ball_param;

void *draw(void *param);
void ball_straight(SDL_Renderer *renderer, ball_param ball);
int judge_swing(int flg_swing, SDL_Point pos_ball, SDL_Rect rect_bat);
int judge_strike(int flg_swing, ball_param ball, SDL_Rect rect_bat);

Uint32 draw_timer(Uint32 interval, void *param);

int flg_swing = 0;
SDL_Rect strike_zone = {350, 300, 100, 100};

int main(int argc, char *argv[])
{
    SDL_Event event;
    SDL_Event quit_event = {SDL_QUIT};
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_TimerID my_timer_id;

    int quit_flg = 1;

    /* 初期化 */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    /* ウィンドウ作成 */
    window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 450, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        printf("Can not create window\n");
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Can not create renderer\n");
        exit(1);
    }
    /* 強制終了時に SDL_Quit() を呼ぶ */
    atexit(SDL_Quit);

    /* タイマー設定 */
    my_timer_id = SDL_AddTimer(33, draw_timer, (void *)renderer);

    /* イベントループ */
    while (quit_flg)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            /* タイマー処理 */
            case SDL_USEREVENT:
            {
                void (*p)(void *) = event.user.data1;
                p(event.user.data2);
                break;
            }

            /* 終了 */
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_RETURN:
                    flg_swing = 1;
                    break;
                case SDLK_ESCAPE:
                    SDL_PushEvent(&quit_event);
                }
                break;
            case SDL_QUIT:
                quit_flg = 0;
                break;
            }
        }
        /* イベントがない場合、少し待つ */
        SDL_Delay(50);
    }
    if (my_timer_id)
        SDL_RemoveTimer(my_timer_id);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

Uint32 draw_timer(Uint32 interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* コールバックでSDL_USEREVENTイベントをキューに入れる。
    このコールバック関数は一定の周期で再び呼ばれる */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = &draw;
    userevent.data2 = param;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

void *draw(void *param)
{ // 描画関数
    SDL_Renderer *renderer;
    static ball_param ball = {400, 100, 10, 0, 10};
    static SDL_Rect rect_bat = {350, 300, 100, 100};
    static SDL_Point pos_ball;
    static int flg_reverse = 0;

    renderer = (SDL_Renderer *)param;

    /* 描画 */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer); // 背景の描画

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &rect_bat); // バット(枠)の描画

    filledCircleColor(renderer, ball.x, ball.y, ball.r, 0xffffffff);
    SDL_RenderPresent(renderer); // ボールの描画

    // 判定用定数に格納
    pos_ball.x = ball.x;
    pos_ball.y = ball.y;

    if (judge_swing(flg_swing, pos_ball, rect_bat))
    {
        ball.yp = -10;
    }
    ball.y = ball.y + ball.yp; // 座標の更新

    if (judge_strike(flg_swing, ball, rect_bat))
    { // ストライク判定(仮)
        printf("ストライク\n");
    }
}

int judge_swing(int flg_swing, SDL_Point pos_ball, SDL_Rect rect_bat)
{
    if (flg_swing && SDL_PointInRect(&pos_ball, &rect_bat))
        return 1;
    else
        return 0;
}

int judge_strike(int flg_swing, ball_param ball, SDL_Rect rect_bat)
{
    if (ball.y == WINDOW_Y && (ball.x >= (strike_zone.x) || ball.x <= (strike_zone.x + strike_zone.w)))
        return 1; // TODO&DISCUSS:ボール球の追加
    else
        return 0;
}