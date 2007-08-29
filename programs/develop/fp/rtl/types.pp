{$mode objfpc}

unit Types;

interface

type
  PLongint = System.PLongint;
  PSmallInt = System.PSmallInt;
  PDouble = System.PDouble;
  PByte = System.PByte;

  TIntegerDynArray = array of Integer;
  TCardinalDynArray = array of Cardinal;
  TWordDynArray = array of Word;
  TSmallIntDynArray = array of SmallInt;
  TByteDynArray = array of Byte;
  TShortIntDynArray = array of ShortInt;
  TInt64DynArray = array of Int64;
  TQWordDynArray = array of QWord;
  TLongWordDynArray = array of LongWord;
  TSingleDynArray = array of Single;
  TDoubleDynArray = array of Double;
  TBooleanDynArray = array of Boolean;
  TStringDynArray = array of AnsiString;
  TWideStringDynArray   = array of WideString;
  TPointerDynArray = array of Pointer;

  TPoint =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
    X : Longint;
    Y : Longint;
  end;
  PPoint = ^TPoint;
  tagPOINT = TPoint;

  TRect =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
    case Integer of
      0: (Left,Top,Right,Bottom : Longint);
      1: (TopLeft,BottomRight : TPoint);
    end;
  PRect = ^TRect;

  TSize =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
     cx : Longint;
     cy : Longint;
  end;
  PSize = ^TSize;
  tagSIZE = TSize;
//  SIZE = TSize;


  TSmallPoint =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
     x : SmallInt;
     y : SmallInt;
  end;
  PSmallPoint = ^TSmallPoint;

  TDuplicates = (dupIgnore, dupAccept, dupError);

type
  TOleChar = WideChar;
  POleStr = PWideChar;
  PPOleStr = ^POleStr;

  TListCallback = procedure(data,arg:pointer) of object;
  TListStaticCallback = procedure(data,arg:pointer);

const
  GUID_NULL: TGUID  = '{00000000-0000-0000-0000-000000000000}';

  STGTY_STORAGE   = 1;
  STGTY_STREAM    = 2;
  STGTY_LOCKBYTES = 3;
  STGTY_PROPERTY  = 4;

  STREAM_SEEK_SET = 0;
  STREAM_SEEK_CUR = 1;
  STREAM_SEEK_END = 2;

  LOCK_WRITE     = 1;
  LOCK_EXCLUSIVE = 2;
  LOCK_ONLYONCE  = 4;

  E_FAIL = HRESULT($80004005);

  STG_E_INVALIDFUNCTION       = HRESULT($80030001);
  STG_E_FILENOTFOUND          = HRESULT($80030002);
  STG_E_PATHNOTFOUND          = HRESULT($80030003);
  STG_E_TOOMANYOPENFILES      = HRESULT($80030004);
  STG_E_ACCESSDENIED          = HRESULT($80030005);
  STG_E_INVALIDHANDLE         = HRESULT($80030006);
  STG_E_INSUFFICIENTMEMORY    = HRESULT($80030008);
  STG_E_INVALIDPOINTER        = HRESULT($80030009);
  STG_E_NOMOREFILES           = HRESULT($80030012);
  STG_E_DISKISWRITEPROTECTED  = HRESULT($80030013);
  STG_E_SEEKERROR             = HRESULT($80030019);
  STG_E_WRITEFAULT            = HRESULT($8003001D);
  STG_E_READFAULT             = HRESULT($8003001E);
  STG_E_SHAREVIOLATION        = HRESULT($80030020);
  STG_E_LOCKVIOLATION         = HRESULT($80030021);
  STG_E_FILEALREADYEXISTS     = HRESULT($80030050);
  STG_E_INVALIDPARAMETER      = HRESULT($80030057);
  STG_E_MEDIUMFULL            = HRESULT($80030070);
  STG_E_PROPSETMISMATCHED     = HRESULT($800300F0);
  STG_E_ABNORMALAPIEXIT       = HRESULT($800300FA);
  STG_E_INVALIDHEADER         = HRESULT($800300FB);
  STG_E_INVALIDNAME           = HRESULT($800300FC);
  STG_E_UNKNOWN               = HRESULT($800300FD);
  STG_E_UNIMPLEMENTEDFUNCTION = HRESULT($800300FE);
  STG_E_INVALIDFLAG           = HRESULT($800300FF);
  STG_E_INUSE                 = HRESULT($80030100);
  STG_E_NOTCURRENT            = HRESULT($80030101);
  STG_E_REVERTED              = HRESULT($80030102);
  STG_E_CANTSAVE              = HRESULT($80030103);
  STG_E_OLDFORMAT             = HRESULT($80030104);
  STG_E_OLDDLL                = HRESULT($80030105);
  STG_E_SHAREREQUIRED         = HRESULT($80030106);
  STG_E_EXTANTMARSHALLINGS    = HRESULT($80030108);
  STG_E_DOCFILECORRUPT        = HRESULT($80030109);
  STG_E_BADBASEADDRESS        = HRESULT($80030110);
  STG_E_INCOMPLETE            = HRESULT($80030201);
  STG_E_TERMINATED            = HRESULT($80030202);

  STG_S_CONVERTED             = $00030200;
  STG_S_BLOCK                 = $00030201;
  STG_S_RETRYNOW              = $00030202;
  STG_S_MONITORING            = $00030203;

