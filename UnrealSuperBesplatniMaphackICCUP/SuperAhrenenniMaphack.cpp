#include <Windows.h>
#include <string>
#include "verinfo.h"

#define ADDR(X,REG)\
	__asm MOV REG, DWORD PTR DS : [ X ] \
	__asm MOV REG, DWORD PTR DS : [ REG ]
int pW3XGlobalClass = 0;
int _W3XConversion = 0;
int _W3XMinimap = 0;
int GameDll = 0;
int _DrawUnit = 0;
int IsGame1 = 0;
int _ConvertCoord1 = 0;
int _ConvertCoord2 = 0;

BOOL FileExists( LPCTSTR fname )
{
	return ::GetFileAttributes( fname ) != DWORD( -1 );
}


int * GetItemCountAndItemArray( int ** itemarray )
{
	int GlobalClassOffset = *( int* ) ( pW3XGlobalClass );
	if ( GlobalClassOffset )
	{
		int ItemsOffset1 = *( int* ) ( GlobalClassOffset + 0x3BC ) + 0x10;
		if ( ItemsOffset1 )
		{
			int * ItemsCount = ( int * ) ( ItemsOffset1 + 0x604 );
			if ( *ItemsCount > 0 )
			{
				*itemarray = ( int * ) *( int* ) ( ItemsOffset1 + 0x608 );
				return ItemsCount;
			}
		}
	}

	*itemarray = 0;
	return 0;
}



int * GetUnitCountAndUnitArray( int ** unitarray )
{
	int GlobalClassOffset = *( int* ) ( pW3XGlobalClass );
	if ( GlobalClassOffset )
	{
		int UnitsOffset1 = *( int* ) ( GlobalClassOffset + 0x3BC );
		if ( UnitsOffset1 )
		{
			int * UnitsCount = ( int * ) ( UnitsOffset1 + 0x604 );
			if ( *UnitsCount > 0 )
			{
				*unitarray = ( int * ) *( int* ) ( UnitsOffset1 + 0x608 );
				return UnitsCount;
			}
		}
	}

	*unitarray = 0;
	return 0;
}


#pragma optimize("",off)
DWORD CoordToMinimap( float Loc, DWORD offst )
{
	float Result;
	DWORD result;
	__asm
	{
		ADDR( _W3XConversion, EAX );
		add EAX, offst;
		PUSH EAX;
		LEA EDX, Loc;
		LEA ECX, Result;
		CALL _ConvertCoord1;
		MOV ECX, DWORD PTR DS : [ EAX ];
		LEA EAX, DWORD PTR DS : [ ECX + 0xFD000000 ];
		XOR EAX, ECX;
		SAR EAX, 0x1F;
		LEA EDX, DWORD PTR DS : [ ECX + 0xFD800000 ];
		NOT EAX;
		AND EAX, EDX;
		LEA ECX, Result;
		MOV DWORD PTR DS : [ ECX ], EAX;
		CALL _ConvertCoord2;
		MOV result, EAX;
	}
	return result;
}

#pragma optimize("",on)
DWORD GetXForMINIMAP( float X )
{
	return CoordToMinimap( X, 0x6C );
}


DWORD GetYForMINIMAP( float Y )
{
	return CoordToMinimap( Y, 0x70 );
}

#pragma optimize("",off)
void DrawPixel( DWORD X, DWORD Y, DWORD Color )
{
	__asm
	{
		ADDR( _W3XMinimap, EDI );
		MOV EDI, DWORD PTR DS : [ EDI + 0x1D8 ];
		MOV EAX, Y;
		SHL EAX, 8;
		MOV ECX, X;
		ADD EAX, ECX;
		MOV ECX, Color;
		MOV DWORD PTR DS : [ EDI + 4 * EAX ], ECX;;
	}
}

#pragma optimize("",on)

