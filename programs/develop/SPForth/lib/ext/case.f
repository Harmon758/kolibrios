\ ����������� ������ CASE
\ � ������ ��������� ����������� ���������� CASE


DECIMAL
VARIABLE   CSP    \ ��������� ����� ��������
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
\  SP@ CSP@ <> 37 ?ERROR ( ABORT" ���� ����� �� CSP !")
  -CSP
;
: CASE ( -> )
  !CSP
; IMMEDIATE
: OF
  POSTPONE OVER POSTPONE =
  [COMPILE] IF POSTPONE DROP
; IMMEDIATE
: ENDOF
  [COMPILE] ELSE
; IMMEDIATE
: ENDCASE
  POSTPONE DROP BEGIN SP@ CSP@ =
  0=  WHILE  [COMPILE] THEN  REPEAT -CSP
; IMMEDIATE
