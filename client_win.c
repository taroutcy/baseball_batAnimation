/*****************************************************************
ファイル名	: client_win.c
機能		: クライアントのユーザーインターフェース処理
*****************************************************************/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <joyconlib.h> //Joy-Conを動かすために必要なヘッダファイルをインクルード
#include <time.h>
#include "common.h"
#include "client_func.h"

static SDL_Window *gMainWindow;
static SDL_Renderer *gMainRenderer;
static SDL_Rect gButtonRect[MAX_CLIENTS + 2];

static int CheckButtonNO(int x, int y, int num);

SDL_Texture *texture2, *texture3, *texture4;
SDL_Surface *image, *image2, *image3, *image4;

clock_t begin, end;

joyconlib_t jc;

//一度だけアニメーションを動作させるための鍵 0:ストップ, 1：実行
int Batter_key = 1;

//時間設定
clock_t begin, end;

//バッターがどの速度でスイングするのかを示す. Speedが0, 1, 2はそれぞれ遅い, 普通, 早い
int Batter_Speed = 0;
int Batter_Speed_back = -1;

/* 勝敗カウント用変数 */
char winstr[64];
char winstr2[64];
int win = 1;	  // 勝ちの数値を格納するための変数
int win_back = 1; // 一つ前の数値を格納するための変数
char lostr[64];
char lostr2[64];
int lo = 1;		 // 引き分けの数値を格納するための変数
int lo_back = 1; // 引き分けの一つ前の数値を格納するための変数

/*****************************************************************
関数名	: InitWindows
機能	: メインウインドウの表示，設定を行う
引数	: int	clientID		: クライアント番号
		  int	num				: 全クライアント数
出力	: 正常に設定できたとき0，失敗したとき-1
*****************************************************************/
int InitWindows(int clientID, int num, char name[][MAX_NAME_SIZE])
{
	int i;
	SDL_Texture *texture;
	SDL_Rect src_rect;
	SDL_Rect dest_rect;
	char clientButton[4][6] = {"g.png", "t.png", "p.png", "3.jpg"}; //グー、チョキ、パーを読み込む
	char endButton[] = "END.png";
	char *s, title[10];

	/* 引き数チェック */
	assert(0 < num && num <= MAX_CLIENTS);

	/* SDLの初期化, Joyconの初期化 */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("failed to initialize SDL.\n");
		return -1;
	}

	SDL_Event quit_event = {SDL_QUIT}; // 特定のイベント名を格納

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
	if ((gMainWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 300, 0)) == NULL)
	{
		printf("failed to initialize videomode.\n");
		return -1;
	}

	gMainRenderer = SDL_CreateRenderer(gMainWindow, -1, SDL_RENDERER_SOFTWARE); // SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC);//0);

	/* ウインドウのタイトルをセット */
	sprintf(title, "client%d", clientID + 1);
	SDL_SetWindowTitle(gMainWindow, title);

	/* 背景を白にする */
	SDL_SetRenderDrawColor(gMainRenderer, 255, 255, 255, 255);
	SDL_RenderClear(gMainRenderer);

	// BGMを流す
	// Mix_PlayMusic(music, -1);

	SDL_JoystickID joyid[2] = {};
	// 接続されているジョイスティックの名前を表示
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{											 // 接続されているジョイスティックの数だけ繰り返す
		SDL_Joystick *joy = SDL_JoystickOpen(i); // ジョイスティックを開く

		if (strstr(SDL_JoystickName(joy), "Right Joy-Con"))
		{
			if (strstr(SDL_JoystickName(joy), "IMU"))
			{
				joyid[1] = SDL_JoystickInstanceID(joy);
			}
			else
			{
				joyid[0] = SDL_JoystickInstanceID(joy);
			}
		}
	}

	// joycon接続しないとエラーするのでJoyCon処理しない方はコメントアウトのままで

	if (joycon_open(&jc, JOYCON_R))
	{
		printf("joycon open failed.\n");
		return -1;
	}

	/* ボタンの作成 */
	for (i = 0; i < num + 2; i++)
	{
		gButtonRect[i].x = 20 + 80 * i;
		gButtonRect[i].y = 10;
		gButtonRect[i].w = 70;
		gButtonRect[i].h = 45;

		s = clientButton[i];

		if (i == num + 1)
		{
			s = endButton;
		}

		image = IMG_Load(s);
		texture = SDL_CreateTextureFromSurface(gMainRenderer, image);
		src_rect = (SDL_Rect){0, 0, image->w, image->h};
		SDL_RenderCopy(gMainRenderer, texture, &src_rect, (&gButtonRect[i]));
		SDL_FreeSurface(image);
	}

	stringColor(gMainRenderer, 380, 20, "Win:", 0xff000000);	  //勝ち数を表示
	stringColor(gMainRenderer, 350, 40, "HIKIWAKE:", 0xff000000); //引き分け数を表示
	stringColor(gMainRenderer, 380, 30, "Lose:", 0xff000000);	  //負け数を表示

	SDL_RenderPresent(gMainRenderer); //描写

	image2 = IMG_Load("hikiwake.png");
	image3 = IMG_Load("kati.png");
	image4 = IMG_Load("make.png");

	if (!(image2 && image3 && image4))
	{ // いずれかの画像読み込みに失敗したら
		printf("failed to load image file: %s\n", IMG_GetError());
	}

	texture2 = SDL_CreateTextureFromSurface(gMainRenderer, image2);
	texture3 = SDL_CreateTextureFromSurface(gMainRenderer, image3);
	texture4 = SDL_CreateTextureFromSurface(gMainRenderer, image4);
}