void GetUnitLocation2D( int unitaddr, float * x, float * y )
{
	if ( unitaddr )
	{
		*x = *( float* ) ( unitaddr + 0x284 );
		*y = *( float* ) ( unitaddr + 0x288 );
	}
	else
	{
		*x = 0.0;
		*y = 0.0;
	}
}


void GetItemLocation2D( int itemaddr, float * x, float * y )
{
	if ( itemaddr )
	{
		int iteminfo = *( int * ) ( itemaddr + 0x28 );
		if ( iteminfo )
		{
			*x = *( float* ) ( iteminfo + 0x88 );
			*y = *( float* ) ( iteminfo + 0x8C );
		}
		else
		{
			*x = 0.0;
			*y = 0.0;
		}
	}
	else
	{
		*x = 0.0;
		*y = 0.0;
	}
}



#pragma optimize("",off)
void DrawObjectOnMap( int unit_or_item )
{
	__asm
	{
		ADDR( pW3XGlobalClass, EDI );
		MOV EDI, DWORD PTR DS : [ EDI + 0x3BC ];
		MOV EAX, DWORD PTR DS : [ EDI + 0x140 ];
		FLD DWORD PTR DS : [ EDI + 0x238 ];
		MOV ESI, unit_or_item;
		PUSH ESI;
		PUSH 0;
		PUSH EAX;
		PUSH -1;
		FSTP DWORD PTR SS : [ ESP ];
		MOV ECX, DWORD PTR DS : [ ESI + 0x28 ];
		PUSH ECX;
		MOV ECX, DWORD PTR DS : [ EDI + 0x16C ];
		CALL _DrawUnit;
	}


}
#pragma optimize("",on)


void DrawUnitAtMinimap( int unitaddr )
{
	float x, y;
	GetUnitLocation2D( unitaddr, &x, &y );
	DWORD _X, _Y;
	_X = GetXForMINIMAP( x );
	_Y = GetXForMINIMAP( y );
	DrawPixel( _X, _Y, 0xaaaaaaaa );
	DrawPixel( _X + 1, _Y + 2, 0xaaaaaaaa );
	DrawPixel( _X - 1, _Y - 1, 0xaaaaaaaa );
	DrawPixel( _X + 1, _Y, 0xaaaaaaaa );
	DrawPixel( _X, _Y + 1, 0xaaaaaaaa );

}


void DrawItemAtMinimap( int itemaddr )
{
	float x, y;
	GetItemLocation2D( itemaddr, &x, &y );
	DWORD _X, _Y;
	_X = GetXForMINIMAP( x );
	_Y = GetXForMINIMAP( y );
	DrawPixel( _X, _Y, 0xaaaaaaaa );
	DrawPixel( _X + 1, _Y + 2, 0xaaaaaaaa );
	DrawPixel( _X - 1, _Y - 1, 0xaaaaaaaa );
	DrawPixel( _X + 1, _Y, 0xaaaaaaaa );
	DrawPixel( _X, _Y + 1, 0xaaaaaaaa );

}

BOOL IsUnitDead( int unitaddr )
{
	if ( unitaddr )
	{
		unsigned int unitflag = *( unsigned int* ) ( unitaddr + 0x5C );
		BOOL UnitNotDead = ( ( unitflag & 0x100u ) == 0 );
		return UnitNotDead == FALSE;
	}
	return TRUE;
}

int UnitVtable = 0;
int ItemVtable = 0;

BOOL IsNotBadItem( int itemaddr, BOOL onlymemcheck = FALSE )
{
	if ( itemaddr > 0 )
	{
		int xaddraddr = ( int ) &ItemVtable;

		if ( *( BYTE* ) xaddraddr != *( BYTE* ) itemaddr )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 1 ) != *( BYTE* ) ( itemaddr + 1 ) )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 2 ) != *( BYTE* ) ( itemaddr + 2 ) )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 3 ) != *( BYTE* ) ( itemaddr + 3 ) )
			return FALSE;



		if ( *( int * ) ( itemaddr + 0x20 ) & 1 )
			return FALSE;

		float hitpoint = *( float * ) ( itemaddr + 0x58 );

		return hitpoint != 0.0f;
	}

	return FALSE;
}

