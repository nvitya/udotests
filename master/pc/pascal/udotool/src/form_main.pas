unit form_main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, StrUtils, Forms, Controls, Graphics, Dialogs, Menus, ComCtrls,
  ExtCtrls, StdCtrls, Buttons, JsonTools,
  udo_device, connections, udo_comm, strparseobj;

type

  { TfrmMain }

  TfrmMain = class(TForm)
    miConnect : TMenuItem;
    miParameters : TMenuItem;
    miDevices : TMenuItem;
    mmenu : TMainMenu;
    miExit : TMenuItem;
    mmiMenu : TMenuItem;
    pnlStatus : TPanel;
    pnlTerminal : TPanel;
    txtConnState : TStaticText;
    miDisconnect : TMenuItem;
    memoTerm : TMemo;
    pnlEdit : TPanel;
    edTerm : TComboBox;
    toolbar1 : TToolBar;
    ToolButton1 : TToolButton;
    tbimages : TImageList;
    tbDisconnect : TToolButton;
    ToolButton2 : TToolButton;
    Separator1 : TMenuItem;
    Separator2 : TMenuItem;
    miScope : TMenuItem;
    Separator3 : TMenuItem;
    tbScope : TToolButton;
    procedure FormCreate(Sender : TObject);
    procedure FormShow(Sender : TObject);
    procedure miConnectClick(Sender : TObject);
    procedure miParametersClick(Sender : TObject);
    procedure miDevicesClick(Sender : TObject);
    procedure miExitClick(Sender : TObject);
    procedure txtConnStateDblClick(Sender : TObject);
    procedure miDisconnectClick(Sender : TObject);
    procedure edTermKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
    procedure miScopeClick(Sender : TObject);
  private

  public
    readbuf : array of byte;

    procedure AddLog(astr : string);

    procedure UpdateConnState;
    procedure ExecuteCommandLine(cmd : string);

    procedure FillTermComboWithParams(params : TUdoParamList);

    procedure LoadConnState;
    procedure SaveConnState;
  end;

var
  frmMain : TfrmMain;

implementation

uses
  LCLType, form_devices, form_parameters, form_connections;

{$R *.lfm}

{ TfrmMain }

procedure TfrmMain.FormCreate(Sender : TObject);
begin
  candevlist.Load;
  connlist.Load;
  SetLength(readbuf, 4 * 1024 * 1024);
  memoTerm.Lines.Text := '';
end;


procedure TfrmMain.miExitClick(Sender : TObject);
begin
  Application.Terminate;
end;

procedure TfrmMain.txtConnStateDblClick(Sender : TObject);
begin
  miConnectClick(Sender);
end;

procedure TfrmMain.miDisconnectClick(Sender : TObject);
begin
  if activeconn <> nil
  then
      activeconn.Disconnect;
end;

procedure TfrmMain.edTermKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
var
  i : integer;
  cmd : string;
begin
  if not edTerm.Focused then EXIT;

  if key = VK_RETURN then
  begin
    cmd := edTerm.Text;

    ExecuteCommandLine(cmd);

    repeat
      i := edTerm.Items.IndexOf(cmd);
      if i >= 0 then edTerm.Items.Delete(i);
    until i < 0;

    {$ifdef WINDOWS}
    edTerm.Items.Insert(0, cmd);
    {$else}
    edTerm.Items.Insert(edTerm.Items.Count, cmd);
    {$endif}
    edTerm.Text := '';
  end;
end;

procedure TfrmMain.miScopeClick(Sender : TObject);
//var
//  frm : TfrmScope;
begin
{
  Application.CreateForm(TfrmScope, frm);
  insert(frm, scopelist, length(scopelist));
  frm.Show;
}
end;

procedure TfrmMain.AddLog(astr : string);
begin
  memoTerm.Lines.Add(astr);
end;

procedure TfrmMain.UpdateConnState;
var
  s : string;
begin
  if activeconn <> nil then
  begin
    if activeconn.device = nil then s := '???'
    else
    begin
      s := activeconn.device.id;
      FillTermComboWithParams(activeconn.device.params);
    end;

    txtConnState.Caption := activeconn.id+' ('+s+') at '+activeconn.addr;
  end
  else
  begin
    txtConnState.Caption := 'Disconnected.';
  end;

  edTerm.Enabled := (activeconn <> nil);

  miDisconnect.Enabled := (activeconn <> nil);

  tbDisconnect.Enabled := miDisconnect.Enabled;
  miScope.Enabled := miDisconnect.Enabled;
  tbScope.Enabled := miDisconnect.Enabled;

end;

