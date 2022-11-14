/*****************************************************************
ファイル名	: server_command.c
機能		: サーバーのコマンド処理
*****************************************************************/

#include "server_common.h"
#include "server_func.h"

static void SetIntData2DataBlock(void *data, int intData, int *dataSize);
static void SetCharData2DataBlock(void *data, char charData, int *dataSize);
static int GetRandomInt(int n);
static void SetJankenJUDGE(void);

int janken1 = -1; //判定に使用する変数(クライアント1)
int janken2 = -1; //判定に使用する変数(クライアント2)

//じゃんけんの勝敗を判定し、クライアントそれぞれにコマンド(勝ちか、負けか)を送信
void SetJankenJUDGE(void)
{
	unsigned char data[MAX_DATA];

	int dataSize, intData;

	dataSize = 0;

	if (((janken1 + janken2) == 0) || ((janken1 + janken2) == 2) || ((janken1 + janken2) == 6)) //引き分けの時
	{
		/* コマンドのセット */
		SetCharData2DataBlock(data, JUDGE_GOBU, &dataSize);

		/* すべてのクライアントに送る */
		SendData(ALL_CLIENTS, data, dataSize);
	}
	else if ( (janken1 + janken2) == 1 ) //グーとチョキの組み合わせの場合
	{
		if (janken2 == GU_COMMAND) //クライアント2がグーなら
		{ 
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
		}
		else //クライアント1がグーなら
		{
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
		}
	}
	else if ((janken1 + janken2) == 3) //グーとパーの組み合わせの場合
	{
		if (janken1 == GU_COMMAND) //クライアント1がグーなら
		{
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
		}
		else //クライアント2がグーなら
		{
			printf("クライアント2がグー");
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
		}
	}else //チョキとパーの組合わせの場合
	{
		if (janken2 == TYOKI_COMMAND) //クライアント2がチョキなら
		{
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
		}
		else //クライアント1がチョキなら
		{
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_WIN, &dataSize); //勝ち
			/* クライアント1に送る */
			SendData(CLIENT_1, data, dataSize);
			dataSize = 0;
			/* コマンドのセット */
			SetCharData2DataBlock(data, JUDGE_LOSE, &dataSize); //負け
			/* クライアント2に送る */
			SendData(CLIENT_2, data, dataSize);
		}
	}
	//じゃんけんのリセット
	janken1 = -1;
	janken2 = -1;
}

/*****************************************************************
関数名	: ExecuteCommand
機能	: クライアントから送られてきたコマンドを元に，
		  引き数を受信し，実行する
引数	: char	command		: コマンド
		  int	pos			: コマンドを送ったクライアント番号
出力	: プログラム終了コマンドが送られてきた時には0を返す．
		  それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command, int pos)
{
	unsigned char data[MAX_DATA];
	int dataSize, intData;
	int endFlag = 1;

	/* 引き数チェック */
	assert(0 <= pos && pos < MAX_CLIENTS);

#ifndef NDEBUG
	printf("#####\n");
	printf("ExecuteCommand()\n");
	printf("Get command %c\n", command);
#endif
	switch (command)
	{
	case END_COMMAND:
		dataSize = 0;
		/* コマンドのセット */
		SetCharData2DataBlock(data, command, &dataSize);

		/* 全ユーザーに送る */
		SendData(ALL_CLIENTS, data, dataSize);

		endFlag = 0;
		break;
	case GU_COMMAND:
		/* クライアント番号を受信する */
		RecvIntData(pos, &intData);
		if (pos == 0) //クライアント1なら
		{
			janken1 = GU_COMMAND; //じゃんけんグーを格納
		}
		else if (pos == 1) //クライアント2なら
		{
			janken2 = GU_COMMAND; //じゃんけんグーを格納
		}
		if ((0 <= janken1) && (0 <= janken2)) //クライアントがそれぞれ、じゃんけんを選択し終えたら
		{
			SetJankenJUDGE();
		}
		break;
	case TYOKI_COMMAND:
		/* クライアント番号を受信する */
		RecvIntData(pos, &intData);
		if (pos == 0)
		{
			janken1 = TYOKI_COMMAND;

		}
		else if (pos == 1)
		{
			janken2 = TYOKI_COMMAND;
		}
		if ((0 <= janken1) && (0 <= janken2))
		{
			dataSize = 0;
			SetJankenJUDGE();
		}
		break;
	case PA_COMMAND:
		/* クライアント番号を受信する */
		RecvIntData(pos, &intData);
		if (pos == 0)
		{
			janken1 = PA_COMMAND;
		}
		else if (pos == 1)
		{
			janken2 = PA_COMMAND;
		}
		if ((0 <= janken1) && (0 <= janken2))
		{
			dataSize = 0;
			SetJankenJUDGE();
		}
		break;
	default:
		/* 未知のコマンドが送られてきた */
		fprintf(stderr, "0x%02x is not command!\n", command);
	}
	return endFlag;
}


/*****
static
*****/
/*****************************************************************
関数名	: SetIntData2DataBlock
機能	: int 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetIntData2DataBlock(void *data, int intData, int *dataSize)
{
	int tmp;

	/* 引き数チェック */
	assert(data != NULL);
	assert(0 <= (*dataSize));

	tmp = htonl(intData);

	/* int 型のデータを送信用データの最後にコピーする */
	memcpy(data + (*dataSize), &tmp, sizeof(int));
	/* データサイズを増やす */
	(*dataSize) += sizeof(int);
}

/*****************************************************************
関数名	: SetCharData2DataBlock
機能	: char 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetCharData2DataBlock(void *data, char charData, int *dataSize)
{
	/* 引き数チェック */
	assert(data != NULL);
	assert(0 <= (*dataSize));

	/* int 型のデータを送信用データの最後にコピーする */
	*(char *)(data + (*dataSize)) = charData;
	/* データサイズを増やす */
	(*dataSize) += sizeof(char);
}

/*****************************************************************
関数名	: GetRandomInt
機能	: 整数の乱数を得る
引数	: int		n	: 乱数の最大値
出力	: 乱数値
*****************************************************************/
static int GetRandomInt(int n)
{
	return rand() % n;
}
