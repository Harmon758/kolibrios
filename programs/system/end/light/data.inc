empty_label: db '                                             ',0 
TEXT_HOME: db 'Home',0
TEXT_ENTER: db 'Enter',0
TEXT_END: db 'End',0
TEXT_CANCEL: db 'x',0

strlen_var dd 0

if lang eq ru
	TEXT_TITLE: db '�����襭�� ࠡ���',0
	TEXT_RDSAVE1: db '���࠭��� ���������, ᤥ����� �',0
	TEXT_RDSAVE2: db '����� ࠡ��� � ��⥬� (Ctrl+S)',0
	TEXT_KERNEL: db '���',0
	TEXT_REBOOT: db '��१���㧪�',0
	TEXT_OFF: db '�몫�祭��',0
else if lang eq ge
	TEXT_TITLE: db 'Shutdown computer',0
	TEXT_RDSAVE1: db 'Save all changes that were done',0
	TEXT_RDSAVE2: db 'during system work (Ctrl+S)',0
	TEXT_KERNEL: db 'Kernel',0
	TEXT_REBOOT: db 'Neustart',0
	TEXT_OFF: db 'Beenden',0
else if lang eq it
	TEXT_TITLE: db 'Shutdown computer',0
	TEXT_RDSAVE1: db 'Save all changes that were done',0
	TEXT_RDSAVE2: db 'during system work (Ctrl+S)',0
	TEXT_KERNEL: db 'Kernel',0
	TEXT_REBOOT: db 'Riavvio',0
	TEXT_OFF: db 'Spegni',0
else
	TEXT_TITLE: db 'Shutdown computer',0
	TEXT_RDSAVE1: db 'Save all changes that were done',0
	TEXT_RDSAVE2: db 'during system work (Ctrl+S)',0
	TEXT_KERNEL: db 'Kernel',0
	TEXT_REBOOT: db 'Reboot',0
	TEXT_OFF: db 'Power off',0
end if

rdsave:
        dd  SSF_START_APP,0,hide,0,0
        db  '/sys/rdsave',0
;---------------------------------------------------------------------
;Lib_DATA
system_dir_Boxlib       db '/sys/lib/box_lib.obj',0
system_dir_libini       db '/sys/lib/libini.obj',0
;---------------------------------------------------------------------
l_libs_start:

library01  l_libs system_dir_Boxlib+9, library_path, system_dir_Boxlib, Box_lib_import
library02  l_libs system_dir_libini+9, library_path, system_dir_libini, libini_import

end_l_libs:
;---------------------------------------------------------------------
align 4
Box_lib_import:
init_checkbox    dd      aInit_checkbox
check_box_draw   dd      aCheck_box_draw
check_box_mouse  dd      aCheck_box_mouse
;version_ch2       dd      aVersion_ch2
                  dd 0,0
aInit_checkbox   db 'init_checkbox2',0
aCheck_box_draw  db 'check_box_draw2',0
aCheck_box_mouse db 'check_box_mouse2',0
;aVersion_ch2      db 'version_ch2',0
;---------------------------------------------------------------------
align 4
libini_import:
init_lib     dd a_init
ini_get_int  dd aini_get_int
ini_set_int  dd aini_set_int
             dd 0,0
a_init       db 'lib_init',0
aini_get_int db 'ini_get_int',0
aini_set_int db 'ini_set_int',0
;---------------------------------------------------------------------

check1 check_box2 32 shl 16 +15, 70 shl 16 +15,6,0xffffff,0x757575,0x00CC00, empty_label, ch_flag_middle

hide        db 'h',0
asettings   db 'settings',0
aautosave   db 'autosave',0
ini_file db  '/sys/settings/rdsave.ini',0


If fade=1

color21 dd 0x01505890
color22 dd 0xbfbfbf
color23 dd 0xa49fa1

color24 dd 0x590011
color25 dd 0x593700
color26 dd 0x373737
color27 dd 0x003700

color28 dd 0x90bfbfbf

else

if lang eq ru
  label7:
      db   '���࠭���� ��ࠧ�...',0
else
  label7:
      db   'Saving RAM-drive...',0
end if

end if
