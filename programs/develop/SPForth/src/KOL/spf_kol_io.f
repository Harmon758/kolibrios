( ������� ����-�뢮�.
)

CREATE LT 0xD C, 0xA C, 0xD C, 0xA C, \ line terminator
CREATE LTL 2 ,   \ line terminator length


: DOS-LINES ( -- )
  0xA0D LT ! 2 LTL !
;
: UNIX-LINES ( -- )
  0xA0A LT ! 1 LTL !
;


: READ-FILE ( c-addr u1 fileid -- u2 ior ) \ 94 FILE
\ ������ u1 ᨬ����� � c-addr �� ⥪�饩 ����樨 䠩��,
\ ��������㥬��� fileid.
\ �᫨ u1 ᨬ����� ���⠭� ��� �᪫�祭��, ior ���� � u2 ࠢ�� u1.
\ �᫨ ����� 䠩�� ���⨣��� �� ���⥭�� u1 ᨬ�����, ior ����
\ � u2 - ������⢮ ॠ�쭮 ���⠭��� ᨬ�����.
\ �᫨ ������ �ந�������� ����� ���祭��, �����頥���
\ FILE-POSITION ࠢ�� ���祭��, �����頥���� FILE-SIZE ��� 䠩��
\ ��������㥬��� fileid, ior � u2 �㫨.
\ �᫨ �������� �᪫��⥫쭠� �����, � ior - ��।������ ॠ����樥�
\ ��� १���� �����/�뢮��, � u2 - ������⢮ ��ଠ�쭮 ��।����� �
\ c-addr ᨬ�����.
\ ����।������� ����� ���������, �᫨ ������ �믮������, �����
\ ���祭��, �����頥��� FILE-POSITION ����� 祬 ���祭��, �����頥���
\ FILE-SIZE ��� 䠩��, ��������㥬��� fileid, ��� �ॡ㥬�� ������
\ ��⠥��� ������ ������ᠭ��� ���� 䠩��.
\ ��᫥ �����襭�� ����樨 FILE-POSITION ������� ᫥������ ������
\ � 䠩�� ��᫥ ��᫥����� ���⠭���� ᨬ����.
  DUP >R .CODE 0!
      R@ .SIZE !
      R@ .DATA !
      R@ 70 SYS2
   EBX@ SWAP
   DUP 6 = IF DROP 0 THEN
   DUP 0=
	IF	R@  .FIRST D@ 
		R@  .SIZE @ 0 D+
		R@  .FIRST D!
	THEN
   RDROP
;

22 CONSTANT MAX_OPEN_FILES

CREATE FILE_STR_BUF FILE_STR 1+ MAX_OPEN_FILES  * ALLOT

: FALLOC  ( -- 0|fid )
  FILE_STR_BUF FILE_STR MAX_OPEN_FILES  * BOUNDS
  BEGIN DUP .NAME @ 0= 
	IF NIP EXIT THEN
	FILE_STR + 2DUP U<
  UNTIL 2DROP 0
;

: OPEN-FILE ( c-addr u fam -- fileid ior )
\ ������ 䠩� � ������, ������� ��ப�� c-addr u, � ��⮤�� ����㯠 fam.
\ ���� ���祭�� fam ��।���� ॠ����樥�.
\ �᫨ 䠩� �ᯥ譮 �����, ior ����, fileid ��� �����䨪���, � 䠩�
\ ����樮��஢�� �� ��砫�.
\ ���� ior - ��।������ ॠ����樥� ��� १���� �����/�뢮��,
\ � fileid ����।����.
  DROP
\  FILE_STR
 FALLOC DUP
 IF
  >R
  R@  FILE_STR ERASE
  R@ .NAME SWAP  MOVE
  R> 0  EXIT
 THEN -1
;

: CLOSE-FILE ( fileid -- ior ) \ 94 FILE
\ ������� 䠩�, ������� fileid.
  .NAME 0! 0
;


USER _fp1
USER _fp2
USER _addr


: READ-LINE ( c-addr u1 fileid -- u2 flag ior ) \ 94 FILE
\ ������ ᫥������ ��ப� �� 䠩��, ��������� fileid, � ������
\ �� ����� c-addr. ��⠥��� �� ����� u1 ᨬ�����. �� ����
\ ��।������� ॠ����樥� ᨬ����� "����� ��ப�" ����� ����
\ ���⠭� � ������ �� ���殬 ��ப�, �� �� ����祭� � ���稪 u2.
\ ���� ��ப� c-addr ������ ����� ࠧ��� ��� ������ u1+2 ᨬ����.
\ �᫨ ������ �ᯥ譠, flag "��⨭�" � ior ����. �᫨ ����� ��ப�
\ ����祭 �� ⮣� ��� ���⠭� u1 ᨬ�����, � u2 - �᫮ ॠ�쭮
\ ���⠭��� ᨬ����� (0<=u2<=u1), �� ���� ᨬ����� "����� ��ப�".
\ ����� u1=u2 ����� ��ப� 㦥 ����祭.
\ �᫨ ������ �ந��������, ����� ���祭��, �����頥���
\ FILE-POSITION ࠢ�� ���祭��, �����頥���� FILE-SIZE ��� 䠩��,
\ ��������㥬��� fileid, flag "����", ior ����, � u2 ����.
\ �᫨ ior �� ����, � �ந��諠 �᪫��⥫쭠� ����� � ior -
\ ��।������ ॠ����樥� ��� १���� �����-�뢮��.
\ ����।������� ����� ���������, �᫨ ������ �믮������, �����
\ ���祭��, �����頥��� FILE-POSITION ����� 祬 ���祭��, �����頥���
\ FILE-SIZE ��� 䠩��, ��������㥬��� fileid, ��� �ॡ㥬�� ������
\ ��⠥��� ������ ������ᠭ��� ���� 䠩��.
\ ��᫥ �����襭�� ����樨 FILE-POSITION ������� ᫥������ ������
\ � 䠩�� ��᫥ ��᫥����� ���⠭���� ᨬ����.
  DUP >R
  FILE-POSITION IF 2DROP 0 0 THEN _fp1 ! _fp2 !
  1+
  OVER _addr !

  R@ READ-FILE ?DUP IF NIP RDROP 0 0 ROT EXIT THEN

  DUP >R 0= IF RDROP RDROP 0 0 0 EXIT THEN \ �뫨 � ���� 䠩��
  _addr @ R@ LT 1+ 1 SEARCH
  IF   \ ������ ࠧ����⥫� ��ப
     DROP  _addr @ -
     DUP 1+ S>D _fp2 @ _fp1 @ D+ RDROP R> REPOSITION-FILE DROP
     DUP _addr @ + 1- C@  0xD = IF 1- THEN
  ELSE   \ �� ������ ࠧ����⥫� ��ப
     2DROP
     R> RDROP  \ �᫨ ��ப� ���⠭� �� ��������� - �㤥� ࠧ१���
  THEN
  TRUE 0
;

: FILE-POSITION ( fileid -- ud ior ) \ 94 FILE
\ ud - ⥪��� ������ � 䠩��, ��������㥬�� fileid.
  .FIRST D@ 0
;

: REPOSITION-FILE ( ud fileid -- ior ) \ 94 FILE
\ ��९���樮��஢��� 䠩�, ��������㥬� fileid, �� ud.
  .FIRST D! 0
;

