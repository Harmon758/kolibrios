( ����� ᯨ᪠ ᫮� ᫮���� - WORDS.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

VARIABLE NNN

: ?CR-BREAK ( NFA -- NFA TRUE | FALSE )
  DUP
  IF DUP ZCOUNT NIP AT-XY? DROP + SCR_WIDTH-S >
     IF CR
        NNN @
        IF    -1 NNN +!  TRUE
        ELSE  ." more?" CR 16 NNN !
              KEY [CHAR] Q <> AND ?DUP 0<>
        THEN
     ELSE TRUE
     THEN
  THEN
;

: NLIST ( A -> )
  @
  CR W-CNT 0!  16 NNN !
  BEGIN  ?CR-BREAK
  WHILE
    W-CNT 1+!
    DUP ID. \ 9 EMIT
	SPACE AT-XY? >R 8 / 1+ 8 * R> AT-XY
    CDR
  REPEAT KEY? IF KEY DROP THEN
  CR CR ." Words: " W-CNT @ U. CR
;

: WORDS ( -- ) \ 94 TOOLS
\ ���᮪ ���� ��।������ � ��ࢮ� ᯨ᪥ ᫮� ���浪� ���᪠. ��ଠ� ������ 
\ �� ॠ����樨.
\ WORDS ����� ���� ॠ������� � �ᯮ�짮������ ᫮� �ଠ⭮�� �८�ࠧ������ 
\ �ᥫ. ���⢥��⢥���, �� ����� �ᯮ���� ��६�頥��� �������, 
\ ��������㥬�� #>.
  CONTEXT @ NLIST
;

