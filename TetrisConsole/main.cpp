
// main.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <Tchar.h>
#include <time.h>
#include "tetris.h"
#include <string>

using namespace std;

HANDLE wHnd;    // Handle to write to the console.
HANDLE rHnd;    // Handle to read from the console.
clock_t clock_previous = 0;

const int kScreenW = 80;
const int kScreenH = 80;

const int kWellX = 10;
const int kWellY = 2;

const int kPointsX = 55;
const int kPointsY = 5;
const int kPointsW = 20;

const int kLevelX = 55;
const int kLevelY = 30;
const int kLevelW = 20;

const int kNextX = 57;
const int kNextY = 10;
const int kNextW = 16;
const int kNextH = 16;


//---------------------------------------------------------------------
//Setup Tetris Screen
void TetrisScreenSetup()
{
    // Set up the handles for reading/writing:
    wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
    rHnd = GetStdHandle(STD_INPUT_HANDLE);
	    // Change the window title:
    SetConsoleTitle(TEXT("Tetris Console Game"));

    // Set up the required window size:
    SMALL_RECT windowSize = {0, 0, kScreenW-1, kScreenH-1 };
    
    // Change the console window size:
    SetConsoleWindowInfo(wHnd, TRUE, &windowSize);

	// Create a COORD to hold the buffer size:
    COORD bufferSize = { kScreenW, kScreenH };

    // Change the internal buffer size:
    SetConsoleScreenBufferSize(wHnd, bufferSize);
}
//------------------------------------------------------------------------
int GridIDToAttributes( int gridID )
{
	int attributes = 0;

	switch ( gridID)
	{
		case 0: attributes = 0;
		break;
		case 1: attributes = BACKGROUND_RED|BACKGROUND_INTENSITY;
		break;
		case 2: attributes = BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		break;
		case 3: attributes = BACKGROUND_GREEN|BACKGROUND_INTENSITY;
		break;
		case 4: attributes = BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		break;
		case 5: attributes = BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY;
		break;
		case 6: attributes = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY;
		break;
		case 7: attributes = BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
		break;
	}

	return attributes;
}
//---------------------------------------------------------------------
void DisplayCurrentPiece( CHAR_INFO* buffer )
{
	for (int y = 0; y < 16; ++y) 
	{
        for (int x = 0; x < 16; ++x) 
		{			
			int ID = g_tPieces[ g_tPieceID ].form[x/4][y/4];
			
			if ( ID != 0)
			{
				buffer[ ( x + (g_tX + g_tY * kScreenW) * 4 + kWellX ) + (y + kWellY) * kScreenW ].Char.AsciiChar = 8;
				buffer[ ( x + (g_tX + g_tY * kScreenW) * 4 + kWellX ) + (y + kWellY) * kScreenW ].Attributes = GridIDToAttributes(ID);
			}
		}
	}	
}
//---------------------------------------------------------------------
void DisplayGrid( CHAR_INFO* buffer )
{
	for (int y = kWellY; y < 4 * ktH + kWellY; ++y) 
	{
        for (int x = kWellX; x < 4 * ktW + kWellX; ++x) 
		{
			buffer[ x + y * kScreenW ].Char.AsciiChar = 8;

			int gridID = g_tGrid[ (x-kWellX) /4][ (y-kWellY) /4];
			buffer[ x + y * kScreenW ].Attributes = GridIDToAttributes( gridID );

		}
	}	
}
//---------------------------------------------------------------------
void GameOver( CHAR_INFO* buffer )
{
	for (int y = kWellY; y < 4 * ktH + kWellY; ++y) 
	{
        for (int x = kWellX; x < 4 * ktW + kWellX; ++x) 
		{
			buffer[ x + y * kScreenW ].Char.AsciiChar = 0;
			buffer[ x + y * kScreenW ].Attributes = BACKGROUND_RED|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
		}
	}
	char gameOver[20];
	sprintf( gameOver, "Game Over !" );
	for( int i=0; i< strlen( gameOver ); ++i )
	{
		buffer[ kWellX + (ktW*4)/2 - strlen( gameOver )/2 + i + ((ktH/2) * 4 + kWellY) * kScreenW].Char.AsciiChar = gameOver[i];
	}
}
//---------------------------------------------------------------------
void DisplayNextPiece( CHAR_INFO* buffer )
{
	for (int y = 0; y < 16; ++y) 
	{
        for (int x = 0; x < 16; ++x) 
		{			
			int ID  = g_tPieces[ g_nextPiece ].form[x/4][y/4]; 
			int offsetX = g_tPieces[ g_nextPiece ].offsetX;
			int offsetY = g_tPieces[ g_nextPiece ].offsetY;
			int width = g_tPieces[ g_nextPiece ].width;
			int height = g_tPieces[ g_nextPiece ].height;
			
			if ( ID != 0)
			{
				buffer[ x + kNextX + ( (4-width)/2 - offsetX)*4 + (kNextY + y + ( (4-height)/2 - offsetY)*4 ) * kScreenW ].Char.AsciiChar = 8;
				buffer[ x + kNextX + ( (4-width)/2 - offsetX)*4 + (kNextY + y + ( (4-height)/2 - offsetY)*4 ) * kScreenW ].Attributes = GridIDToAttributes(ID);
			}
		}
	}	
}

