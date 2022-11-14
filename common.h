/*****************************************************************
ファイル名	: common.h
機能		: サーバーとクライアントで使用する定数の宣言を行う
*****************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>

#define PORT (u_short)8888  /* ポート番号 */

#define MAX_CLIENTS 4       /* クライアント数の最大値 */
#define MAX_NAME_SIZE 10    /* ユーザー名の最大値*/

#define MAX_DATA 200        /* 送受信するデータの最大値 */

#define END_COMMAND 'E'     /* プログラム終了コマンド */
#define DIAMOND_COMMAND 'D' /* 菱形表示コマンド */
#define GU_COMMAND 0        /* グーを示すコマンド */
#define TYOKI_COMMAND 1     /* チョキを示すコマンド */
#define PA_COMMAND 3        /* パーを示すコマンド */
#define JUDGE_WIN 'W'       /* 勝ちを示すコマンド */
#define JUDGE_LOSE 'L'      /* 負けを示すコマンド */
#define JUDGE_GOBU 'G'      /* 引き分けを示すコマンド */
#define JUDGE_HIT 'H'       /* ヒットを示すコマンド */
#define JUGE_TWOBASE 'T'    /* ツーベースを示すコマンド */
#define JUGE_HOMERUN 'R'    /* ホームランを示すコマンド */

#endif
