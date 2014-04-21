( �᭮��� ������஢���� ᫮�� "���-������"
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
)

( ��������� ��� ����ணࠬ����� �⮣� ����.
  ESP - 㪠��⥫� �⥪� �����⮢
  EBP - 㪠��⥫� �⥪� ������
  EDI - ��࠭塞� ॣ���� [㪠��⥫� ������ ��⮪� � SPF]
)

HEX

\ ================================================================
\ �⥪��� �������樨

?HS

Code DUP ;( x -- x x ) \ 94
; �த㡫�஢��� x.
     LEA EBP, [EBP-4]
     mov [ebp],eax
     RET
EndCode


\ ' DUP TO 'DUP_V

Code ?DUP ;( x -- 0 | x x ) \ 94
; �த㡫�஢��� x, �᫨ �� ����.
     OR  EAX, EAX
     JNZ { ' DUP }
     RET
EndCode

Code 2DUP ;( x1 x2 -- x1 x2 x1 x2 ) \ 94
; �த㡫�஢��� ���� �祥� x1 x2.
     MOV EDX, [EBP]
     MOV [EBP-4], EAX
     MOV [EBP-8], EDX
     LEA EBP, [EBP-8]
     RET
EndCode

Code DROP ;( x -- ) \ 94
; ����� x � �⥪�.
     mov eax,[ebp]
     LEA EBP, [EBP+4]
     RET
EndCode

\ ' DROP TO 'DROP_V

Code MAX ;( n1 n2 -- n3 ) \ 94
; n3 - ����襥 �� n1 � n2.
     CMP EAX, [EBP]
     JL  { ' DROP }
     LEA EBP, [EBP+4]
     RET
EndCode

Code MIN ;( n1 n2 -- n3 ) \ 94
 ; n3 - ����襥 �� n1 � n2.
     CMP EAX, [EBP]
     JG  { ' DROP }
     LEA EBP, [EBP+4]
     RET
EndCode

Code UMAX       ;( u1 u2 -- n3 ) \ RETurn the lesser of unsigned u1 and
                                ; unsigned u2
     CMP  EAX, [EBP]
     JB { ' DROP }
     LEA EBP, [EBP+4]
     RET
EndCode

Code UMIN       ;( u1 u2 -- n3 ) \ RETurn the lesser of unsigned u1 and
                                ; unsigned u2
     CMP EAX, [EBP]
     JA { ' DROP }
     LEA EBP, [EBP+4]
     RET
EndCode

Code 2DROP ;( x1 x2 -- ) \ 94
; ����� � �⥪� ���� �祥� x1 x2.
     MOV     EAX , [EBP+4]
     ADD     EBP , 8
     RET
EndCode

Code SWAP ;( x1 x2 -- x2 x1 ) \ 94
; �������� ���⠬� ��� ���孨� ����� �⥪�
;     XCHG EAX  { EBP }
     MOV   EDX, [EBP]
     MOV   [EBP],  EAX
     MOV   EAX, EDX
     RET
EndCode

Code 2SWAP ;( x1 x2 x3 x4 -- x3 x4 x1 x2 ) \ 94
; �������� ���⠬� ��� ���孨� ���� �祥�.
     MOV ECX, [EBP]
     MOV EBX, [EBP+4]
     MOV EDX, [EBP+8]
     MOV [EBP+8], ECX
     MOV [EBP+4], EAX
     MOV [EBP], EDX
     MOV EAX, EBX
     RET
EndCode

Code OVER ;( x1 x2 -- x1 x2 x1 ) \ 94
; �������� ����� x1 �� ���設� �⥪�.
     LEA EBP, [EBP-4]
     MOV  [EBP],  EAX
     MOV  EAX, [EBP+4]
     RET
EndCode

Code 2OVER ;( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 ) \ 94
; ����஢��� ���� �祥� x1 x2 �� ���設� �⥪�.
     MOV EDX, [EBP+8]
     MOV [EBP-4], EAX
     MOV [EBP-8], EDX
     MOV EAX, [EBP+4]
     LEA EBP, [EBP-8]
     RET
EndCode

Code NIP ;( x1 x2 -- x2 ) \ 94 CORE EXT
; ����� ���� ����� ��� ���設�� �⥪�.
     ADD EBP,  4
     RET
EndCode

Code ROT ;( x1 x2 x3 -- x2 x3 x1 ) \ 94
; �ப����� �� ���孨� ����� �⥪�.
;     XCHG EAX     [EBP]
;     XCHG EAX   4  [EBP]
     MOV  EDX, [EBP]
     MOV  [EBP], EAX
     MOV  EAX, [EBP+4]
     MOV  [EBP+4], EDX
     RET
EndCode


Code -ROT ;( x1 x2 x3 -- x3 x1 x2 ) ; !!!!!
; ���⭮� ROT
     MOV  EDX, [EBP+4]
     MOV  [EBP+4], EAX
     MOV  EAX, [EBP]
     MOV  [EBP], EDX
     RET
EndCode

Code PICK      ;( ... +n -- ... w ) \ Copy the nth stack item to tos.
      MOV    EAX, [EBP + EAX*4 ]
      RET
EndCode

Code ROLL ;( xu xu-1 ... x0 u -- xu-1 ... x0 xu ) \ 94 CORE EXT
; ����� u. �������� u+1 ����� �� ���設� �⥪�.
; ����।������� ����� ���������, �᫨ ��। �믮������� ROLL
; �� �⥪� ����� 祬 u+2 ����⮢.
     OR EAX, EAX
     JZ SHORT LL1
     MOV ECX, EAX
     LEA EAX, [EAX*4]
     MOV EDX, EBP
     ADD EDX, EAX
     MOV EBX, [EDX]
LL2: LEA EDX, [EDX-4]
     MOV EAX, [EDX]
     MOV [EDX+4], EAX
     DEC ECX
     JNZ SHORT LL2
     MOV EAX, EBX
     JMP SHORT LL3
LL1: MOV EAX, [EBP]
LL3: LEA EBP, [EBP+4]
     RET
EndCode

Code TUCK ;( x1 x2 -- x2 x1 x2 ) \ 94
     LEA EBP, [EBP-4]
     MOV  EDX, [EBP+4]
     MOV  [EBP],  EDX
     MOV  [EBP+4],  EAX
     RET
EndCode

\ ================================================================
\ �⥪ �����⮢

Code 2>R   ; 94 CORE EXT
; ��������: ᥬ��⨪� ����।�����.
; �믮������: ;( x1 x2 -- ) ;( R: -- x1 x2 )
; ��७��� ���� �祥� x1 x2 �� �⥪ �����⮢. �������᪨
; �������⭮ SWAP >R >R.
     POP   EBX
     PUSH  DWORD PTR [EBP]
     PUSH  EAX
     LEA   EBP, [EBP+8]
     MOV   EAX, [EBP-4]
     JMP   EBX
EndCode

Code 2R>  ; 94 CORE EXT
; ��������: ᥬ��⨪� ����।�����.
; �믮������: ;( -- x1 x2 ) ;( R: x1 x2 -- )
; ��७��� ���� �祥� x1 x2 � �⥪� �����⮢. �������᪨
; �������⭮ R> R> SWAP.  \ !!!!
     LEA EBP, [EBP-8]
     POP EBX
     MOV [EBP+4], EAX
     POP EAX
     POP DWORD PTR [EBP]
     PUSH EBX
     RET
EndCode

Code R@    ; 94
; �ᯮ������: ;( -- x ) ;( R: x -- x )
; ��������: ᥬ��⨪� � ०��� ������樨 ����।�����.
     LEA EBP, [EBP-4]
     MOV [EBP], EAX
     MOV EAX, [ESP + 4 ]
     RET
EndCode

Code 2R@  ; 94 CORE EXT
; ��������: ᥬ��⨪� ����।�����.
; �믮������: ;( -- x1 x2 ) ;( R: x1 x2 -- x1 x2 )
; ����஢��� ���� �祥� x1 x2 � �⥪� �����⮢. �������᪨
; �������⭮ R> R> 2DUP >R >R SWAP.
     LEA EBP, [EBP-8]
     MOV [EBP+4], EAX
     MOV EAX, [ESP + { 2 CELLS } ]
     MOV DWORD PTR [EBP],  EAX
     MOV  EAX, [ESP + 4 ]
     RET
EndCode

\ ================================================================
\ ����樨 � �������

Code @ ;( a-addr -- x ) \ 94
; x - ���祭�� �� ����� a-addr.
     MOV EAX, [EAX ]
     RET
EndCode

Code ! ;( x a-addr -- ) \ 94
; ������� x �� ����� a-addr.
     MOV EDX, [EBP]
     MOV DWORD PTR [EAX ],  EDX
     MOV EAX , [EBP+4]
     ADD EBP , 8
     RET
EndCode

Code C@ ;( c-addr -- char ) \ 94
; ������� ᨬ��� �� ����� c-addr. ������騥 ���訥 ���� �祩�� �㫥��.
     MOVZX EAX, BYTE PTR [EAX ]
     RET
EndCode

Code C! ;( char c-addr -- ) \ 94
; ������� char �� ����� a-addr.
     MOV EDX, [EBP]
     MOV BYTE PTR [EAX ],  DL
     MOV EAX , [EBP+4]
     ADD EBP , 8
     RET
EndCode

Code W@ ;( c-addr -- word )
; ������� word �� ����� c-addr. ������騥 ���訥 ���� �祩�� �㫥��.
     MOVZX   EAX, WORD PTR [EAX ]
     RET
EndCode

Code W! ;( word c-addr -- )
; ������� word �� ����� a-addr.
     MOV EDX, [EBP]
     MOV WORD PTR [EAX ],  DX
     MOV   EAX , [EBP+4]
     ADD   EBP , 8
     RET
EndCode

Code 2@ ;( a-addr -- x1 x2 ) \ 94
; ������� ���� �祥� x1 x2, ����ᠭ��� �� ����� a-addr.
; x2 �� ����� a-addr, x1 � ᫥���饩 �祩��.
; �����ᨫ쭮 DUP CELL+ @ SWAP @
     MOV EDX, [EAX + 4 ]
     LEA EBP, [EBP-4]
     MOV DWORD PTR [EBP],  EDX
     MOV EAX,    DWORD PTR [EAX ]
     RET
EndCode

Code 2! ;( x1 x2 a-addr -- ) \ 94
; ������� ���� �祥� x1 x2 �� ����� a-addr,
; x2 �� ����� a-addr, x1 � ᫥������ �祩��.
; �����ᨫ쭮 SWAP OVER ! CELL+ !
     MOV EDX, [EBP]
     MOV [EAX], EDX
     MOV EDX, [EBP+4]
     MOV [EAX+4], EDX
     LEA EBP, [EBP+0CH]
     MOV EAX, [EBP-4]
     RET
EndCode

Code D@ ;( a-addr -- x1 x2 )
; 2@ SWAP
     MOV EDX, [EAX]
     LEA EBP, [EBP-4]
     MOV DWORD PTR [EBP],  EDX
     MOV EAX,    DWORD PTR [EAX+4]
     RET
EndCode

Code D! ;( x1 x2 a-addr -- )
; >R SWAP R> 2!
	MOV EDX, [EBP]
	MOV [EAX+4], EDX
	MOV EDX, [EBP+4]
	MOV [EAX], EDX
	LEA EBP, [EBP+0CH]
	MOV EAX, [EBP-4]
	RET
EndCode


Code EBX@ ;( -- EBX )
	LEA EBP, [EBP-4]
	mov [ebp],eax
	MOV EAX,EBX
	RET
EndCode

\ ================================================================
\ ���᫥���

Code 1+ ;( n1|u1 -- n2|u2 ) \ 94
; �ਡ����� 1 � n1|u1 � ������� �㬬� u2|n2.
     LEA EAX, [EAX+1]
     RET
EndCode

Code 1- ;( n1|u1 -- n2|u2 ) \ 94
; ������ 1 �� n1|u1 � ������� ࠧ����� n2|u2.
     LEA EAX, [EAX-1]
     RET
EndCode

Code 2+ ;( W -> W+2 )
     LEA EAX, [EAX+2]
     RET
EndCode

Code 2- ;( W -> W-2 )
     LEA EAX, [EAX-2]
     RET
EndCode

Code 2* ;( x1 -- x2 ) \ 94
; x2 - १���� ᤢ��� x1 �� ���� ��� �����, � �����������
; �������� ���稬��� ��� �㫥�.
;  SHL EAX
;  LEA EAX, [EAX+EAX]
  LEA EAX, [EAX*2]
  RET 
EndCode

Code CELL+ ;( a-addr1 -- a-addr2 ) \ 94
; ������ ࠧ��� �祩�� � a-addr1 � ������� a-addr2.
     LEA EAX, [EAX+4]
     RET
EndCode

Code CELL- ;( a-addr1 -- a-addr2 ) \ 94
; ������ ࠧ��� �祩�� � a-addr1 � ������� a-addr2.
     LEA EAX, [EAX-4]
     RET
EndCode

Code CELLS ;( n1 -- n2 ) \ 94
; n2 - ࠧ��� n1 �祥�.
  LEA     EAX,  DWORD PTR [EAX *4 ]
  RET 
EndCode


Code + ;( n1|u1 n2|u2 -- n3|u3 ) \ 94
; ������� n1|u1 � n2|u2 � ������� �㬬� n3|u3.
     ADD EAX,  DWORD PTR [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code D+ ;( d1|ud1 d2|ud2 -- d3|ud3 ) \ 94 DOUBLE
; ������� d1|ud1 � d2|ud2 � ���� �㬬� d3|ud3.
       MOV EDX,    DWORD PTR [EBP]
       ADD DWORD PTR [EBP + { 2 CELLS } ],  EDX
       ADC EAX, DWORD PTR [EBP +4 ]
       LEA EBP, [EBP+8]
       RET
EndCode

Code D- ;( d1 d2 -- d3 ) \ 94 DOUBLE
; perform a double subtract (64bit)
       MOV EDX,   DWORD PTR [EBP]
       SUB DWORD PTR [EBP + { 2 CELLS } ],  EDX
       SBB [EBP+4], EAX
       MOV EAX, DWORD PTR [EBP +4 ]
       LEA EBP, [EBP+8]
       RET
EndCode
               
Code - ;( n1|u1 n2|u2 -- n3|u3 ) \ 94
; ������ n2|u2 �� n1|u1 � ������� ࠧ����� n3|u3.
     NEG EAX
     ADD EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code 1+! ;( A -> )
     INC   DWORD PTR [EAX ]
     MOV   EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code 0! ;( A -> )
     MOV   DWORD PTR [EAX ], 0 
     MOV   EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code COUNT ;( c-addr1 -- c-addr2 u ) \ 94
; ������� ��ப� ᨬ����� �� ��ப� � ���稪�� c-addr1.
; c-addr2 - ���� ��ࢮ�� ᨬ���� �� c-addr1.
; u - ᮤ�ন��� ���� c-addr1, ��饥�� ������ ��ப� ᨬ�����,
; ��稭��饩�� � ���� c-addr2.
     LEA EBP, [EBP-4]
     LEA  EDX,   DWORD PTR [EAX +1 ]
     MOV  DWORD PTR [EBP],    EDX
     MOVZX EAX,  BYTE PTR [EAX ]
     RET
EndCode

Code * ;( n1|u1 n2|u2 -- n3|u3 ) \ 94
; ��६������ n1|u1 � n2|u2 � ������� �ந�������� n3|u3.
     IMUL DWORD PTR [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code AND ;( x1 x2 -- x3 ) \ 94
; x3 - ����⮢�� "�" x1 � x2.
     AND EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code OR ;( x1 x2 -- x3 ) \ 94
; x3 - ����⮢�� "���" x1 � x2.
     OR EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code XOR ;( x1 x2 -- x3 ) \ 94
; x3 - ����⮢�� "�᪫���饥 ���" x1 � x2.
     XOR EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code INVERT ;( x1 -- x2 ) \ 94
; ������஢��� �� ���� x1 � ������� �������� ������� x2.
     NOT EAX
     RET
EndCode

Code NEGATE ;( n1 -- n2 ) \ 94
; n2 - ��䬥��᪠� ������� n1.
     NEG EAX
     RET
EndCode

Code ABS ;( n -- u ) \ 94
; u - ��᮫�⭠� ����稭� n.
     TEST  EAX, EAX
     JS  { ' NEGATE }
     RET
EndCode

Code DNEGATE ;( d1 -- d2 ) \ 94 DOUBLE
; d2 १���� ���⠭�� d1 �� ���.
     MOV  EDX, [EBP]
     NEG  EAX
     NEG  EDX
     SBB  EAX,   0
     MOV  DWORD PTR [EBP],  EDX
     RET
EndCode

Code NOOP ;( -> )
     RET
EndCode

Code S>D ;( n -- d ) \ 94
; �८�ࠧ����� �᫮ n � ������� �᫮ d � ⥬ �� �᫮�� ���祭���.
     LEA EBP, [EBP-4]
     MOV  [EBP], EAX
     CDQ
     MOV  EAX, EDX
     RET
EndCode

Code D>S ;( d -- n ) \ 94 DOUBLE
; n - �������� d.
; �᪫��⥫쭠� ����� ���������, �᫨ d ��室���� ��� ���������
; �������� �������� �ᥫ.
     MOV     EAX, [EBP]
     ADD     EBP, 4
     RET
EndCode

Code U>D ;( U -> D ) \ ������ �᫮ �� ������� �筮�� �㫥�
     LEA EBP, [EBP-4]
     MOV  [EBP], EAX
     XOR  EAX, EAX
     RET
EndCode

Code C>S ;( c -- n )  \ ������ CHAR
     MOVSX  EAX, AL
     RET
EndCode

Code UM* ;( u1 u2 -- ud ) \ 94
; ud - �ந�������� u1 � u2. �� ���祭�� � ��䬥⨪� ����������.
       MUL  DWORD PTR [EBP] 
       MOV  [EBP], EAX
       MOV  EAX, EDX
       RET
EndCode

Code / ;( n1 n2 -- n3 ) \ 94
; ������ n1 �� n2, ������� ��⭮� n3.
; �᪫��⥫쭠� ����� ���������, �᫨ n2 ࠢ�� ���.
; �᫨ n1 � n2 ࠧ������� �� ����� - �����頥�� १���� ������ ��
; ॠ����樨.
     MOV  ECX,  EAX
     MOV  EAX, [EBP]
     CDQ
     IDIV ECX
     LEA EBP, [EBP+4]
     RET
EndCode

Code U/ ;( W1, W2 -> W3 ) \ ����������� ������� W1 �� W2
     MOV ECX, EAX
     MOV EAX, [EBP]
     XOR EDX, EDX
     LEA EBP, [EBP+4]
     DIV ECX
     RET
EndCode

Code +! ;( n|u a-addr -- ) \ 94     \ !!!!!
; �ਡ����� n|u � �����୮�� ��� �� ����� a-addr.
     MOV EDX, [EBP]
     ADD DWORD PTR [EAX ],  EDX
     MOV EAX, [EBP+4]
     LEA EBP, [EBP+8]
     RET
EndCode

Code MOD ;( n1 n2 -- n3 ) \ 94
; ������ n1 �� n2, ������� ���⮪ n3.
; �᪫��⥫쭠� ����� ���������, �᫨ n2 ࠢ�� ���.
; �᫨ n1 � n2 ࠧ������� �� ����� - �����頥�� १���� ������ ��
; ॠ����樨.
     MOV  ECX,  EAX
     MOV  EAX, [EBP]
     LEA EBP, [EBP+4]
     CDQ
     IDIV ECX
     MOV  EAX, EDX
     RET
EndCode

Code /MOD ;( n1 n2 -- n3 n4 ) \ 94
; ������ n1 �� n2, ���� ���⮪ n3 � ��⭮� n4.
; ���������筠� ����� ���������, �᫨ n2 ���.
       MOV ECX, EAX
       MOV EAX, [EBP]
       CDQ
       IDIV ECX
       MOV [EBP], EDX
       RET
EndCode

Code UMOD ;( W1, W2 -> W3 ) \ ���⮪ �� ������� W1 �� W2
     MOV ECX,  EAX
     XOR EDX, EDX
     MOV EAX, [EBP]
     LEA EBP, [EBP+4]
     DIV ECX
     MOV EAX, EDX
     RET
EndCode

Code UM/MOD ;( ud u1 -- u2 u3 ) \ 94
; ������ ud �� u1, ������� ��⭮� u3 � ���⮪ u2.
; �� ���祭�� � ��䬥⨪� ����������.
; �᪫��⥫쭠� ����� ���������, �᫨ u1 ���� ��� ��⭮�
; ��室���� ��� ��������� �������� ����������� �ᥫ.
       MOV ECX, EAX
       MOV EDX, [EBP]
       MOV EAX, [EBP+4]
       DIV ECX
       LEA EBP, [EBP+4]
       MOV [EBP], EDX

       RET
EndCode

Code 2/ ;( x1 -- x2 ) \ 94
; x2 - १���� ᤢ��� x1 �� ���� ��� ��ࠢ� ��� ��������� ���襣� ���.
     SAR   EAX,1
     RET
EndCode

Code */MOD ;( n1 n2 n3 -- n4 n5 ) \ 94
; �������� n1 �� n2, ������� �஬������ ������� १���� d.
; ��������� d �� n3, ������� ���⮪ n4 � ��⭮� n5.
     MOV     EBX, EAX
     MOV     EAX, [EBP]
     MOV     ECX, [EBP+4]
     IMUL    ECX
     IDIV    EBX
     MOV  [EBP+4], EDX
     LEA EBP, [EBP+4]
     RET
EndCode

Code M* ;( n1 n2 -- d ) \ 94
; d - ������� १���� 㬭������ n1 �� n2.
     IMUL DWORD PTR [EBP]
     MOV  [EBP], EAX
     MOV  EAX,  EDX
     RET
EndCode

Code LSHIFT ;( x1 u -- x2 ) ; 94
; �������� x1 �� u ��� �����. �������� �㫨 � �������� ���稬� ����,
; �᢮�������� �� ᤢ���.
; ���������筠� ����� ���������, �᫨ u ����� ��� ࠢ��
; ��� ��� � �祩��.
     MOV ECX, EAX
     MOV EAX, [EBP]
     SHL EAX, CL
     LEA EBP, [EBP+4]
     RET
EndCode

Code RSHIFT ;( x1 u -- x2 ) \ 94
; �������� x1 �� u ��� ��ࠢ�. �������� �㫨 � �������� ���稬� ����,
; �᢮�������� �� ᤢ���.
; ���������筠� ����� ���������, �᫨ u ����� ��� ࠢ��
; ��� ��� � �祩��.
     MOV ECX, EAX
     MOV EAX, [EBP]
     SHR EAX, CL
     LEA EBP, [EBP+4]
     RET
EndCode

Code SM/REM ;( d1 n1 -- n2 n3 ) \ 94
; ��������� d1 �� n1, ������� ᨬ����筮� ��⭮� n3 � ���⮪ n2.
; �室�� � ��室�� ��㬥��� �������.
; ���������筠� ����� ���������, �᫨ n1 ����, ��� ��⭮� ���
; ��������� �������� �������� �ᥫ.
     MOV EBX, EAX
     MOV EDX, [EBP]
     MOV EAX, [EBP+4]
     IDIV EBX
     LEA EBP, [EBP+4]
     MOV [EBP], EDX
     RET
EndCode

Code FM/MOD ;( d1 n1 -- n2 n3 ) \ 94
; ��������� d1 �� n1, �������� ������� n3 � ������� n2.
; ������� � �������� ��������� ��������.
; ������������� �������� ���������, ���� n1 ����, ��� ������� ���
; ��������� ��������� �������� �����.
        MOV ECX, EAX
        MOV EDX, [EBP]
        MOV EBX, EDX
        MOV EAX, [EBP+4]
        IDIV ECX
        TEST EDX, EDX            ; �������-�� ����?
        JZ  SHORT @@1
        XOR EBX, ECX             ; � ��������� ������� �����?
        JNS SHORT @@1
        DEC EAX
        ADD EDX, ECX
@@1:    LEA EBP, [EBP+4]
        MOV [EBP], EDX
        RET
EndCode

\ ================================================================
\ �ࠢ�����

Code = ;( x1 x2 -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� x1 ����⭮ ࠢ�� x2.
     XOR  EAX, [EBP]
     SUB  EAX, 1
     SBB  EAX, EAX
     LEA EBP, [EBP+4]
     RET
EndCode

Code <> ;( x1 x2 -- flag ) \ 94 CORE EXT
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� x1 �� ࠢ�� x2.
     XOR  EAX, [EBP]
     NEG  EAX
     SBB  EAX,  EAX
     LEA EBP, [EBP+4]
     RET
EndCode

Code < ;( n1 n2 -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� n1 ����� n2.
       CMP  [EBP], EAX
       SETGE AL
       AND  EAX, 01
       DEC  EAX
       LEA EBP, [EBP+4]
       RET
EndCode

Code > ;( n1 n2 -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� n1 ����� n2.
       CMP  EAX, [EBP]
       SETGE AL
       AND  EAX, 01
       DEC  EAX
       LEA EBP, [EBP+4]
       RET
EndCode

Code WITHIN     ;( n1 low high -- f1 ) \ f1=true if ((n1 >= low) & (n1 < high))
      MOV  EBX, [EBP+4]
      SUB  EAX, [EBP]
      SUB  EBX, [EBP]
      SUB  EBX, EAX
      SBB  EAX, EAX
      ADD  EBP, 8
      RET
EndCode

Code D< ;( d1 d2 -- flag ) \ DOUBLE
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� d1 ����� d2.
     MOV EBX, [EBP]
     CMP DWORD PTR [EBP +8 ], EBX
     SBB DWORD PTR [EBP +4 ], EAX
     MOV EAX,  0
     SIF <
       DEC EAX
     STHEN
     ADD EBP,   0CH
     RET
EndCode

Code D> ;( d1 d2 -- flag ) \ DOUBLE
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� d1 ����� d2.
    MOV EBX, [EBP]
    CMP EBX, [EBP+8]
    SBB EAX, [EBP+4]
    SAR EAX,   1FH
    ADD EBP,   0CH
    RET
EndCode

Code U< ;( u1 u2 -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� u1 ����� u2.
    CMP  [EBP], EAX
    SBB  EAX, EAX
    ADD  EBP,  04
    RET
EndCode

Code U> ;( u1 u2 -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� u1 ����� u2.
    CMP  EAX, [EBP]
    SBB  EAX, EAX
    ADD  EBP,   04
    RET
EndCode

Code 0< ;( n -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� n ����� ���.
    SAR EAX,   1F
    RET
EndCode

Code 0= ;( x -- flag ) \ 94
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� x ࠢ�� ���.
     SUB  EAX, 1
     SBB  EAX, EAX 
     RET
EndCode

Code 0<> ;( x -- flag ) \ 94 CORE EXT
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� x �� ࠢ�� ���.
     NEG  EAX
     SBB  EAX, EAX
     RET
EndCode

Code D0= ;( xd -- flag ) \ 94 DOUBLE
; flag "��⨭�" ⮣�� � ⮫쪮 ⮣��, ����� xd ࠢ�� ���.
     OR   EAX, [EBP]
     SUB  EAX, 1
     SBB  EAX, EAX 
     LEA EBP, [EBP+4]
     RET
EndCode

Code D= ;( xd1 xd2 -- flag ) \ 94 DOUBLE
; flag is true if and only if xd1 is bit-for-bit the same as xd2
     MOV  EDX,[EBP]
     XOR  EAX,[EBP+4]
     XOR  EDX,[EBP+8]
      OR  EAX,EDX
     SUB  EAX,1
     SBB  EAX,EAX
     LEA  EBP,[EBP+0CH]
     RET
EndCode

Code D2* ;( xd1 -- xd2 ) \ 94 DOUBLE
; xd2 is the result of shifting xd1 one bit toward the most-significant
; bit, filling the vacated least-significant bit with zero     
     SHL DWORD PTR [EBP],  1
     RCL  EAX,   1
     RET
EndCode

Code D2/ ;( xd1 -- xd2 ) \ 94 DOUBLE
; xd2 is the result of shifting xd1 one bit toward the least-significant bit,
; leaving the most-significant bit unchanged
     SAR  EAX,  1
     RCR DWORD PTR [EBP], 1
     RET
EndCode

\ ================================================================
\ ��ப�

Code -TRAILING ;( c-addr u1 -- c-addr u2 ) \ 94 STRING
; �᫨ u1 ����� ���, u2 ࠢ�� u1, 㬥��襭���� �� ������⢮ �஡���� � ����
; ᨬ���쭮� ��ப�, �������� c-addr � u1. �᫨ u1 ���� ��� ��� ��ப� ��⮨�
; �� �஡����, u2 ����.
     PUSH EDI
     MOV  ECX, EAX
     SIF C0<>
       MOV EDI, DWORD PTR [EBP]
       ADD EDI, ECX
       DEC EDI 
       MOV AL,  20H
       STD
       REPZ SCASB
       SIF 0<>
          INC ECX 
       STHEN
       CLD
       MOV  EAX, ECX 
     STHEN
     POP EDI
     RET
EndCode

Code COMPARE ;( c-addr1 u1 c-addr2 u2 -- n ) \ 94 STRING !!!!!
; �ࠢ���� ��ப�, �������� c-addr1 u1, � ��ப��, �������� c-addr2 u2.
; ��ப� �ࠢ��������, ��稭�� � �������� ���ᮢ, ᨬ��� �� ᨬ�����, �� �����
; �������� ���⪮� �� ��ப ��� �� ��宦����� ࠧ��稩. �᫨ ��� ��ப�
; �������, n ����. �᫨ ��� ��ப� ������� �� ����� �������� ���⪮� ��
; ��ப, � n ����� ������ (-1), �᫨ u1 ����� u2, ���� ������ (1).
; �᫨ ��� ��ப� �� ������� �� ����� �������� ���⪮� �� ��ப, � n �����
; ������ (-1), �᫨ ���� ��ᮢ�����騩 ᨬ��� ��ப�, �������� c-addr1 u1
; ����� ����襥 �᫮��� ���祭��, 祬 ᮮ⢥�����騩 ᨬ��� � ��ப�,
; �������� c-addr2 u2, � ������ � ��⨢��� ��砥.
   PUSH EDI
   MOV  ECX, EAX
   SUB  EAX, EAX
   CMP  ECX, [EBP +4 ]
   SIF 0<>
      SIF U<
         INC   EAX
      SELSE
         DEC EAX
         MOV ECX, [EBP +4 ]
      STHEN
   STHEN
   MOV ESI, [EBP + { 2 CELLS } ]
   MOV EDI, [EBP]
   REPE CMPSB
   SIF 0<>
      SIF U>=
         MOV EAX,  1
      SELSE
         MOV EAX, -1
      STHEN
   STHEN
   LEA  EBP, [EBP + { 3 CELLS } ]
   POP EDI          
   RET
EndCode

Code SEARCH ;( c-addr1 u1 c-addr2 u2 -- c-addr3 u3 flag ) \ 94 STRING
; �ந����� ���� � ��ப�, �������� c-addr1 u1, ��ப�, �������� c-addr2 u2.
; �᫨ 䫠� "��⨭�", ᮢ������� ������� �� ����� c-addr3 � ��⠢訬��� u3
; ᨬ������. �᫨ 䫠� "����", ᮢ������� �� �������, � c-addr3 ���� c-addr1,
; � u3 ���� u1.        \ !!!!!
      LEA EBP, [EBP-4]
      MOV  [EBP], EAX
      PUSH EDI
      CLD
      MOV EBX, DWORD PTR [EBP]
      OR  EBX, EBX
      SIF 0<> 
        MOV EDX, DWORD PTR [EBP + { 2 CELLS } ]
        MOV EDI, DWORD PTR [EBP + { 3 CELLS } ]
        ADD EDX, EDI
        SBEGIN
           MOV ESI, DWORD PTR [EBP +4 ]
           LODSB
           MOV ECX, EDX
           SUB ECX, EDI
           JECXZ LLD
           REPNZ
           SCASB
           JNE SHORT LLD   ; �� �ᥩ ��ப� ��� ��ࢮ�� ᨬ���� �᪮��� ��ப�
           CMP EBX,   1
           JZ SHORT LLC   ; �᪮��� ��ப� ����� ����� 1 � �������
           MOV ECX, EBX
           DEC ECX
           MOV EAX, EDX
           SUB EAX, EDI
           CMP EAX, ECX
           JC SHORT LLD   ; ���⮪ ��ப� ���� �᪮��� ��ப�
           PUSH EDI
           REPZ CMPSB
           POP EDI
        SUNTIL 0=
LLC:    DEC EDI       ; ��諨 ������ ᮢ�������
        SUB EDX, EDI
        MOV  DWORD PTR [EBP + { 3 CELLS } ], EDI
        MOV  DWORD PTR [EBP + { 2 CELLS } ], EDX
      STHEN
      MOV EAX,   -1 
      JMP SHORT LLA
LLD:  XOR EAX, EAX
LLA:  LEA EBP, [EBP+4]
      MOV [EBP], EAX
      POP EDI
      MOV EAX, [EBP]
      LEA EBP, [EBP+4]
      RET
EndCode

Code CMOVE ;( c-addr1 c-addr2 u -- ) \ 94 STRING
; �᫨ u ����� ���, ����஢��� u ��᫥����⥫��� ᨬ����� �� ����࠭�⢠
; ������ ��稭�� � ���� c-addr1 � c-addr2, ᨬ��� �� ᨬ�����, ��稭�� �
; ������ ���ᮢ � ���訬.
     MOV  EDX, EDI
     MOV  ECX, EAX
     MOV  EDI, DWORD PTR [EBP]
     MOV  ESI, DWORD PTR [EBP +4 ]
     CLD
     REPZ   MOVSB
     LEA EBP, [EBP+0CH]
     MOV EAX, [EBP-4]
     MOV EDI, EDX
     RET
EndCode

: QCMOVE CMOVE ;

Code CMOVE> ;( c-addr1 c-addr2 u -- ) \ 94 STRING
; �᫨ u ����� ���, ����஢��� u ��᫥����⥫��� ᨬ����� �� ����࠭�⢠
; ������ ��稭�� � ���� c-addr1 � c-addr2, ᨬ��� �� ᨬ�����, ��稭�� �
; ����� ���ᮢ � ����訬.

       MOV EDX, EDI
       MOV ECX, EAX
       MOV EDI, [EBP]
       MOV ESI, [EBP+4]
       STD
       ADD EDI, ECX
       DEC EDI
       ADD ESI, ECX
       DEC ESI
       REP MOVSB
       MOV EDI, EDX
       LEA EBP, [EBP+0CH]
       MOV EAX, [EBP-4]
     RET
EndCode

Code FILL ;( c-addr u char -- ) \ 94  \ !!!!!
; �᫨ u ����� ���, ��᫠�� char � u ���⮢ �� ����� c-addr.
   MOV EDX, EDI
   MOV ECX, [EBP]
   MOV EDI, [EBP+4]
   CLD
   REP STOSB
   MOV EDI, EDX
   LEA EBP, [EBP+0CH]
   MOV EAX, [EBP-4]
   RET
EndCode

Code ZCOUNT ;( c-addr -- c-addr u )
     LEA EBP, [EBP-4]
     MOV  [EBP], EAX
     XOR  EBX, EBX
     SBEGIN
       MOV  BL, BYTE PTR [EAX ]
       INC  EAX 
       OR   BL,  BL
     SUNTIL 0=
     DEC  EAX
     SUB  EAX, [EBP]
     RET
EndCode

\ ================================================================
\ �����⥫� �⥪��

Code SP! ;( A -> )
     LEA EBP, [EAX+4]
     MOV EAX, [EBP-4]
     RET
EndCode

Code RP! ;( A -> )
     POP EBX
     MOV ESP, EAX
     MOV EAX, [EBP]
     LEA EBP, [EBP+4]
     JMP EBX
EndCode

Code SP@ ;( -> A )
     LEA EBP, [EBP-4]
     MOV [EBP], EAX
     MOV EAX, EBP
     RET
EndCode

Code RP@ ;( -- RP )
     LEA EBP, [EBP-4]
     MOV  [EBP], EAX
     LEA  EAX, [ESP + 4 ]
     RET
EndCode


\ ================================================================
\ ������� ��⮪� (����� ����� ���)

Code TlsIndex! ;( x -- ) \ 㪠��⥫� �����쭮�� �㫠 ��⮪�
     MOV  EDI, EAX
     MOV  EAX, [EBP]
     LEA EBP, [EBP+4]
     RET
EndCode

Code TlsIndex@ ;( -- x )
     LEA EBP, [EBP-4]
     MOV [EBP], EAX
     MOV  EAX, EDI
     RET
EndCode

\ ================================================================
\ �����

Code C-J
	LEA EBP, [EBP-4]
	MOV  [EBP], EAX
	MOV EAX, DWORD PTR [ESP + { 3 CELLS } ]
	SUB EAX, DWORD PTR [ESP + { 4 CELLS } ]
	RET
EndCode

( inline'� ��� �������樨 横��� )

Code C-DO
      LEA  EBP, [EBP+8]
      MOV  EDX, 80000000H
      SUB  EDX, [EBP-8]
      LEA  EBX, [EAX+EDX]
      MOV  EAX, [EBP-4]
      MOV  EDX, EDX  ; FOR OPT
;      PUSH EDX
;      PUSH EBX
      RET
EndCode

Code C-?DO
      CMP  EAX, [EBP-8]
      SIF  0=
        MOV  EAX, [EBP-4]
        JMP  EBX
      STHEN
      PUSH EBX
      MOV     EBX , 80000000
      SUB  EBX, [EBP-8]
      PUSH EBX  ; 80000000h-to
      ADD  EBX, EAX
      PUSH EBX  ; 80000000H-to+from
      MOV  EAX, [EBP-4]
      RET
EndCode

Code ADD[ESP],EAX 
  ADD [ESP] , EAX 
 RET
EndCode

Code C-I
   LEA EBP, [EBP-4]
   MOV  [EBP], EAX
   MOV  EAX, DWORD PTR [ESP]
   SUB  EAX, [ESP+4]
   RET
EndCode

Code C->R
     PUSH EAX
     MOV  EAX, [EBP]
     LEA  EBP, [EBP+4]
     RET
EndCode

Code C-R>
     LEA  EBP, [EBP-4]
     MOV  [EBP], EAX
     POP EAX
     RET
EndCode

Code C-RDROP
     ADD  ESP, 4 
     RET
EndCode

Code C-2RDROP
     ADD  ESP, 8
     RET
EndCode

Code C-3RDROP
     ADD  ESP, 0CH
     RET
EndCode

TRUE [IF]
Code C-EXECUTE ;( i*x xt -- j*x ) \ 94
; ����� xt � �⥪� � �믮����� �������� �� ᥬ��⨪�.
; ��㣨� ��������� �� �⥪� ��।������� ᫮���, ���஥ �믮������.
     MOV  EDX, EAX
     MOV  EAX, [EBP]
     LEA  EBP, [EBP+4]
     CALL EDX
     RET
EndCode
[THEN]

Code EXECUTE ;( i*x xt -- j*x ) \ 94
; ����� xt � �⥪� � �믮����� �������� �� ᥬ��⨪�.
; ��㣨� ��������� �� �⥪� ��।������� ᫮���, ���஥ �믮������.
     MOV EBX, EAX
     MOV EAX, [EBP]
     LEA EBP, [EBP+4]
     JMP EBX
EndCode

Code @EXECUTE ;( i*x xt -- j*x )
     MOV EBX, EAX
     MOV EAX, [EBP]
     LEA EBP, [EBP+4]
     JMP [EBX]
EndCode

\ ================================================================
\ �����প� LOCALS

Code DRMOVE ;( x1 ... xn n*4 -- )
; ��७��� n �ᥫ � �⥪� ������ �� �⥪ �����⮢
     POP  EDX ; ���� ������
     MOV  ESI, EAX
LL1: 
     PUSH DWORD PTR [EBP+ESI-4]
     SUB  ESI, 4
     JNZ  SHORT LL1
     ADD  EBP, EAX
     MOV  EAX, [EBP]
     LEA  EBP, [EBP+4]
     JMP  EDX
EndCode

Code NR> ;( R: x1 ... xn n -- D: x1 ... xn n )
; ��७��� n �ᥫ � �⥪� �����⮢ �� �⥪ ������
; �᫨ n=0 �������� 0
     POP  EDX ; ���� ������
     LEA  EBP, [EBP-4]
     MOV  [EBP], EAX
     POP  EAX
     OR   EAX, EAX
     JNZ  @@2
     JMP  EDX

@@2: LEA  EAX, [EAX*4]
     MOV  ESI, EAX
@@1: 
     MOV  EBX, EBP
     SUB  EBX, ESI
     POP  DWORD PTR [EBX]
     SUB  ESI,  4
     JNZ  SHORT @@1
     SUB  EBP, EAX
     SAR  EAX,  2
     JMP  EDX
EndCode

Code N>R ;( D: x1 ... xn n -- R: x1 ... xn n )
; ��७��� n �ᥫ � �⥪� ������ �� �⥪ �����⮢
     LEA  EBP, [EBP-4]
     MOV  [EBP], EAX
     LEA EAX, [EAX*4+4]

     POP  EDX ; ���� ������
     MOV  ESI, EAX
@@1: 
     PUSH DWORD PTR [EBP+ESI-4]
     SUB  ESI,  4
     JNZ  SHORT @@1
     ADD  EBP, EAX
     MOV  EAX, [EBP]
     LEA  EBP, [EBP+4]
     JMP  EDX
EndCode

Code NRCOPY ;( D: i*x i -- D: i*x i R: i*x i )
; ᪮��஢��� n �ᥫ � �⥪� ������ �� �⥪ �����⮢
     MOV  ECX, EAX
     LEA  ECX, [ECX*4]

     POP  EDX ; ���� ������
     JECXZ @@2
     MOV  ESI, ECX
@@1: 
     PUSH DWORD PTR [ESI+EBP-4]
     SUB  ESI,  4
     JNZ  SHORT @@1
@@2:
     PUSH EAX
     JMP  EDX
EndCode

Code RP+@ ;( offs -- x )
; ����� �᫮ � ᬥ饭��� offs ���� �� ���設� �⥪� �����⮢ (0 RP+@ == RP@)
     MOV EAX, [EAX+ESP+4]
     RET
EndCode
     
Code RP+ ;( offs -- addr )
; ����� ���� � ᬥ饭��� offs ���� �� ���設� �⥪� �����⮢
	LEA EAX, [EAX+ESP+4]
	RET
EndCode

Code RP+! ;( x offs -- )
; ������� �᫮ x �� ᬥ饭�� offs ���� �� ���設� �⥪� �����⮢
	MOV  EBX, [EBP]
	MOV  [EAX+ESP+4], EBX
	LEA  EBP, [EBP+8]
	MOV  EAX, [EBP-4]
	RET
EndCode

Code RALLOT ;( n -- addr )
; ��१�ࢨ஢��� n �祥� �� �⥪� �����⮢,
; ᤥ���� � ���樠����樥� (� � �᫨ ����� 8� �뤥���, exception �����)
     POP  EDX
     MOV  ECX, EAX
     XOR  EAX, EAX
@@1: PUSH EAX
     DEC  ECX
     JNZ  SHORT @@1
     MOV  EAX, ESP
     JMP  EDX
EndCode

Code (RALLOT) ;( n -- )
; ��१�ࢨ஢��� n �祥� �� �⥪� �����⮢
     POP  EDX
     MOV  ECX, EAX
     XOR  EAX, EAX
@@1: PUSH EAX
     DEC  ECX
     JNZ  SHORT @@1
     MOV  EAX, [EBP]
     LEA  EBP, [EBP+4]
     JMP  EDX
EndCode

Code RFREE ;( n -- )
; ������ n �祥� �⥪� �����⮢
     POP  EDX
     LEA  ESP, [ESP+EAX*4]
     MOV EAX, [EBP]
     LEA EBP, [EBP+4]
     JMP  EDX
EndCode

Code (LocalsExit) ;( -- )
; ������ ������ � �⥪ ����⮢, �᫮ ���� ����� �� �⥪�
     POP  EBX
     ADD  ESP, EBX
     RET
EndCode

Code TIMER@ ;( -- tlo thi ) \ ���쪮 ��� Intel Pentium � ���!!!
; �������� ���祭�� ⠩��� ������ ��� ud
   MOV [EBP-4], EAX
   RDTSC
   MOV [EBP-8], EDX
   LEA EBP,  [EBP-8]
   XCHG EAX, [EBP]
   RET
EndCode

\ ��� ��⠫��� �����஢ �᪮��������:
\ : TIMER@ 0 GetTickCount ;

Code TRAP-CODE ;( D: j*x u R: i*x i -- i*x u )
; �ᯮ����⥫쭮� ᫮�� ��� ����⠭������� ���祭��, ��࠭�����
; ��। CATCH �� �⥪� �����⮢
     POP  EDX
     POP  ESI
     OR   ESI, ESI
     JZ   @@2
     LEA  ESI, [ESI*4]
     MOV  ECX, ESI
@@1: MOV  EBX, [ESI+ESP-4]
     MOV  [ESI+EBP-4], EBX
     SUB  ESI, 4
     JNZ  SHORT @@1
     ADD  ESP, ECX
@@2: JMP  EDX
EndCode

DECIMAL