//---------------------------------------------------------------------
void Display()
{
	// Setup BG
	CHAR_INFO buffer[ kScreenW * kScreenH ];
	COORD charBufSize = { kScreenW, kScreenH };
    COORD characterPos = {0,0};
	SMALL_RECT writeArea = {0,0,kScreenW-1, kScreenW-1 }; 

	for (int y = 0; y < kScreenH; ++y) 
	{
        for (int x = 0; x < kScreenW; ++x) 
		{
			buffer[ x + kScreenW * y ].Char.AsciiChar = 0;
			buffer[ x + kScreenW * y ].Attributes = BACKGROUND_BLUE;
		}
	}

	//WELL
	for (int y = kWellY; y < kWellY + ktH * 4; ++y) 
	{
		for (int x = kWellX; x < kWellX + ktW*4; ++x) 
		{
			buffer[ x + kScreenW * y ].Char.AsciiChar = 0;
			buffer[ x + kScreenW * y ].Attributes = BACKGROUND_BLUE;
		}
	}

	//POINTS
	for (int x = kPointsX; x < kPointsX + kPointsW; ++x) 
	{
		buffer[ x + kScreenW * kPointsY ].Char.AsciiChar = 0;
		buffer[ x + kScreenW * kPointsY ].Attributes = BACKGROUND_RED|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
		
	}
	char points[50];
	sprintf( points, "%d", g_tPoints );
	for( int i=0; i< strlen( points ); ++i )
	{
		buffer[ kPointsX + kScreenW * kPointsY + kPointsW - strlen( points ) + i ].Char.AsciiChar = points[i];
	}

	//NEXT
	for (int y = kNextY; y < kNextY + kNextH; ++y) 
	{
		for (int x = kNextX; x < kNextX + kNextW; ++x) 
		{
			buffer[ x + kScreenW * y ].Char.AsciiChar = 0;
			buffer[ x + kScreenW * y ].Attributes = 0;

		}
	}
			DisplayNextPiece(buffer);
	//LEVEL
	for (int x = kLevelX; x < kLevelX + kLevelW; ++x) 
	{
		buffer[ x + kScreenW * kLevelY ].Char.AsciiChar = 0;
		buffer[ x + kScreenW * kLevelY ].Attributes = BACKGROUND_RED|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
	}
	char level[50];
	sprintf( level, "%d", g_tLevel );
	for( int i=0; i< strlen( level ); ++i )
	{
		buffer[ kLevelX + kScreenW * kLevelY + kLevelW - strlen( level ) + i ].Char.AsciiChar = level[i];
	}

	//GAME OVER
	if (g_bGameOver == false)
	{
		DisplayGrid( buffer );
		DisplayCurrentPiece( buffer );
	}

	if (g_bGameOver == true)
	{
		GameOver( buffer );

	}

	WriteConsoleOutputA(wHnd, buffer, charBufSize, characterPos, &writeArea);
}


//---------------------------------------------------------------------
//Main

int _tmain(int argc, _TCHAR* argv[]) 
{
	srand( (unsigned int)time(NULL) );

	TetrisInit ();
	// How many events have happened?
    DWORD numEvents = 0;

    // How many events have we read from the console?
    DWORD numEventsRead = 0;
    
	bool bPlaying = true;
	clock_previous = clock();

	while( bPlaying )
	{

		// Find out how many console events have happened:
        GetNumberOfConsoleInputEvents(rHnd, &numEvents);

        // If it's not zero (something happened...)
        if (numEvents != 0) 
		{

            // Create a buffer of that size to store the events
            INPUT_RECORD *eventBuffer = new INPUT_RECORD[numEvents];

            // Read the console events into that buffer, and save how
            // many events have been read into numEventsRead.
            ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);

            // Now, cycle through all the events that have happened:
            for (DWORD i = 0; i < numEventsRead; ++i) 
			{

                // Check the event type: was it a key?
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown ) 
				{
                    // Yes! Was the key code the escape key?
                    if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_LEFT) 
					{
						TetrisInput( kTE_MOVE_LEFT );
                    }
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_RIGHT) 
					{
						TetrisInput( kTE_MOVE_RIGHT );
                    }
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_DOWN) 
					{
						TetrisInput( kTE_MOVE_DOWN );
                    }
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_UP) 
					{
						TetrisInput( kTE_ROTATE );
                    }
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_SPACE ) 
					{
						TetrisInput( kTE_SPACE );
                    }
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode==VK_ESCAPE) 
					{
						bPlaying = false;
                    }
                }
            }

            // Clean up our event buffer:
            delete[] eventBuffer;		
        }

		clock_t clock_now = clock();
		clock_t diff = clock_now - clock_previous;
		
		TetrisUpdate( (diff * 1000) / CLOCKS_PER_SEC );

		clock_previous = clock_now;

		Display();

	}
}
