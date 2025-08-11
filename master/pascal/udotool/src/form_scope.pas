unit form_scope;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  Spin, Process, udo_device, Buttons, Grids, Types, jsontools, udo_comm, vscope_data;

type

  { TfrmScope }

  TfrmScope = class(TForm)
    pnlScope : TPanel;
    Label3 : TLabel;
    cbTriggerSlope : TComboBox;
    Label4 : TLabel;
    Label8 : TLabel;
    edPretriggerPercent : TSpinEdit;
    Label9 : TLabel;
    Label5 : TLabel;
    txtDeviceBufferSize : TStaticText;
    Label6 : TLabel;
    txtRecLen : TStaticText;
    btnStartScope : TBitBtn;
    Label7 : TLabel;
    txtScopeStatus : TStaticText;
    Label11 : TLabel;
    edMaxSamples : TSpinEdit;
    Panel2 : TPanel;
    pnlGridBtns : TPanel;
    btnAdd : TBitBtn;
    btnChange : TBitBtn;
    btnDelete : TBitBtn;
    btnUp : TBitBtn;
    btnDown : TBitBtn;
    grid : TDrawGrid;
    Label2 : TLabel;
    edTriggerCh : TSpinEdit;
    edTriggerValue : TEdit;
    cbSmpMul : TComboBox;
    Label13 : TLabel;
    Label10 : TLabel;
    txtVscopeExe : TStaticText;
    odVscope : TOpenDialog;
    btnVscopeBrowse : TButton;
    Bevel1 : TBevel;
    txtActualSamples : TStaticText;
    Label12 : TLabel;
    timStatusPoll : TTimer;

    procedure FormCreate(Sender : TObject);
    procedure FormClose(Sender : TObject; var CloseAction : TCloseAction);

    procedure btnAddClick(Sender : TObject);
    procedure btnChangeClick(Sender : TObject);
    procedure btnDeleteClick(Sender : TObject);
    procedure btnUpClick(Sender : TObject);
    procedure btnDownClick(Sender : TObject);
    procedure gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
    procedure cbSmpMulChange(Sender : TObject);
    procedure btnVscopeBrowseClick(Sender : TObject);
    procedure btnStartScopeClick(Sender : TObject);
    procedure timStatusPollTimer(Sender : TObject);

  public

    dev_max_channels  : int32;
    dev_scope_buffer  : uint32;
    dev_cycle_time_ns : uint32;
    sample_bytes      : uint32;
    sample_count      : uint32;
    sample_time_ns    : uint64;

    channels : array of TUdoParam;
    rawdata  : array of byte;

    scope : TScopeData;

    procedure EditChannel(aidx : integer);
    function  SelectedCh : integer;
    procedure SelectChannel(aidx : integer);
    procedure UpdateChGrid;

    procedure LoadScope;
    procedure SaveScope;

    procedure UpdateSampling;

    procedure GetDeviceData;
    procedure FillSamplingCombo;

    procedure StartScope;
    procedure PollScopeStatus;

    procedure ReadScopeData;
    procedure LaunchVscope(afilename : string);
  end;

var
  frmScope : TfrmScope;

implementation

uses
  connections, form_parameters;

{$R *.lfm}

{ TfrmScope }

procedure TfrmScope.btnAddClick(Sender : TObject);
begin
  if length(channels) >= dev_max_channels then
  begin
    MessageDlg('Add Channel', 'Maximal channel count reached!', mtError, [mbAbort], 0);
    EXIT;
  end;

  EditChannel(-1);
end;

procedure TfrmScope.btnChangeClick(Sender : TObject);
begin
  EditChannel(SelectedCh);
end;

procedure TfrmScope.btnDeleteClick(Sender : TObject);
begin
  if length(channels) > 0 then
  begin
    delete(channels, SelectedCh, 1);
    UpdateChGrid;
  end;
end;

procedure TfrmScope.btnUpClick(Sender : TObject);
var
  ch  : integer;
  par : TUdoParam;
begin
  if grid.Row > 1 then
  begin
    ch := SelectedCh;
    par := channels[ch];
    delete(channels, ch, 1);
    dec(ch);
    insert(par, channels, ch);
    grid.Row := ch + 1;
    UpdateChGrid;
  end;
end;

procedure TfrmScope.btnDownClick(Sender : TObject);
var
  ch  : integer;
  par : TUdoParam;
begin
  if grid.Row >= 1 then
  begin
    ch := SelectedCh;
    par := channels[ch];
    delete(channels, ch, 1);
    inc(ch);
    insert(par, channels, ch);
    grid.Row := ch + 1;
    UpdateChGrid;
  end;
