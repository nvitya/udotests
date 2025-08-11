unit form_parameters;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, Grids, ExtCtrls,
  Buttons, StdCtrls, Types, udo_device;

type

  { TfrmParameters }

  TfrmParameters = class(TForm)
    btnClose : TBitBtn;
    btnDelete : TBitBtn;
    btnEdit : TBitBtn;
    btnNew : TBitBtn;
    btnCopy : TBitBtn;
    cbDevice : TComboBox;
    grid : TDrawGrid;
    Label1 : TLabel;
    pnlTop : TPanel;
    pnlbot : TPanel;
    btnDevices : TSpeedButton;
    btnSelect : TBitBtn;
    Label2 : TLabel;
    fltName : TEdit;
    procedure btnCloseClick(Sender : TObject);
    procedure btnDeleteClick(Sender : TObject);
    procedure btnDevicesClick(Sender : TObject);
    procedure btnEditClick(Sender : TObject);
    procedure btnSelectClick(Sender : TObject);
    procedure btnCopyClick(Sender : TObject);
    procedure cbDeviceChange(Sender : TObject);
    procedure FormShow(Sender : TObject);
    procedure gridDblClick(Sender : TObject);
    procedure gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
    procedure FormClose(Sender : TObject; var CloseAction : TCloseAction);
    procedure gridKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
    procedure FormCreate(Sender : TObject);
    procedure btnNewClick(Sender : TObject);
  private

  public

    selecting : boolean;
    preselect_item : TUdoParam;

    curdev   : TUdoDevice;
    itemlist : TUdoParamList;

    function SelectedItem : TUdoParam;
    procedure UpdateGrid;
    procedure UpdateDevices;

    procedure EditItem(apar : TUdoParam; acopy : TUdoParam = nil);
    procedure SelectItem(apar: TUdoParam);
  end;

var
  frmParameters : TfrmParameters = nil;

function ParametersShowSelect(astart : TUdoParam) : TUdoParam;

implementation

uses
  lcltype, form_param_edit, form_devices, connections;

function ParametersShowSelect(astart : TUdoParam) : TUdoParam;
var
  frm : TfrmParameters;
begin
  result := nil;

  Application.CreateForm(TfrmParameters, frm);
  frm.selecting := true;
  frm.preselect_item := astart;
  if frm.ShowModal = mrOK then
  begin
    result := frm.SelectedItem;
  end;
  frm.Free;
end;

{$R *.lfm}

{ TfrmParameters }

procedure TfrmParameters.EditItem(apar : TUdoParam; acopy : TUdoParam);
var
  frm : TfrmParamEdit;
begin
  Application.CreateForm(TfrmParamEdit, frm);
  frm.itemlist := self.itemlist;
  if acopy <> nil then
  begin
    frm.Load(acopy);
    frm.item := nil;
    frm.Caption := 'Copy Parameter';
  end
  else
  begin
    frm.Load(apar);
  end;
  if frm.ShowModal = mrOk then
  begin
    // jump to the row
    UpdateGrid;
    SelectItem(frm.item);
  end;
  frm.Free;
end;

procedure TfrmParameters.SelectItem(apar : TUdoParam);
var
  i : integer;
begin
  for i := 1 to itemlist.Count do
  begin
    if itemlist[i-1] = apar then
    begin
      grid.Row := i;
      exit;
    end;
  end;
end;


procedure TfrmParameters.FormClose(Sender : TObject; var CloseAction : TCloseAction);
begin
  CloseAction := caFree;
  frmParameters := nil;
end;

procedure TfrmParameters.gridKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
begin
  if not grid.Focused then Exit;

  if      key = VK_RETURN then
  begin
    if selecting then btnSelect.Click else btnEdit.Click;
  end
  else if key = VK_INSERT then btnNew.Click
  else if key = VK_DELETE then btnDelete.Click
  ;

end;

procedure TfrmParameters.FormCreate(Sender : TObject);
begin
  selecting := False;
  preselect_item := nil;
end;

