==================== ���᪨� ====================
�����: 0.1.80.1 (0.2 beta)
Launch - �ணࠬ�� ��� ����᪠ �ਫ������ �� ��४�਩ ���᪠.
�� ����᪥ �⠥� 䠩� launch.cfg � /sys/etc, ��⥬ � ��४�ਨ ����᪠.
��᫥ �⮣� ᬮ��� ��ࠬ���� ��������� ��ப�. �ਮ��� ��ࠬ��஢ - ��
���浪� ���뢠���.
�� ��ࠬ��஢ ��������� ��ப� ���� ॠ�������� ⮫쪮 ��� �ணࠬ�� � ��㬥���, ��।������ ��.
�� �����񭭮� �ᯮ�짮����� Kobra �� ������ᮢ���� (�室�騥 � ��㯯� launch_reactive) �ਫ������ ����������
� ��砥 �ᯥ譮�� ����᪠ (���뫠���� ᮮ�饭�� dword 1 dword tid, tid - �����䨪��� ����饭���� �����).
����ன��:
main.path - ���� � ��४��� ���᪠
debug.debug - ��樨 �⫠��� (no - ��� �⫠��� ��� console - �뢮� �१ ���᮫�)
debug.level - �஢��� �⫠��� (0 - ⮫쪮 ᮮ�饭�� 㤠筮/��㤠筮, 1 - �뢮���� ᮮ�饭�� ��� ������ ��४�ਨ)
kobra.use - �ᯮ�짮����� Kobra

��������������:
��� ࠡ��� �㦭� libconfig.

==================== English ====================
Version: 0.1.80.1 (0.2 beta)
Launch is a programme that launches applications from search dirictories.
On the start it reads file launch.cfg in /sys/etc and in current dirictory.
Than it reads command line arguments. Priority of arguments is as reading.
Now there are only few command line arguments: the name of application and its arguments.
If using Kobra is enabled all intrested (members of launch_reactive group) applications are notified if
application is launched (sending message dword 1 dword tid, tid - identifier of launched process).
Configuration:
main.path - path to search dirictories
debug.debug - debug options (no or console)
debug.level - debug level (0 - show only ok/error messages, 1 - show for each directory)
kobra.use - using of Kobra
ATTENTION:
you need libconfig to use launch.