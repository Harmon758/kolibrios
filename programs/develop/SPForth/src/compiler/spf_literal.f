( �८�ࠧ������ �᫮��� ���ࠫ�� �� ������樨.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

: ?SLITERAL1 ( c-addr u -> ... )
  \ �८�ࠧ����� ��ப� � �᫮
  0 0 2SWAP
  OVER C@ [CHAR] - = IF 1- SWAP 1+ SWAP TRUE ELSE FALSE THEN >R
  >NUMBER
  DUP 1 > IF ." -?" -2001 THROW THEN \ ABORT" -?"
  IF C@ [CHAR] . <> IF -2002 THROW THEN \ ABORT" -??"
       R> IF DNEGATE THEN
       [COMPILE] 2LITERAL
  ELSE DROP D>S
       R> IF NEGATE THEN
       [COMPILE] LITERAL
  THEN
;
: ?LITERAL1 ( T -> ... )
  \ �८�ࠧ����� ��ப� � �᫮
  COUNT ?SLITERAL1
;
: HEX-SLITERAL ( addr u -> flag )
  BASE @ >R HEX
  0 0 2SWAP 2- SWAP 2+ SWAP >NUMBER
  ?DUP IF
    1 = SWAP C@ [CHAR] L = AND 0= IF 2DROP FALSE R> BASE ! EXIT THEN
  ELSE DROP THEN
  D>S POSTPONE LITERAL TRUE
  R> BASE !
;

: INCLUDED_S  -2003 THROW
 INCLUDED ;

: ?SLITERAL2 ( c-addr u -- ... )
  ( ���७�� ��ਠ�� ?SLITERAL1:
    �᫨ ��ப� - �� �᫮, � ��⠥��� �ࠪ⮢��� ��
    ��� ��� 䠩�� ��� ���-INCLUDED)
  DUP 1 > IF OVER W@ 0x7830 ( 0x) = 
    IF 2DUP 2>R HEX-SLITERAL IF RDROP RDROP EXIT ELSE 2R> THEN THEN
  THEN
  2DUP 2>R ['] ?SLITERAL1 CATCH
  IF   2DROP 2R>
       OVER C@ [CHAR] " = OVER 2 > AND
       IF 2 - SWAP 1+ SWAP THEN ( �ࠫ ����窨, �᫨ ����)
       2DUP + 0 SWAP C!
       ['] INCLUDED_S CATCH
       DUP 2 = OVER 3 = OR OVER 161 = OR ( 䠩� �� ������ ��� ���� �� ������,
       ��� ��ࠧ�襭��� ��� 䠩��)
       IF  -2003 THROW \ ABORT"  -???"
       ELSE  THROW THEN
  ELSE RDROP RDROP
  THEN
;
: ?LITERAL2 ( c-addr -- ... )
  ( ���७�� ��ਠ�� ?LITERAL1:
    �᫨ ��ப� - �� �᫮, � ��⠥��� �ࠪ⮢��� ��
    ��� ��� 䠩�� ��� ���-INCLUDED)
  COUNT ?SLITERAL2
;
