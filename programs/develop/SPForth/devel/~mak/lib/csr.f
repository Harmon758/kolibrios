[IFNDEF]   CSP
VARIABLE   CSP    \ ��������� ����� ��������
[THEN]
6 CONSTANT L-CAS# \ ���������� ������� �����������
CREATE     S-CSP   L-CAS# CELLS ALLOT \ ���� ��������
S-CSP CSP !

: +CSP ( -> P)    \ �������� �������
  CSP @ DUP CELL+ CSP !
;
: -CSP ( -> )     \ ������ �������
  CSP @ 1 CELLS - CSP !
;

: !CSP ( -> )     \ ���������������� �������
  SP@ +CSP !
;

: CSP@ ( -> A)
  CSP @ 1 CELLS - @
;
: ?CSP ( -> )     \ ��������� ������������� �����
  SP@ CSP@ <> 37 ?ERROR ( ABORT" ���� ����� �� CSP !")
  -CSP
;