end;

procedure TfrmScope.gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
var
  s : string;
  par : TUdoParam;
  c : TCanvas;
  ts : TTextStyle;
begin
  if arow = 0 then Exit;  // keep the header as it is
  if length(channels) < 1 then Exit;

  par := channels[aRow - 1];

  // give some margins:
  Inc(arect.Left, 2);
  Dec(arect.Right, 2);

  c := grid.Canvas;
  ts := c.TextStyle;

  if 0 = acol then
  begin
    s := IntToStr(aRow);
    ts.Alignment := taCenter;
  end
  else if 1 = acol then
  begin
    s := par.name;
  end
  else if 2 = acol then
  begin
    s := CDtypeToString(par.dtype);
    ts.Alignment := taCenter;
  end
  else if 3 = acol then
  begin
    s := par.descr;
  end
  else s := '?';

  c.TextStyle := ts;
  c.TextRect(aRect, arect.Left, arect.top, s);
end;

procedure TfrmScope.FormCreate(Sender : TObject);
begin
  scope := TScopeData.Create;

  GetDeviceData;
  txtDeviceBufferSize.Caption := IntToStr(dev_scope_buffer div 1024) + ' kByte';
  FillSamplingCombo;

  LoadScope;

  UpdateChGrid;
  UpdateSampling;
end;

procedure TfrmScope.cbSmpMulChange(Sender : TObject);
begin
  UpdateSampling;
end;

procedure TfrmScope.FormClose(Sender : TObject; var CloseAction : TCloseAction);
begin
  SaveScope;
  scope.Free;

  CloseAction := caFree;
  frmScope := nil;
end;

procedure TfrmScope.btnVscopeBrowseClick(Sender : TObject);
begin
  if odVscope.Execute then
  begin
    txtVscopeExe.Caption := odVscope.FileName;
  end;
end;

procedure TfrmScope.btnStartScopeClick(Sender : TObject);
begin
  StartScope;
end;

procedure TfrmScope.timStatusPollTimer(Sender : TObject);
begin
  PollScopeStatus;
end;

procedure TfrmScope.GetDeviceData;
begin
  try
    dev_max_channels := udocomm.ReadU32($5001, 0);
  except //on Exception do
    dev_max_channels := 8;
  end;

  try
    dev_scope_buffer := udocomm.ReadU32($5002, 0);
  except //on Exception do
    dev_scope_buffer := 32 * 1024;
  end;

  try
    dev_cycle_time_ns := udocomm.ReadU32($5003, 0);
  except //on Exception do
    dev_cycle_time_ns := 62500;
  end;
end;

procedure TfrmScope.FillSamplingCombo;
var
  smul : uint32;
  stime_ns : uint32;
begin
  cbSmpMul.Items.Clear;
  smul := 1;
  while smul < 1024 do
  begin
    stime_ns := dev_cycle_time_ns * smul;
    if stime_ns >= 1000000 then
    begin
      cbSmpMul.Items.Add(IntToStr(stime_ns div 1000000)+' ms');
    end
    else if stime_ns >= 1000 then
    begin
      cbSmpMul.Items.Add(IntToStr(stime_ns div 1000)+' us');
    end
    else
    begin
      cbSmpMul.Items.Add(IntToStr(stime_ns)+' ns');
    end;

    smul := smul shl 1;
  end;
  cbSmpMul.ItemIndex := 0;
end;

procedure TfrmScope.StartScope;
var
  i : integer;
  sst : integer;
  par : TUdoParam;
  trigval : integer;
begin

  // stop the scope if running
  while True do
  begin
    sst := udocomm.ReadU8($5009, 0);
    if (sst = 0) or (sst = 8)
    then
        break;

    udocomm.WriteU8($5008, 0, 0);  // stop the scope
    application.ProcessMessages;
    sleep(10);
  end;

  for i := 0 to dev_max_channels-1 do // fill all the channels
  begin
    if i < length(channels) then
    begin
      par := channels[i];
      udocomm.WriteU32($5020 + i, 0, par.index shl 16 + (par.offset and $FF) shl 8 + par.ByteSize);
    end
    else
    begin
      udocomm.WriteU32($5020 + i, 0, 0); // clear the unused channels
    end;
  end;

  // record settings
  udocomm.WriteU32($5010, 0, 1 shl cbSmpMul.ItemIndex);  // sample mul
  udocomm.WriteU32($5011, 0, edMaxSamples.Value);

  // trigger
  udocomm.WriteU8($5012, 0, edTriggerCh.Value);
  trigval := StrToIntDef(edTriggerValue.Text, 0);
  udocomm.WriteI32($5013, 0, trigval);
  udocomm.WriteU8($5014, 0, cbTriggerSlope.ItemIndex);
  udocomm.WriteU32($5016, 0, edPretriggerPercent.Value);

  // start the scope
  udocomm.WriteU8($5008, 0, 3);

  //PollScopeStatus;
  //application.ProcessMessages;

  timStatusPoll.Enabled := true;
