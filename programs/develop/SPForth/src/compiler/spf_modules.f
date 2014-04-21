(  Working with forth modules
   Copyright [C] 2000 D.Yakimov day@forth.org.ru
)

: MODULE: ( "name" -- old-current )
\ start a forth module
\ ���� ����� ������ ��� ����������, ���������� ���������� � ����
  >IN @ ['] ' CATCH
  IF >IN ! VOCABULARY GET-CURRENT
     ALSO LATEST NAME> EXECUTE DEFINITIONS
  ELSE 
     NIP GET-CURRENT SWAP ALSO EXECUTE DEFINITIONS
  THEN
;

: EXPORT ( old-current -- old-current )
\ export some module definitions
   DUP SET-CURRENT
;

: ;MODULE ( old-current -- )
\ finish the module
   SET-CURRENT PREVIOUS
;

: {{ ( "name" -- )
\ ������ � ORDER wordlist, �-�� ���� "name"
\ ��� vocabulary ���� "name" - vocabulary
        DEPTH >R
        ALSO ' EXECUTE
        DEPTH R> <>             IF      \ wid on the stack?
             CONTEXT !          THEN
; IMMEDIATE

: }}
   PREVIOUS
; IMMEDIATE