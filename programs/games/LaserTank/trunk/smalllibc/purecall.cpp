#include "kosSyst.h"
static char pureCallMessage[] = "PURE function call!";

// ����� ������������ ������
int __cdecl _purecall()
{
	rtlDebugOutString( pureCallMessage );
	kos_ExitApp();
	return 0;
}

