
{ � FreePascal 2.2.0 ����஢�� cp866 �� ॠ��������. }
{-$codepage cp866}

{$mode objfpc}
{$smartlink on}
{$apptype console}

{ �� ����� ������ ��ᬠ�ਢ����� �믮������ �ਫ������ ⮫쪮 ��� ���᮫쭮�,
  �.�. ��४⨢� console ��易⥫쭠, ��������� �ணࠬ�� �� ������⢨� �⮩
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
  {��।������ ��ࠬ��஢ ����}
  kos_definewindow(200, 200, 200, 50, $23AABBCC);
  {kos_definewindow �� ����� ��ࠬ��� ��� �뢮�� ���������,
   ������ �� �⤥�쭮� �㭪樥� kos_setcaption}
  {�⮡ࠦ���� ��������� ����}
  kos_setcaption('������ ���������');
  {�뢮� ᮮ�饭��}
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
  kos_speaker(@Notes);
end;


function DoButton: Boolean;
{ ��ࠡ�⪠ ᮡ��� ������ ������ GUI }
var
 Button: DWord;
begin
  {������� ��� ����⮩ ������}
  Button := kos_getbutton();
  {�᫨ [x], � ������ ����, � ����� �஢��஢��� �����⨥ �ਫ������}
  Result := Button <> 1;
end;


function ProcessMessage: Boolean;
{ �������� � ��ࠡ�⪠ ᮡ�⨩.

  @return: �����頥� False, �᫨ �뫮 ᮡ�⨥ � �����襭�� �ਫ������.
  @rtype: True ��� False }
var
  Event: DWord;
begin
  Result := True;
  {������� ᮡ��� �� ��⥬�}
  Event := kos_getevent();
  case Event of
    SE_PAINT   : DoPaint;  {����ᮢ�� ����}
    SE_KEYBOARD: DoKey;    {ᮡ�⨥ �� ����������}
    SE_BUTTON  : Result := DoButton; {ᮡ먥 �� ������, ����� ��।�����
                                      �����襭�� �ਫ������, �᫨ ��୥� False}
  end;
end;


procedure MainLoop;
{ ������ 横� �ਫ������ }
var
  ThreadSlot: TThreadSlot;
begin
  {����ࠨ���� ᮡ���, ����� �� ��⮢� ��ࠡ��뢠��}
  kos_maskevents(ME_PAINT or ME_KEYBOARD or ME_BUTTON);
  {��ࠡ�⪠ ᮡ�⨩}
  while ProcessMessage do;
end;


begin
  WriteLn('Look for a new window, I''m just a konsole ;-)');
  MainLoop;
end.