int GameVersion = 0;
int IsUnitVisibledAddr = 0;

#pragma optimize("",off)

BOOL __declspec( naked ) __cdecl IsUnitVisibled126a( int unitaddr, int player )
{
	__asm
	{
		MOV ECX, [ ESP + 0x4 ];
		PUSH ESI;
		MOV EAX, ECX;
		JMP IsUnitVisibledAddr;
	}
}

BOOL __declspec( naked ) __cdecl IsUnitVisibled127a( int unitaddr, int player )
{
	__asm
	{
		PUSH EBP;
		MOV EBP, ESP;
		MOV ECX, [ EBP + 0x8 ];
		PUSH ESI;
		MOV EAX, ECX;
		JMP IsUnitVisibledAddr;
	}
}
#pragma optimize("",on)





typedef int( __cdecl * pPlayer )( int number );
pPlayer PlayerReal;

int Player( int number )
{
	int returnplayer = PlayerReal( number );
	if ( returnplayer >= 0x10000 )
		return returnplayer;
	return -1;
}



BOOL __cdecl IsUnitVisibled( int unitaddr, int playernumber )
{
	int  pPlayer = Player( playernumber );
	if ( !unitaddr || !pPlayer )
		return TRUE;
	if ( GameVersion == 0x126a )
		return IsUnitVisibled126a( unitaddr, pPlayer );
	else if ( GameVersion == 0x127a )
		return IsUnitVisibled127a( unitaddr, pPlayer );
	return TRUE;
}

int pGlobalPlayerData = 0;


void * GetGlobalPlayerData( )
{
	if ( *( int * ) ( pGlobalPlayerData ) > 0 )
	{
		return ( void * ) *( int* ) ( pGlobalPlayerData );
	}
	return nullptr;
}



int GetLocalPlayerNumber( )
{
	void * gldata = GetGlobalPlayerData( );
	if ( gldata != nullptr && gldata )
	{
		int playerslotaddr = ( int ) gldata + 0x28;
		return ( int ) *( short * ) ( playerslotaddr );
	}
	else
		return 0;
}



BOOL IsNotBadUnit( int unitaddr )
{

	if ( unitaddr > 0 )
	{
		int xaddr = UnitVtable;
		int xaddraddr = ( int ) &xaddr;



		if ( *( BYTE* ) xaddraddr != *( BYTE* ) unitaddr )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 1 ) != *( BYTE* ) ( unitaddr + 1 ) )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 2 ) != *( BYTE* ) ( unitaddr + 2 ) )
			return FALSE;
		else if ( *( BYTE* ) ( xaddraddr + 3 ) != *( BYTE* ) ( unitaddr + 3 ) )
			return FALSE;


		unsigned int isdolbany = *( unsigned int* ) ( unitaddr + 0x5C );
		unsigned int isdolbany2 = *( unsigned int* ) ( unitaddr + 0x20 );

		BOOL returnvalue = !( isdolbany2 & 1 ) && isdolbany != 0x1001u && !IsUnitDead( unitaddr );
		if ( returnvalue && !IsUnitVisibled( unitaddr, GetLocalPlayerNumber( )))
		{
			return TRUE;
		}


	}

	return FALSE;
}


void __stdcall DrawAllUnits( )
{
	int * unitsarray = 0;
	int * UnitsCount = GetUnitCountAndUnitArray( &unitsarray );
	if ( UnitsCount > 0 && * UnitsCount > 0 && unitsarray )
	{
		for ( int i = 0; i < *UnitsCount; i++ )
		{
			if ( unitsarray[ i ] && IsNotBadUnit( unitsarray[ i ] ) )
			{
				DrawObjectOnMap( unitsarray[ i ] );
				//	DrawUnitAtMinimap( unitsarray[ i ] );
			}
		}
	}
}




