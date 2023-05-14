unit form_scope;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  Spin, udo_device, Buttons, Grids, Types, jsontools, udo_comm;

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
    procedure btnAddClick(Sender : TObject);
    procedure btnChangeClick(Sender : TObject);
    procedure btnDeleteClick(Sender : TObject);
    procedure btnUpClick(Sender : TObject);
    procedure btnDownClick(Sender : TObject);
    procedure gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
    procedure FormCreate(Sender : TObject);
    procedure cbSmpMulChange(Sender : TObject);
    procedure FormClose(Sender : TObject; var CloseAction : TCloseAction);
    procedure btnVscopeBrowseClick(Sender : TObject);

  public

    dev_scope_buffer  : uint32;
    dev_cycle_time_ns : uint32;
    sample_bytes      : uint32;
    sample_count      : uint32;

    channels : array of TUdoParam;
    //function GetChannelByIndex(aindex : integer) : TScopeChannel;

    procedure EditChannel(aidx : integer);
    function  SelectedCh : integer;
    procedure SelectChannel(aidx : integer);
    procedure UpdateChGrid;

    procedure LoadScope;
    procedure SaveScope;

    procedure UpdateSampling;

    procedure GetDeviceData;
    procedure FillSamplingCombo;

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

procedure TfrmScope.GetDeviceData;
begin
  try
    dev_scope_buffer := udocomm.UdoReadInt($5002, 0);
  except //on Exception do
    dev_scope_buffer := 32 * 1024;
  end;

  try
    dev_cycle_time_ns := udocomm.UdoReadInt($5003, 0);
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
  reclen_ns, sample_time_ns : uint64;
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

  if sample_count = 0 then
  begin
    txtRecLen.Caption := '-';
  end
  else
  begin
    sample_time_ns := dev_cycle_time_ns * uint32(1 shl cbSmpMul.ItemIndex);
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