//クライアントの勝敗の結果を画面に表示する
void Present(int i)
{
	SDL_Rect src_rect2 = {0, 0, image2->w, image2->h};
	SDL_Rect dest_rect2 = {120, 150, 200, 60};

	SDL_Rect src_rect3 = {0, 0, image3->w, image3->h};
	SDL_Rect dest_rect3 = {170, 140, 120, 60};

	SDL_Rect src_rect4 = {0, 0, image4->w, image4->h};
	SDL_Rect dest_rect4 = {170, 140, 120, 60};
	//白色で塗りつぶす
	boxColor(gMainRenderer, 50, 65, 380, 400, 0xffffffff);

	if (i == 2)
	{
		SDL_RenderCopy(gMainRenderer, texture3, &src_rect3, &dest_rect3); // テクスチャをレンダラーにコピー（設定のサイズで）
		sprintf(winstr, "Win:  %d", win);
		sprintf(winstr2, "Win:  %d", win_back);
		stringColor(gMainRenderer, 380, 20, winstr2, 0xffffffff); //一つ前の数値を削除
		stringColor(gMainRenderer, 380, 20, winstr, 0xff000000);  //勝ち数を表示
		SDL_RenderPresent(gMainRenderer);
		win_back = win;
		win = win + 1;
	}
	else if (i == 3)
	{
		SDL_RenderCopy(gMainRenderer, texture4, &src_rect4, &dest_rect4); // テクスチャをレンダラーにコピー（設定のサイズで）
		sprintf(lostr, "Lose: %d", lo);
		sprintf(lostr2, "Lose: %d", lo_back);
		stringColor(gMainRenderer, 380, 30, lostr2, 0xffffffff); //一つ前の数値を削除
		stringColor(gMainRenderer, 380, 30, lostr, 0xff000000);	 //負け数を表示
		SDL_RenderPresent(gMainRenderer);
		lo_back = lo;
		lo = lo + 1;
	}
}

/*****************************************************************
関数名	: DestroyWindow
機能	: SDLを終了する
引数	: なし
出力	: なし
*****************************************************************/
void DestroyWindow(void)
{
	SDL_Quit();
}