void __stdcall DrawAllItems( )
{
	int * itemsarray = 0;
	int * ItemsCount = GetItemCountAndItemArray( &itemsarray );
	if ( ItemsCount > 0 && * ItemsCount > 0 && itemsarray )
	{
		for ( int i = 0; i < *ItemsCount; i++ )
		{
			if ( itemsarray[ i ] && IsNotBadItem( itemsarray[ i ] ) )
			{
				DrawObjectOnMap( itemsarray[ i ] );
				//	DrawItemAtMinimap( itemsarray[ i ] );
			}
		}
	}
}


void __stdcall DrawALL( )
{
	if ( *( BOOL* ) IsGame1 )
	{
		DrawAllUnits( );
		DrawAllItems( );
	}
}


void Init126aVer( )
{
	PlayerReal = ( pPlayer ) ( GameDll + 0x3BBB30 );
	pGlobalPlayerData = 0xAB65F4 + GameDll;
	GameVersion = 0x126a;
	IsUnitVisibledAddr = GameDll + 0x3C7AF0 + 0xA;
	pW3XGlobalClass = GameDll + 0xAB4F80;
	IsGame1 = GameDll + 0xAB62A4;
	_DrawUnit = GameDll + 0x60FCE0;
	_W3XMinimap = GameDll + 0xAB6214;
	_W3XConversion = GameDll + 0xAB7368;
	_ConvertCoord1 = GameDll + 0x6EEEF0;
	_ConvertCoord2 = GameDll + 0x6EEC20;
	UnitVtable = GameDll + 0x931934;
	ItemVtable = GameDll + 0x9320B4;
}


void Init127aVer( )
{
	pGlobalPlayerData = 0xBE4238 + GameDll;
	GameVersion = 0x127a;
	PlayerReal = ( pPlayer ) ( GameDll + 0x1F1E70 );
	IsUnitVisibledAddr = GameDll + 0x1E8E80 + 0xC;
	pW3XGlobalClass = GameDll + 0xBE6350;
	IsGame1 = GameDll + 0xBE6530;
	_DrawUnit = GameDll + 0x0CAAE0;
	_W3XMinimap = GameDll + 0xBE6DC4;
	_W3XConversion = GameDll + 0xBB82BC;
	_ConvertCoord1 = GameDll + 0x0528B0;
	_ConvertCoord2 = GameDll + 0x052F30;
	UnitVtable = GameDll + 0xA4A704;
	ItemVtable = GameDll + 0xA4A2EC;
}

int Class1Clone[ 58 ];
int Class1Org[ 58 ];

#pragma optimize("",off)


void __declspec( naked ) NeedDrawAll12( )
{
	__asm
	{
		pushad;
		call DrawALL;
		popad;
		jmp[ Class1Org + 12 * 4 ];
	}
}


#pragma optimize("",on)

#define IsKeyPressed(CODE) (GetAsyncKeyState(CODE) & 0x8000) > 0





void PrintAddress( int addr, int id )
{
	char printaddr[ 30 ];
	sprintf_s( printaddr, 30, "%X[%d]", addr, id );
	MessageBox( 0, printaddr, printaddr, 0 );
}

