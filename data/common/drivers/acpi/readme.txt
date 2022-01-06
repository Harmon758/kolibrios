================================ ENG ================================

Current driver installation is semi-manual. 
To turn on APIC you have to:

1) Run Installer (install.kex)
2) Wait 3 seconds and get sure that there is a message about succesfull
   file generation /sys/drivers/devices.dat
   Note: log can be found in /tmp0/1/acpi.log
3) Make kernel restart (MENU -> END -> HOME key)
4) Check that kernel and drivers are working well.
5) Save kolibri.img. Now each time you boot APIC would be turned on automatically.

================================ RUS ================================

��������� �������� �������� � �������� ������ ������, � ��� ������.
����� �������� APIC ����:

1) ��������� ���������� (install.kex)
2) ��������� 3 ������� � ���������, ��� ���������� ���������
   �� �������� ��������� /sys/drivers/devices.dat
   ��� �������� ��������� � /tmp0/1/acpi.log
3) ������� ������� ���� (���� -> ���������� ������ -> ����)
4) ��������� ������ ���� � ���������
5) ��������� �����. ������ APIC ����� ���������� ��� ������ �������� ����.

������ ����������.

���� � �������� �������������, ��� �� ������������ � ����� ���������� ���������, ������ ������� ACPI � ���������� ������� ������ IOAPIC � Local APIC. �� ������ ����� ������� APIC_init ��������� ������� ������ � ��������� ���� devices.dat. ���� ���� �������� �������, ���� ����������� IOAPIC � Local APIC, ����������� ��������� ���������� � ����� APIC � ������ ������ ����� IRQ � ���������������� ������������ PCI ���������� �� devices.dat.

��������� https://board.kolibrios.org/viewtopic.php?f=1&t=1195&hilit=devices.dat&start=105#p37822