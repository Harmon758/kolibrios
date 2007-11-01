@echo off

rem ��� ᡮન ���� ����室��� � ��६����� UNITS (��।����� ����)
rem 㪠���� �ᯮ������� �����, � ���ன ��室���� �⪮�����஢���� ���㫨
rem RTL ��� KolibriOS. ���ਬ��, �᫨ ��室���� RTL ��室���� � ����� my/rtl,
rem � ᮡ࠭�� ���㫨 RTL - ᪮॥ �ᥣ� � my/units. ����� ���������
rem ������� ���� ��७��� ��� ����� (lrl) � ��४��� my.

rem ��� ��, ��� ᡮન, ��� ����������� �⨫�� exe2kos.exe � FreePascal 2.2.0.


set NAME=lrl
set NAMEEXE=%NAME%.exe
set NAMEKEX=%NAME%.kex

set BUILD=-FUbuild
set UNITS=-Fu../units

fpc %NAME%.pp -n -Twin32 -Se5 -XXs -Sg -O3pPENTIUM3 -CfSSE -WB0 %BUILD% %UNITS%
if errorlevel 1 goto error

exe2kos.exe %NAMEEXE% %NAMEKEX%
del %NAMEEXE%
move %NAMEKEX% bin
goto end

:error
echo An error occured while building %NAME%

:end