void Watcher( )
{
	while ( true )
	{
		while ( *( BOOL* ) IsGame1 )
		{
			Sleep( 100 );
		}

		while ( !*( BOOL* ) IsGame1 )
		{
			Sleep( 100 );
		}


		Sleep( 5000 );

		if ( *( BOOL* ) IsGame1 )
		{

			//[[[[Game.dll+AB4F80]+1C]+C]]+24

			int pOff1 = pW3XGlobalClass;
			pOff1 = *( int* ) pOff1;
			if ( pOff1 > 0 )
			{
				pOff1 = *( int* ) ( pOff1 + 0x1C );
				if ( pOff1 > 0 )
				{
					pOff1 = *( int* ) ( pOff1 + 0xC );
					if ( pOff1 > 0 && *( int* ) pOff1 != ( int ) &Class1Clone[ 0 ] )
					{
						memset( Class1Org, 0, sizeof( Class1Org ) );
						memset( Class1Clone, 0, sizeof( Class1Clone ) );

						CopyMemory( &Class1Clone[ 0 ], ( void* ) ( *( int* ) pOff1 ), 57 * 4 );
						CopyMemory( &Class1Org[ 0 ], ( void* ) ( *( int* ) pOff1 ), 57 * 4 );



						*( int* ) pOff1 = ( int ) &Class1Clone[ 0 ];

						Class1Clone[ 12 ] = ( int ) &NeedDrawAll12;
					

					}
				}
			}
		}

		while ( *( BOOL* ) IsGame1 )
		{
			Sleep( 100 );
		}
	}
}

char freemh2[ 200 ];
char freemh3[ 200 ];
DWORD WINAPI THREADX( LPVOID )
{
	MessageBox( 0, freemh3, freemh2, 0 );
	Watcher( );

	return 0;
}


void InitializeSuperPuperMaphack( )
{
	HMODULE hGameDll = GetModuleHandle( "WarcisGame.dll" );
	if ( !hGameDll )
	{
		MessageBox( 0, "SuperPuperMaphack problem!\nNo game.dll found.", "Game.dll not found", 0 );
		return;
	}

	GameDll = ( int ) hGameDll;

	CFileVersionInfo gdllver;
	gdllver.Open( hGameDll );
	// Game.dll version (1.XX)
	int GameDllVer = gdllver.GetFileVersionQFE( );
	gdllver.Close( );

	if ( GameDllVer == 6401 )
	{
			sprintf_s( freemh2, 200, "%s%s%s%s%s%s%s%s%s", "A", "bs", "ol", "Ma", "pha", "ck", "Fre", "e!", " 26a" );
			sprintf_s( freemh3, 200, "%s%s%s%s%s%s%s", "Беспл", "атный", "Ma", "p", "hack", "\t\t\t", "\r\nAbso!" );

			Init126aVer( );
			CreateThread( 0, 0, THREADX, 0, 0, 0 );
	}
	else if ( GameDllVer == 52240 )
	{
		
			sprintf_s( freemh2, 200, "%s%s%s%s%s%s%s%s%s", "A", "bs", "ol", "Ma", "pha", "ck", "Fre", "e!", " 27a" );
			sprintf_s( freemh3, 200, "%s%s%s%s%s%s%s", "Беспл", "атный", "Ma", "p", "hack", "\t\t\t", "\r\nAbso!" );

			Init127aVer( );
			CreateThread( 0, 0, THREADX, 0, 0, 0 );
	}
	else
	{
		sprintf_s( freemh2, 200, "%s%s%s%s%s%s%s%s%s", "A", "bs", "ol", "Ma", "pha", "ck", "Fre", "e!", " 26a" );
		sprintf_s( freemh3, 200, "%s%s%s%s%s%s%s", "Беспл", "атный", "Ma", "p", "hack", "\t\t\t", "\r\nAbso!" );

		Init126aVer( );
		CreateThread( 0, 0, THREADX, 0, 0, 0 );
	}



}


BOOL __stdcall DllMain( HINSTANCE i, DWORD r, LPVOID )
{
	if ( r == DLL_PROCESS_ATTACH )
	{
		
		char freemh1[ 100 ];
		sprintf_s( freemh1,100, "%s%s%s%s%s%s%s%s%s%s%s.txt\0", "Fr", "ee", "Ma", "ph", "ac", "kB", "yA", "bs", "ol", "!", "!" );
		if ( FileExists( freemh1 ) )
		{
			InitializeSuperPuperMaphack( );
		}
		else
		{
			MessageBox( 0, "Вы забыли о самом главном!\n", "Вы потеряли файл !", 0 );
		}
	}
	return TRUE;
}