procedure TfrmMain.ExecuteCommandLine(cmd : string);
var
  sp : TStrParseObj;
  idstr : string;
  objidx  : int32;
  offs : int32;
  par : TUdoParam;
  dev : TUdoDevice;
  rlen : integer;
  wvalue : integer;
begin
  dev := activeconn.device;
  if dev = nil then EXIT;

  AddLog('> '+cmd);

  sp.Init(cmd);

  if sp.ReadTo(' .=') then
  begin
    idstr := sp.PrevStr();
  end
  else
  begin
    idstr := cmd;
  end;

  try
    objidx := Hex2Dec(idstr);
  except
    objidx := -1;
  end;
  offs := 0;

  if objidx > 0 then
  begin
    // numeric id was given
    if sp.CheckSymbol('.') then
    begin
      // read subindex
      if sp.ReadAlphaNum() then
      begin
        offs := sp.PrevToInt();
      end;
    end;
    par := nil;
  end
  else
  begin
    // string id, search in the parameters
    par := dev.params.Find(UpperCase(idstr));
    if par = nil then
    begin
      AddLog(' Error: parameter unknown');
      EXIT;
    end;

    objidx := par.index;
    offs := par.offset;
    if sp.CheckSymbol('.') then
    begin
      if sp.ReadAlphaNum() then
      begin
        offs := sp.PrevToInt();
      end;
    end;
  end;

  sp.SkipSpaces();
  if sp.CheckSymbol('=') then
  begin
    // write
    sp.SkipSpaces();
    if sp.CheckSymbol('0x') or sp.CheckSymbol('0X') then
    begin
      // hex data
      if not sp.ReadAlphaNum() then
      begin
        AddLog('invalid hex value');
        EXIT;
      end;
      wvalue := sp.PrevHexToInt();
    end
    else
    begin
      if not sp.ReadAlphaNum() then
      begin
        AddLog('invalid value');
        EXIT;
      end;
      wvalue := sp.PrevToInt();
    end;
    try
      udocomm.UdoWrite(objidx, offs, wvalue, 4);

      AddLog(IntToHex(objidx shr 8, 4)+'.'+IntToStr(objidx and $FF)+' <- '+IntToStr(wvalue));

    except
      on e : Exception do
      begin
        AddLog(' Error: '+e.Message);
        EXIT;
      end;
    end;
  end
  else
  begin
    // read
    try
      rlen := udocomm.UdoRead(objidx, offs, readbuf[0], length(readbuf));
    except
      on e : Exception do
      begin
        AddLog(' Error: '+e.Message);
        EXIT;
      end;
    end;

    if par <> nil then
    begin
      // format the result according the parameter
      AddLog('  = '+par.FormatValue(readbuf[0], rlen));
    end
    else
    begin
      AddLog('  = '+TUdoParam.FormatValueGeneric(readbuf[0], rlen));
    end;
  end;
end;

procedure TfrmMain.FillTermComboWithParams(params : TUdoParamList);
var
  i, pari : integer;
  par : TUdoParam;
begin
  for pari := 0 to params.count - 1 do
  begin
    par := params[pari];
    i := edTerm.Items.IndexOf(par.name);
    if i < 0 then edTerm.Items.Add(par.name);
  end;
end;

procedure TfrmMain.LoadConnState;
var
  jroot, jv : TJsonNode;
  conn : TConnection;
begin
  jroot := TJsonNode.Create;
  try
    jroot.LoadFromFile('activeconn.json');
    if jroot.Find('CONNID', jv) then
    begin
      conn := connlist.Find(jv.AsString);
      if conn <> nil then conn.Connect;
    end;
  except
    // ignore errors
  end;

  jroot.Free;
end;

procedure TfrmMain.SaveConnState;
var
  jroot : TJsonNode;
begin
  jroot := TJsonNode.Create;
  if activeconn <> nil
  then
      jroot.Add('CONNID', activeconn.id)
  else
      jroot.Add('CONNID', '');

  jroot.SaveToFile('activeconn.json');
  jroot.Free;
end;

procedure TfrmMain.miDevicesClick(Sender : TObject);
begin
  if frmDevices = nil then
  begin
    Application.CreateForm(TfrmDevices, frmDevices);
  end;
  frmDevices.Show;
end;

procedure TfrmMain.miParametersClick(Sender : TObject);
begin
  if frmParameters = nil then
  begin
    Application.CreateForm(TfrmParameters, frmParameters);
  end;
  frmParameters.Show;
end;

procedure TfrmMain.miConnectClick(Sender : TObject);
begin
  if frmConnect = nil then
  begin
    Application.CreateForm(TfrmConnect, frmConnect);
  end;
  frmConnect.Show;
end;

procedure TfrmMain.FormShow(Sender : TObject);
begin
  LoadConnState;
  UpdateConnState;
end;


end.

