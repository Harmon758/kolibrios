( 28.Mar.2000 Andrey Cherezov  Copyright [C] RU FIG

  �ᯮ�짮���� ���� ᫥����� ���஢:
  Ruvim Pinka; Dmitry Yakimov; Oleg Shalyopa; Yuriy Zhilovets;
  Konstantin Tarasov; Michail Maximov.

  !! ����⠥� ⮫쪮 � SPF4.
)

( ���⮥ ���७�� ��-���� ������묨 ��६���묨.
  ����������� ��� �ᯮ�짮����� LOCALS �⠭���� 94.

  ������� �६����� ��६�����, ������� ⮫쪮 �����
  ⥪�饣� ᫮�� � ��࠭�祭��� �६���� �맮�� �������
  ᫮�� �믮������ � ������� ᫮�� "{". ����� ��।������ 
  ᫮�� �ᯮ������ ���������, �������� �⥪���� ���樨 ����
  { ᯨ᮪_���樠����஢�����_������� \ �.������.������� -- �� 㣮��� }
  ���ਬ��:

  { a b c d \ e f -- i j }

  ��� { a b c d \ e f[ EVALUATE_��ࠦ���� ] -- i j }
  �� ����� �� ��� ��६����� f[ �㤥� �뤥��� �� �⥪� �����⮢ ���⮪
  ����� ������ n ����. �ᯮ�짮����� ��६����� f[ ���� ���� ��砫� �⮣�
  ���⪠. \� �⨫� MPE\

  ��� { a b c d \ e [ 12 ] f -- i j }
  �� ����� �� ��� ��६����� f �㤥� �뤥��� �� �⥪� �����⮢ ���⮪
  ����� ������ 12 ����. �ᯮ�짮����� ��६����� f ���� ���� ��砫� �⮣�
  ���⪠. 

  ����� "\ �.������.�������" ����� ������⢮����, ���ਬ��:

  { item1 item2 -- }

  �� ���⠢��� ��-���� ��⮬���᪨ �뤥���� ���� �
  �⥪� �����⮢ ��� ��� ��६����� � ������ �맮�� ᫮��
  � ��⮬���᪨ �᢮������� ���� �� ��室� �� ����.

  ���饭�� � ⠪�� ������� ��६���� - ��� � VALUE-��६����
  �� �����. �᫨ �㦥� ���� ��६�����, � �ᯮ������ "^ ���"
  ��� "AT ���".


  ����� \ ����� �ᯮ�짮���� |
  ����� -> ����� �ᯮ�짮���� TO

  �ਬ���:

  : TEST { a b c d \ e f -- } a . b . c .  b c + -> e  e .  f .  ^ a @ . ;
   Ok
  1 2 3 4 TEST
  1 2 3 5 0 1  Ok

  : TEST { a b -- } a . b . CR 5 0 DO I . a . b . CR LOOP ;
   Ok
  12 34 TEST
  12 34
  0 12 34
  1 12 34
  2 12 34
  3 12 34
  4 12 34
   Ok

  : TEST { a b } a . b . ;
   Ok
  1 2 TEST
  1 2  Ok

  : TEST { a b \ c } a . b . c . ;
   Ok
  1 2 TEST
  1 2 0  Ok

  : TEST { a b -- } a . b . ;
   Ok
  1 2 TEST
  1 2  Ok

  : TEST { a b \ c -- d } a . b . c . ;
   Ok
  1 2 TEST
  1 2 0  Ok

  : TEST { \ a b } a . b .  1 -> a  2 -> b  a . b . ;
   Ok
  TEST
  0 0 1 2  Ok

  ����� �������� ��६����� �������� � �������᪮�
  �६����� ᫮��� ⮫쪮 � ������ �������樨 ᫮��, �
  ��᫥ �⮣� �������� � ����� ������㯭�.

  �ᯮ�짮���� ��������� "{ ... }" ����� ������ ��।������ �����
  ⮫쪮 ���� ࠧ.

  ��������� �⮩ ������⥪� �������� � ⥪�騩 ᫮���� �������樨
  ���쪮 ��� ᫮��:
  ᫮���� "vocLocalsSupport" � "{"
  �� ��⠫�� ��⠫� "���⠭�" � ᫮���, �ᯮ�짮���� ��
  �� ४���������.
)

REQUIRE [IF] ~MAK\CompIF.f

C" 'DROP_V" FIND NIP 0=
[IF]  ' DROP VALUE 'DROP_V
: 'DROP 'DROP_V ;
[THEN]

C" 'DUP_V" FIND NIP 0=
[IF]  ' DUP VALUE 'DUP_V
:  'DUP  'DUP_V ;
[THEN]

C" 'DROP" FIND NIP 0=
[IF]  ' DROP VALUE 'DROP
[THEN]

C" 'DUP" FIND NIP 0=
[IF]  ' DUP VALUE 'DUP
[THEN]

\ C" '(LocalsExit)_V" FIND NIP 0=
\ [IF]  ' (LocalsExit)_V VALUE '(LocalsExit)_V
\ [THEN]

MODULE: vocLocalsSupport_M

VARIABLE uLocalsCnt
VARIABLE uLocalsUCnt
VARIABLE uPrevCurrent
VARIABLE uAddDepth

: LocalOffs ( n -- offs )
  2+ CELLS uAddDepth @ +
;

BASE @ HEX
 
' RP@ 7 + @ 0xC3042444 = 

[IF]

: R_ALLOT, 
  DUP  SHORT?
  OPT_INIT SetOP
  IF    8D C, 64 C, 24 C,  C, \ mov esp, offset [esp]
  ELSE  8D C, A4 C, 24 C,  , \ mov esp, offset [esp]
  THEN
  OPT_CLOSE
;  

C" MACRO," FIND NIP 0= 
[IF] : MACRO, INLINE,  ;
[THEN]

: CompileLocalRec ( u -- )
  LocalOffs DUP   
  'DUP MACRO,
  SHORT?
  OPT_INIT SetOP
  IF    8D C, 44 C, 24 C, C, \ lea eax, offset [esp]
  ELSE  8D C, 84 C, 24 C,  , \ lea eax, offset [esp]
  THEN  OPT
  OPT_CLOSE
;

: CompileLocal@ ( n -- )
  'DUP MACRO,
  LocalOffs DUP  SHORT?
  OPT_INIT SetOP
  IF    8B C, 44 C, 24 C, C, \ mov eax, offset [esp]
  ELSE  8B C, 84 C, 24 C,  , \ mov eax, offset [esp]
  THEN  OPT
  OPT_CLOSE
;

: CompileLocal! ( n -- )
  LocalOffs DUP  SHORT?
  OPT_INIT SetOP
  IF    89 C, 44 C, 24 C, C, \ mov  offset [esp], eax
  ELSE  89 C, 84 C, 24 C,  , \ mov  offset [esp], eax
  THEN  OPT
  OPT_CLOSE
  'DROP MACRO,
;

\ : CompileLocal@ ( n -- )
\   LocalOffs LIT, POSTPONE RP+@
\ ;


[ELSE]

: R_ALLOT,
 ] POSTPONE LITERAL  S"  RP@ + RP! " EVALUATE
 POSTPONE [ ;

: CompileLocalRec ( u -- )
  LocalOffs
  POSTPONE LITERAL
\  S"  RP@ + " EVALUATE
;

: CompileLocal@ ( n -- )
  CompileLocalRec 
  S" @ " EVALUATE
;

: CompileLocal! ( n -- )
  CompileLocalRec
  S" ! " EVALUATE
;

[THEN]

VARIABLE TEMP-DP

: CompileLocalsInit
  TEMP-DP @ DP ! 
  uPrevCurrent @ SET-CURRENT
  uLocalsUCnt @ ?DUP
  IF NEGATE CELLS R_ALLOT,
  THEN
  uLocalsCnt @ uLocalsUCnt @ - ?DUP 
  IF DUP CELLS NEGATE uAddDepth +!  0 DO  S" >R " EVALUATE LOOP THEN
  uLocalsCnt  @ ?DUP 
  IF CELLS POSTPONE LITERAL S" >R ['] (LocalsExit) >R" EVALUATE
     -2 CELLS uAddDepth +!
  THEN
;


\ : CompileLocal@ ( n -- )
\   LocalOffs LIT, POSTPONE RP+@
\ ;


BASE !

WORDLIST CONSTANT widLocals@

CREATE  TEMP-BUF 1000 ALLOT

: LocalsStartup
  GET-CURRENT uPrevCurrent !
  ALSO vocLocalsSupport_M
  ALSO widLocals@ CONTEXT ! DEFINITIONS
  HERE TEMP-DP !
  TEMP-BUF DP ! 
  widLocals@  0!
  uLocalsCnt 0!
  uLocalsUCnt 0!
  uAddDepth 0!
;
: LocalsCleanup
  PREVIOUS PREVIOUS
;

: ProcessLocRec ( "name" -- u )
  [CHAR] ] PARSE
  STATE 0!
  EVALUATE CELL 1- + CELL / \ ������ ���� 4
  -1 STATE ! 
\  DUP uLocalsCnt +!
  uLocalsCnt @
;

: CreateLocArray
  [CHAR] [ PSKIP
  ProcessLocRec
  CREATE ,
  DUP uLocalsCnt +!  
;

: LocalsRecDoes@ ( -- u )
  DOES>  @ CompileLocalRec
;

: LocalsRecDoes@2 ( -- u )
  ProcessLocRec , 
  DUP uLocalsCnt +!
  DOES> @ CompileLocalRec
;

: LocalsDoes@
  uLocalsCnt @ ,
  uLocalsCnt 1+!
  DOES>  @ CompileLocal@
;

: ;; POSTPONE ; ; IMMEDIATE


: ^ 
  ' >BODY @ 
  CompileLocalRec
; IMMEDIATE


: -> ' >BODY @ CompileLocal!  ; IMMEDIATE

WARNING DUP @ SWAP 0!

: AT
  [COMPILE] ^
; IMMEDIATE

: TO ( "name" -- )
  >IN @ NextWord widLocals@ SEARCH-WORDLIST 1 =
  IF >BODY @ CompileLocal! DROP
  ELSE >IN ! [COMPILE] TO
  THEN
; IMMEDIATE

WARNING !

: � POSTPONE -> ; IMMEDIATE

WARNING @ WARNING 0!
\ ===
\ ��८�।������ ᮮ⢥������� ᫮� ��� ���������� �ᯮ�짮����
\ �६���� ��६���� �����  横�� DO LOOP  � ������ᨬ� �� ���������
\ ᮤ�ন���� �⥪� �����⮢  ᫮����   >R   R>
C" DO_SIZE" FIND NIP 0=
[IF] 3 CELLS CONSTANT DO_SIZE
[THEN]


: DO    POSTPONE DO      DO_SIZE              uAddDepth +! ; IMMEDIATE
: ?DO   POSTPONE ?DO     DO_SIZE              uAddDepth +! ; IMMEDIATE
: LOOP  POSTPONE LOOP    DO_SIZE NEGATE       uAddDepth +! ; IMMEDIATE
: +LOOP POSTPONE +LOOP   DO_SIZE NEGATE       uAddDepth +! ; IMMEDIATE
: >R    POSTPONE >R     [  1 CELLS ] LITERAL  uAddDepth +! ; IMMEDIATE
: R>    POSTPONE R>     [ -1 CELLS ] LITERAL  uAddDepth +! ; IMMEDIATE
: RDROP POSTPONE RDROP  [ -1 CELLS ] LITERAL  uAddDepth +! ; IMMEDIATE
: 2>R   POSTPONE 2>R    [  2 CELLS ] LITERAL  uAddDepth +! ; IMMEDIATE
: 2R>   POSTPONE 2R>    [ -2 CELLS ] LITERAL  uAddDepth +! ; IMMEDIATE

\ ===

\  uLocalsCnt  @ ?DUP 
\  IF CELLS RLIT, ['] (LocalsExit) RLIT, THEN

: ;  LocalsCleanup
     S" ;" EVAL-WORD
; IMMEDIATE

WARNING !

\ =====================================================================


EXPORT

: {
  
  LocalsStartup
  BEGIN
    BL PSKIP PeekChar DUP [CHAR] \ <> 
                    OVER [CHAR] - <>  AND
                    OVER [CHAR] } <>  AND
                    OVER [CHAR] | <>  AND
                    SWAP [CHAR] ) XOR AND
  WHILE
    CREATE LocalsDoes@ IMMEDIATE
  REPEAT
  PeekChar >IN 1+! DUP [CHAR] } <>
  IF
     DUP [CHAR] \ =
    SWAP [CHAR] | = OR
    IF
      BEGIN
        BL PSKIP PeekChar DUP 
         DUP [CHAR] - <> 
        SWAP [CHAR] } <>  AND
        SWAP [CHAR] ) XOR AND
      WHILE
        PeekChar [CHAR] [ =
        IF  CreateLocArray  LocalsRecDoes@
        ELSE
             CREATE LATEST DUP C@ + C@
             [CHAR] [ =
             IF  
               LocalsRecDoes@2
             ELSE
               LocalsDoes@ 1
             THEN
        THEN  DUP U.
        uLocalsUCnt +!
        IMMEDIATE
      REPEAT
    THEN
    [CHAR] } PARSE 2DROP
  ELSE DROP THEN
  CompileLocalsInit
;; IMMEDIATE

;MODULE
