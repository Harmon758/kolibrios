{$codepage cp866}
{$mode objfpc}
{$smartlink on}
{$apptype console}

{ �� ����� ������ ��ᬠ�ਢ����� �믮������ �ਫ������ ⮫쪮 ��� ���᮫쭮�,
  �.�. ��४⨢� concole ��易⥫쭠, ��������� �ணࠬ�� �� ������⢨� �⮩
  ��४⨢� �।��।����� �����. ��࠭�஢���� ����� �ᯮ�짮���� �㭪樨
  Write, WriteLn, Read, ReadLn �⭮�⥫쭮 �⠭���⭮� ���᮫� �����/�뢮��.
}

program Example;

{ �� �㭪樨 ����騥 � ᢮�� ����� ��䨪� 'kos_' ����� �����ମ����ᨬ묨
  � ॠ�������� ⮫쪮 ��� KolibriOS. �� �ᯮ�짮����� � ���� �ணࠬ����
  �ਫ������� ��⥣���᪨ �� ४����������, �뭮�� �� ��⮤�, �ᯮ����騥
  �� �㭪樨, � �⤥��� ���㫨 (� �ᯮ���� ����室��� ����ࠪ樨).
}

procedure DoPaint;
{ �뢮� ᮤ�ন���� ���� �ਫ������ }
begin
  kos_begindraw();
  {��।������ ��ࠬ��஢ ���� (0)}
  kos_definewindow(200, 200, 200, 50, $23AABBCC);
  {kos_definewindow �� ����� ��ࠬ��� ��� �뢮�� ���������,
   ������ �� �⤥�쭮� �㭪樥�}
  {kos_setcaption, �⮡ࠦ���� ��������� ���� (71.1)}
  kos_setcaption('������ ���������');
  {�뢮� ᮮ�饭�� (4)}
  kos_drawtext(3, 8, '������ ���� �������...');
  kos_enddraw();
end;

procedure DoKey;
{ ��ࠡ�⪠ ᮡ��� ������ ������ }
var
  Key: DWord;
  Notes: array[0..3] of Byte;
begin
  Key := kos_getkey();
  {����ࠨ���� ���� ��� ���}
  Notes[0] := $90;
  Notes[1] := Key shr 8;
  Notes[2] := $00;
  {���ந������}
  kos_speak(@Notes);
end;


function DoButton: Boolean;
{ ��ࠡ�⪠ ᮡ��� ������ ������ GUI }
var
 Button: DWord;
begin
  {������� ��� ����⮩ �����}
  Button := kos_getbutton();
  {�᫨ X, � �����襭�� �ਫ������}
  Result := Button = 1;
end;


function ProcessMessage: Boolean;
{ @return: �����頥� False, �᫨ �뫮 ᮡ�⨥ � �����襭�� �ਫ������.
  @rtype: True ��� False }
var
  Event: DWord;
begin
  Result := False;
  {������� ᮡ��� �� ��⥬�}
  Event := kos_getevent();
  case Event of
    SE_PAINT   : DoPaint;  {����ᮢ�� ����}
    SE_KEYBOARD: DoKey;    {ᮡ�⨥ �� ����������}
    SE_BUTTON  : Result := DoButton; {ᮡ먥 �� ������, ����� ��।�����
                                      �����襭�� �ਫ������, �᫨ ��୥� True}
  end;
end;


procedure MainLoop;
{ ������ 横� �ਫ������ }
var
  ThreadSlot: TThreadSlot;
begin
  {ᤥ���� �� ���� ��⨢��}
  ThreadSlot := kos_getthreadslot(ThreadID);
  kos_setactivewindow(ThreadSlot);
  {����ࠨ���� ᮡ���, ����� �� ��⮢� ��ࠡ��뢠��}
  kos_maskevents(ME_PAINT or ME_KEYBOARD or ME_BUTTON);
  {������ 横�}
  while not ProcessMessage do;
end;


begin
  WriteLn('Look for a new window, I''m just a konsole, hi mike ;-)');
  MainLoop;
end.
