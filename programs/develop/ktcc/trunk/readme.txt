The main file of metcc is "tcc.c". It certainly can be compiled by MinGW Studio. 
In order to compile MenuetOS program you must have start.o, metcc.exe in the same 
directory. The command line should be of type "metcc.exe program.c melibc.a -oprogram".
In order to compile "melibc.a" you should configure paths is compile.js and run it. 
------------------------------------------------------------------------------------
��� ���������� melibc ���������� ��������� ������ libc/make.cmd
�� ��������� ��������� ��� � ���������� ��������� PATH � ��� ������ ���� � ������ mingw32
� � ���������� fasm.
------------------------------------------------------------------------------------
��� ����� ��������� ���������� ���������� �� ����� � ���� 
http://meos.sysbin.com/viewtopic.php?t=565&highlight=metcc
For more help go to link above