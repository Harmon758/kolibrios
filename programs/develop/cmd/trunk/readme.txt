  CMD - ������������� ��������� ������ ��� Menuet
        �������� by Chemist dmitry_gt@tut.by

        ������ 0.27

	������� ����� ������� ������.

-----------------------------------------------------------------------

  CMD - Command Line Interpreter for Menuet
	Copyleft Chemist - dmitry_gt@tut.by

	Version 0.27

	As my english is not well, I will tell a few words about this
	program. It supports 14 commands, such as cp (copy file),
	rn (rename file), ps (process list), kill (kill process) etc.
	You can type 'help' after starting this shell to examine with
	other commands. Also it uses several special symbols, such as
	&, /, . and +. Use & sumbol to enter params. For example,
	type tinypad&cmd.asm to open cmd.asm file in tinypad editor.
	/ symbol is used to run special command scripts for CMD.
	It's syntax is: /filename without extension (For example,
	type /autoexec to execute autoexec.cmd file for CMD). And +
	symbol is used in cp and rn commands. You must type
	cp example.asm+example.bak to copy example.asm file to
	example.bak. Use dot to launch program, if CMD command and
	other external command match. For example, type help for
	'help' command or type .help to run 'help' program.

	This shell supports executeing special command
	scripts (something like .BAT files in MS-DOS). This files
	have an .CMD extinsion and u must use / symbol to execute it.
	U can use any CMD commands and filenames in this scripts.
	But you can't run other scripts from any CMD script (/ symbol).
	I recommed to use tipypad editor to edit this scripts and do
	not leave spaces and other symbols after commands in the script
	file, because it's executeing is not very developed yet.
	And it's strongly recommended to use ends command in the end
	of the script, because there are some problems with redrawing
	the window after executing scripts without this command.
	Every time when you launch CMD autoexec.cmd file automatically
	executes. You can edit or delete this file if you want.

	This version of CMD shell supports IPC. It mean, than you can
	write your own programs for this shell. Look at .ASM files in
	the Examples directory in this archive. I think that you will
	understand them without any difficulties. Do not forget, that
	you need CMDIPC.ASM file to compile this sources. It's
	recommended to compile it with MACROS.INC file (included in
	this archieve) to make their size smaller.

	And now about some bugs etc.

	----------------------------

	I've noticed, that there are some difficulties with files with
	1 or 2 symbols in extension. I recommend do not use such files
	not only in CMD, but almost in every programs in MeOS. It's
	possible to create such file in tinypad, and then (in MS-DOS or
	Windows) Scandisk will find errors in filenames etc. CMD do
	not support properly such filenames now.

	In CMDIPC.INC and CMD.ASM I used 5-th function (pause), because
	I need it when CMD communicates with IPC programs. It's
	even possible, that it will not enough 1/100 sec. on slow PC's,
	because CMD need this time to finish IPC-command from other
	IPC-program. U can change ebx value in CMDIPC.ASM in pause1:
	if you want. But it slows communication betwen CMD and
	IPC-programs for CMD. I hope, that you understand my words. :)

	Now (in this version) you can launch only one CMD shell.
	Because it's difficult to make communication betwen several
	copyes of CMD in memory and IPC-programs. I will solve this
	problem in future.

	I've tested this shell only on Ivan Poddubny's russian
	distributive. Also I recommend you to use it. :) CMD shell
	is included into this distributive (but possible not it's
	final version).

	Source code of this program is not optimized yet. I have such
	manner of programming from my childhood that my source code
	is not very "compact"

	etc. ;-)

	And now other information:
	--------------------------

	I do not want CMD design to change. I like white symbols on
	the black background. If you want to change the source and
	redistribute it, please, do not change it's design. :)

	If you will find some bugs or you have some wishes (or even you
	correct some mistakes in my english texts) -
	email me: dmitry_gt@tut.by

	And this program is distributed "as is". Use it on your own
	risk. ;-)

	And again - my English is not well. :)

	That's all!

	19.06.2004,
	Chemist
	
	Now russian text :)

