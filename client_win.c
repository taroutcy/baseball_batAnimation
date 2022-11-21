/*****************************************************************
ファイル名	: client_win.c
機能		: クライアントのユーザーインターフェース処理
*****************************************************************/

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <joyconlib.h>  //Joy-Conを動かすために必要なヘッダファイルをインクルード
#include <time.h>

#include "client_func.h"
#include "common.h"

static SDL_Window *gMainWindow;
static SDL_Rect gButtonRect[MAX_CLIENTS + 2];
static int CheckButtonNO(int x, int y, int num);

// 野球の描写および打つ判定の関数
void *draw(void *param);
void ball_straight(SDL_Renderer *renderer, ball_param ball);
int judge_swing(int flg_swing, SDL_Point pos_ball, SDL_Rect rect_bat);
int judge_strike(int flg_swing, ball_param ball, SDL_Rect rect_bat);

void *animeBatter(int i);

// 野球関連
int flg_swing = 0;
SDL_Rect strike_zone = {350, 300, 100, 100};

// 画像関連変数
SDL_Texture *texture2, *texture3, *texture4;
SDL_Surface *image, *image2, *image3, *image4;

SDL_Surface *bat[3] = {NULL, NULL, NULL};
int img_num[3] = {50, 40, 25};  // 分割する画像の枚数
SDL_Texture *img_texture[3];

// JoyConの状態を格納する変数
joyconlib_t jc;

// 一度だけアニメーションを動作させるための鍵 0:ストップ, 1：実行
int Batter_key = 0;

// バッターがどの速度でスイングするのかを示す. Speedが0, 1, 2はそれぞれ遅い, 普通, 早い
int Batter_Speed = 0;
int Batter_Speed_back = -1;

/* 勝敗カウント用変数 */
char winstr[64];
char winstr2[64];
int win = 1;       // 勝ちの数値を格納するための変数
int win_back = 1;  // 一つ前の数値を格納するための変数
char lostr[64];
char lostr2[64];
int lo = 1;       // 引き分けの数値を格納するための変数
int lo_back = 1;  // 引き分けの一つ前の数値を格納するための変数

