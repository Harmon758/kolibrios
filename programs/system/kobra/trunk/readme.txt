==================== ���᪨� ====================
�����: 0.1.1
Kobra (Kolibri Bus for Reaching Applications) - ����� (�ࢥ�), ��������騩 �ਫ������ ��� ������� ��� � ��㣮�.
��� ࠡ��� � Kobra ����室��� ��ॣ����஢����� �� �ࢥ� (�१ IPC). ��᫥ ॣ����樨 �ਫ������ ����㯭� ᫥���騥 �㭪樨:
-���㯨�� � ��㯯� ��⮪�� (�᫨ ��� - ᮧ������)
-��� �� ��㯯�
-��᫠�� ᮮ�饭�� �ᥬ ��⮪�� �����-���� ��㯯�
-������� ��� ����������� ������ � ᯨ᪮� ��㯯 � ��⮪��, �室��� � ��� (� ᬥ饭�� ᯨ᪠ ��㯯) (���� �� ॠ��������)
����� ᪮॥ �ᥣ� ������� ��� ��᪮�쪮 �㭪権.

�ਬ�஢ ࠡ��� ���� ���, ������ � ������襥 �६� �ᯮ�짮����� Kobra �㤥� ॠ�������� � Launch � � ����� �࠭�⥫� �࠭�.

==================== English ====================
Version: 0.1.1
Kobra (Kolibri Bus for Reaching Applications) is a daemon (server), which allows applications to easier communicate with each other.
For working with Kobra application should register on server (via IPC). After registration following functions are available for application:
-Join thread group (if there is no such group, it will be created)
-Leave group
-Send message for all threads in some group
-Get name of named memory area with list of groups and threads in them (and offset of group list) (not realised yet)
Later there may be few more functions.

There is no examples of work but using of Kobra will be realised in Launch and new screen saver.