( ��।����騥 ᫮��, ᮧ���騥 ᫮���� ���� � ᫮���.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

USER LAST-CFA
USER-VALUE LAST-NON

: REVEAL ( --- )
\ Add the last created definition to the CURRENT wordlist.
  LAST @ CURRENT @ ! ;

: SHEADER ( addr u -- )
  _SHEADER  REVEAL
;

: _SHEADER ( addr u -- )
  0 C,     ( flags )
  HERE 0 , ( cfa )
  DUP LAST-CFA !
  -ROT  WARNING @
  IF 2DUP GET-CURRENT SEARCH-WORDLIST
     IF DROP 2DUP TYPE ."  isn't unique" CR THEN
  THEN
  CURRENT @ SWORD,
  ALIGN
  HERE SWAP ! ( ��������� cfa )
;

: HEADER ( "name" -- )  PARSE-WORD SHEADER ;

: CREATED ( addr u -- )
\ ������� ��।������ ��� c-addr u � ᥬ��⨪�� �믮������, ���ᠭ��� ����.
\ �᫨ 㪠��⥫� ����࠭�⢠ ������ �� ��஢���, ��१�ࢨ஢��� ����
\ ��� ��ࠢ�������. ���� 㪠��⥫� ����࠭�⢠ ������ ��।����
\ ���� ������ name. CREATE �� १�ࢨ��� ���� � ���� ������ name.
\ name �믮������: ( -- a-addr )
\ a-addr - ���� ���� ������ name. �����⨪� �믮������ name �����
\ ���� ���७� � ������� DOES>.
  SHEADER
  HERE DOES>A ! ( ��� DOES )
  CREATE-CODE COMPILE,
;

: CREATE ( "<spaces>name" -- ) \ 94
   PARSE-WORD CREATED
;

: (DOES1) \ � ����, ����� ࠡ�⠥� �����६���� � CREATE (���筮)
  R> DOES>A @ CFL + -
  DOES>A @ 1+ ! ;

Code (DOES2)
   SUB  EBP, 4
   MOV [EBP], EAX
   POP  EBX
   POP  EAX
   PUSH EBX
   RET
EndCODE

: DOES>  \ 94
\ ��������: ᥬ��⨪� ����।�����.
\ ���������: ( C: clon-sys1 -- colon-sys2 )
\ �������� ᥬ��⨪� �६��� �믮������, ������ ����, � ⥪�饬�
\ ��।������. �㤥� ��� ��� ⥪�饥 ��।������ ᤥ���� ������
\ ��� ���᪠ � ᫮��� �� �������樨 DOES>, ������ �� ॠ����樨.
\ �����頥� colon-sys1 � �ந������ colon-sys2. �������� ᥬ��⨪�
\ ���樠����樨, ������ ����, � ⥪�饬� ��।������.
\ �६� �믮������: ( -- ) ( R: nest-sys1 -- )
\ �������� ᥬ��⨪� �믮������ ��᫥����� ��।������ name, �� ᥬ��⨪�
\ �믮������ name, ������ ����. �������� �ࠢ����� � ��뢠�饥 ��।�-
\ �����, �������� nest-sys1. ����।������� ����� ���������, �᫨ name
\ �� �뫮 ��।����� �१ CREATE ��� ��।������� ���짮��⥫�� ᫮��,
\ ��뢠�饥 CREATE.
\ ���樠������: ( i*x -- i*x a-addr ) ( R: -- nest-sys2 )
\ ���࠭��� ��������� �� ॠ����樨 ���ଠ�� nest-sys2 � ��뢠�饬
\ ��।������. �������� ���� ���� ������ name �� �⥪. �������� �⥪�
\ i*x �।�⠢���� ��㬥��� name.
\ name �믮������: ( i*x -- j*x )
\ �믮����� ���� ��।������, ����� ��稭����� � ᥬ��⨪� ���樠����樨,
\ ����������� DOES>, ���஥ ������஢��� name. �������� �⥪� i*x � j*x
\ �।�⠢���� ��㬥��� � १����� ᫮�� name, ᮮ⢥��⢥���.
  ['] (DOES1) COMPILE,
  ['] (DOES2) COMPILE,  \   ['] C-R>    MACRO, 
; IMMEDIATE

: VOCABULARY ( "<spaces>name" -- )
\ ������� ᯨ᮪ ᫮� � ������ name. �믮������ name ������� ���� ᯨ᮪
\ � ���浪� ���᪠ �� ᯨ᮪ � ������ name.
  WORDLIST DUP
  CREATE
  ,
  LATEST OVER CELL+ ! ( ��뫪� �� ��� ᫮���� )
  GET-CURRENT SWAP PAR! ( ᫮����-�।�� )
\  FORTH-WORDLIST SWAP CLASS! ( ����� )
  VOC
  ( DOES> �� ࠡ�⠥� � �⮬ ��)
  (DOES1) (DOES2) \ ⠪ ᤥ��� �� DOES>, ��।������ ���
  @ CONTEXT !
;

: VARIABLE ( "<spaces>name" -- ) \ 94
\ �ய����� ����騥 �஡���. �뤥���� name, ��࠭�祭��� �஡����.
\ ������� ��।������ ��� name � ᥬ��⨪�� �믮������, ������ ����.
\ ��१�ࢨ஢��� ���� �祩�� ����࠭�⢠ ������ � ��஢����� ���ᮬ.
\ name �ᯮ������ ��� "��६�����".
\ name �믮������: ( -- a-addr )
\ a-addr - ���� ��१�ࢨ஢����� �祩��. �� ���樠������ �祩�� �⢥砥� 
\ �ணࠬ��
  CREATE
  0 ,
;
: CONSTANT ( x "<spaces>name" -- ) \ 94
\ �ய����� ����騥 �஡���. �뤥���� name, ��࠭�祭��� �஡����.
\ ������� ��।������ ��� name � ᥬ��⨪�� �믮������, ������ ����.
\ name �ᯮ������ ��� "����⠭�".
\ name �믮������: ( -- x )
\ �������� x �� �⥪.
  HEADER
  CONSTANT-CODE COMPILE, ,
;
: VALUE ( x "<spaces>name" -- ) \ 94 CORE EXT
\ �ய����� ����騥 �஡���. �뤥���� name, ��࠭�祭��� �஡����. ������� 
\ ��।������ ��� name � ᥬ��⨪�� �믮������, ��।������� ����, � ��砫�� 
\ ���祭��� ࠢ�� x.
\ name �ᯮ������ ��� "���祭��".
\ �믮������: ( -- x )
\ �������� x �� �⥪. ���祭�� x - �, ���஥ �뫮 ����, ����� ��� ᮧ��������,
\ ���� �� �ᯮ������ �ࠧ� x TO name, ����� ����� ���祭�� x, 
\ ���樨஢����� � name.
  HEADER
  CONSTANT-CODE COMPILE, ,
  TOVALUE-CODE COMPILE,
;
: VECT ( -> )
  ( ᮧ���� ᫮��, ᥬ��⨪� �믮������ ���ண� ����� ������,
    �����뢠� � ���� ���� xt �� TO)
  HEADER
  VECT-CODE COMPILE, ['] NOOP ,
  TOVALUE-CODE COMPILE,
;

: ->VARIABLE ( x "<spaces>name" -- ) \ 94
  HEADER
  CREATE-CODE COMPILE,
  ,
;

: USER-ALIGNED ( -- a-addr n )
   USER-HERE 3 + 2 RSHIFT ( 4 / ) 4 * DUP
   USER-HERE -
;

: USER-CREATE ( "<spaces>name" -- )
  HEADER
  HERE DOES>A ! ( ��� DOES )
  USER-CODE COMPILE,
  USER-ALIGNED
  USER-ALLOT  ,
;
: USER ( "<spaces>name" -- ) \ ������� ��६���� ��⮪�
  USER-CREATE
  4 USER-ALLOT
;

' _TOUSER-VALUE-CODE TO TOUSER-VALUE-CODE

: USER-VALUE ( "<spaces>name" -- ) \ 94 CORE EXT
  HEADER
  USER-VALUE-CODE COMPILE,
  USER-ALIGNED SWAP ,
  CELL+ USER-ALLOT
  TOUSER-VALUE-CODE COMPILE,
;

: ->VECT ( x -> )
  HEADER
  VECT-CODE COMPILE, ,
  TOVALUE-CODE COMPILE,
;

: : _: ;

: _: ( C: "<spaces>name" -- colon-sys ) \ 94
\ �ய����� ����騥 ࠧ����⥫�. �뤥���� ���, ��࠭�祭��� �஡����.
\ ������� ��।������ ��� �����, ���뢠���� "��।������ �१ �����稥".
\ ��⠭����� ���ﭨ� �������樨 � ����� ⥪�饥 ��।������, ����稢
\ colon-sys. �������� ᥬ��⨪� ���樠����樨, ���ᠭ��� ����, � ⥪�饥
\ ��।������. �����⨪� �믮������ �㤥� ��।����� ᫮����, ᪮������-
\ ����묨 � ⥫� ��।������. ����饥 ��।������ ������ ���� ��������
\ �� ���᪥ � ᫮��� �� �� ���, ���� �� �㤥� �����襭�.
\ ���樠������: ( i*x -- i*x ) ( R: -- nest-sys )
\ ���࠭��� ���ଠ�� nest-sys � �맮�� ��।������. ����ﭨ� �⥪�
\ i*x �।�⠢��� ��㬥��� �����.
\ ��� �믮������: ( i*x -- j*x )
\ �믮����� ��।������ �����. ����ﭨ� �⥪� i*x � j*x �।�⠢����
\ ��㬥��� � १����� ����� ᮮ⢥��⢥���.
  PARSE-WORD _SHEADER ]
  HERE TO :-SET
;

\ S" ~mak\CompIF.f" INCLUDED

