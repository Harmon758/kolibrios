CR .( SFF.F)
\ ' ANSI>OEM TO ANSI><OEM 
\ ' OEM>ANSI TO ANSI><OEM 

REQUIRE DUPENDCASE ~mak/case.f

CR .( DC=) ' DUPENDCASE .

VARIABLE START-LAB
VARIABLE FINISH-LAB
VARIABLE START-LIST
VARIABLE FINISH-LIST
VARIABLE START-LIST2
VARIABLE FINISH-LIST2
VARIABLE FINISH-LIST3
VARIABLE START-VAR
VARIABLE FINISH-VAR
VARIABLE START-ARRAY
VARIABLE FINISH-ARRAY
 0 VALUE IMAGE-END
S" lib/ext/disasm2.f" INCLUDED
 1000 CELLS ALLOCATE DROP DUP START-VAR   ! FINISH-VAR   !
 1000 CELLS ALLOCATE DROP DUP START-LIST  ! FINISH-LIST  !
 1000 CELLS ALLOCATE DROP DUP START-LIST2 ! FINISH-LIST2 !
 1000 CELLS ALLOCATE DROP DUP START-LAB   ! FINISH-LAB   !
 100  CELLS ALLOCATE DROP DUP START-ARRAY ! FINISH-ARRAY !

