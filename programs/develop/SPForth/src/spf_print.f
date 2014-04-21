( ����� ��������� ������ �����.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �������������� �� 16-���������� � 32-��������� ��� - 1995-96��
  ������� - �������� 1999 [������� �� USER-���������� �
  ������ CODE-���� ���������������� �������������]
)

4096 DUP CONSTANT NUMERIC-OUTPUT-LENGTH
USER-CREATE SYSTEM-PAD
USER-ALLOT \ ������� ���������� �������������� - ����������� ����� PAD

: HEX ( -- ) \ 94 CORE EXT
\ ���������� ���������� BASE ������ �����������.
  16 BASE !
;

: DECIMAL ( -- ) \ 94
\ ���������� ��������� ������� ��������� ������ ������.
  10 BASE !
;

: HOLD ( char -- ) \ 94
\ �������� char � ������ ��������� �������� ������.
\ �������������� �������� ���������, ���� ������������ HOLD
\ ��� <# � #>, ������������������� �������������� �����.
  HLD @ 1- DUP HLD ! C!
;

: HOLDS ( addr u -- ) \ from eserv src
  TUCK + SWAP 0 ?DO DUP I - 1- C@ HOLD LOOP DROP
;

: <# ( -- ) \ 94
\ ������ ��������� �������������� �����.
  PAD 1- HLD !
  0 PAD 1- C!
;

: DIGIT> ( c -- c1 )
 DUP 10 < 0= IF 7 + THEN 48 + ;

: # ( ud1 -- ud2 ) \ 94
\ �������� ud1 �� �������� BASE �������� ���� ����� � ����� �
\ �������� �� � ����� ���������� �������������� �����,
\ ������� ������� ud2.
\ �������������� �������� ���������, ���� ������������ #
\ ��� <# � #>, ������������������� �������������� �����.
  0 BASE @ UM/MOD >R BASE @ UM/MOD R>
  ROT DIGIT> HOLD
;

: #S ( ud1 -- ud2 ) \ 94
\ �������� ����� D1 �� ����� # �� ��������� ����.
\ ud2 - ����.
\ �������������� �������� ���������, ���� ������������ #S
\ ��� <# � #>, ������������������� �������������� �����.
  BEGIN
    # 2DUP D0=
  UNTIL
;

: #> ( xd -- c-addr u ) \ 94
\ ������ xd. ������� ����� ���������� �������������� ��������� � ����
\ ������ ��������, �������� c-addr � u.
\ ��������� ����� ������ ������� � ���� ������.
  2DROP HLD @ PAD OVER - 1-
;

: SIGN ( n -- ) \ 94
\ ���� n ������������, �������� � ������ ���������� ��������������
\ ����� �����.
\ �������������� �������� ���������, ���� ������������ SIGN
\ ��� <# � #>, ������������������� �������������� �����.
  0< IF [CHAR] - HOLD THEN
;

: (D.)  ( d -- addr len )  DUP >R DABS <# #S R> SIGN #> ;

: D.    ( d -- )   (D.) TYPE SPACE ;

: . ( n -- )   S>D D. ;

: D.R ( d w -- )   >R (D.) R> OVER - 0MAX SPACES TYPE ;

: .R  ( n w -- )   >R  S>D  R>  D.R ;

: U.R ( u w -- )   0 SWAP D.R ;

: U. ( u -- ) \ 94
\ ���������� u � ��������� �������.
  U>D D.
;

: .0
  >R 0 <# #S #> R> OVER - 0 MAX DUP 
    IF 0 DO [CHAR] 0 EMIT LOOP
    ELSE DROP THEN TYPE 
;

: >PRT
  DUP BL U< IF DROP [CHAR] . THEN
;

: PTYPE
  0 DO DUP C@ >PRT EMIT 1+ LOOP DROP
;

: DUMP ( addr u -- ) \ 94 TOOLS
  DUP 0= IF 2DROP EXIT THEN
  BASE @ >R HEX
  15 + 16 U/ 0 DO
    CR DUP 4 .0 SPACE
    SPACE DUP 16 0
      DO I 4 MOD 0= IF SPACE THEN
        DUP C@ 2 .0 SPACE 1+
      LOOP SWAP 16  PTYPE
  LOOP DROP R> BASE !
;

: (.") ( T -> )
  COUNT TYPE
;
\ ' (.") TO (.")-CODE

: DIGIT ( C, N1 ->> N2, TF / FF )
\ N2 - �������� ������ C ���
\ ����� � ������� ��������� �� ��������� N1
   >R
   [CHAR] 0 - 10 OVER U<
   IF 
      DUP [CHAR] A [CHAR] 0 -     < IF  RDROP DROP 0 EXIT      THEN
      DUP [CHAR] a [CHAR] 0 -  1- > IF [CHAR] a  [CHAR] A - -  THEN
          [CHAR] A [CHAR] 0 - 10 - -
   THEN R> OVER U> DUP 0= IF NIP THEN ;

: >NUMBER ( ud1 c-addr1 u1 -- ud2 c-addr2 u2 ) \ 94
\ ud2 - ��������� �������������� �������� ������, �������� c-addr1 u1,
\ � �����, ��������� ����� � BASE, � ����������� ������ � ud1 �����
\ ��������� ud1 �� ����� � BASE. �������������� ������������ �����
\ ������� �� ������� ���������������� �������, ������� ������� "+" � "-",
\ ��� �� ������� �������������� ������.
\ c-addr2 - ����� ������� ��������������� ������� ��� ������� �������
\ �� ������ ������, ���� ������ ���� ��������� �������������.
\ u2 - ����� ����������������� �������� � ������.
\ ������������� �������� ���������, ���� ud2 ������������� �� �����
\ ��������������.
  BEGIN
    DUP
  WHILE
    >R
    DUP >R
    C@ BASE @ DIGIT 0=     \ ud n flag
    IF R> R> EXIT THEN     \ ud n  ( ud = udh udl )
    SWAP BASE @ UM* DROP   \ udl n udh*base
    ROT BASE @ UM* D+      \ (n udh*base)+(udl*baseD)
    R> 1+ R> 1-
  REPEAT
;

: SCREEN-LENGTH ( addr n -- n1 ) \ ��������-�����
\ ���� ����� ������ ��� ������ (��� ������)
\  - ����� ���������, ������� ������ ������ �� ������.
\ addr n  - ������. n1 ����� ��������� �� �����.
  0 -ROT OVER + SWAP ?DO
    I C@ 9 = IF 3 RSHIFT 1+ 3 LSHIFT
    ELSE 1+ THEN
  LOOP
;
