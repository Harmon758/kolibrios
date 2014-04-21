\ 94 CORE EXT

: .R ( n1 n2 -- ) \ 94 CORE EXT
\ ������� �� ����� n1 ����������� ������ � ���� ������� n2 ��������.
\ ���� ����� ��������, ����������� ��� ����������� n1, ������ ��� n2,
\ ������������ ��� ����� ����� ��� ������� �������� � ���� �����������
\ ������.
  >R DUP >R ABS
  S>D <# #S R> SIGN #>
  R> OVER - 0 MAX SPACES TYPE
;
: 0> ( n -- flag ) \ 94 CORE EXT
\ flag "������" ����� � ������ �����, ����� n ������ ����
  0 >
;

: MARKER ( "<spaces>name" -- ) \ 94 CORE EXT
\ ���������� ������� �������. �������� name, ������������ ���������.
\ ������� ����������� � ���������� ����������, ��������� ����.
\ name ����������: ( -- )
\ ������������ ������������� ������ ������� � ��������� ������� ������
\ � ���������, ������� ��� ����� ����� ������������ name. ������ 
\ ����������� name � ��� ����������� �����������. �� ��������� 
\ ����������� ��������������� ����� ���������� ���������, ������� 
\ ����� ���� ������� � ���������� ������������� ��� ������������� 
\ ������������� ������. ������� ������ ��������������� ����������, 
\ ��� ��������� ������� ���������, �� ����������.
  HERE
\  [C]HERE , [E]HERE ,
  GET-CURRENT ,
  GET-ORDER DUP , 0 ?DO DUP , @ , LOOP
  CREATE ,
  DOES> @ DUP \ ONLY
\  DUP @ [C]DP ! CELL+
\  DUP @ [E]DP ! CELL+
  DUP @ SET-CURRENT CELL+
  DUP @ >R R@ CELLS 2* + 1 CELLS - R@ 0
  ?DO DUP DUP @ SWAP CELL+ @ OVER ! SWAP 2 CELLS - LOOP
  DROP R> SET-ORDER
  DP !
;

: SAVE-INPUT ( -- xn ... x1 n )  \ 94 CORE EXT
\ x1 - xn ��������� ������� ��������� ������������ �������� ������ ���
\ ������������ ������������� ������ RESTORE-INPUT.
  SOURCE-ID 0>
  IF TIB #TIB @ 2DUP C/L 2 + ALLOCATE THROW DUP >R SWAP CMOVE
     R> TO TIB  >IN @
     SOURCE-ID FILE-POSITION THROW
     5
  ELSE BLK @ >IN @ 2 THEN
;
: RESTORE-INPUT ( xn ... x1 n -- flag ) \ 94 CORE EXT
\ ������� ������������ ������������ �������� ������ � ���������, 
\ ���������� x1 - xn. flag "������", ���� ������������ �������� 
\ ������ �� ����� ���� �������������.
\ �������������� �������� ���������, ���� ������� �����, 
\ �������������� ����������� �� ��� ��, ��� � ������� ������� �����.
  SOURCE-ID 0>
  IF DUP 5 <> IF 0 ?DO DROP LOOP -1 EXIT THEN
     DROP SOURCE-ID REPOSITION-FILE ?DUP IF >R 2DROP DROP R> EXIT THEN
     >IN ! #TIB ! TO TIB FALSE
  ELSE DUP 2 <> IF 0 ?DO DROP LOOP -1 EXIT THEN
     DROP >IN ! BLK ! FALSE
  THEN
;
: U.R ( u n -- ) \ 94 CORE EXT
\ ������� �� ����� u ����������� ������ � ���� ������� n ��������.
\ ���� ����� ��������, ����������� ��� ����������� u, ������ ��� n,
\ ������������ ��� ����� ����� ��� ������� �������� � ���� �����������
\ ������.
  >R  U>D <# #S #>
  R> OVER - 0 MAX SPACES TYPE
;
\EOF
: UNUSED ( -- u ) \ 94 CORE EXT
\ u - ����� ������, ���������� � �������, ���������� HERE,
\ � ������.
  IMAGE-SIZE
  HERE IMAGE-BASE - -
;