type
  PCLSID = PGUID;
  TCLSID = TGUID;

  LARGE_INT = Int64;
  Largeint = LARGE_INT;
  PDWord = ^DWord;

  PDisplay = Pointer;
  PEvent = Pointer;

  TXrmOptionDescRec = record
  end;
  XrmOptionDescRec = TXrmOptionDescRec;
  PXrmOptionDescRec = ^TXrmOptionDescRec;

  Widget = Pointer;
  WidgetClass = Pointer;
  ArgList = Pointer;
  Region = Pointer;

  _FILETIME =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
     dwLowDateTime : DWORD;
     dwHighDateTime : DWORD;
  end;
  TFileTime = _FILETIME;
  FILETIME = _FILETIME;
  PFileTime = ^TFileTime;

  tagSTATSTG =
{$ifndef FPC_REQUIRES_PROPER_ALIGNMENT}
  packed
{$endif FPC_REQUIRES_PROPER_ALIGNMENT}
  record
     pwcsName : POleStr;
     dwType : Longint;
     cbSize : Largeint;
     mtime : TFileTime;
     ctime : TFileTime;
     atime : TFileTime;
     grfMode : Longint;
     grfLocksSupported : Longint;
     clsid : TCLSID;
     grfStateBits : Longint;
     reserved : Longint;
  end;
  TStatStg = tagSTATSTG;
  STATSTG = TStatStg;
  PStatStg = ^TStatStg;

  IClassFactory = Interface(IUnknown) ['{00000001-0000-0000-C000-000000000046}']
     Function CreateInstance(Const unkOuter : IUnknown;Const riid : TGUID;Out vObject) : HResult;StdCall;
     Function LockServer(fLock : LongBool) : HResult;StdCall;
  End;

  ISequentialStream = interface(IUnknown) ['{0c733a30-2a1c-11ce-ade5-00aa0044773d}']
     function Read(pv : Pointer;cb : DWord;pcbRead : PDWord) : HRESULT;stdcall;
     function Write(pv : Pointer;cb : DWord;pcbWritten : PDWord) : HRESULT;stdcall;
  end;

  IStream = interface(ISequentialStream) ['{0000000C-0000-0000-C000-000000000046}']
     function Seek(dlibMove : LargeInt; dwOrigin : Longint;
       out libNewPosition : LargeInt) : HResult;stdcall;
     function SetSize(libNewSize : LargeInt) : HRESULT;stdcall;
     function CopyTo(stm: IStream;cb : LargeInt;out cbRead : LargeInt;
       out cbWritten : LargeInt) : HRESULT;stdcall;
     function Commit(grfCommitFlags : Longint) : HRESULT;stdcall;
     function Revert : HRESULT;stdcall;
     function LockRegion(libOffset : LargeInt;cb : LargeInt;
       dwLockType : Longint) : HRESULT;stdcall;
     function UnlockRegion(libOffset : LargeInt;cb : LargeInt;
       dwLockType : Longint) : HRESULT;stdcall;
     Function Stat(out statstg : TStatStg;grfStatFlag : Longint) : HRESULT;stdcall;
     function Clone(out stm : IStream) : HRESULT;stdcall;
  end;

