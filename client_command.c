/*****************************************************************
ファイル名	: client_command.c
機能		: クライアントのコマンド処理
*****************************************************************/

#include "common.h"
#include "client_func.h"

static void SetIntData2DataBlock(void *data, int intData, int *dataSize);
static void SetCharData2DataBlock(void *data, char charData, int *dataSize);
static void RecvCircleData(void);
static void RecvRectangleData(void);
static void RecvDiamondData(void);

//サーバーからデータが送られてくるまでクライアントからデータを送らないようにするための変数
int key1 = 1;
int key2 = 1;

/*****************************************************************
関数名	: ExecuteCommand
機能	: サーバーから送られてきたコマンドを元に，
          引き数を受信し，実行する
引数	: char	command		: コマンド
出力	: プログラム終了コマンドがおくられてきた時には0を返す．
          それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command)
{
    int endFlag = 1;

#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n", command);
#endif
    //key変数のリセット
    key1 = 1;
    key2 = 1;
    switch (command)
    {
    case END_COMMAND:
        endFlag = 0;
        break;
    case JUDGE_HIT:
        Present(1);
        break;
    case JUDGE_TWOBASE:
        Present(2);
        break;
    case JUDGE_HOMERUN:
        Present(3);
        break;
    }
    return endFlag;
}

/*****************************************************************
関数名	: SendGu
機能	: サーバーにグー(データ)を送る
引数	: int pos	    : クライアント番号
出力	: なし
*****************************************************************/
void SendGu(int pos, int ClientID)
{
    unsigned char data[MAX_DATA];
    int dataSize,intData;

    /* 引き数チェック */
    assert(0 <= pos && pos < MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n", pos);
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, GU_COMMAND, &dataSize);
    /* クライアント番号のセット */
    SetIntData2DataBlock(data, pos, &dataSize);

    if ( (ClientID == 0) && (key1 == 1) )
    {
        /* データの送信 */
        SendData(data, dataSize);
        key1 = 0;    //停止
    }
    else if ((ClientID == 1) && (key2 == 1))
    {
        /* データの送信 */
        SendData(data, dataSize);
        key2 = 0; //停止
    }
}

/*****************************************************************
関数名	: SendTyoki
機能	: サーバーへチョキ(データ)を送る
引数	: int pos	    : クライアント番号
出力	: なし
*****************************************************************/
void SendTyoki(int pos, int ClientID)
{
    unsigned char data[MAX_DATA];
    int dataSize;

    /* 引き数チェック */
    assert(0 <= pos && pos < MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n", pos);
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, TYOKI_COMMAND, &dataSize);
    /* クライアント番号のセット */
    SetIntData2DataBlock(data, pos, &dataSize);

    if ((ClientID == 0) && (key1 == 1))
    {
        /* データの送信 */
        SendData(data, dataSize);
        key1 = 0; //停止
    }
    else if ((ClientID == 1) && (key2 == 1))
    {
        /* データの送信 */
        SendData(data, dataSize);
        key2 = 0; //停止
    }}

/*****************************************************************
関数名	: SendPa
機能	: サーバーにパー(データ)を送る
引数	: int pos	    : クライアント番号
出力	: なし
*****************************************************************/
void SendPa(int pos, int ClientID)
{
    unsigned char data[MAX_DATA];
    int dataSize;

    /* 引き数チェック */
    assert(0 <= pos && pos < MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n", pos);
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, PA_COMMAND, &dataSize);
    /* クライアント番号のセット */
    SetIntData2DataBlock(data, pos, &dataSize);

    if ((ClientID == 0) && (key1 == 1))
    {
        /* データの送信 */
        SendData(data, dataSize);
        key1 = 0; //停止
    }
    else if ((ClientID == 1) && (key2 == 1))
    {
        /* データの送信 */
        SendData(data, dataSize);
        key2 = 0; //停止
    }
}

/*****************************************************************
関数名	: SendEndCommand
機能	: プログラムの終了を知らせるために，
          サーバーにデータを送る
引数	: なし
出力	: なし
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char data[MAX_DATA];
    int dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data, END_COMMAND, &dataSize);

    /* データの送信 */
    SendData(data, dataSize);
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

    /* char 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}
