
\ Temporary variables
( 24.09.1997 ��१�� �. )
\ April 12th, 2000 - 14:44 Mihail Maksimov
\ ������� �������樨 !! ... !! � >| ... | , �������஢�� |DOES

\ ��⨬���஢���� ��ਠ��. ��६���� ����� �ᯮ�짮���� � ����� DO LOOP
( 10.06.1999 Ruvim Pinka, idea - Mihail Maksimov )

( ���⮥ ���७�� ��-���� ������묨 ��६���묨.
  ����������� ��� �ᯮ�짮����� LOCALS �⠭���� 94.

  ������� �६����� ��६�����, ������� ⮫쪮 �����
  ⥪�饣� ᫮�� � ��࠭�祭��� �६���� �맮�� �������
  ᫮�� �믮������ � ������� ᫮�� "|" �������筮
  ����⮫��: ����� ��।������ ᫮�� �ᯮ������
  ���������
  | ᯨ᮪ �������� ��६����� �१ �஡�� |

  �� ���⠢��� ��-���� ��⮬���᪨ �뤥���� ���� �
  �⥪� �����⮢ ��� ��� ��६����� � ������ �맮�� ᫮��
  � ��⮬���᪨ �᢮������� ���� �� ��室� �� ����.

  ���饭�� � ⠪�� ������� ��६���� - ��� � �����
  ��६���� �� ����� � ᫥���騬� @ � !
  ����� �������� ��६����� �������� � �������᪮�
  ᫮��� TEMP-NAMES ⮫쪮 � ������ �������樨 ᫮��, �
  ��᫥ �⮣� �������� � ����� ������㯭�.
)
\ ���樠������ �६����� ��६����� ���祭�ﬨ, ����騬� ��
\ �⥪� (���ਬ��, �室�묨 ��ࠬ��ࠬ�), �������� "ᯨ᪮�"
\ � ������� �������樨
\ (( ����� ���樠�����㥬�� �������� ��६����� ))
\ ����� ������ ���� ࠭�� ������ � ᫮�� � ������� | ... |

( �ᯮ�짮����� �������� ��६����� ����� 横��� DO LOOP
  ���������� �� ��稭�, ���ᠭ��� � �⠭���� 94.

  �� ������� �ᯮ�짮���� ������� ��६���� � �⨫� VALUE-��६�����
  ����� �ᯮ�짮���� ���������
  || ᯨ᮪ �������� ��६����� �१ �஡�� ||
  ����� ��� ��६����� ���� ������ �� ����, � ᢮� ���祭��.
  ���⢥��⢥��� ��᢮���� ���祭�� �㤥� �����⢫����� �������樥�
  -> ���
  �� �������� � ��᢮����� ���祭�� VALUE-��६���� ᫮��� TO.
)

VARIABLE TEMP-CNT
WORDLIST CONSTANT TEMP-NAMES

: INIT-TEMP-NAMES
  ALSO TEMP-NAMES CONTEXT !
  TEMP-CNT 0!
;
: DEL-NAMES ( A -- )
  DUP>R
  @
  BEGIN
    DUP 0<>
  WHILE
    DUP CDR SWAP 5 - FREE THROW
  REPEAT DROP
  R> 0!
;
: DEL-TEMP-NAMES
  TEMP-NAMES DEL-NAMES
;
HEX
: COMPIL, ( A -- )
  0E8 DOES>A @ C! DOES>A 1+!              \ ��設��� ������� CALL
  DOES>A @ CELL+ - DOES>A @ !
  DOES>A @ 1- DOES>A !