end;

procedure TfrmScope.PollScopeStatus;
var
  s : string;
  scope_status : integer;
begin
  {
  	0: inactive, no data available to read
  	1: prefilling
  	3: permanent recording at inactive trigger
  	5: waiting for trigger
  	7: trigger hit, finishing the sampling
  	8: sampling finished, data available for read
  }

  scope_status := udocomm.ReadU8($5009, 0);

  if 0 = scope_status then
  begin
    s := 'Inactive';
    timStatusPoll.Enabled := false;
  end
  else if 1 = scope_status then
  begin
    s := 'Prefilling';
  end
  else if 2 = scope_status then
  begin
    s := 'permanent recording';
  end
  else if 5 = scope_status then
  begin
    s := 'Waiting for trigger';
  end
  else if 7 = scope_status then
  begin
    s := 'Post-Trigger sampling';
  end
  else if 8 = scope_status then
  begin
    timStatusPoll.Enabled := false;
    txtScopeStatus.Caption := 'Reading Data...';
    Application.ProcessMessages;

    ReadScopeData;

    s := 'Data Ready';
  end
  else
  begin
    s := 'Unhandled state: '+IntToStr(scope_status);
  end;

  txtScopeStatus.Caption := s;
end;

procedure TfrmScope.ReadScopeData;
var
  r : integer;
  par : TUdoParam;
  pb : PByte;
  didx : uint32;
  scnt : uint32;
  w : TWaveData;
  wi : integer;
  chnum : integer;
begin
  SetLength(rawdata, dev_scope_buffer);
  r := udocomm.ReadBlob($500A, 0, rawdata[0], length(rawdata));
  if r < length(rawdata) then SetLength(rawdata, r);

  scnt := uint32(length(rawdata)) div sample_bytes;

  chnum := length(channels);
  wi := 0;
  scope.waves.Clear;
  for par in channels do
  begin
    w := scope.AddWave(par.name, sample_time_ns / 1000000000);
    SetLength(w.data, scnt);  // prepare the data
    w.dataunit := par.vunit;
    w.viewoffset := 4 - wi;
    inc(wi);
  end;

  // convert the raw data to doubles
  didx := 0;
  pb := @rawdata[0];
  while didx < scnt do
  begin
    for wi := 0 to chnum - 1 do
    begin
      par := channels[wi];
      w := scope.waves[wi];
      w.data[didx] := par.ConvertToDouble(pb);
      Inc(pb, par.ByteSize);
    end;

    Inc(didx);
  end;

  scope.SaveToJsonFile('udotool.vscope');
  LaunchVscope('udotool.vscope');
end;

procedure TfrmScope.LaunchVscope(afilename : string);
var
  proc : TProcess;
begin
  proc := TProcess.Create(nil);
  proc.Executable := txtVscopeExe.Caption;
  proc.Parameters.Add(afilename);
  proc.Options := [poNoConsole, poDetached, poNewProcessGroup];
{
  TProcessOption = (poRunSuspended,poWaitOnExit,
                    poUsePipes,poStderrToOutPut,
                    poNoConsole,poNewConsole,
                    poDefaultErrorMode,poNewProcessGroup,
                    poDebugProcess,poDebugOnlyThisProcess,poDetached,
                    poPassInput,poRunIdle);
}
  proc.Execute;
  proc.Free;

  // ExecuteProcess(txtVscopeExe.Caption, [vsfilename], []);

end;

procedure TfrmScope.EditChannel(aidx : integer);
var
  par : TUdoParam = nil;
begin
  if aidx >= 0 then par := channels[aidx];
  par := ParametersShowSelect(par);
  if par <> nil then
  begin
    if aidx < 0 then
    begin
      Insert(par, channels, length(channels));
      aidx := length(channels) - 1;
    end
    else
    begin
      channels[aidx] := par;
    end;

    UpdateChGrid;
    SelectChannel(aidx);
  end;
end;

function TfrmScope.SelectedCh : integer;
begin
  result := grid.row - 1;