procedure TfrmParameters.btnNewClick(Sender : TObject);
begin
  EditItem(nil, nil);
end;

function TfrmParameters.SelectedItem : TUdoParam;
begin
  if (grid.Row > 0) and (grid.Row <= itemlist.count)
  then
    result := itemlist[grid.Row - 1]
  else
    result := nil;
end;

procedure TfrmParameters.UpdateGrid;
begin
  grid.RowCount := 1 + itemlist.count;
  grid.Refresh;
end;

procedure TfrmParameters.UpdateDevices;
var
  i : integer;
begin
  // fill the device combo
  cbDevice.Items.Clear;
  for i := 0 to candevlist.Count-1 do cbDevice.Items.Add(candevlist[i].id);
  cbDevice.ItemIndex := candevlist.ActiveIndex;
end;

procedure TfrmParameters.btnCloseClick(Sender : TObject);
begin
  Close;
end;

procedure TfrmParameters.btnDeleteClick(Sender : TObject);
begin
  if MessageDlg('Are you sure to delete this parameter?', mtConfirmation, mbYesNo, 0) = mrYes then
  begin
    itemlist.Remove(SelectedItem);
    UpdateGrid;
  end;
end;

procedure TfrmParameters.btnDevicesClick(Sender : TObject);
var
  oldidx : integer;
begin
  oldidx := cbDevice.ItemIndex;
  FormDevicesShowModal;
  UpdateDevices;
  cbDevice.ItemIndex := oldidx;
end;

procedure TfrmParameters.btnEditClick(Sender : TObject);
begin
  EditItem(SelectedItem);
end;

procedure TfrmParameters.btnSelectClick(Sender : TObject);
begin
  ModalResult := mrOK;
end;

procedure TfrmParameters.btnCopyClick(Sender : TObject);
begin
  EditItem(nil, SelectedItem);
end;

procedure TfrmParameters.cbDeviceChange(Sender : TObject);
begin
  candevlist.ActiveIndex := cbDevice.ItemIndex;

  itemlist := candevlist[cbDevice.ItemIndex].params;
  UpdateGrid;
end;

procedure TfrmParameters.FormShow(Sender : TObject);
begin
  btnSelect.Visible := selecting;

  UpdateDevices;
  if activeconn <> nil then
  begin
    cbDevice.ItemIndex := candevlist.GetIndex(activeconn.device);
  end;
  itemlist := candevlist[cbDevice.ItemIndex].params;
  UpdateGrid;
  if selecting and (preselect_item <> nil) then
  begin
    SelectItem(preselect_item);
  end;
end;

procedure TfrmParameters.gridDblClick(Sender : TObject);
begin
  if selecting
  then
      btnSelect.Click
  else
      btnEdit.Click;
end;

procedure TfrmParameters.gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
var
  s : string;
  par : TUdoParam;
  c : TCanvas;
  ts : TTextStyle;
begin
  if arow = 0 then Exit;  // keep the header as it is

  par := itemlist[aRow - 1];
  if par = nil then Exit;

  // give some margins:
  Inc(arect.Left, 2);
  Dec(arect.Right, 2);

  c := grid.Canvas;
  ts := c.TextStyle;

  if 0 = acol then
  begin
    s := par.name;
  end
  else if 1 = acol then
  begin
    s := IntToHex(par.index, 4);
    ts.Alignment := taCenter;
  end
  else if 2 = acol then
  begin
    s := IntToStr(par.offset);
    ts.Alignment := taCenter;
  end
  else if 3 = acol then
  begin
    s := CDtypeToString(par.dtype);
    ts.Alignment := taCenter;
  end
  else if 4 = acol then
  begin
    s := par.vunit;
    ts.Alignment := taCenter;
  end
  else if 5 = acol then
  begin
    s := format('%1.6f', [par.scale]);
  end
  else if 6 = acol then
  begin
    s := par.descr;
  end
  else s := '?';

  c.TextStyle := ts;
  c.TextRect(aRect, arect.Left, arect.top, s);
end;

end.