/*****************************************************************
関数名	: InitWindows
機能	: メインウインドウの表示，設定を行う
引数	: int	clientID		: クライアント番号
          int	num				: 全クライアント数
出力	: 正常に設定できたとき0，失敗したとき-1
*****************************************************************/
int InitWindows(int clientID, int num, char name[][MAX_NAME_SIZE]) {
    int i;
    SDL_Texture *texture;
    SDL_Rect src_rect;
    SDL_Rect dest_rect;
    char clientButton[4][6] = {"g.png", "t.png", "p.png", "3.jpg"};  // グー、チョキ、パーを読み込む
    char endButton[] = "END.png";
    char *s, title[10];

    /* 引き数チェック */
    assert(0 < num && num <= MAX_CLIENTS);

    /* SDLの初期化, Joyconの初期化 */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | IMG_INIT_PNG) < 0) {
        printf("failed to initialize SDL.\n");
        return -1;
    }

    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Event quit_event = {SDL_QUIT};  // 特定のイベント名を格納

    /*
    // SDL_mixerの初期化（MP3ファイルを使用）
    Mix_Init(MIX_INIT_MP3);

    // オーディオデバイスの初期化
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        printf("failed to initialize SDL_mixer.\n");
        SDL_Quit();
        exit(-1);
    }
    // BGMと効果音のサウンドファイルの読み込み
    if ((music = Mix_LoadMUS("newbgm1.mp3")) == NULL || (chunk = Mix_LoadWAV("guiterA_ac.wav")) == NULL || (chunk2 = Mix_LoadWAV("guiterB_ac.wav")) == NULL || (chunk3 = Mix_LoadWAV("guiterC_ac.wav")) == NULL || (chunk4 = Mix_LoadWAV("guiterD_ac.wav")) == NULL || (chunk5 = Mix_LoadWAV("guiterE_ac.wav")) == NULL || (chunk6 = Mix_LoadWAV("guiterF_ac.wav")) == NULL || (chunk7 = Mix_LoadWAV("guiterA_el.wav")) == NULL || (chunk8 = Mix_LoadWAV("guiterB_el.wav")) == NULL || (chunk9 = Mix_LoadWAV("guiterC_el.wav")) == NULL || (chunk10 = Mix_LoadWAV("guiterD_el.wav")) == NULL || (chunk11 = Mix_LoadWAV("guiterE_el.wav")) == NULL || (chunk12 = Mix_LoadWAV("guiterF_el.wav")) == NULL || (music2 = Mix_LoadMUS("ゲームクリア.mp3")) == NULL || (music3 = Mix_LoadMUS("ゲーム失敗.mp3")) == NULL)
    {
        printf("failed to load music and chunk.\n");
        Mix_CloseAudio(); // オーディオデバイスの終了
        SDL_Quit();
        exit(-1);
    }*/

    /* メインのウインドウを作成する */
    if ((gMainWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 450, 0)) == NULL) {
        printf("failed to initialize videomode.\n");
        return -1;
    }

    gMainRenderer = SDL_CreateRenderer(gMainWindow, -1, SDL_RENDERER_SOFTWARE);  // SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC);//0);

    /* ウインドウのタイトルをセット */
    sprintf(title, "client%d", clientID + 1);
    SDL_SetWindowTitle(gMainWindow, title);

    /* 背景を白にする */
    SDL_SetRenderDrawColor(gMainRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gMainRenderer);

    // BGMを流す
    // Mix_PlayMusic(music, -1);

    SDL_JoystickID joyid[2] = {};
    // 接続されているジョイスティックの名前を表示
    for (int i = 0; i < SDL_NumJoysticks(); i++) {  // 接続されているジョイスティックの数だけ繰り返す
        SDL_Joystick *joy = SDL_JoystickOpen(i);    // ジョイスティックを開く

        if (strstr(SDL_JoystickName(joy), "Right Joy-Con")) {
            if (strstr(SDL_JoystickName(joy), "IMU")) {
                joyid[1] = SDL_JoystickInstanceID(joy);
            } else {
                joyid[0] = SDL_JoystickInstanceID(joy);
            }
        }
    }

    // joycon接続しないとエラーするのでJoyCon処理しない方はコメントアウトのままで

    if (joycon_open(&jc, JOYCON_R)) {
        printf("joycon open failed.\n");
        return -1;
    }

    // バットの画像読み込み
    bat[0] = IMG_Load("bat/slow.png");
    bat[1] = IMG_Load("bat/normal.png");
    bat[2] = IMG_Load("bat/fast.png");

    img_texture[0] = SDL_CreateTextureFromSurface(gMainRenderer, bat[0]);
    img_texture[1] = SDL_CreateTextureFromSurface(gMainRenderer, bat[1]);
    img_texture[2] = SDL_CreateTextureFromSurface(gMainRenderer, bat[2]);

    if (!bat[0] || !bat[1] || !bat[2]) {
        printf("img not read\n");
    }

    stringColor(gMainRenderer, 380, 20, "Win:", 0xffffffff);       // 勝ち数を表示
    stringColor(gMainRenderer, 350, 40, "HIKIWAKE:", 0xffffffff);  // 引き分け数を表示
    stringColor(gMainRenderer, 380, 30, "Lose:", 0xffffffff);      // 負け数を表示
    SDL_RenderPresent(gMainRenderer);                              // 描写
}

// クライアントの勝敗の結果を画面に表示する
void Present(int i) {
    // 白色で塗りつぶす
    boxColor(gMainRenderer, 50, 65, 380, 400, 0xffffffff);  //

    // ツーベースの時
    if (i == 2) {
        sprintf(winstr, "Win:  %d", win);
        sprintf(winstr2, "Win:  %d", win_back);
        stringColor(gMainRenderer, 380, 20, winstr2, 0xffffffff);  // 一つ前の数値を削除
        stringColor(gMainRenderer, 380, 20, winstr, 0xff000000);   // 勝ち数を表示
        SDL_RenderPresent(gMainRenderer);
        win_back = win;
        win = win + 1;
    }
}