end;

procedure TfrmScope.SelectChannel(aidx : integer);
begin
  grid.Row := aidx + 1;
end;

procedure TfrmScope.UpdateChGrid;
begin
  grid.RowCount := 1 + length(channels);
  grid.Refresh;
  UpdateSampling;
end;

procedure TfrmScope.SaveScope;
var
  jroot, jn, jch : TJsonNode;
  par : TUdoParam;
begin
  jroot := TJsonNode.Create;
  jn := jroot.Add('CHANNELS', nkArray);
  for par in channels do
  begin
    jch := jn.Add();
    jch.Add('PARAM', par.name);
  end;

  jroot.Add('SMPMUL',         cbSmpMul.ItemIndex);
  jroot.Add('MAX_SAMPLES',    edMaxSamples.Value);
  jroot.Add('TRIGGER_CH',     edTriggerCh.Value);
  jroot.Add('TRIGGER_LEVEL',  StrToIntDef(edTriggerValue.Text, 0));
  jroot.Add('TRIGGER_SLOPE',  cbTriggerSlope.ItemIndex);
  jroot.Add('PRETRIGGER_PERCENT',  edPretriggerPercent.Value);
  jroot.Add('VSCOPE_EXE',     txtVscopeExe.Caption);

  jroot.SaveToFile('scope.json');
  jroot.Free;
end;

procedure TfrmScope.LoadScope;
var
  jroot, jn, jch, jv : TJsonNode;
  par : TUdoParam;
  i : integer;
begin
  channels := [];

  jroot := TJsonNode.Create;
  try
    jroot.LoadFromFile('scope.json');
  except
    //
  end;

  if jroot.Find('CHANNELS', jn) then
  begin
    for i := 0 to jn.Count - 1 do
    begin
      jch := jn.Child(i);
      if jch.Find('PARAM', jv) then
      begin
        par := activeconn.device.params.Find(jv.AsString);
        if par <> nil then
        begin
          insert(par, channels, length(channels));
        end;
      end;
    end;
  end;

  if jroot.Find('SMPMUL', jv)        then cbSmpMul.ItemIndex := trunc(jv.asNumber);
  if jroot.Find('MAX_SAMPLES', jv)   then edMaxSamples.Value := trunc(jv.asNumber);
  if jroot.Find('TRIGGER_CH', jv)    then edTriggerCh.Value := trunc(jv.asNumber);
  if jroot.Find('TRIGGER_LEVEL', jv) then edTriggerValue.Text := IntToStr(trunc(jv.asNumber));
  if jroot.Find('TRIGGER_SLOPE', jv) then cbTriggerSlope.ItemIndex := trunc(jv.asNumber);
  if jroot.Find('PRETRIGGER_PERCENT', jv) then edPretriggerPercent.Value := trunc(jv.asNumber);
  if jroot.Find('VSCOPE_EXE', jv)    then txtVscopeExe.Caption := jv.asString
                                     else txtVscopeExe.Caption := 'vscope.exe';

  jroot.Free;
end;

procedure TfrmScope.UpdateSampling;
var
  reclen_ns : uint64;
  par: TUdoParam;
begin
  sample_bytes := 0;
  for par in channels do
  begin
    sample_bytes += par.ByteSize;
  end;

  if sample_bytes > 0 then sample_count := dev_scope_buffer div sample_bytes
                      else sample_count := 0;

  if (edMaxSamples.Value > 0) and (sample_count > 0) and (edMaxSamples.Value < sample_count) then
  begin
    sample_count := edMaxSamples.Value;
  end;

  txtActualSamples.Caption := IntToStr(sample_count);

  sample_time_ns := dev_cycle_time_ns * uint32(1 shl cbSmpMul.ItemIndex);

  if sample_count = 0 then
  begin
    txtRecLen.Caption := '-';
  end
  else
  begin
    reclen_ns := uint64(sample_time_ns) * sample_count;

    if reclen_ns >= 1000000000 then
    begin
      txtRecLen.Caption := format('%.3f s', [reclen_ns / 1000000000]);
    end
    else if reclen_ns >= 1000000 then
    begin
      txtRecLen.Caption := format('%.3f ms', [reclen_ns / 1000000]);
    end
    else if reclen_ns >= 1000 then
    begin
      txtRecLen.Caption := format('%.3f us', [reclen_ns / 1000]);
    end
    else
    begin
      txtRecLen.Caption := IntToStr(reclen_ns)+' ns';
    end;
  end;
end;

initialization
begin
  frmScope := nil;
end;

end.

