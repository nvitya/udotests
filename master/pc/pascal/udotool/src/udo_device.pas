unit udo_device;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, jsontools;

const
  CDTYPE_SIZE_MASK = $0F;
  CDTYPE_TYPE_MASK = $F0;
  CDTYPE_INT       = $00;
  CDTYPE_UINT      = $10;
  CDTYPE_FLOAT     = $20;
  CDTYPE_STRING    = $40;
  CDTYPE_BLOB      = $60;

type

  { TUdoParam }

  TUdoParam = class
  private
    procedure SetIndex(aindex : uint16);
    function  GetIndex : uint16;
    procedure SetOffset(aoffset : uint16);
    function  GetOffset : uint16;

  public
    objid : uint32;  // 32 bit: IIIIoooo, IIII = index, oooo = offset
    name  : string[16];
    dtype : byte;
    vunit : string[16];
    descr : string;
    scale : double;

    property index  : uint16 read GetIndex write SetIndex;
    property offset : uint16 read GetOffset write SetOffset;

    constructor Create(aindex : uint16; aoffset : uint16);

    function ByteSize : uint32;
    function ScopeDef : uint32;

    function FormatValue(const buf; buflen : uint32) : string;
    class function FormatValueGeneric(const buf; buflen : uint32) : string;

    function ConvertToDouble(aptr : pointer) : double;
  end;

  { TUdoParamList }

  TUdoDevice = class;

  TUdoParamList = class
  protected
    fdevice : TUdoDevice;
    flist : array of TUdoParam;

    function GetParamByIndex(aindex : integer) : TUdoParam;
  public

    constructor Create(adevice : TUdoDevice);
    procedure Clear;

    procedure Save;

    function Count : integer;

    function Find(index : uint16; offset : uint16) : TUdoParam; overload;
    function Find(objid : uint32) : TUdoParam; overload;
    function Find(aname : string) : TUdoParam; overload;

    function Add(index : uint16; offset : uint16) : TUdoParam;
    procedure Remove(apar : TUdoParam);

    property Items[aindex : integer] : TUdoParam read GetParamByIndex; default;

  end;

  { TUdoDevice }

  TUdoDevice = class
  public
    id       : string[16];
    params   : TUdoParamList;
    jsonfile : string;
    jroot    : TJsonNode;

    constructor Create(aid : string; ajson : string);
    destructor Destroy; override;

    procedure Load;
    procedure Save;

  end;

  { TUdoDevList }

  TUdoDevList = class
  private
    function GetActive : TUdoDevice;
    function GetItemByIndex(aindex : integer) : TUdoDevice;
    procedure SetActive(AValue : TUdoDevice);
    procedure SetActiveIndex(AValue : integer);
  protected
    flist : array of TUdoDevice;
    factiveindex : integer;
    jsonfile : string;
    jroot    : TJsonNode;
  public
    function Find(aname : string) : TUdoDevice; overload;
    function GetIndex(aitem : TUdoDevice) : integer;
    function Count : integer;
    property Active : TUdoDevice read GetActive write SetActive;
    property ActiveIndex : integer read factiveindex write SetActiveIndex;

    property Items[aindex : integer] : TUdoDevice read GetItemByIndex; default;

    function Add(aid : string; ajson : string) : TUdoDevice;
    procedure Remove(aitem : TUdoDevice);

    constructor Create;
    procedure Clear;
    procedure Load;
    procedure Save;
  end;

var
  candevlist : TUdoDevList;

function CDtypeToString(adtype : byte) : string;

implementation

function CDtypeToString(adtype : byte) : string;
var
  dt : byte;
  bitsize : byte;
  s : string;
