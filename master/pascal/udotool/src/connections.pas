unit connections;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, JsonTools, udo_device;

type
  TConnType = (ctUDOIP, ctUDOSL);

  { TConnection }

  TConnection = class
  public
    id : string;
    addr : string;
    conntype : TConnType;
    device : TUdoDevice;

    constructor Create(aid : string);

    procedure Connect;
    procedure Disconnect;

  end;

  { TConnList }

  TConnList = class
  private
    function GetItemByIndex(aindex : integer) : TConnection;
  protected
    flist : array of TConnection;
    jroot : TJsonNode;
    jsonfile : string;
  public

    constructor Create;
    procedure Clear;

    function Count : integer;

    function Add(aid : string) : TConnection;
    procedure Remove(aconn : TConnection);
    function Find(aid : string) : TConnection;

    property Items[aindex : integer] : TConnection read GetItemByIndex; default;

    procedure Load;
    procedure Save;

  end;

var
  connlist : TConnList;
  activeconn : TConnection = nil;

function GetConnTypeName(ct : TConnType) : string;

implementation

uses
  udo_comm, commh_udoip, commh_udosl, form_main;

function GetConnTypeName(ct : TConnType) : string;
begin
  if      ct = ctUDOSL then result := 'UDO-SL'
  else if ct = ctUDOIP then result := 'UDO-IP'
  else result := '?';
end;

{ TConnList }

function TConnList.GetItemByIndex(aindex : integer) : TConnection;
begin
  if (aindex >= 0) and (aindex < length(flist))
  then
    result := flist[aindex]
  else
    result := nil;
end;

constructor TConnList.Create;
begin
  flist := [];
  jsonfile := 'connections.json';
  jroot := TJsonNode.Create;
end;

procedure TConnList.Clear;
var
  item : TConnection;
begin
  for item in flist do  item.Free;
  flist := [];
end;

function TConnList.Count : integer;
begin
  result := length(flist);
end;

function TConnList.Add(aid : string) : TConnection;
var
  i : integer;
begin
  result := Find(aid);
  if result = nil then
  begin
    result := TConnection.Create(aid);
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

procedure TConnList.Remove(aconn : TConnection);
var
  i : integer;
begin
  for i := 0 to length(flist) - 1 do
  begin
    if flist[i] = aconn then
    begin
      aconn.Free;
      delete(flist, i, 1);
    end;
  end;
end;

function TConnList.Find(aid : string) : TConnection;
begin
  for result in flist do
  begin
    if result.id = aid then Exit;
  end;
  result := nil;
end;

procedure TConnList.Load;
var
  jarr, jn, jv : TJsonNode;
  item : TConnection;
  i : integer;
  id : string;
begin
  Clear;

  if FileExists(jsonfile)
  then
    jroot.LoadFromFile(jsonfile);

  jarr := jroot.Find('CONNLIST');
  if jarr = nil then Exit;

  for i := 0 to jarr.Count-1 do
  begin
    jn := jarr.Child(i);

    jv := jn.Find('ID');
    if jv <> nil then
    begin
      id := jv.AsString;
      item := Add(id);
      item.device := nil;
      if jn.Find('DEVID', jv) then
      begin
        item.device := candevlist.Find(jv.asString);
      end;
      if item.device = nil then item.device := candevlist[0];

      jv := jn.Find('CONNTYPE');
      if jv <> nil then item.conntype := TConnType(trunc(jv.AsNumber));
      jv := jn.Find('ADDR');
      if jv <> nil then item.addr := jv.AsString;
    end;
  end;
end;

procedure TConnList.Save;
var
  jnarr, jn : TJsonNode;
  item : TConnection;
begin
  jroot.Clear;
  jnarr := jroot.Add('CONNLIST', nkArray);
  for item in flist do
  begin
    jn := jnarr.Add();
    jn.Add('ID', item.id);
    if item.device <> nil
    then
        jn.Add('DEVID', item.device.id);
    jn.Add('CONNTYPE', ord(item.conntype));
    jn.Add('ADDR', item.addr);
  end;

  jroot.SaveToFile(jsonfile);
end;

{ TConnection }

constructor TConnection.Create(aid : string);
begin
  id := aid;
  addr := '';
  device := nil;
  conntype := ctUDOIP;
end;

procedure TConnection.Connect;
begin
  udocomm.Close;

  if conntype = ctUDOIP then
  begin
    udoip_commh.Close;
    udoip_commh.ipaddrstr := addr;

    udocomm.SetHandler(udoip_commh);

    udocomm.Open;
  end
  else if conntype = ctUDOSL then
  begin
    udosl_commh.Close;
    udosl_commh.devstr := addr;

    udocomm.SetHandler(udosl_commh);

    udocomm.Open;
  end
  else
  begin
    raise Exception.Create('Unsupported connection type: '+GetConnTypeName(conntype));
  end;

  activeconn := self;
  frmMain.SaveConnState;
  frmMain.UpdateConnState;
end;

procedure TConnection.Disconnect;
begin
  udocomm.Close;
  activeconn := nil;
  frmMain.SaveConnState;
  frmMain.UpdateConnState;
end;

initialization
begin
  connlist := TConnList.Create;
end;

end.

