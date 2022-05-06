//-----------------------------------------------------------------------
//Tetris Game Header

struct TetrisPiece
{
	int form[4][4];
	int offsetX;
	int offsetY;
	int height;
	int	width;
	int next;
	int previous;
	bool bStartPiece;
};

enum ETetrisEvent
{
	kTE_MOVE_LEFT,
	kTE_MOVE_RIGHT,
	kTE_MOVE_DOWN,
	kTE_ROTATE,
	kTE_SPACE
};

const int ktW = 10;
const int ktH = 19;
const int kMaxPieces = 50;

const int kScore_DropPiece = 50;
const int kScore_Line[ 5 ] = { 0, 500, 1500, 3000, 10000 };

extern bool g_bGameOver;

extern int g_tGrid[ktW][ktH];
extern int g_tPoints;
extern int g_tLevel;
extern int g_tPieceID;
extern TetrisPiece g_tPieces[ kMaxPieces ];
extern int g_tX;
extern int g_tY;
extern int pieceW;
extern int g_nextPiece;


void TetrisInit();
void TetrisInput( ETetrisEvent event );
void TetrisUpdate( unsigned long elapsed );