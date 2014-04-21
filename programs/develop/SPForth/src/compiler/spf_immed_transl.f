( ����� ������������ �믮������, �ᯮ��㥬� � ०��� �������樨.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
  ������஢���� ���ᨬ��� �.�.
  email:mak@mail.rtc.neva.ru
  http://informer.rtc.neva.ru/
  � � {812}105-92-03
  � � {812}552-47-64
)

: TO \ 94 CORE EXT
\ ��������: ( x "<spaces>name" -- )
\ �ய����� ����騥 �஡��� � �뤥���� name, ��࠭�祭��� �஡����.
\ ������� x � name. ����।������� ����� ���������, �᫨ name ��
\ ��।����� �१ VALUE.
\ ���������: ( "<spaces>name" -- )
\ �ய����� ����騥 �஡��� � �뤥���� name, ��࠭�祭��� �஡����.
\ �������� ᥬ��⨪� �६��� �믮������, ������ ����, � ⥪�饬� ��।������.
\ ����।������� ����� ���������, �᫨ name �� ��।����� �१ VALUE.
\ �६� �믮������: ( x -- )
\ ������� x � name.
\ �ਬ�砭��: ����।������� ����� ���������, �᫨ POSTPONE ��� [COMPILE]
\ �ਬ������� � TO.
  '
  >BODY CELL+ STATE @
  IF COMPILE, ELSE EXECUTE THEN
; IMMEDIATE

: COMPILE,_M COMPILE, ;

: POSTPONE \ 94
\ ��������: ᥬ��⨪� �� ��।�����.
\ ���������: ( "<spaces>name" -- )
\ �ய����� ����騥 ࠧ����⥫�. �뤥���� ���, ��࠭�祭��� �஡�����.
\ ���� ���. �������� ᥬ��⨪� �������樨 ����� � ⥪�饥 ��।������.
  ?COMP
  PARSE-WORD SFIND DUP
  0= IF -321 THROW THEN
  1 = IF COMPILE,
      ELSE LIT, ['] COMPILE,_M COMPILE, THEN
; IMMEDIATE

: \   \ 94 CORE EXT
\ ���������: �믮����� ᥬ��⨪� �믮������, ������ ����.
\ �믮������: ( "ccc<eol>" -- )
\ �뤥���� � ������ ���⮪ ࠧ��ࠥ��� ������.
\ \ - ᫮�� ������������ �ᯮ������.
  1 PARSE 2DROP
; IMMEDIATE

: .(  \ 94 CORE EXT
\ ���������: �믮����� ᥬ��⨪� �믮������, ������ ����.
\ �믮������: ( "ccc<paren>" -- )
\ �뤥���� � �뢥�� �� ��ᯫ�� ccc, ��࠭�祭�� �ࠢ�� ᪮���� ")".
\ .( - ᫮�� ������������ �ᯮ������.
  [CHAR] ) PARSE TYPE
; IMMEDIATE

: (  ( "ccc<paren>" -- ) \ 94 FILE
\ ������� ᥬ��⨪� CORE (, ����稢:
\ ����� ࠧ��ࠥ��� ⥪�⮢� 䠩�, �᫨ ����� ࠧ��ࠥ��� ������ ���⨣���
\ ࠭��, 祬 ������� �ࠢ�� ᪮���, ᭮�� ��������� �室��� ���� ᫥���饩
\ ��ப�� �� 䠩��, ��⠭����� >IN � ���� � �த������ ࠧ���, �������
\ ��� ����� �� �� ���, ���� �� �㤥� ������� �ࠢ�� ᪮��� ��� ��
\ �㤥� ���⨣��� ����� 䠩��.
  BEGIN
    [CHAR] ) DUP PARSE + C@ = 0=
  WHILE
    REFILL 0= IF EXIT THEN
  REPEAT
; IMMEDIATE

: [COMPILE]  \ 94 CORE EXT
\ ��������: ᥬ��⨪� ����।�����.
\ ���������: ( "<spaces>name" -- )
\ �ய����� ����騥 �஡���. �뤥���� name, ��࠭�祭��� �஡�����.
\ ���� name. �᫨ ��� ����� ���� ᥬ��⨪� �������樨, 祬 "��-㬮�砭��",
\ �������� �� � ⥪�饥 ��।������; ���� �������� ᥬ��⨪� �믮������ name.
\ ����।������� ����� ���������, �᫨ name �� �������.
  ?COMP
  '
  COMPILE,
; IMMEDIATE

: ; [;] ;  IMMEDIATE

: [;] ( -- )
  RET, [COMPILE] [ REVEAL
  ClearJpBuff
  0 TO LAST-NON
;

: EXIT
  RET,
; IMMEDIATE

: \EOF  ( -- )
\ �����稢��� �࠭���� ⥪�饣� ��⮪�
  BEGIN REFILL 0= UNTIL
  POSTPONE \
;

: FIELD  ( offset size "new-name< >" -- offset+size )
      : OVER
        DUP IF   DUP  LIT,  ['] + COMPILE,
            THEN DROP
       POSTPONE ;
       + ;  

0 [IF]
: --
  CREATE OVER , +
  (DOES1) (DOES2) @ +
;

[ELSE]
: -- FIELD ;
[THEN]
