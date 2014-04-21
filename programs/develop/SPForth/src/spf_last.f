DECIMAL

\ ' DUP  VALUE 'DUP_V
\ ' DROP VALUE 'DROP_V

USER     HLD  \ ��६����� - ������ ��᫥���� �����, ��७�ᥭ��� � PAD

0 VALUE  H-STDIN    \ ��� 䠩�� - �⠭���⭮�� �����
1 VALUE  H-STDOUT   \ ��� 䠩�� - �⠭���⭮�� �뢮��
1 VALUE  H-STDERR   \ ��� 䠩�� - �⠭���⭮�� �뢮�� �訡��

USER ALIGN-BYTES

: ALIGNED ( addr -- a-addr ) \ 94
\ a-addr - ���� ��஢����� ����, ����訩 ��� ࠢ�� addr.
  ALIGN-BYTES @ DUP 0= IF 1+ DUP ALIGN-BYTES ! THEN
  2DUP
  MOD DUP IF - + ELSE 2DROP THEN
;

: ALIGN ( -- ) \ 94
\ �᫨ 㪠��⥫� ����࠭�⢠ ������ �� ��஢��� -
\ ��஢���� ���.
  DP @ ALIGNED DP @ - ALLOT
;

: ALIGN-NOP ( n -- )
\ ��஢���� HERE �� n � ��������� NOP
  HERE DUP ROT 2DUP
  MOD DUP IF - + ELSE 2DROP THEN
  OVER - DUP ALLOT 0x90 FILL
;

: IMMEDIATE ( -- ) \ 94
\ ������� ��᫥���� ��।������ ᫮��� ������������ �ᯮ������.
\ �᪫��⥫쭠� ����� ���������, �᫨ ��᫥���� ��।������
\ �� ����� �����.
  LAST @ NAME>F DUP C@ &IMMEDIATE OR SWAP C!
;


: :NONAME ( C: -- colon-sys ) ( S: -- xt ) \ 94 CORE EXT
\ ������� �믮����� ⮪�� xt, ��⠭����� ���ﭨ� �������樨 � 
\ ����� ⥪�饥 ��।������, �ந����� colon-sys. �������� ᥬ��⨪�
\ ���樠����樨 � ⥪�饬� ��।������.
\ �����⨪� �믮������ xt �㤥� ������ ᫮����, ᪮�����஢���묨 
\ � ⥫� ��।������. �� ��।������ ����� ���� ����� �믮����� ��
\ xt EXECUTE.
\ �᫨ �ࠢ���騩 �⥪ ॠ������� � �����짮������ �⥪� ������,
\ colon-sys �㤥� ���孨� ����⮬ �� �⥪� ������.
\ ���樠������: ( i*x -- i*x ) ( R: -- nest-sys )
\ ���࠭��� ��������� �� ॠ����樨 ���ଠ�� nest-sys � �맮�� 
\ ��।������. �������� �⥪� i*x �।�⠢���� ��㬥��� xt.
\ xt �믮������: ( i*x -- j*x )
\ �믮����� ��।������, �������� xt. �������� �⥪� i*x � j*x 
\ �।�⠢���� ��㬥��� � १����� xt ᮮ⢥��⢥���.
  HERE ]
  HERE TO :-SET ;

: INCLUDED INCLUDED_ ;
  ' NOOP       TO <PRE>
  ' FIND1      TO FIND
  ' ?LITERAL2  TO ?LITERAL
  ' ?SLITERAL2 TO ?SLITERAL
  ' OK1        TO OK.
  ' (ABORT1")  TO (ABORT")

VECT TYPE ' _TYPE TO TYPE
VECT EMIT ' _EMIT TO EMIT


: H. BASE @ SWAP HEX U. BASE ! ;

: TST S" /rd/1/autoload.f" INCLUDED_ ;
: TST1 S" WORDS" EVALUATE ;

