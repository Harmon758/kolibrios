( �������� ᫮����� ��⥩ � ᫮��३ WORDLIST.
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
HEX
1 CONSTANT &IMMEDIATE \ ����⠭� ��� ���祭�� 䫠��� IMMEDIATE
2 CONSTANT &VOC

\ �������� wid - �����䨪��� ᯨ᪠ ᫮�, ������饣� �� �⠭����� 
\ ᫮��, ���ᯥ稢���� ॠ����樥�. ��� ᯨ᮪ ᫮� ����砫쭮 ᯨ᮪ 
\ �������樨 � ���� ��砫쭮�� ���浪� ���᪠.
: >BODY ( xt -- a-addr ) \ 94
\ a-addr - ���� ���� ������, ᮮ⢥�����騩 xt.
\ �᪫��⥫쭠� ����� ���������, �᫨ xt �� �� ᫮��,
\ ��।�������� �१ CREATE.
(  1+ @ �뫮 � ���ᨨ 2.5 )
  5 +
;

: SWORD, ( addr u wid -> ) \ ���������� ��������� ���� � ������,
         \ ������� ��ப�� addr u, � ᯨ��, ��������� wid.
         \ ��ନ��� ⮫쪮 ���� ����� � �裡 �
         \ �⢥������ ����� �� ALLOT.
  HERE CELL+
  DUP LAST !
  SWAP DUP @ , !
  S, 0 C,
;

: WORDLIST ( -- wid ) \ 94 SEARCH
\ ������� ���� ���⮩ ᯨ᮪ ᫮�, ������� ��� �����䨪��� wid.
\ ���� ᯨ᮪ ᫮� ����� ���� �����饭 �� �।���⥫쭮 ��।������� 
\ ᯨ᪮� ᫮� ��� ����� �������᪨ ��।������� � ����࠭�⢥ ������.
\ ���⥬� ������ ����᪠�� ᮧ����� ��� ������ 8 ����� ᯨ᪮� ᫮� � 
\ ���������� � ����騬�� � ��⥬�.
  HERE VOC-LIST  @ ,  VOC-LIST !
  HERE 0 , \ ����� �㤥� 㪠��⥫� �� ��� ��᫥����� ᫮�� ᯨ᪠
       0 , \ ����� �㤥� 㪠��⥫� �� ��� ᯨ᪠ ��� ����������
       0 , \ wid ᫮����-�।��
       0 , \ ����� ᫮���� = wid ᫮����, ��।����饣� ᢮��⢠ �������
;


: CLASS! ( cls wid -- ) CELL+ CELL+ CELL+ ! ;
: CLASS@ ( wid -- cls ) CELL+ CELL+ CELL+ @ ;
: PAR!   ( Pwid wid -- ) CELL+ CELL+ ! ;
: PAR@   ( wid -- Pwid ) CELL+ CELL+ @ ;


: ID. ( NFA[E] -> )
  ZCOUNT TYPE
;

\ -9 -- flags
\ -8 -- cfa
\ -4 -- LFA
\  0 -- NFA

Code NAME>L ;( NFA -> LFA )
	LEA EAX, [EAX-4]
     RET
EndCode

Code NAME>C ;( NFA -> 'CFA )
	LEA EAX, [EAX-8]
     RET
EndCode

Code NAME> ;( NFA -> CFA )
	MOV EAX, [EAX-8]
     RET
EndCode

Code NAME>F ;( NFA -> FFA )
	LEA EAX, [EAX-9]
     RET
EndCode

Code CDR ;( NFA1 -> NFA2 )
     OR EAX, EAX
     SIF 0<>
		MOV EAX, [EAX-4]
     STHEN
     RET
EndCode

: ?IMMEDIATE ( NFA -> F )
  NAME>F C@ &IMMEDIATE AND
;

: ?VOC ( NFA -> F )
  NAME>F C@ &VOC AND
;
0 [IF]
: IMM ( -- ) \ 94
\ ������� ��᫥���� ��।������ ᫮��� ������������ �ᯮ������.
\ �᪫��⥫쭠� ����� ���������, �᫨ ��᫥���� ��।������
\ �� ����� �����.
  LAST @ NAME>F DUP C@ &IMMEDIATE OR SWAP ." I=" 2DUP H. H.
;
: IMMEDIATE ( -- ) \ 94
\ ������� ��᫥���� ��।������ ᫮��� ������������ �ᯮ������.
\ �᪫��⥫쭠� ����� ���������, �᫨ ��᫥���� ��।������
\ �� ����� �����.
  LAST @ NAME>F DUP C@ &IMMEDIATE OR SWAP C!
;
[THEN]
: VOC ( -- )
\ ������� ��᫥���� ��।������� ᫮�� �ਧ����� "᫮����".
  LAST @ NAME>F DUP C@ &VOC OR SWAP C!
;

\ ==============================================
\ �⫠��� - ���� ᫮�� �� ����� � ��� ⥫�


\ ==============================================
\ �⫠��� - ���� ᫮�� �� ����� � ��� ⥫�

: N_UMAX ( nfa nfa1 -- nfa|nfa1 )
 OVER DUP IF NAME> THEN
 OVER DUP IF NAME> THEN U< IF NIP EXIT THEN DROP ;

: WL_NEAR_NFA ( addr wid - addr nfa | addr 0 )
   @
   BEGIN 2DUP DUP IF NAME> THEN U<
   WHILE CDR
   REPEAT
;

0
[IF]

: NEAR_NFA ( addr - nfa addr | 0 addr )
   0 SWAP 
   VOC-LIST
    BEGIN  @ DUP
    WHILE    DUP >R CELL+ WL_NEAR_NFA SWAP >R N_UMAX R>  R>
    REPEAT   DROP
;

[ELSE]

: WL_NEAR_NFA_N ( addr nfa - addr nfa | addr 0 )
   BEGIN 2DUP DUP IF NAME> THEN U<
   WHILE CDR
   REPEAT
;

: WL_NEAR_NFA_M (  addr wid - nfa2 addr | 0 addr )
   0 -ROT
   CELL+ @
   BEGIN  DUP
   WHILE  WL_NEAR_NFA_N  \  nfa addr nfa1
       SWAP >R 
       DUP  >R  N_UMAX 
       R>  DUP  IF CDR THEN
       R>  SWAP
   REPEAT DROP
;

: NEAR_NFA ( addr - nfa addr | 0 addr )
   0 SWAP 
   VOC-LIST
   BEGIN  @ DUP
   WHILE  DUP  >R   WL_NEAR_NFA_M
   >R  N_UMAX  R>  R>
   REPEAT DROP
;

[THEN]

: WordByAddr  ( addr -- c-addr u )
\ ���� ᫮��, ⥫� ���ண� �ਭ������� ����� ����
   DUP         DP @ U> IF DROP S" <not in the image>" EXIT THEN
   NEAR_NFA DROP  DUP 0= IF DROP S" <not found>"        EXIT THEN
   COUNT
;


DECIMAL