-----------------------------------------------------------------------
 
  ��������� �������:

	ls [filename] - ����� ������ ������. ���� ����� ������� 
                        ������� ��� �����, �� ������� �������� ��� 
                        �� �������.

	cp [source_file+destination_file] - ������� ��� 
                        ����������� ������. ��������� ����� �� 
                        �������, � � ������ ������ ������� 
                        ��������������� ���������.

        rn [source_file+destination_file] - ������� ��� 
                        �������������� ������. ��� �� 
                        ��������� ����� �� �������, � � 
                        ������ ������ ������� ���������������
			���������.

	ps            - ����� ���������� � ��������� � �������.

        kill          - �������� ������� � �������. ��������, 
                        ����� ������� ������� ������� 4-� 
                        ������� ����� ��������, � �� PID. ����� 
                        �������� ������ � ��������� ������� ��� 
                        ������ ������� ps.

	help          - ����� ������� ���������� ����������.

        ver           - ����� ������������ ������ ��������������.

        cls           - ������� ������.

        exit          - ����� �� ��������������.

        del [filename] - ������� ���� � ��������. ��� ���������� 
                        �������������� ����� ����� �������� 
                        ��������������� ��������� �� ������.

	shutdown      - ��������� ������ �������.

	pause         - ������� ������� �������. ������������ ��� 
                        "��������������" ������������ � ���������� 
                        ��������� ��� ������� (��������������). 
                        ��������, ����� ������������ ������ ������ 
                        
                        echo Do you want to delete cmd.asm?
                        pause
                        del cmd.asm

                        ��� ����, ����� ���������������� ������� 
                        ������������, ����� �� �� ������� ���� cmd.asm 
                        ��� �������� ������ �������.

	pause >nul	�� �� �����, ������ ��� ������ ������
			'Press ane key to continue (ESC - cancel)'

	echo [text]     - ����� ������ �� �����. ������������� ��� 
                        ������ ������������ ���������� �� ���������� 
                        �������. ���� ������ ������� echo ��� ������, 
                        �� ��� ������ ������� ������� �� ��������� 
                        ������.

	ends            - �������, ��������� ������ �� ����������� 
                        ��������. ������ ��� �� ����������� ����������,
                        �.�. ������ ��������� �������, � ������� � 
                        ����� ������ �������� ������� ��� ������� EOL, 
                        �������� �������� � ������������ ����. ������ 
                        ����� ��� ������� ������������ � ������� 
                        �������� � ����� ������ ����� ������� ���� 
                        ��������.

-----------------------------------------------------------------------
	
  ����������� �������:

	/[��������� ������] - ������������ ��� ������ �� ���������� 
                        ���������� ������� �� �������. �� ����� 
                        �������������� � ��������������� � ����� 
                        ��������� ��������. ���� ���������� �����-
                        ������� ������� .cmd, �� ��� ��������� 
                        �������������.

	& - ������ ������ ������������ ��� �������� ���������� 
            ��������� ����������. ��������, ������� tinypad&cmd.asm 
            �������� ��������� tinypad �������� cmd.asm, 
            ��������������, tinypad ������� ���� cmd.asm.

	+ - ��������� ����� ��������� � ��������������� ������ � 
            �������� cp � rn.

	. - ��������� ��������� ����� ����� ���������, ���� ���� ���
	    ��������� � �������� CMD. ��������, help - ���������
	    ������� 'help', �� .help - ��������� ������� ���������
	    'help' (���� ����� ���� ����� � ���� :).

-----------------------------------------------------------------------

  ��������� �� �������:

	��� �������������� ��������� ������ ������������ ������� ESC, 
        BACKSPACE. ESC - ��� �������� ���� ��������� ������, 
        BACKSPACE - ��� �������� ���������� ���������� �������. �� 
        ����� ������, ������������� ����� ������, ��� HOME, END, ARROW 
        KEY etc. �� ����� ������, �.�. �������� ������� ������� 
        ������ � �� ������� ���������� ��������������. ������� � 
        ������� ��� �������� ���, ��� ���� � MS-DOS 6.22.

        ������� UPARROW ������������ ��� ������� ��������� ��������� 
        �������.

-----------------------------------------------------------------------

  ������������� IPC �� ������� ����������:

	�� ������ ������ ���������, ����������������� � CMD ����� IPC.
	� ��������, � �������, ��� ����� ���������� ��� ���������
	���������� ���������� ��� ������ CMD (���� �������� � ������).
	
	��� �����, � ����� ��������� ��� ���������� ���������� ����
	CMDIPC.INC (�����������, �� ����� ������ ��������� �����
	���������� ��������� ����������). ����� ����� ��� ������
	�������� 6 ������� IPC, ������� �� ������ �������� � �������
	call �� ����� �� ���������. ��� �� ��������:

	---------------------------------------------------------------

	initipc - ������������� IPC ��� ������ � CMD.

	call initipc - ���������������� IPC ��� ������ � CMD.

	��������! ����������� � ����� ������ ���������. CMD ����� �����
		  ������ 10/100 ������� ��� ����, ����� ��������
		  ��������� �� IPC (���� ����� ������ ����������).

	---------------------------------------------------------------

	print   - ������� ������ � ����� CMD.

	mov eax,strlen
	mov ebx,string
	call print

	��� strlen - ����� ������ � ������,
	string     - ��������� �� ������.

	call print - ����� ������.

	---------------------------------------------------------------

	cls     - �������� ����� CMD.

	call cls - ������� ������� ������.

	---------------------------------------------------------------

	eol     - ���������� ������.

	call eol - ������� ������ ������.

	---------------------------------------------------------------

	getkey  - ������� ��� ������� ������� � CMD.

	call getkey - ������� ������� ������� � ������� �� ���.

	�����: byte [key] - ��� ������� �������.

	��������: ����� ������� ��������� �� CMD, �������
		  ������������ IPC, �� ���� CMD ������������ �����
		  ����� ������� ���������. �������, ��� �� �����
		  ������ ������ �� ���� ��� ����, ����� ������
		  ���-������ � �������, ��� ��� ���� � ������� �������.

	---------------------------------------------------------------

	endipc  - ��������� ������ IPC-���������.

	call endipc - ��������� ���������.

	---------------------------------------------------------------

	�������, �� ������ ���������� ���� HELLO.ASM, ������� ����� �
	���� ������. ��� ��� ������ ���� �������. �����������, �����
	��� �� ������ ������������ ��������� � IPC-���������� ��� CMD.
	�������� ������ PARAM.ASM.

	������, � ����� CMD � � CMDIPC.INC ������������ 5-� �������
	(�����) ��� ����, �����	���� ����� ����-����� ���������
	��������� �� ��� ����� IPC ��������. � ���� � ��� ���� ������
	�����������, �� ��������, ����������� ������� �����
	������������. ��� �� ������ ��������, �������� ��������
	ebx ����� ������� ������� ���� (eax,5 - �����). � ���������
	������ ����� ���������� ������ IPC ��� ���-������ ������
	�������� (������ - ���������). ��, �����������, ������
	��������� � ���� ���. ;-)

