( ��楤��� �६��� �믮������ ��� CONSTANT, VARIABLE, etc.
  ��-������ᨬ� ᫮��.
  Copyright [C] 1992-1999 A.Cherezov ac@forth.org
  �८�ࠧ������ �� 16-ࠧ�來��� � 32-ࠧ�來� ��� - 1995-96��
  ������� - ᥭ���� 1999
  ������஢���� ���ᨬ��� �.�.
  email:mak@mail.rtc.neva.ru
  http://informer.rtc.neva.ru/
  � � {812}105-92-03
  � � {812}552-47-64
)

Code _CREATE-CODE
     SUB  EBP,  4
     MOV [EBP] , EAX
     POP EAX
     RET
EndCode

Code _CONSTANT-CODE
     SUB EBP , 4
     MOV [EBP] , EAX
     POP EAX
     MOV EAX, [EAX]
     RET
EndCode

Code _USER-CODE
     SUB  EBP, 4
     MOV [EBP], EAX
     POP EAX
     MOV EAX, [EAX]
     ADD EAX,  EDI
     RET
EndCode

Code USER+ ;( offs -- addr )
     ADD EAX,  EDI
     RET
EndCode

Code _USER-VALUE-CODE
     SUB  EBP ,  4
     MOV [EBP] , EAX
     POP EAX
     MOV EAX, [EAX]
     ADD EAX,  EDI
     MOV EAX, [EAX]
     RET
EndCode


Code _USER-VECT-CODE
     POP  EBX
     MOV  EBX, [EBX]
     LEA  EBX, [EDI+EBX]
     MOV  EBX, [EBX]
     JMP  EBX
     RET
EndCode

Code _VECT-CODE
     POP EBX
     JMP [EBX]
EndCode

Code _TOVALUE-CODE
     POP EBX
     SUB EBX, 9
     MOV [EBX] , EAX
     MOV EAX, [EBP]
     ADD EBP, 4
     RET
EndCode

Code _TOUSER-VALUE-CODE
     POP EBX
     SUB EBX,  9
     MOV EBX, [EBX] ; ᬥ饭�� user-��६�����
     ADD EBX, EDI
     MOV [EBX] , EAX
     MOV EAX, [EBP]
     ADD EBP,  4
     RET
EndCode


Code _SLITERAL-CODE
     SUB   EBP, 8
     MOV   [EBP+4], EAX
     POP   EBX
     MOVZX EAX,  BYTE PTR [EBX]
     INC   EBX
     MOV   [EBP], EBX
     ADD   EBX, EAX
;     INC   EBX
     JMP   EBX
EndCode

Code _CLITERAL-CODE
     SUB   EBP, 4
     MOV   [EBP] , EAX
     POP   EAX
     MOVZX EBX, BYTE PTR [EAX]
     LEA   EBX, [EAX+EBX+1]
     JMP   EBX
EndCode

\EOF
' _CLITERAL-CODE VALUE CLITERAL-CODE
'   _CREATE-CODE VALUE   CREATE-CODE
'     _USER-CODE VALUE     USER-CODE
' _CONSTANT-CODE VALUE CONSTANT-CODE
'  _TOVALUE-CODE VALUE  TOVALUE-CODE
