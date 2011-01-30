#include "kosSyst.h"
#include "kosFile.h"
#include "gfxdef.h"
#include "mainWnd.h"
#include "gameWnd.h"
#include "top10wnd.h"




void kos_Main()
{
	int hdrNum, i;
	SCompBmpHeader *hdrList;
	char *cPtr;

	// �������� ��� ������ �� ����
	cPtr = strrchr( kosExePath, '/' );
	// �������� ;)
/*	if ( cPtr == NULL )
	{
		//
		rtlDebugOutString( "Invalid path to executable." );
		//
		return;
	}*/
	//
	cPtr[1] = 0;
	//
	strcpy( top10FilePath, kosExePath );
	//
	strcpy( top10FilePath + ((cPtr - kosExePath) + 1), "fara.t10" );
	//
	PrepareTop10();
	// ��������� ��� ����� � ��������
	strcpy( cPtr + 1, "fara.gfx" );
	// ��������� �������� �� �����
	CKosFile gfxRes( kosExePath );

	// ������ ���������� �������� � �����
	if ( gfxRes.Read( (Byte *)&hdrNum, sizeof(hdrNum) ) != sizeof(hdrNum) ) return;
	// ����� ��� ���������
	hdrList = new SCompBmpHeader[hdrNum];
	// ��������� ��������� ��������
	gfxRes.Read( (Byte *)hdrList, sizeof(SCompBmpHeader) * hdrNum );
	// ������������� ��������
	mainWndFace.LoadFromArch( &(hdrList[MAIN_FACE_NDX]), &gfxRes, MAIN_FACE_NDX );
	gameFace.LoadFromArch( &(hdrList[GAME_FACE_NDX]), &gfxRes, GAME_FACE_NDX );
	gameBlocks.LoadFromArch( &(hdrList[BUTTONS_NDX]), &gfxRes, BUTTONS_NDX );
	gameNumbers.LoadFromArch( &(hdrList[NUMBERS_NDX]), &gfxRes, NUMBERS_NDX );
	// ������������ ��������� � ������� ��������
	for ( i = 0; i < 4; i++ )
		gameBlocksZ[i].Scale(32-i*8,gameBlocks.GetBits());
	//
	delete hdrList;
	// �������������� �����
	for ( i = 0; i < blocksNum; i++ )
	{
		//
		fishki[i] = new CFishka( &gameBlocks, i * blockSize, RGB(0x0) );
	}
	//
	maxGameLevel = START_LEVEL;
	// ��������� ����� ��� ������� ����
	kos_SetMaskForEvents( 0x27 );
	//
	while ( MainWndLoop() == MW_START_GAME )
	{
		GameLoop();
		Top10Loop();
	}
	//
	for ( i = 0; i < blocksNum; i++ )
	{
		delete fishki[i];
	}
	//
	delete hdrList;
}