begin
  dt := (adtype and CDTYPE_TYPE_MASK);
  bitsize := 8 * (adtype and CDTYPE_SIZE_MASK + 1);
  s := IntToStr(bitsize);
  if      dt = CDTYPE_STRING then result := 'str'
  else if dt = CDTYPE_BLOB   then result := 'blo'
  else if dt = CDTYPE_FLOAT  then result := 'f'+s
  else if dt = CDTYPE_UINT   then result := 'u'+s
  else if dt = CDTYPE_INT    then result := 'i'+s
  else result := '?';
end;

{ TUdoDevList }

function TUdoDevList.GetItemByIndex(aindex : integer) : TUdoDevice;
begin
  if (aindex >= 0) and (aindex < length(flist))
  then
    result := flist[aindex]
  else
    result := nil;
end;

function TUdoDevList.GetIndex(aitem : TUdoDevice) : integer;
begin
  result := 0;
  while result < length(flist) do
  begin
    if flist[result] = aitem then Exit;
    Inc(result);
  end;

  result := -1;
end;

procedure TUdoDevList.SetActive(AValue : TUdoDevice);
begin
  factiveindex := GetIndex(avalue);
end;

procedure TUdoDevList.SetActiveIndex(AValue : integer);
begin
  if factiveindex = AValue then Exit;
  factiveindex := AValue;
end;

function TUdoDevList.GetActive : TUdoDevice;
begin
  if (factiveindex >= 0) and (factiveindex < length(flist))
  then
    result := flist[factiveindex]
  else
    result := nil;
end;


function TUdoDevList.Find(aname : string) : TUdoDevice;
begin
  for result in flist do
  begin
    if result.id = aname then Exit;
  end;
  result := nil;
end;

function TUdoDevList.Count : integer;
begin
  result := length(flist);
end;

function TUdoDevList.Add(aid : string; ajson : string) : TUdoDevice;
var
  i : integer;
begin
  result := Find(aid);
  if result = nil then
  begin
    result := TUdoDevice.Create(aid, ajson);
    i := 0;
    while i < length(flist) do
    begin
      if flist[i].id > result.id then break;
      Inc(i);
    end;
    // find the insert position
    insert(result, flist, i);
  end;
end;

procedure TUdoDevList.Remove(aitem : TUdoDevice);
var
  i : integer;
begin
  for i := 0 to length(flist) - 1 do
  begin
    if flist[i] = aitem then
    begin
      aitem.Free;
      delete(flist, i, 1);
    end;
  end;
end;


constructor TUdoDevList.Create;
begin
  flist := [];
  factiveindex := 0;
  jroot := TJsonNode.Create;
  jsonfile := 'devices.json';
end;

procedure TUdoDevList.Clear;
var
  item : TUdoDevice;
begin
  for item in flist do  item.Free;
  flist := [];
end;

procedure TUdoDevList.Load;
var
  jarr, jn, jv : TJsonNode;
  dev : TUdoDevice;
  i : integer;

  id, jf : string;
begin
  Clear;

  if FileExists(jsonfile)
  then
    jroot.LoadFromFile(jsonfile);

  jarr := jroot.Find('DEVICES');
  if jarr = nil then Exit;

  for i := 0 to jarr.Count-1 do
  begin
    jn := jarr.Child(i);

    jv := jn.Find('ID');
    if jv <> nil then
    begin
      id := jv.AsString;
      jf := jn.Find('JSON').AsString;

      dev := Add(id, jf);
      dev.Load;
    end;
  end;
end;

procedure TUdoDevList.Save;
var
  jnarr, jn : TJsonNode;
  dev : TUdoDevice;
begin
  jroot.Clear;
  jnarr := jroot.Add('DEVICES', nkArray);
  for dev in flist do
  begin
    jn := jnarr.Add();
    jn.Add('ID', dev.id);
    jn.Add('JSON', dev.jsonfile);
  end;

  jroot.SaveToFile(jsonfile);
end;

{ TUdoDevice }

constructor TUdoDevice.Create(aid : string; ajson : string);
begin
  id := aid;
  if ajson = ''
  then
    jsonfile := 'devices/'+id+'.json'
  else
    jsonfile := ajson;

  jroot := TJsonNode.Create;
  params := TUdoParamList.Create(self);
