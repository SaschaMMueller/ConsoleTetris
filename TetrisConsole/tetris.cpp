// Tetris Game Logic
#include "tetris.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//DATA
int g_tPoints = 0;
int g_tLevel = 1;
int g_nDropped = 0;
int g_tGrid[ktW][ktH];

TetrisPiece g_tPieces[ kMaxPieces ];

int g_tnStartPieces = 0;
int g_tnPieces = 0;
int g_nextPiece = 0;

int g_tPieceID = -1;
int g_tX = 0;
int g_tY = 0;

unsigned long g_tick = 0;

bool g_bGameOver = false;
bool bDropped = false;

//forward declarations
void TetrisScreenSetup();

//------------------------------------------------------------------
void FillGridTest()
{
	g_tGrid[1][0] = 1;

	/*for (int i=0; i<ktH; ++i)
	{
		for (int j=0; j<ktW; ++j)
		{
			g_tGrid[i][j] = rand()% (g_tnStartPieces + 1);
		}
	}*/
}
//------------------------------------------------------------------------------

void UpdatePieceInfo (int pieceID)
{
	int startX = -1;
	int endX = -1;
	int startY = -1;
	int endY = -1;
	
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			if ( g_tPieces[pieceID].form[i][j] != 0)
			{
				if ( startX == -1 || i < startX )
				{
					startX = i;
				}
				if ( endX == -1 || i > endX )
				{
					endX = i;
				}
				if ( startY == -1 || j < startY )
				{
					startY = j;
				}
				if ( endY == -1 || j > endY )
				{
					endY = j;
				}
			}
		}	
	}

	g_tPieces[pieceID].offsetX = startX;
	g_tPieces[pieceID].width = endX - startX + 1;
	g_tPieces[pieceID].offsetY = startY;
	g_tPieces[pieceID].height = endY - startY + 1;
}
//-----------------------------------------------------------------------
bool CheckParam ( FILE* pFile, char* name ) 
{
	char param[50];
	memset( param, 0, sizeof( param ) );
	fscanf( pFile, "%49s", &param );
	if( strcmp( param, name ) != 0 )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------
int ChoosePiece()
{
	int id = rand()%g_tnStartPieces;

	int nFound = 0;
	for( int i = 0; i< g_tnPieces; ++i )
	{
		if( g_tPieces[ i ].bStartPiece )
		{
			if( nFound == id )
			{
				return i;
			}
			nFound++;
		}
	}
	printf("Error generating starting piece!");
	return -1;
}
//------------------------------------------------------------------------------
bool CheckValidMove( int x, int y, int piece )
{
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			if ( g_tPieces[piece].form[i][j] != 0)
			{
				if ( ( x + i < 0 )|| ( x + i >= ktW ) || ( y + j >= ktH ) || ( g_tGrid[x + i][y + j] != 0 ) )
				{
					return false;
				}
			}
		}
	}
	return true;
}


//-----------------------------------------------------------------------------
void StartNewPiece ()
{
	
	g_tPieceID = g_nextPiece;
	//set piece in the middle of the well
	g_tX = ktW/2 - g_tPieces[ g_tPieceID ].offsetX - g_tPieces[ g_tPieceID ].width / 2;
	g_tY = 0;
	g_nextPiece = ChoosePiece();
	
	if ( CheckValidMove(g_tX, g_tY, g_tPieceID ) == false )
	{
		g_bGameOver = true;
	}
}
//-----------------------------------------------------------------------
void TetrisInit ()
{
	memset( g_tGrid, 0, sizeof( g_tGrid ) );
	memset( g_tPieces, 0, sizeof( g_tPieces ) );

	FILE* pFile = fopen ( "pieces.txt", "rt" );
	if ( pFile == 0 )
	{
		printf( "cant find file" );
		return;
	}

	int nPieces = 0;
	bool bError = false;
//.............................................................................
	//Read Pieces Info
	do
	{
		//ID.................................
		if ( CheckParam ( pFile, "ID:" )== false)
		{
			if( g_tnPieces == 0 )
			{
				printf ( "cant find parameter ID:\n" );
				bError = true;
			}
			break;
		}
		int id=-1;
		int result = fscanf( pFile, "%d", &id );
		if ( result == 0 )
		{
			bError = true;
			break;
		}

		if ( g_tnPieces != id )
		{
			printf ( "IDs not in order\n" );
			bError = true;
			break;
		}

		g_tnPieces++;
		if ( g_tnPieces == kMaxPieces )
		{
			printf ( "Too many Pieces (over %d)\n", kMaxPieces );
			bError = true;
			break;
		}

		//Start.................................
		if ( CheckParam ( pFile, "START:" )== false)
		{
			printf ( "cant find parameter START:\n" );
			bError = true;
			break;
		}
		char buffer[50];
		result = fscanf( pFile, "%49s", &buffer );
		if ( result == 0 )
		{
			bError = true;
			break;
		}
		
		bool bStart = false;
		if( strcmp( buffer, "TRUE") == 0 )
		{
			bStart = true;
			g_tnStartPieces++;
		}
		g_tPieces[ id ].bStartPiece = bStart;

		//FORM.................................
		if ( CheckParam ( pFile, "FORM:" )== false)
		{
			printf ( "cant find parameter FORM:\n" );
			bError = true;
			break;
		}

		for ( int i = 0; i < 4; ++i )
		{
			int line[4];
			result = fscanf( pFile, "%d %d %d %d", &line[0], &line[1], &line[2], &line[3] );
			if ( result == 0 )
			{
				bError = true;
				break;
			}
			g_tPieces[ id ].form[ 0 ][ i ] = line [ 0 ];
			g_tPieces[ id ].form[ 1 ][ i ] = line [ 1 ];
			g_tPieces[ id ].form[ 2 ][ i ] = line [ 2 ];
			g_tPieces[ id ].form[ 3 ][ i ] = line [ 3 ];
		}

		UpdatePieceInfo( id );

		//PREVIOUS.................................
		if ( CheckParam ( pFile, "PREVIOUS:" )== false)
		{
			printf ( "cant find parameter PREVIOUS:\n" );
			bError = true;
			break;
		}
		int previous = -1;
		result = fscanf( pFile, "%d", &previous );
		if ( result == 0 )
		{
			bError = true;
			break;
		}
		g_tPieces[ id ].previous = previous;

		//NEXT.................................
		if ( CheckParam ( pFile, "NEXT:" )== false)
		{
			printf ( "cant find parameter NEXT:\n" );
			bError = true;
			break;
		}
		int next = -1;
		result = fscanf( pFile, "%d", &next );
		if ( result == 0 )
		{
			bError = true;
			break;
		}
		g_tPieces[ id ].next = next;
	}
//..................................................................
	while( !bError );
	
	fclose( pFile );

	if( bError )
	{
		printf("Error while loading file\n");
		return;
	}

	TetrisScreenSetup ();
	//FillGridTest();
	StartNewPiece ();
}


