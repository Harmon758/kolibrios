( ���� ᫮� � ᫮����� � �ࠢ����� ���浪�� ���᪠.
  ��-������ᨬ� ��।������.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
  ������஢���� ���ᨬ��� �.�.
  email:mak@mail.rtc.neva.ru
  http://informer.rtc.neva.ru/
  � � {812}105-92-03
  � � {812}552-47-64
)

VECT FIND

0x10 CELLS CONSTANT CONTEXT_SIZE

CREATE SEARCH-BUFF 0x81 ALLOT

Code ZSEARCH-WORDLIST ;( z-addr wid -- 0 | xt 1 | xt -1 ) \ 94 SEARCH
; ���� ��।������, �������� ��ப�� c-addr u � ᯨ᪥ ᫮�, ��������㥬�� 
; wid. �᫨ ��।������ �� �������, ������ ����.
; �᫨ ��।������ �������, ������ �믮����� ⮪�� xt � ������� (1), �᫨ 
; ��।������ ������������ �ᯮ������, ���� ����� ������� (-1).
;	PUSH	WORD PTR [EBP]
	MOV	EDX, [EBP]
	PUSH	EDX
	MOV	EAX, [EAX]
	PUSH	EAX
	LEA	EBP,  [EBP+4]
	CALL	{' GETPR}
	test	eax, eax
	JZ	END
	LEA	EBP, [EBP-4]
	mov	[ebp],eax
	MOVZX	EAX, BYTE PTR [EDX-9]
	DEC	EAX
	OR	EAX,1
	
END:       RET
EndCode

: SEARCH-WORDLIST ( c-addr u wid -- 0 | xt 1 | xt -1 )
  >R 0x7F AND SEARCH-BUFF  ASCII-Z
  R>  ZSEARCH-WORDLIST

;

: SFIND ( addr len --- addr len 0| xt 1|xt -1 )
\ Search all word lists in the search order for the name in the
\ counted string at c-addr. If not found return the name address and 0.
\ If found return the execution token xt and -1 if the word is non-immediate
\ and 1 if the word is immediate.
  CONTEXT
  BEGIN	DUP @
  WHILE	>R
	2DUP  R@ @ SEARCH-WORDLIST ?DUP
	IF    RDROP 2NIP  EXIT \ Exit if found.
	THEN
	R> CELL+
  REPEAT @
;

: FIND1 ( c-addr -- c-addr 0 | xt 1 | xt -1 ) \ 94 SEARCH
\ ������� ᥬ��⨪� CORE FIND ᫥���騬:
\ �᪠�� ��।������ � ������, ������� ��ப�� � ���稪�� c-addr.
\ �᫨ ��।������ �� ������� ��᫥ ��ᬮ�� ��� ᯨ᪮� � ���浪� ���᪠,
\ �������� c-addr � ����. �᫨ ��।������ �������, �������� xt.
\ �᫨ ��।������ ������������ �ᯮ������, ������ ⠪�� ������� (1);
\ ���� ⠪�� ������ ����� ������� (-1). ��� ������ ��ப�, ���祭��,
\ �����頥�� FIND �� �६� �������樨, ����� �⫨����� �� ���祭��,
\ �����頥��� �� � ०��� �������樨.
  COUNT SFIND
  DUP 0= IF 2DROP 1- 0 THEN ;

: DEFINITIONS ( -- ) \ 94 SEARCH
\ ������� ᯨ᪮� �������樨 �� �� ᯨ᮪ ᫮�, �� � ���� ᯨ᮪ � ���浪� 
\ ���᪠. ����� ��᫥����� ��।������ ���� ��������� � ᯨ᮪ �������樨.
\ ��᫥���騥 ��������� ���浪� ���᪠ �� ������ �� ᯨ᮪ �������樨.
  CONTEXT @ SET-CURRENT
;

: GET-ORDER_DROP ( CONTEXT -- widn .. wid1 )
  DUP @ DUP IF >R CELL+ RECURSE R> EXIT THEN 2DROP ;

: GET-ORDER     ( -- widn .. wid1 n )
	DEPTH >R
	CONTEXT GET-ORDER_DROP
	DEPTH R> - ;

: SET-ORDER     ( widn .. wid1 n -- )
                DUP 0<
                IF      DROP ONLY
                ELSE    CONTEXT CONTEXT_SIZE ERASE
                        0
                        ?DO     CONTEXT I CELLS+ !
                        LOOP
                THEN    ;


: FORTH ( -- ) \ 94 SEARCH EXT
\ �८�ࠧ����� ���冷� ���᪠, ����騩 �� widn, ...wid2, wid1 (��� wid1 
\ ��ᬠ�ਢ����� ����) � widn,... wid2, widFORTH-WORDLIST.
  FORTH-WORDLIST CONTEXT !
;

: ONLY ( -- ) \ 94 SEARCH EXT
\ ��⠭����� ᯨ᮪ ���᪠ �� ������騩 �� ॠ����樨 ��������� ᯨ᮪ ���᪠.
\ ��������� ᯨ᮪ ���᪠ ������ ������� ᫮�� FORTH-WORDLIST � SET-ORDER.
  CONTEXT CELL+ 0!
  FORTH
;

: ALSO ( -- ) \ 94 SEARCH EXT
\ �८�ࠧ����� ���冷� ���᪠, ����騩 �� widn, ...wid2, wid1 (��� wid1 
\ ��ᬠ�ਢ����� ����) � widn,... wid2, wid1, wid1. ����।������� ����� 
\ ���������, �᫨ � ���浪� ���᪠ ᫨誮� ����� ᯨ᪮�.
 CONTEXT CONTEXT CELL+ CONTEXT_SIZE CMOVE> ;


: PREVIOUS ( -- ) \ 94 SEARCH EXT
\ �८�ࠧ����� ���冷� ���᪠, ����騩 �� widn, ...wid2, wid1 (��� wid1 
\ ��ᬠ�ਢ����� ����) � widn,... wid2. ����।������� ����� ���������,
\ �᫨ ���冷� ���᪠ �� ���� ��। �믮������� PREVIOUS.
  _PREVIOUS ;

: _PREVIOUS ( -- ) \ 94 SEARCH EXT
 CONTEXT CELL+ CONTEXT CONTEXT_SIZE CMOVE  ;

: VOC-NAME. ( wid -- ) \ �������� ��� ᯨ᪠ ᫮�, �᫨ �� ��������
  DUP FORTH-WORDLIST = IF DROP ." FORTH"  EXIT THEN
\  DUP KERNEL-WORDLIST = IF DROP ." KERNEL"  EXIT THEN
  DUP CELL+ @ DUP IF ID. DROP ELSE DROP ." <NONAME>:" U. THEN
;

: ORDER ( -- ) \ 94 SEARCH EXT
\ �������� ᯨ᪨ � ���浪� ���᪠, �� ��ࢮ�� ��ᬠ�ਢ������ ᯨ᪠ �� 
\ ��᫥�����. ����� �������� ᯨ᮪ ᫮�, �㤠 ��������� ���� ��।������.
\ ��ଠ� ����ࠦ���� ������ �� ॠ����樨.
\ ORDER ����� ���� ॠ������� � �ᯮ�짮������ ᫮� �ଠ⭮�� �८�ࠧ������
\ �ᥫ. �������⥫쭮 �� ����� ࠧ����� ��६�頥��� �������, 
\ ��������㥬�� #>.
  GET-ORDER ." Context: "
  0 ?DO ( DUP .) VOC-NAME. SPACE LOOP CR
  ." Current: " GET-CURRENT VOC-NAME. CR
;

: LATEST ( -> NFA )
  CURRENT @ @
;