//どのバッターのスイングアニメーションを流すのかを判断する
void animeBatter_JUDGE(void)
{
	//バッターアニメーションは一度目か
	if (Batter_key == 1)
	{
		Batter_key = 0; //連続でスイングできないように初期化
		// 遅いスイングアニメーションを流す
		if (Batter_Speed == 0)
		{
			printf("遅い");
			animeBatter(0);
		}
		//普通のスイングアニメーションを流す
		if (Batter_Speed == 1)
		{
			printf("普通");
			animeBatter(1);
		}
		//早いスイングアニメーションを流す
		if (Batter_Speed == 2)
		{
			printf("早い");
			animeBatter(2);
		}
	}
}

//バッターアニメーションを流す
void animeBatter(int i){


}


//どのピッチャーアニメーションを流すのかを判断する
void animePitya_JUDGE(void)
{
}

/*****************************************************************
関数名	: WindowEvent
機能	: メインウインドウに対するイベント処理を行う
引数	: int		num		: 全クライアント数
出力	: なし
*****************************************************************/
void WindowEvent(int num, int clientID)
{
	SDL_Event event;
	SDL_MouseButtonEvent *mouse;
	int buttonNO;

	joycon_get_state(&jc);
	/* 引き数チェック */
	assert(0 < num && num <= MAX_CLIENTS);

	// イベントを取得したなら
	while (SDL_PollEvent(&event))
	{
		// イベントの種類ごとに処理
		switch (event.type)
		{
		case SDL_KEYDOWN: // キーボードのキーが押された時
			// 押されたキーごとに処理
			switch (event.key.keysym.sym)
			{
			case SDLK_RETURN: // Enterキーが押された時
				printf("enter\n");
				SendEndCommand();
				break;
			}
			break;
		// ジョイコンの加速度設定
		case SDL_JOYAXISMOTION:
			//もしプレイヤー1ならば
			if (clientID == 0)
			{
				if (jc.axis[2].acc_y > 40.0 || jc.axis[2].acc_y < -40.0)
				{
					printf("大");
					Batter_Speed = 2;
					animeBatter_JUDGE();
				}
				else if (jc.axis[2].acc_y > 27.0 || jc.axis[2].acc_y < -27.0)
				{
					printf("中");
					Batter_Speed = 1;
					animeBatter_JUDGE();
				}
				else if (jc.axis[2].acc_y > 20.0 || jc.axis[2].acc_y < -20.0)
				{
					printf("小");
					Batter_Speed = 0;
					animeBatter_JUDGE();
				}
			}
			//もしプレイヤー2ならば
			if (clientID == 1)
			{
				if (jc.axis[2].acc_y > 60.0 || jc.axis[2].acc_y < -60.0)
				{
					// printf("大");
					Batter_Speed = 2;
					animeBatter_JUDGE();
				}
				else if (jc.axis[2].acc_y > 40.0 || jc.axis[2].acc_y < -40.0)
				{
					// printf("中");
					Batter_Speed = 1;
					animeBatter_JUDGE();
				}
				else if (jc.axis[2].acc_y > 20.0 || jc.axis[2].acc_y < -20.0)
				{
					// printf("小");
					Batter_Speed = 0;
					animeBatter_JUDGE();
				}
			}
			break;
		// ジョイスティックのボタンが押された時
		case SDL_JOYBUTTONDOWN:
			//ジョイコンのボタンと対応したギターコードを挿入
			if (jc.button.btn.A == 1)
			{
				printf("\n%d\n", Batter_Speed);
				Batter_Speed = 0; //リセット
				Batter_Speed_back = -1;
				Batter_key = 1;
			}
			if (jc.button.btn.B == 1)
			{
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
static int CheckButtonNO(int x, int y, int num)
{
	int i;

	for (i = 0; i < num + 2; i++)
	{
		if (gButtonRect[i].x < x &&
			gButtonRect[i].y < y &&
			gButtonRect[i].x + gButtonRect[i].w > x &&
			gButtonRect[i].y + gButtonRect[i].h > y)
		{
			return i;
		}
	}
	return -1;
}
