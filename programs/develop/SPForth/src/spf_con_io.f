( ���᮫�� ����-�뢮�.
  ��-������ᨬ� ᫮�� [�⭮�⥫쭮...].
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)
32 VALUE BL ( -- char ) \ 94
\ char - ���祭�� ᨬ���� "�஡��".

: SPACE ( -- ) \ 94
\ �뢥�� �� �࠭ ���� �஡��.
  BL EMIT
;

: SPACES ( n -- ) \ 94
\ �᫨ n>0 - �뢥�� �� ��ᯫ�� n �஡����.
  BEGIN
    DUP
  WHILE
    BL EMIT 1-
  REPEAT DROP
;

VARIABLE PENDING-CHAR \ ��������� ���� -> ��६����� ������쭠�, �� USER

VECT DO-KEY?

' NOOP TO DO-KEY?

: KEY?
  EVENT-CASE DO-KEY?
  &KEY @ 0<> ;

\ : KEY?
\ 0 ;

VECT KEY


' _KEY TO KEY

