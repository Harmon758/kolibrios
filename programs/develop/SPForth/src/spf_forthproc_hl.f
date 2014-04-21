( ��⠢訥�� ᫮�� "���-������" � ���� ��᮪��஢����� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

0 CONSTANT FALSE ( -- false ) \ 94 CORE EXT
\ ������ 䫠� "����".

-1 CONSTANT TRUE ( -- true ) \ 94 CORE EXT
\ ������ 䫠� "��⨭�", �祩�� � �ᥬ� ��⠭������묨 ��⠬�.

4 CONSTANT CELL

: */ ( n1 n2 n3 -- n4 ) \ 94
\ �������� n1 �� n2, ������� �஬������ ������� १���� d.
\ ��������� d �� n3, ������� ��⭮� n4.
  */MOD NIP
;

: CHAR+ ( c-addr1 -- c-addr2 ) \ 94
\ �ਡ����� ࠧ��� ᨬ���� � c-addr1 � ������� c-addr2.
  1+
;
: CHARS ( n1 -- n2 ) \ 94
\ n2 - ࠧ��� n1 ᨬ�����.
; IMMEDIATE

: MOVE ( addr1 addr2 u -- ) \ 94
\ �᫨ u ����� ���, ����஢��� ᮤ�ন��� u ���� �� addr1 � addr2.
\ ��᫥ MOVE � u ����� �� ����� addr2 ᮤ�ন��� � �筮�� � ��,
\ �� �뫮 � u ����� �� ����� addr1 �� ����஢����.
  >R 2DUP SWAP R@ + U< \ �����祭�� �������� � �������� ���筨�� ��� �����
  IF 2DUP U<           \ � �� �����
     IF R> CMOVE> ELSE R> CMOVE THEN
  ELSE R> CMOVE THEN ;

: ERASE ( addr u -- ) \ 94 CORE EXT
\ �᫨ u ����� ���, ������ �� ���� ������� �� u ���� �����,
\ ��稭�� � ���� addr.
  0 FILL ;

: BLANK ( addr len -- )     \ fill addr for len with spaces (blanks)
  BL FILL ;

: DABS ( d -- ud ) \ 94 DOUBLE
\ ud ��᮫�⭠� ����稭� d.
  DUP 0< IF DNEGATE THEN
;

255 CONSTANT MAXCOUNTED   \ maximum length of contents of a counted string

\ : 0X BASE @ HEX >R BL WORD ?LITERAL
\      R> BASE ! ; IMMEDIATE
: "CLIP"        ( a1 n1 -- a1 n1' )   \ clip a string to between 0 and MAXCOUNTED
                MAXCOUNTED AND ;

: PLACE         ( addr len dest -- )
                SWAP "CLIP" SWAP
 2DUP C! CHAR+ SWAP CHARS MOVE ;

: +PLACE        ( addr len dest -- ) \ append string addr,len to counted
\ string dest
    >R "CLIP" MAXCOUNTED  R@ C@ -  MIN R>
\ clip total to MAXCOUNTED string
    2DUP 2>R

    COUNT CHARS + SWAP MOVE
    2R> +! ;

: C+PLACE       ( c1 a1 -- )    \ append char c1 to the counted string at a1
                DUP 1+! COUNT + 1- C! ;

: STR>R ( addr u -- addr1 u)
\ �������� ��ப� addr u �� �⥪ �����⮢
\ �������� addr1 ���� ����� ��ப�
;

0  VALUE  DOES-CODE

: $!         ( addr len dest -- )
   PLACE ;

: ASCII-Z     ( addr len buff -- buff-z )        \ make an ascii string
   DUP >R $! R> COUNT OVER + 0 SWAP C! ;

: 0MAX 0 MAX ;

: ASCIIZ>  ZCOUNT ;

: R>     ['] C-R>    INLINE, ;   IMMEDIATE
: >R     ['] C->R    INLINE, ;   IMMEDIATE

: 2CONSTANT  ( d --- )
\ Create a new definition that has the following runtime behavior.
\ Runtime: ( --- d) push the constant double number on the stack. 
  CREATE HERE 2! 8 ALLOT DOES> 2@ ;

: U/MOD 0 SWAP UM/MOD ;

: 2NIP 2SWAP 2DROP ;

: ON TRUE SWAP ! ;
: OFF ( a--) 0! ;