/*****************************************************************
関数名	: DestroyWindow
機能	: SDLを終了する
引数	: なし
出力	: なし
*****************************************************************/
void DestroyWindow(void) {
    SDL_Quit();
}

Uint32 draw_timer(Uint32 interval, void *param) {
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

// 割り込みで呼び出す描写関数
void *draw(void *param) {  // 描画関数
    static ball_param ball = {400, 100, 10, 0, 10};
    static SDL_Rect rect_bat = {350, 300, 100, 100};
    static SDL_Point pos_ball;
    static int flg_reverse = 0;

    gMainRenderer = (SDL_Renderer *)param;

    // 判定用定数に格納
    pos_ball.x = ball.x;
    pos_ball.y = ball.y;

    if (judge_swing(flg_swing, pos_ball, rect_bat)) {
        ball.yp = -10;
    }
    ball.y = ball.y + ball.yp;  // 座標の更新

    if (judge_strike(flg_swing, ball, rect_bat)) {  // ストライク判定(仮)
        printf("ストライク\n");
    }

    /* 描画 */
    SDL_SetRenderDrawColor(gMainRenderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(gMainRenderer, &rect_bat);  // バット(枠)の描画

    filledCircleColor(gMainRenderer, ball.x, ball.y, ball.r, 0xffffffff);
    SDL_RenderPresent(gMainRenderer);  // ボールの描画
}

int judge_swing(int flg_swing, SDL_Point pos_ball, SDL_Rect rect_bat) {
    if (flg_swing && SDL_PointInRect(&pos_ball, &rect_bat))
        return 1;
    else
        return 0;
}

int judge_strike(int flg_swing, ball_param ball, SDL_Rect rect_bat) {
    if (ball.y == WINDOW_Y && (ball.x >= (strike_zone.x) || ball.x <= (strike_zone.x + strike_zone.w)))
        return 1;  // TODO&DISCUSS:ボール球の追加
    else
        return 0;
}

Uint32 draw_timer_bat(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    /* コールバックでSDL_USEREVENTイベントをキューに入れる。
    このコールバック関数は一定の周期で再び呼ばれる */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = &animeBatter;
    userevent.data2 = param;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

// バッターアニメーションを流す
void *animeBatter(int i) {
    static int count_disp = 0;  // 表示した回数

    SDL_Surface *img = NULL;
    int img_size = 1000;  // 一枚の画像の縦と横の大きさ

    if (Batter_key == 1) {
        SDL_Rect imgRect = (SDL_Rect){img_size * count_disp, 0, img_size, img_size};
        SDL_Rect drawRect = (SDL_Rect){200, 200, 200, 200};

        SDL_RenderCopy(gMainRenderer, img_texture[0], &imgRect, &drawRect);
        SDL_RenderPresent(gMainRenderer);

        count_disp++;

        if (count_disp == img_num[0]) {
            Batter_key = 0;
        }
    }
}

// どのバッターのスイングアニメーションを流すのかを判断する
void animeBatter_JUDGE(void) {
    // バッターアニメーションは一度目か
    if (Batter_key == 1) {
        Batter_key = 0;  // 連続でスイングできないように初期化
        // 遅いスイングアニメーションを流す
        if (Batter_Speed == 0) {
            printf("遅い");
            animeBatter(0);
        }
        // 普通のスイングアニメーションを流す
        if (Batter_Speed == 1) {
            printf("普通");
            animeBatter(1);
        }
        // 早いスイングアニメーションを流す
        if (Batter_Speed == 2) {
            printf("早い");
            animeBatter(2);
        }
    }
}

// どのピッチャーアニメーションを流すのかを判断する
void animePitya_JUDGE(void) {
}

/*****************************************************************
関数名	: WindowEvent
機能	: メインウインドウに対するイベント処理を行う
引数	: int		num		: 全クライアント数
出力	: なし
*****************************************************************/
void WindowEvent(int num, int clientID) {
    SDL_Event event;
    SDL_MouseButtonEvent *mouse;
    int buttonNO;

    joycon_get_state(&jc);
    /* 引き数チェック */
    assert(0 < num && num <= MAX_CLIENTS);

    SDL_SetRenderDrawColor(gMainRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gMainRenderer);  // 背景の描画

    // SDL_Surface *img = NULL;
    // int img_size = 1000;  // 一枚の画像の縦と横の大きさ

    // SDL_Rect imgRect = (SDL_Rect){0, 0, img_size, img_size};
    // SDL_Rect drawRect = (SDL_Rect){0, 0, 100, 100};

    // SDL_RenderCopy(gMainRenderer, img_texture[0], &imgRect, &drawRect);

    // イベントを取得したなら
    while (SDL_PollEvent(&event)) {
        // イベントの種類ごとに処理
        switch (event.type) {
                /* タイマー処理 */
            case SDL_USEREVENT: {
                void (*p)(void *) = event.user.data1;
                p(event.user.data2);
                break;
            }
            case SDL_KEYDOWN:  // キーボードのキーが押された時
                // 押されたキーごとに処理
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:  // Enterキーが押された時
                        printf("enter\n");
                        flg_swing = 1;
                        Batter_key = 1;
                        break;
                    case SDLK_ESCAPE:
                        SendEndCommand();
                        break;
                }
                break;
            // ジョイコンの加速度設定
            case SDL_JOYAXISMOTION:
                // もし打者ならば
                if (clientID == 0) {
                    if (jc.axis[2].acc_y > 40.0 || jc.axis[2].acc_y < -40.0) {
                        printf("大");
                        flg_swing = 1;
                        Batter_Speed = 2;
                        animeBatter_JUDGE();

                    } else if (jc.axis[2].acc_y > 27.0 || jc.axis[2].acc_y < -27.0) {
                        printf("中");
                        flg_swing = 1;
                        Batter_Speed = 1;
                        animeBatter_JUDGE();

                    } else if (jc.axis[2].acc_y > 20.0 || jc.axis[2].acc_y < -20.0) {
                        printf("小");
                        flg_swing = 1;
                        Batter_Speed = 0;
                        animeBatter_JUDGE();
                    }
                }
                // もし投手ならば
                if (clientID == 1) {
                    if (jc.axis[2].acc_y > 60.0 || jc.axis[2].acc_y < -60.0) {
                        // printf("大");
                        Batter_Speed = 2;
                        animeBatter_JUDGE();
                    } else if (jc.axis[2].acc_y > 40.0 || jc.axis[2].acc_y < -40.0) {
                        // printf("中");
                        Batter_Speed = 1;
                        animeBatter_JUDGE();
                    } else if (jc.axis[2].acc_y > 20.0 || jc.axis[2].acc_y < -20.0) {
                        // printf("小");
                        Batter_Speed = 0;
                        animeBatter_JUDGE();
                    }
                }
                break;
            // ジョイスティックのボタンが押された時
            case SDL_JOYBUTTONDOWN:
                // ジョイコンのボタンと対応したギターコードを挿入
                if (jc.button.btn.A == 1) {
                    printf("\n%d\n", Batter_Speed);
                    Batter_Speed = 0;  // リセット
                    Batter_Speed_back = -1;
                    Batter_key = 1;
                }
                if (jc.button.btn.B == 1) {
                }
                break;
            // SDLの利用を終了する時（イベントキューにSDL_QUITイベントが追加された時）
            case SDL_QUIT:
                SendEndCommand();
                break;
        }
    }

    SDL_Delay(10);
}

/*****
static
*****/
/*****************************************************************
関数名	: CheckButtonNO
機能	: クリックされたボタンの番号を返す
引数	: int	   x		: マウスの押された x 座標
          int	   y		: マウスの押された y 座標
          char	   num		: 全クライアント数
出力	: 押されたボタンの番号を返す
          ボタンが押されていない時は-1を返す
*****************************************************************/
static int CheckButtonNO(int x, int y, int num) {
    int i;

    for (i = 0; i < num + 2; i++) {
        if (gButtonRect[i].x < x &&
            gButtonRect[i].y < y &&
            gButtonRect[i].x + gButtonRect[i].w > x &&
            gButtonRect[i].y + gButtonRect[i].h > y) {
            return i;
        }
    }
    return -1;
}