/*
//------------------------------------------------------------------------------
bool CheckValidRotation( int piece )
{
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			if ( g_tPieces[piece].form[i][j] != 0)
			{
				if ( ( x + i < 0 )|| ( x + i >= ktW ) || ( y + j >= ktH ) || ( g_tGrid[x + i][y + j] != 0 ) )
				{
					return false;
				}
			}
		}
	}
	return true;
}*/

//----------------------------------------------------------------------------
void TetrisInput( ETetrisEvent event )
{
	if ( g_bGameOver == true)
	{
		return;
	}

	int x = g_tX;
	int y = g_tY;
	int pieceID = g_tPieceID;

	if( event == kTE_MOVE_LEFT )
	{
		x--;
	}

	if( event == kTE_MOVE_RIGHT )
	{
		x++;
	}

	if( event == kTE_SPACE )
	{
		while( CheckValidMove( x, y+1, pieceID ) == true)
		{
			y++;
		}
		
		g_tY = y;
		bDropped = true;
	}

	if( event == kTE_MOVE_DOWN )
	{
		y++;
	}
	if( event == kTE_ROTATE )
	{
		pieceID = g_tPieces[pieceID].next;
	}

	if( CheckValidMove( x, y, pieceID ) )
	{
		g_tX = x;
		g_tY = y;
		g_tPieceID = pieceID;
	}
}

//-----------------------------------------------------------------------------
int BakeCurrentPiece(int pieceID)
{
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			if ( g_tPieces[pieceID].form[i][j] != 0)
			{
				g_tGrid[g_tX + i][g_tY + j] = g_tPieces[pieceID].form[i][j];
			}
		}
	}
	g_nDropped++;

	if ( g_nDropped == 10 && g_tLevel < 10 )
	{
		g_nDropped = 0;
		g_tLevel++;
	}
	return g_tLevel;
}
//-----------------------------------------------------------------------------
void DeleteLine(int line)
{
	for ( int j = line; j > 1; --j )
	{
		for ( int i = 0; i < ktW; ++i )
		{
			g_tGrid[i][j] = g_tGrid[i][ j - 1];
		}
	}
}
//-----------------------------------------------------------------------------
int CheckForLines()
{
	int nLines = 0;
	for ( int j = 0; j < ktH; ++j )
	{
		bool bFull = true;
		for ( int i = 0; i < ktW; ++i )
		{
			if ( g_tGrid[i][j] == 0 ) 
			{
				bFull = false;
			}
		}

		//is line full or not?
		if ( bFull == true ) 
		{
			DeleteLine(j);
			nLines++;
		}
	}
	return nLines;
}
//-----------------------------------------------------------------------------
void TetrisUpdate( unsigned long elapsed )
{	
	if ( g_bGameOver == true )
	{
		return;
	}
	g_tick = g_tick + elapsed;
	if( (g_tick > (1100 - g_tLevel * 100)) || bDropped == true)
	{
		if (bDropped == false)
		{
			g_tick = g_tick - (1100 - g_tLevel * 100);
		}
		bDropped = false;
		if( CheckValidMove( g_tX, g_tY+1, g_tPieceID ) )
		{	
			g_tY = g_tY + 1;
		}
		//Piece arrives at bottom
		else 
		{
			BakeCurrentPiece(g_tPieceID);
			g_tPoints += kScore_DropPiece;
			int nLines = CheckForLines ();
			g_tPoints += kScore_Line[nLines];
			StartNewPiece ();
		}
	}

	//printf("Elapsed: %d\n", elapsed );
	
}