end;

destructor TUdoDevice.Destroy;
begin
  params.Free;
  jroot.Free;
  inherited Destroy;
end;

procedure TUdoDevice.Load;
var
  pn : integer;
  jarr, jn, jv : TJsonNode;
  par : TUdoParam;

  idx, offs : integer;
  name : string;
begin
  params.Clear;
  try
    jroot.LoadFromFile(jsonfile);
  except
    // ignore load error
    Exit;
  end;

  jarr := jroot.Find('PARAMS');
  if jarr = nil then Exit;
  for pn := 0 to jarr.Count-1 do
  begin
    jn := jarr.Child(pn);
    jv := jn.Find('INDEX');
    if jv <> nil then idx := StrToIntDef('$'+jv.AsString, -1)
                 else idx := -1;

    jv := jn.Find('OFFSET');
    if jv <> nil then offs := trunc(jv.AsNumber)
                 else offs := 0;

    jv := jn.Find('NAME');
    if jv <> nil then name := jv.AsString
                 else name := '';

    par := params.Add(idx, offs);
    par.name := name;

    if jn.Find('DTYPE', jv) then par.dtype := trunc(jv.AsNumber);
    if jn.Find('UNIT',  jv) then par.vunit := jv.AsString;
    if jn.Find('SCALE', jv) then par.scale := jv.AsNumber;
    if jn.Find('DESCR', jv) then par.descr := jv.AsString;
  end;

  jroot.SaveToFile(jsonfile);
end;

procedure TUdoDevice.Save;
var
  pn : integer;
  jarr, jn : TJsonNode;
  par : TUdoParam;
begin
  jroot.Clear;
  jarr := jroot.Add('PARAMS', nkArray);
  for pn := 0 to params.Count-1 do
  begin
    par := params[pn];
    jn := jarr.Add();
    jn.Add('INDEX', IntToHex(par.index, 4));
    jn.Add('OFFSET', par.offset);
    jn.Add('NAME', par.name);
    jn.Add('DTYPE', par.dtype);
    jn.Add('UNIT', par.vunit);
    jn.Add('SCALE', par.scale);
    jn.Add('DESCR', par.descr);
  end;

  jroot.SaveToFile(jsonfile);
end;

{ TUdoParamList }

function TUdoParamList.GetParamByIndex(aindex : integer) : TUdoParam;
begin
  if (aindex >= 0) and (aindex < length(flist))
  then
    result := flist[aindex]
  else
    result := nil;
end;

constructor TUdoParamList.Create(adevice : TUdoDevice);
begin
  fdevice := adevice;
  flist := [];
end;

procedure TUdoParamList.Clear;
var
  par : TUdoParam;
begin
  for par in flist do
  begin
    par.Free;
  end;
  flist := [];
end;

procedure TUdoParamList.Save;
begin
  fdevice.Save;
end;

function TUdoParamList.Count : integer;
begin
  result := length(flist);
end;

function TUdoParamList.Find(index : uint16; offset : uint16) : TUdoParam;
begin
  result := Find(index shr 16 + offset);
end;

function TUdoParamList.Find(objid : uint32) : TUdoParam;
begin
  // just a linear search, not so effective
  for result in flist do
  begin
    if result.objid = objid then Exit;
  end;
  result := nil;
end;

function TUdoParamList.Find(aname : string) : TUdoParam;
begin
  // just a linear search, not so effective
  for result in flist do
  begin
    if result.name = aname then Exit;
  end;
  result := nil;
end;

function TUdoParamList.Add(index : uint16; offset : uint16) : TUdoParam;
var
  i : integer;
begin
  result := Find(index, offset);
  if result = nil then
  begin
    result := TUdoParam.Create(index, offset);
    i := 0;
    while i < length(flist) do
    begin
      if flist[i].objid > result.objid then break;
      Inc(i);
    end;
    // find the insert position
    insert(result, flist, i);
  end;