;
DECIMAL
C" LAST-HERE" FIND NIP
[IF] 
  : TEMP-DOES ( N -- ) ( -- ADDR )
    ['] DUP MACRO,
    0x8D C, 0x44 C, 0x24 C, C,  \  LEA     EAX , X [ESP]
    HERE TO LAST-HERE  \  ࠧ�襭� ��⨬���஢���
    ;
[ELSE]
  : TEMP-DOES ( N -- ) ( -- ADDR )
     POSTPONE RP@ LIT, POSTPONE +  ;
[THEN]

: |TEMP-DOES ( N -- ) ( -- VALUE )
  TEMP-DOES ['] @ COMPILE,
;
: |TEMP-DOES! ( N --  ) ( X -- )
  TEMP-DOES ['] ! COMPILE,
;

VARIABLE  add_depth   add_depth 0!   

\ ��㡨�� � �⥪� �����⮢ �� ��砫� ��६�����

: !TEMP-CREATE ( addr u -- )
  DUP 20 + ALLOCATE THROW >R
  R@ CELL+ CHAR+ 2DUP C!
  CHAR+ SWAP MOVE ( name )
  TEMP-NAMES @
  R@ CELL+ CHAR+ TEMP-NAMES ! ( latest )
  R@ CELL+ CHAR+ COUNT + DUP>R ! ( link )
  R> CELL+ DUP DOES>A ! R@ ! ( cfa )
  &IMMEDIATE R> CELL+ C! ( flags )
  ['] _CREATE-CODE COMPIL,
  TEMP-CNT @ DOES>A @ 5 + !
  TEMP-CNT 1+!
  POSTPONE >R   DOES> @  2 +  CELLS  add_depth @ +  |TEMP-DOES ;

: TEMP-CREATE ( addr u -- )
 !TEMP-CREATE   DOES> @  2 +  CELLS  add_depth @ +   TEMP-DOES ;

: ->            ' 5 + @  2 +  CELLS  add_depth @ +  |TEMP-DOES!
; IMMEDIATE

: |DROP    R> RP@ + RP! ;

' |DROP VALUE '|DROP

: !!!!;  ( N N1 -- )
  DROP  TEMP-CNT @ CELLS LIT,    POSTPONE >R
  DROP            '|DROP LIT,    POSTPONE >R ;

: !!
  BEGIN  NextWord 2DUP S" !!" COMPARE 0<>
  WHILE          !TEMP-CREATE
  REPEAT  !!!!;  ; IMMEDIATE

: ||
  BEGIN  NextWord 2DUP S" ||" COMPARE 0<>
  WHILE  0 LIT,  !TEMP-CREATE
  REPEAT !!!!;   ; IMMEDIATE

: |
  BEGIN  NextWord 2DUP S" |"  COMPARE 0<>
  WHILE  0 LIT,   TEMP-CREATE
  REPEAT !!!!;   ; IMMEDIATE

: >|
  BEGIN  NextWord 2DUP S" |"  COMPARE 0<>
  WHILE           TEMP-CREATE
  REPEAT !!!!;   ; IMMEDIATE

: ((
  0
  BEGIN
    BL WORD DUP COUNT S" ))" COMPARE 0<>
  WHILE
    FIND IF >R 1+ ELSE 5012 THROW THEN
  REPEAT DROP
  BEGIN
    DUP 0<>
  WHILE
\    R> EXECUTE POSTPONE !     ( ��ࠢ���� ��� �����প� || )
    R> 5 + @    2 + CELLS add_depth @ + 
    |TEMP-DOES!
    1-
  REPEAT DROP
; IMMEDIATE


\ ===
\ ��८�।������ ᮮ⢥������� ᫮� ��� ���������� �ᯮ�짮����
\ �६���� ��६���� �����  横�� DO LOOP  � ������ᨬ� �� ���������
\ ᮤ�ন���� �⥪� �����⮢  ᫮����   >R   R>


: DO     POSTPONE DO     [  3 CELLS ] LITERAL  add_depth +!
; IMMEDIATE

: LOOP   POSTPONE LOOP   [ -3 CELLS ] LITERAL  add_depth +!
; IMMEDIATE

: +LOOP  POSTPONE +LOOP  [ -3 CELLS ] LITERAL  add_depth +!
; IMMEDIATE

: >R     POSTPONE >R     [  1 CELLS ] LITERAL  add_depth +!
; IMMEDIATE

: R>     POSTPONE R>     [ -1 CELLS ] LITERAL  add_depth +!
; IMMEDIATE

\ ===


: :: : ;

: : ( -- )
  : INIT-TEMP-NAMES
;
:: ; ( -- )
  DEL-TEMP-NAMES PREVIOUS
  POSTPONE ;
  add_depth 0!      \ �� ��直� ��砩 ;)
; IMMEDIATE