-----------------------------------------------------------------------

  ��������� ������ � �����������:
	
	��� ������ � �������� CMD � �������� MenuetOS ������, � 
	�������,��� ��������� ��������� � Menuet ����� �� 
	�������������� MS-DOS. ��� � ������ ������� �������� ������, 
	� ������� ���� 1.1, b.bb � �.�. ������� �������� ��������� 
	��������� �������� ��� �������������� ��������� ��� 
	������������ ������ � MeOS � ����������� �� � MS-DOS � Windows
	��������. � �������� ��� �� ������ CMD, ��, ��������, � 
	��������. ������� � ���������� ������������ � Menuet ����� ���
	��� ���������� ������, ��� � ������ ����������� (����������
	��� 3 �����, �.�. filename.ext,	� �� filename.ex). � ��������
	�������� ������ LS, LS ���_�����, CP, DEL � RN �������, ��
	���� ��� ������������� ����� ����������	�����������, 
	����������� � ������� � Menuet. �.�. ���� ��������,
	��� �� �� ������� �������� � ������� � ������, ��������� � 
	��������, � ����� skandisk ������ ������ ������ ��� �������� 
	�������� ��������� �������, ����� ��������� �� ���� ����, � �� 
	�� ����� ������ �� windows ��� MS-DOS. � ��������, � ���������� 
	��������� ������ �� ���� PC, ������� �������� ��. :)

	P.S. ������� LS ������� ��������� ��� ����� �� ��������, � ���
	����� ��, ������� ������� ����������� � �� ����� �������� �
	MS-DOS, LS ���_����� � �.�. ����� ��� �� �� ��������. � 
	�������� ����� �� ������� ����� � � windows. �.�. �� �������
	��������� ����� ���� ������, ��������, � windows commander'e,
	�� ������� �� �� �������.
       
	��� ����, ����� ���� ����� CMD ���������� IPC-������, �
	CMDIPC.ASM ��� �������� ������������ 5-� ������� (�����),
	������� ������� ����������� ����� ��������� ����� IPC-��������.

	� ������ ������ �� �� ������ ��������� ������ ������
	��������� CMD ������������. ��� ������� � ���, ��� � ��� ��
	������� ����������� ������ ���������� �������� �
	IPC-����������� ������������.

	��� ��������� �� ������ ������ ���������������.
	
	etc. :)

-----------------------------------------------------------------------

	������ � ���������� ������������ ���� autoexec.cmd, ������� 
        ������������� ����������� ��� ������� ��������������. ��� 
        ����� ������� ��� ������������.

	��� ������ ������� ������������ ������� �������� /example

	������ � ���������� � ��������� ���������������� ����� 
	��������� �������� ������ � � ������, � ������� ������� 
        �������� �� dmitry_gt@tut.by

	� ��������� ����� �� ����������� ����� ������������ � �������, 
        �.�. �� � ��������� ������ ����� ������� � � ������, ��� ��� 
        � �� ������ ������ �����-����� ������������� (� ��������� 
        MenuetOS, �������). � ������, ��� ������������ ����� ��������-
        ���������� �� ��������� ����� � ����������� � ���������.

	���� ���-������ �������� �������� ���������, �� � �� ��������
	�� �������� � ������ (��� �� �������� ;-) ).

	� ���� ������� (� ��� ����� �������������� � ���������� ������)
 	������� �������� �� dmitry_gt@tut.by

	� ��� ����� �������, �� ��������� ��������������� "as is", �
	����� �� ����� ��������������� �� ��������� �����, �����������
	����������.

	19.06.2004,
	Chemist

