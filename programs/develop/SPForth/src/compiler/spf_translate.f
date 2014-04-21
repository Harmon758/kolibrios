( �࠭���� ��室��� ⥪�⮢ �ணࠬ�.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

VECT OK.
VECT <MAIN>
VECT ?LITERAL
VECT ?SLITERAL
USER-VALUE SOURCE-ID-XT \ �᫨ �� ࠢ�� ���, � ᮤ�ন� ��������饥

: DEPTH ( -- +n ) \ 94
\ +n - �᫮ �������� �祥�, ��室����� �� �⥪� ������ ��।
\ ⥬ ��� �㤠 �뫮 ����饭� +n.
  SP@ S0 @ - NEGATE 4 U/
;
: ?STACK ( -> ) \ �뤠�� �訡�� "���௠��� �⥪�", �᫨ �� ����� 祬 ����
  SP@ S0 @ SWAP U< IF S0 @ SP! -4 THROW THEN
;
: ?COMP ( -> )
  STATE @ 0= IF -312 THROW THEN ( ���쪮 ��� ०��� �������樨 )
;

: WORD ( char "<chars>ccc<char>" -- c-addr ) \ 94
\ �ய����� ����騥 ࠧ����⥫�. ����� ᨬ����, ��࠭�祭��
\ ࠧ����⥫�� char.
\ �᪫��⥫쭠� ����� ���������, �᫨ ����� �����祭��� ��ப�
\ ����� ���ᨬ��쭮� ����� ��ப� � ���稪��.
\ c-addr - ���� ��६����� ������, ᮤ�ঠ饩 �����祭��� ᫮��
\ � ���� ��ப� � ���稪��.
\ �᫨ ࠧ��ࠥ��� ������� ���� ��� ᮤ�ন� ⮫쪮 ࠧ����⥫�,
\ १�������� ��ப� ����� �㫥��� �����.
\ � ����� ��ப� ����頥��� �஡��, �� ����砥�� � ����� ��ப�.
\ �ணࠬ�� ����� �������� ᨬ���� � ��ப�.
  DUP PSKIP PARSE
  DUP HERE C! HERE 1+ SWAP CMOVE
  BL HERE COUNT + !
  HERE
;
1 [IF]
: ' ( "<spaces>name" -- xt ) \ 94
\ �ய����� ����騥 �஡���. �뤥���� name, ��࠭�祭��� �஡����. ���� name 
\ � ������ xt, �믮����� ⮪�� ��� name. ����।������� ����� ���������, 
\ �᫨ name �� �������.
\ �� �६� ������樨  ' name EXECUTE  ࠢ��ᨫ쭮  name.
  PARSE-WORD SFIND 0=
  IF -321 THROW THEN (  -? )
;

[THEN]

: CHAR ( "<spaces>name" -- char ) \ 94
\ �ய����� ����騥 ࠧ����⥫�. �뤥���� ���, �࣠��祭��� �஡�����.
\ �������� ��� ��� ��ࢮ�� ᨬ���� �� �⥪.
  PARSE-WORD DROP C@ ;

CREATE ILAST-WORD 0 , 0 ,

: INTERPRET_ ( -> ) \ ������஢��� �室��� ��⮪
  SAVEERR? ON
  BEGIN
    PARSE-WORD DUP
  WHILE 2DUP  ILAST-WORD 2!
\	." <" TYPE ." >"
    SFIND ?DUP
    IF
         STATE @ =
         IF COMPILE, ELSE EXECUTE THEN
    ELSE
         S" NOTFOUND" SFIND 
         IF EXECUTE
         ELSE 2DROP ?SLITERAL THEN
\          ?SLITERAL
    THEN
    ?STACK
  REPEAT 2DROP
;

VARIABLE &INTERPRET
' INTERPRET_ &INTERPRET !

: INTERPRET &INTERPRET @ EXECUTE ;

\ : HALT ( ERRNUM -> ) \ ��室 � ����� �訡��
\  >R exit ;

: .SN ( n --)
\ ��ᯥ���� n ���孨� ����⮢ �⥪�
   >R BEGIN
         R@
      WHILE
        SP@ R@ 1- CELLS + @ DUP 0< 
        IF DUP U>D <# #S #> TYPE
           ." (" ABS 0 <# #S [CHAR] - HOLD #> TYPE ." ) " ELSE . THEN
        R> 1- >R
      REPEAT RDROP
;

: OK1
  STATE @ 0=
  IF ."  Ok" DEPTH 70 UMIN 
     0 ?DO [CHAR] . EMIT LOOP CR 
  THEN
;

: EVAL-WORD ( a u -- )
\ ������஢��� ( �࠭᫨஢���) ᫮�� � ������  a u
    SFIND ?DUP    IF
    STATE @ =  IF 
    COMPILE,   ELSE 
    EXECUTE    THEN
                  ELSE
    -2003 THROW THEN
;

: [   \ 94 CORE
\ ��������: ᥬ��⨪� ����।�����.
\ ���������: �믮����� ᥬ��⨪� �믮������, ������ ����.
\ �믮������: ( -- )
\ ��⠭����� ���ﭨ� ������樨. [ ᫮�� ������������ �믮������.
  STATE 0!
; IMMEDIATE

: ] ( -- ) \ 94 CORE
\ ��⠭����� ���ﭨ� �������樨.
  TRUE STATE !
;

: QUIT ( -- ) ( R: i*x ) \ CORE 94
\ ������ �⥪ �����⮢, ������� ���� � SOURCE-ID.
\ ��⠭����� �⠭����� �室��� ��⮪ � ���ﭨ� ������樨.
\ �� �뢮���� ᮮ�饭��. �������� ᫥���饥:
\ - �ਭ��� ��ப� �� �室���� ��⮪� �� �室��� ����, ���㫨�� >IN
\   � ��⥯��஢���.
\ - �뢥�� ������饥 �� ॠ����樨 ��⥬��� �ਣ��襭��, �᫨
\   ��⥬� ��室���� � ���ﭨ� ������樨, �� ������ �����襭�,
\   � ��� ������������ ���権.

\ R0 @ RP! ( �� ������ �⮣�, �⮡� ��������� "['] QUIT CATCH" )
  CONSOLE-HANDLES
  0 TO SOURCE-ID
  [COMPILE] [
  <MAIN>
;

: MAIN1 ( -- )
  BEGIN REFILL
  WHILE INTERPRET OK.
  REPEAT _BYE
;
' MAIN1 TO <MAIN>

: SAVE-SOURCE ( -- i*x i )
  SOURCE-ID-XT  SOURCE-ID   >IN @   SOURCE   CURSTR @   6
;

: RESTORE-SOURCE ( i*x i  -- )
  6 <> IF ABORT THEN
  CURSTR !    SOURCE!  >IN !  TO SOURCE-ID   TO SOURCE-ID-XT
;

: EVALUATE-WITH ( ( i*x c-addr u xt -- j*x )
\ ���� c-addr u �室�� ��⮪��, ���᫨�� �� �������஬ xt.
  SAVE-SOURCE N>R 
  >R  SOURCE!  -1 TO SOURCE-ID
  R> ( ['] INTERPRET) CATCH
  NR> RESTORE-SOURCE
  THROW
;

: EVALUATE ( i*x c-addr u -- j*x ) \ 94
\ ���࠭�� ⥪�騥 ᯥ�䨪�樨 �室���� ��⮪�.
\ �����뢠�� -1 � SOURCE-ID. ������ ��ப�, �������� c-addr u,
\ �室�� ��⮪�� � �室�� ���஬, ��⠭�������� >IN � 0
\ � ���������. ����� ��ப� ࠧ��࠭� �� ���� - ����⠭��������
\ ᯥ�䨪�樨 �।��饣� �室���� ��⮪�.
\ ��㣨� ��������� �⥪� ��।������� �믮��塞묨 �� EVALUATE ᫮����.
  ['] INTERPRET EVALUATE-WITH
;

: FQUIT
	BEGIN REFILL
	WHILE INTERPRET
 REPEAT ;

: INCLUDE-FILE ( i*x fileid -- j*x ) \ 94 FILE
	>IN  @ >R
	SOURCE-ID >R  TO SOURCE-ID
	RP@ #TIB @ ALIGNED - RP!
	TIB	RP@ #TIB @ CMOVE
	SOURCE 2>R 
\	TCR ." IF"
	['] FQUIT CATCH	SAVEERR
\	['] NOOP CATCH	SAVEERR	

	2R> SOURCE!
	RP@ TIB  #TIB @ CMOVE
	RP@ #TIB @ ALIGNED + RP!
	R> TO SOURCE-ID
	R> >IN ! THROW      ;

: INCLUDED_  ( c-addr u ---- )
\ Open the file with name c-addr u and interpret all lines contained in it.
	R/O  OPEN-FILE THROW \ ABORT" Can't open include file"
	DUP >R
	['] INCLUDE-FILE CATCH
	R> CLOSE-FILE DROP THROW
;

: REQUIRED ( waddr wu laddr lu -- )
  2SWAP SFIND
  IF DROP 2DROP
  ELSE 2DROP INCLUDED_ THEN
;
: REQUIRE ( "word" "libpath" -- )
  PARSE-NAME PARSE-NAME 2DUP + 0 SWAP C!
  REQUIRED
;

: AUTOEXEC S" /sys/INIT.F" INCLUDED_ ;
