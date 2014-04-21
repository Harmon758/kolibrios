( ����� ��ப� � ��室�� ⥪�⮬ �ணࠬ�� �� ����.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  ������� 1999: PARSE � SKIP �८�ࠧ����� �� CODE
  � ��᮪��஢���� ��।������. ��६���� �८�ࠧ����� � USER.
)

512  VALUE  C/L \ ���ᨬ���� ࠧ��� ��ப�, ������ ����� ����� � TIB

: SOURCE ( -- c-addr u ) \ 94
\ c-addr - ���� �室���� ����. u - ������⢮ ᨬ����� � ���.
  TIB #TIB @
;

: SOURCE! ( c-addr u -- ) 
\ ��⠭�����  c-addr u �室�� ���஬ (�筥�, �������� ࠧ��� - PARSE-AREA)
  #TIB ! TO TIB >IN 0! ;

: EndOfChunk ( -- flag )
  >IN @ SOURCE NIP < 0=        \ >IN �� �����, 祬 ����� 砭��
;

: CharAddr ( -- c-addr )
  SOURCE DROP >IN @
\ CR ." CA=" DEPTH .SN
 +
;

: PeekChar ( -- char )
  CharAddr C@       \ ᨬ��� �� ⥪�饣� ���祭�� >IN
;

: IsDelimiter ( char -- flag )
  BL 1+ <
;

: GetChar ( -- char flag )
  EndOfChunk
  IF 0 FALSE
  ELSE PeekChar TRUE THEN
;

: OnDelimiter ( -- flag )
  GetChar SWAP IsDelimiter AND
;

: SkipDelimiters ( -- ) \ �ய����� �஡���� ᨬ����
  BEGIN
    OnDelimiter
  WHILE
    >IN 1+!
  REPEAT >IN @  >IN_WORD ! ;

: OnNotDelimiter ( -- flag )
  GetChar SWAP IsDelimiter 0= AND
;

: SkipWord ( -- ) \ �ய����� ���஡���� ᨬ����
  BEGIN
    OnNotDelimiter
  WHILE
    >IN 1+!
  REPEAT
;
: SkipUpTo ( char -- ) \ �ய����� �� ᨬ���� char
  BEGIN
    DUP GetChar \ ." SC="  DUP M.
  >R <> R> AND
  WHILE
    >IN 1+!
  REPEAT DROP
;

: ParseWord ( -- c-addr u )
  CharAddr \ CR ." P=" DUP 9 TYPE
 >IN @
\ CR ." XZ=" DEPTH .SN
  SkipWord  >IN @
\ CR ." X1=" DEPTH .SN
 - NEGATE
\ CR ." X2=" DEPTH .SN
\ CR ." PZ=" 2DUP  TYPE
;
CREATE UPPER_SCR  31 ALLOT

: UPC  ( c -- c' )
   DUP [CHAR] Z U>
   IF  0xDF AND
   THEN   ;

: UPPER ( ADDR LEN -- )
  0 ?DO COUNT UPC OVER 1- C! LOOP DROP ;

: UPPER_NW  ( ADDR LEN -- ADDR' LEN )
   UPPER_SCR PLACE 
   UPPER_SCR COUNT 2DUP UPPER ;

: PARSE-WORD  ( "name" -- c-addr u )
 \ http://www.complang.tuwien.ac.at/forth/ansforth/parse-word.html 
 \ �� ᫮�� ⥯��� �㤥� �ᯮ�짮���� � INTERPRET
  \ - 㤮����: �� �ᯮ���� WORD �, ᮮ⢥��⢥���, �� ����� � HERE;
  \ � ࠧ����⥫ﬨ ��⠥� �� �� <=BL, � ⮬ �᫥ TAB � CRLF
  SkipDelimiters ParseWord
  >IN 1+! \ �ய��⨫� ࠧ����⥫� �� ᫮���
\  UPPER_V @ EXECUTE
;

: NextWord PARSE-WORD ;
: PARSE-NAME PARSE-WORD ;

: PARSE ( char "ccc<char>" -- c-addr u ) \ 94 CORE EXT
\ �뤥���� ccc, ��࠭�祭��� ᨬ����� char.
\ c-addr - ���� (����� �室���� ����), � u - ����� �뤥������ ��ப�.
\ �᫨ ࠧ��ࠥ��� ������� �뫠 ����, १�������� ��ப� ����� �㫥���
\ �����.
  CharAddr >IN @
  ROT SkipUpTo
  >IN @ - NEGATE
  >IN 1+!
;

: PSKIP ( char "ccc<char>" -- )
\ �ய����� ࠧ����⥫� char.
  BEGIN
    DUP GetChar >R = R> AND
  WHILE
    >IN 1+!
  REPEAT DROP
;