function EqualRect(const r1,r2 : TRect) : Boolean;
function Rect(Left,Top,Right,Bottom : Integer) : TRect;
function Bounds(ALeft,ATop,AWidth,AHeight : Integer) : TRect;
function Point(x,y : Integer) : TPoint;
function PtInRect(const Rect : TRect; const p : TPoint) : Boolean;
function IntersectRect(var Rect : TRect; const R1,R2 : TRect) : Boolean;
function UnionRect(var Rect : TRect; const R1,R2 : TRect) : Boolean;
function IsRectEmpty(const Rect : TRect) : Boolean;
function OffsetRect(var Rect : TRect;DX : Integer;DY : Integer) : Boolean;
function CenterPoint(const Rect: TRect): TPoint;
function InflateRect(var Rect: TRect; dx: Integer; dy: Integer): Boolean;
function Size(AWidth, AHeight: Integer): TSize;
function Size(ARect: TRect): TSize;

implementation


function EqualRect(const r1,r2 : TRect) : Boolean;

begin
  EqualRect:=(r1.left=r2.left) and (r1.right=r2.right) and (r1.top=r2.top) and (r1.bottom=r2.bottom);
end;


function Rect(Left,Top,Right,Bottom : Integer) : TRect;

begin
  Rect.Left:=Left;
  Rect.Top:=Top;
  Rect.Right:=Right;
  Rect.Bottom:=Bottom;
end;


function Bounds(ALeft,ATop,AWidth,AHeight : Integer) : TRect;

begin
  Bounds.Left:=ALeft;
  Bounds.Top:=ATop;
  Bounds.Right:=ALeft+AWidth;
  Bounds.Bottom:=ATop+AHeight;
end;


function Point(x,y : Integer) : TPoint;

begin
  Point.x:=x;
  Point.y:=y;
end;

function PtInRect(const Rect : TRect;const p : TPoint) : Boolean;

begin
  PtInRect:=(p.y>=Rect.Top) and
            (p.y<Rect.Bottom) and
            (p.x>=Rect.Left) and
            (p.x<Rect.Right);
end;


function IntersectRect(var Rect : TRect;const R1,R2 : TRect) : Boolean;

begin
  Rect:=R1;
  with R2 do
    begin
    if Left>R1.Left then
      Rect.Left:=Left;
    if Top>R1.Top then
      Rect.Top:=Top;
    if Right<R1.Right then
      Rect.Right:=Right;
    if Bottom<R1.Bottom then
      Rect.Bottom:=Bottom;
    end;
  if IsRectEmpty(Rect) then
    begin
    FillChar(Rect,SizeOf(Rect),0);
    IntersectRect:=false;
    end
  else
    IntersectRect:=true;
end;

function UnionRect(var Rect : TRect;const R1,R2 : TRect) : Boolean;
begin
  Rect:=R1;
  with R2 do
    begin
    if Left<R1.Left then
      Rect.Left:=Left;
    if Top<R1.Top then
      Rect.Top:=Top;
    if Right>R1.Right then
      Rect.Right:=Right;
    if Bottom>R1.Bottom then
      Rect.Bottom:=Bottom;
    end;
  if IsRectEmpty(Rect) then
    begin
    FillChar(Rect,SizeOf(Rect),0);
    UnionRect:=false;
    end
  else
    UnionRect:=true;
end;

function IsRectEmpty(const Rect : TRect) : Boolean;
begin
  IsRectEmpty:=(Rect.Right<=Rect.Left) or (Rect.Bottom<=Rect.Top);
end;

function OffsetRect(var Rect : TRect;DX : Integer;DY : Integer) : Boolean;
begin
  if assigned(@Rect) then
    begin
    with Rect do
      begin
      inc(Left,dx);
      inc(Top,dy);
      inc(Right,dx);
      inc(Bottom,dy);
      end;
    OffsetRect:=true;
    end
  else
    OffsetRect:=false;
end;

function CenterPoint(const Rect: TRect): TPoint;

begin
  With Rect do
    begin
    Result.X:=(Left+Right) div 2;
    Result.Y:=(Top+Bottom) div 2;
    end;
end;

function InflateRect(var Rect: TRect; dx: Integer; dy: Integer): Boolean;
begin
  if Assigned(@Rect) then
  begin
    with Rect do
    begin
      dec(Left, dx);
      dec(Top, dy);
      inc(Right, dx);
      inc(Bottom, dy);
    end;
    Result := True;
  end
  else
    Result := False;
end;

function Size(AWidth, AHeight: Integer): TSize;
begin
  Result.cx := AWidth;
  Result.cy := AHeight;
end;

function Size(ARect: TRect): TSize;
begin
  Result.cx := ARect.Right - ARect.Left;
  Result.cy := ARect.Bottom - ARect.Top;
end;



end.
