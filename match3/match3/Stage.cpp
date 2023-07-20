#include"Stage.h"
#include"DxLib.h"
#include"InputContorol.h"

/***********************************
* マクロ定義
************************************/
#define HEIGHT                (12)     //ブロック配置サイズ（高さ）
#define WIDTH                 (12)     //ブロック配置サイズ（幅）
#define BLOCKSIZE             (48)     //ブロックサイズ
#define BLOCK_IMAGE_MAX       (10)     //ブロック画像数

#define ITEM_MAX              (8)     //アイテム最大数

#define SELECT_CURSOR         (0)
#define NEXT_CURSOR           (1)
#define TMP_CURSOR            (2)

/***********************************
* 型定義
************************************/
typedef struct
{
	int flg;
	int x, y;
	int width, height;
	int image;
	int backup;
}T_Object;

typedef struct
{
	int x;
	int y;
}T_CURSOR;

enum
{
	E_NONE,
	I_ONSE,
	E_SECOND
};

/***********************************
* 変数宣言
************************************/
T_Object Block[HEIGHT][WIDTH];          //ブロックオブジェクトデータ
T_CURSOR Select[3];                          //セレクトカーソル座標
int Item[ITEM_MAX];
int ClickStatus;
int Stage_State;
int Stage_Mission;
int Stage_Score;
int ClearFlag;

int BlockImage[BLOCK_IMAGE_MAX];     //ブロック画像
int StageImage;                             //背景医用画像

int ClickSE;                            //クリックSE
int FadeOutSE;                              //フェードアウトSE
int MoveBlockSE;                        //ブロック移動SE

/***********************************
* プロトタイプ宣言
************************************/
int combo_check(int x, int y);
void combo_check_h(int y, int x, int* cnt, int* col);
void combo_check_w(int y, int x, int* cnt, int* col);
void save_block(void);
void restore_block(void);

/***********************************
* ステージ制御機能：初期化処理
* 引　数：なし
* 戻り値：エラー情報
************************************/

int StageInitialize(void)
{
	int ret = 0;
	int i;

	//画像読込み
	LoadDivGraph("images/block.png", BLOCK_IMAGE_MAX, BLOCK_IMAGE_MAX, 1,
		BLOCKSIZE, BLOCKSIZE, BlockImage);
	StageImage = LoadGraph("images/stage.png");

	//音源読込み
	ClickSE = LoadSoundMem("sounds/click_se.mp3");
	FadeOutSE = LoadSoundMem("sounds/fadeout_se.mp3");
	MoveBlockSE = LoadSoundMem("sounds/moveblock_se.mp3");

	//ブロック生成処理
	CreateBlock();

	ClickStatus = E_NONE;
	Stage_State = 0;
	Stage_Score = 0;
	ClearFlag = FALSE;

	for ( i = 0; i < 3; i++)
	{
		Select[i].x = 0;
		Select[i].y = 0;
	}

	//エラーチェック
	for ( i = 0; i < BLOCK_IMAGE_MAX; i++)
	{
		if (BlockImage[i]==-1)
		{
			ret = -1;
			break;
		}
	}

	if (StageImage==-1)
	{
		ret = -1;
	}
	if (ClickSE==-1)
	{
		ret = -1;
	}
	if (FadeOutSE == -1)
	{
		ret = -1;
	}
	if (MoveBlockSE == -1)
	{
		ret = -1;
	}

	return ret;
}

/***********************************
* ステージ制御機能：ステージの描画
* 引　数：なし
* 戻り値：なし
************************************/

void StageDraw(void)
{
	DrawGraph(0, 0, StageImage, FALSE);

	//アイテムの取得個数を描画
	for (int i = 0; i < ITEM_MAX; i++)
	{
		DrawRotaGraph(540, 245 + i * 30, 0.5f, 0, BlockImage[i + 1], TRUE, 0);
		DrawFormatString(580, 235 + i * 30, 0xffffff, "%d", Item[i]);
	}

	//ブロックを描画
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (Block[i][j].flg==TRUE&&Block[i][j].image!=NULL)
			{
				DrawGraph(Block[i][j].x, Block[i][j].y,
					BlockImage[Block[i][j].image], TRUE);
			}
		}
	}

	//選択ブロックを描画
	DrawGraph(Select[SELECT_CURSOR].x * BLOCKSIZE, Select[SELECT_CURSOR].y *
		BLOCKSIZE, BlockImage[9], TRUE);
	if (ClickStatus!=E_NONE)
	{
		DrawGraph(Select[NEXT_CURSOR].x * BLOCKSIZE, Select[NEXT_CURSOR].y *
			BLOCKSIZE, BlockImage[9], TRUE);
	}

	//ミッションを描画
	SetFontSize(20);
	DrawFormatString(590.211, GetColor(255, 255, 255), "%3d", Stage_Mission);

	//アイテムの取得個数を描画
	for (int i = 0; i < ITEM_MAX; i++)
	{
		DrawRotaGraph(540, 245 + i * 30, 0.5f, 0, BlockImage[i + 1], TRUE, 0);
		DrawFormatString(580, 235 + i * 30, GetColor(255, 255, 255), "%3d",
			Item[i]);
	}
}

/***********************************
* ステージ制御機能：ブロック生成処理
* 引　数：なし
* 戻り値：なし
************************************/

void CreateBlock(void)
{
	int Check = 0;
	int i, j;

	do
	{
		Check = 0;
		for ( j = 0; j < HEIGHT; j++)
		{
			for ( j = 0; j < WIDTH; j++)
			{
				if (j==0||j==WIDTH-1||i==HEIGHT-1||i==0)
				{
					Block[i][j].flg = FALSE;
					Block[i][j].image = NULL;
				}
				else
				{
					Block[i][j].flg = TRUE;
					Block[i][j].x =(j-1)*BLOCKSIZE;
					Block[i][j].y =(i-1)*BLOCKSIZE;
					Block[i][j].width =BLOCKSIZE;
					Block[i][j].height =BLOCKSIZE;
					Block[i][j].image = GetRand(7) + 1;//1~8の乱数
				}
			}
		}
		for ( i = 1; i < HEIGHT-1; i++)
		{
			for ( j = 1; j < WIDTH-1; j++)
			{
				if (Block[i][j].image==NULL)
				{
					Block[i][j].image = GetRand(7) + 1;
				}
			}
		}
		
	} while (Check!=0);
}