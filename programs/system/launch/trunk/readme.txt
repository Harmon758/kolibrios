==================== ���᪨� ====================
�����: 0.1.3
Launch - �ணࠬ�� ��� ����᪠ �ਫ������ �� ��४�਩ ���᪠.
�� ����᪥ �⠥� 䠩� launch.cfg � /sys/etc, ��⥬ � ��४�ਨ ����᪠.
��᫥ �⮣� ᬮ��� ��ࠬ���� ��������� ��ப�. �ਮ��� ��ࠬ��஢ - ��
���浪� ���뢠���.
�� ��ࠬ��஢ ��������� ��ப� ���� ॠ�������� ⮫쪮 ��� �ணࠬ�� � ��㬥���, ��।������ ��.
����ன��:
main.path - ���� � ��४��� ���᪠
debug.debug - ��樨 �⫠��� (no - ��� �⫠��� ��� console - �뢮� �१ ���᮫�)
debug.level - �஢��� �⫠��� (0 - ⮫쪮 ᮮ�饭�� 㤠筮/��㤠筮, 1 - �뢮���� ᮮ�饭�� ��� ������ ��४�ਨ)
��������������:
��� �뢮�� � ���᮫� �㦭� �����񭭠� ������⥪� console.obj!

==================== English ====================
Version: 0.1.3
Launch is a programme that launches applications from search dirictories.
On the start it reads file launch.cfg in /sys/etc and in current dirictory.
Than it reads command line arguments. Priority of arguments is as reading.
Now there are few command line arguments: the name of application and its arguments.
Configuration:
main.path - path to search dirictories
debug.debug - debug options (no or console)
debug.level - debug level (0 - show only ok/error messages, 1 - show for each directory)
ATTENTION:
to use console output you need new console.obj library!