end;

procedure TUdoParamList.Remove(apar : TUdoParam);
var
  i : integer;
begin
  for i := 0 to length(flist) - 1 do
  begin
    if flist[i] = apar then
    begin
      apar.Free;
      delete(flist, i, 1);
      EXIT;
    end;
  end;
end;

{ TUdoParam }

function TUdoParam.GetOffset : uint16;
begin
  result := objid and $FFFF;
end;

procedure TUdoParam.SetIndex(aindex : uint16);
begin
  objid := (aindex shl 16) or (objid and $FFFF);
end;

function TUdoParam.GetIndex : uint16;
begin
  result := (objid shr 16) and $FFFF;
end;

procedure TUdoParam.SetOffset(aoffset : uint16);
begin
  objid := aoffset or (objid and $FFFF0000);
end;


constructor TUdoParam.Create(aindex : uint16; aoffset : uint16);
begin
  objid := (aindex shl 16) + (aoffset and $FFFF);
  name  := '?';
  vunit := '';
  descr := '';
  dtype := CDTYPE_INT + 3;
  scale := 1.0;
end;

function TUdoParam.ByteSize : uint32;
begin
  result := 1 + (dtype and CDTYPE_SIZE_MASK);
end;

function TUdoParam.ScopeDef : uint32;
begin
  result := (index shl 16) or ((offset and $FF) shl 8) or ByteSize;
end;

function TUdoParam.FormatValue(const buf; buflen : uint32) : string;
begin
  result := FormatValueGeneric(buf, buflen);
  //'buflen='+IntToStr(buflen);
end;

class function TUdoParam.FormatValueGeneric(const buf; buflen : uint32) : string;
var
  n : integer;
  pb : PByte;
begin
  if buflen > 16 then
  begin
    result := IntToStr(buflen) + ' bytes';
  end
  else if buflen <= 4 then
  begin
    if       buflen = 1 then
    begin
      result := format('%u = %.2Xh', [PByte(@buf)^, PByte(@buf)^]);
    end
    else if  buflen = 2 then
    begin
      result := format('%d = %.4Xh', [PInt16(@buf)^, PInt16(@buf)^ and $FFFF]);
    end
    else
    begin
      result := format('%d = %.8Xh', [PInt32(@buf)^, PInt32(@buf)^]);
    end
  end
  else
  begin
    // hex dump
    result := '';
    pb := PByte(@buf);
    for n := 1 to buflen do
    begin
      result += format(' %.2X', [pb^]);
    end;
  end;
end;

function TUdoParam.ConvertToDouble(aptr : pointer) : double;
var
  dt : byte;
  bitsize : byte;
  d : double;
begin
  dt := (dtype and CDTYPE_TYPE_MASK);
  bitsize := 8 * (dtype and CDTYPE_SIZE_MASK + 1);

  if dt = CDTYPE_INT then
  begin
    if      bitsize = 32 then d := PInt32(aptr)^
    else if bitsize = 16 then d := PInt16(aptr)^
    else if bitsize = 64 then d := PInt64(aptr)^
    else                      d := PInt8(aptr)^;
  end
  else if dt = CDTYPE_UINT then
  begin
    if      bitsize = 32 then d := PUInt32(aptr)^
    else if bitsize = 16 then d := PUInt16(aptr)^
    else if bitsize = 64 then d := PUInt64(aptr)^
    else                      d := PUInt8(aptr)^;
  end
  else if dt = CDTYPE_FLOAT then
  begin
    if      bitsize = 64 then d := PDouble(aptr)^
    else                      d := PSingle(aptr)^
  end
  else
  begin
    d := 0;
  end;

  result := d * scale;
end;

initialization
begin
  candevlist := TUdoDevList.Create;
end;

end.