\ REQUIRE [IF] ~mak/CompIF.f
REQUIRE [IFNDEF] ~nn/lib/ifdef.f
\ REQUIRE (* ~af/lib/comments.f

: CC HERE DROP ; IMMEDIATE

: KDD KEY DROP ;

VARIABLE HSSSS
VARIABLE ZSSSS
0 VALUE ALITERAL-CODE

\ !!! REQUIRE Z" ~mak/~af/lib/c/zstr.f

[IFNDEF] PARSE-WORD : PARSE-WORD NextWord  ;
[THEN]

[IFNDEF] PSKIP  : PSKIP SKIP ;
[THEN]
\ : KEY MKEY ;: KEY? MKEY? ;
\ REQUIRE SEE lib/ext/disasm.f 
\ REQUIRE SEE lib/ext/disasm1.f 
: B, C, ; : B@ C@ ; : B! C! ; : /CHAR 1 ;
: PARSE-NAME NextWord ;
\ : UMIN 2DUP U< IF DROP EXIT THEN NIP ;

REQUIRE {              ~ac\lib\locals.f

\ REQUIRE {              lib\ext\locals.f

[IFDEF] z z : z d
[THEN]

[IFDEF] d z ; POSTPONE d d IMMEDIATE
[THEN]

WARNING 0! \ ����� �� ���� ��������� isn't unique

S" lib/include/tools.f"                     INCLUDED

C" CELL-"  FIND NIP 0=
[IF] : CELL- 1 CELLS - ; 
[THEN]
C" U>"  FIND NIP 0=
[IF] : U> SWAP U< ; 
[THEN]
C" D-" FIND NIP 0=
[IF]
: D- ( D1 D2  -- FLAG )
      DNEGATE D+ ;
[THEN]
C" D=" FIND NIP 0=
[IF]
: D= ( D1 D2  -- FLAG )
       D- D0= ;
[THEN]

C" \S" FIND NIP 0=
[IF]
: \S            \ comment to end of file
     SOURCE-ID FILE-SIZE DROP
     SOURCE-ID REPOSITION-FILE DROP
     [COMPILE] \ ; IMMEDIATE
[THEN]

S" ~mak/utils_.f"                    INCLUDED

\ S" lib/ext/spf-asm.f"            INCLUDED
\ ALSO ASSEMBLER ALSO ASM-HIDDEN
\ '  NOOP IS  CODE-ALIGN 
\ PREVIOUS PREVIOUS

S" ~mak/asm/ASM.FRT"                   INCLUDED


\ S" lib/include/tools.f"          INCLUDED


C" LAST-HERE" FIND NIP 0= VALUE INLINEVAR

' DUP VALUE 'DUP
0 VALUE RESERVE
USER-HERE CONSTANT USER-HERE-SET
USER-HERE-SET TO RESERVE
MODULE: GSPF0
 S" src/global.f"              INCLUDED
;MODULE

S" src/global.f"              INCLUDED
S" src/tc_spfopt.f"           INCLUDED

' _CONSTANT-CODE	TO CONSTANT-CODE
' _CREATE-CODE		TO CREATE-CODE
' _CLITERAL-CODE	TO CLITERAL-CODE      
' _SLITERAL-CODE	TO SLITERAL-CODE      

\ : TOMM_SIZE TO MM_SIZE ;

 DIS-OPT
\ VOCABULARY GSPF0
: ?HS
 HERE           CELL- @ 
 HERE HSSSS @ + CELL- @  <>
IF CR
   HERE           CELL- @ H.
   HERE HSSSS @ + CELL- @ H.
   -1  ABORT"  HSSSS "
THEN
;
: TT 0 IF THEN ;

\ : CODE ?HS CODE ;

\ ALSO GGSPF0
 ALSO GSPF0 DEFINITIONS

: >R POSTPONE >R ; IMMEDIATE
: R> POSTPONE R> ; IMMEDIATE
CR
 0x10 TOMM_SIZE
HERE   DUP H. 
 HERE 0xF OR 1+ DP !
HERE   DUP H.  MM_SIZE H. 

  HERE ZSSSS  !   0 HSSSS  !
0x11223344 , 0x55667788 , ?HS
 S" src/gspf0.f"              INCLUDED
CR MM_SIZE H.

PREVIOUS  ( PREVIOUS  ) DEFINITIONS

\ ALSO GSPF0
MM_SIZE H.
[IFDEF] S"_L"  S" _LL"   S"_L" PLACE [THEN]

\  S" src/global.f"              INCLUDED
CR
 0x10 TOMM_SIZE
HERE   DUP H. 
 HERE 0xF OR 1+ DP !
HERE   DUP H.  MM_SIZE H.
 ZSSSS @ HERE - HSSSS !
0x11223344 , 0x55667788 , ?HS
S" src/gspf0.f"              INCLUDED

\  ��� ����� ���������� ����� �� ���� ����� ��� �����.
\  ����� �� ���� ������, ����� �� ���� ������� ������ ���� ���������. ������ ����.
\  HERE TO IMAGE-END

 \ VARIABLE lm
\  VECT m
 \ 0 CONSTANT m
\  : doTest 2000000 0 DO m @  2 +  I @ 4 + * I ! LOOP  ;
\ ���������������� ���� ��� ������ � ���������� ���� * ����� �������� ���� ��� ����������� 
\ ��� ���������� ������ ���� ������  
\   : doTest 10 0 DO  I .  LOOP  ;

 \ ��� ��������� ������������ ��� ������.

\ ����� ����������� �������� �� �����������.

\ H-STDOUT VALUE File
\ HERE TO IMAGE-END
\ : File:
\    NextWord DUP >R
\    HEAP-COPY DUP R> R/W CREATE-FILE-SHARED THROW TO File FREE THROW
\  ;

\ File: Test.log \ ��� ��� ����� ���� �������.

\ H-STDOUT >R File TO H-STDOUT DROP \ ��������� ������
\ ��������� ������������

\ S" example.f" INCLUDED

\ ����� ������� �� ����� ����������������.
\  ���� � ����� ��������� ����� ����������� ��������� ����-��������, ��
\  ����� ���������������� ������. ����� ��� ����� ��������� �� ����� ������ �����������.
\ ���� ����� ��� ���������, �� ��������������� ������� �����������.
\ * ���� ���
\ '  doTest DISASM-LIST  \ ������ " ' " �������� ����� ���������� �� ��� �����, �����  DISASM-LIST ������� �� ��������.
\ '  WildCMP-U1 DISASM-LIST
\ �����
\ * � ����� ��� 
\ TYPE-ALL

\ File FLUSH-FILE THROW R> TO H-STDOUT \ ���������� ������
 \ CR
 \ : B I I I */ ;
\ ' B SEE2 
\ SEE  B
 \ ' doTest DISASM-LIST
\ ' doTest SEE2
\ ' m SEE2
 \ ' B DISASM-LIST  \ ������ " ' " �������� ����� ���������� �� ��� �����, �����  DISASM-LIST ������� �� ��������.
\ �����

 \ CR TYPE-ALL

\ START-LIST @ FREE DROP
\ START-LIST2 @ FREE DROP
\ START-LAB  @ FREE DROP
\ START-VAR  @ FREE DROP
\ START-ARRAY @ FREE DROP
