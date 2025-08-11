unit form_devices;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, Grids,
  Buttons, udo_device, Types;

type

  { TfrmDevices }

  TfrmDevices = class(TForm)
    btnEdit : TBitBtn;
    btnDelete : TBitBtn;
    btnClose : TBitBtn;
    btnNew : TBitBtn;
    grid : TDrawGrid;
    rightpanel : TPanel;
    procedure btnCloseClick(Sender : TObject);
    procedure btnDeleteClick(Sender : TObject);
    procedure btnEditClick(Sender : TObject);
    procedure btnNewClick(Sender : TObject);
    procedure FormClose(Sender : TObject; var CloseAction : TCloseAction);
    procedure FormCreate(Sender : TObject);
    procedure FormShow(Sender : TObject);
    procedure gridDblClick(Sender : TObject);
    procedure gridDrawCell(Sender : TObject; aCol, aRow : Integer;
      aRect : TRect; aState : TGridDrawState);
    procedure gridKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
  private

  public

    itemlist : TUdoDevList;  // just an alias to the candevlist

    procedure EditItem(aitem : TUdoDevice; acopy : TUdoDevice = nil);

    function SelectedItem : TUdoDevice;
    procedure UpdateGrid;

    procedure SelectItem(aitem : TUdoDevice);

  end;

var
  frmDevices : TfrmDevices = nil;

procedure FormDevicesShowModal;

implementation

uses
  lcltype, form_device_edit;

procedure FormDevicesShowModal;
begin
  if frmDevices <> nil then frmDevices.Free;
  Application.ProcessMessages;
  Application.CreateForm(TfrmDevices, frmDevices);
  frmDevices.ShowModal;
end;

{$R *.lfm}

{ TfrmDevices }

procedure TfrmDevices.FormClose(Sender : TObject; var CloseAction : TCloseAction);
begin
  CloseAction := caFree;
  frmDevices := nil;
end;

procedure TfrmDevices.FormCreate(Sender : TObject);
begin
  itemlist := candevlist;
end;

procedure TfrmDevices.FormShow(Sender : TObject);
begin
  UpdateGrid;
end;

procedure TfrmDevices.gridDblClick(Sender : TObject);
begin
  btnEdit.Click;
end;

procedure TfrmDevices.gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
var
  s : string;
  item : TUdoDevice;
  c : TCanvas;
  ts : TTextStyle;
begin
  if arow = 0 then Exit;  // keep the header as it is

  item := itemlist[aRow - 1];
  if item = nil then Exit;

  // give some margins:
  Inc(arect.Left, 2);
  Dec(arect.Right, 2);

  c := grid.Canvas;
  ts := c.TextStyle;

  if 0 = acol then s := item.id
  else if 1 = acol then s := item.jsonfile
  else s := '?';

  c.TextStyle := ts;
  c.TextRect(aRect, arect.Left, arect.top, s);
end;

procedure TfrmDevices.gridKeyDown(Sender : TObject; var Key : Word; Shift : TShiftState);
begin
  if not grid.Focused then Exit;

  if      key = VK_RETURN then btnEdit.Click
  else if key = VK_INSERT then btnNew.Click
  else if key = VK_DELETE then btnDelete.Click
  ;
end;

procedure TfrmDevices.EditItem(aitem : TUdoDevice; acopy : TUdoDevice);
var
  frm : TfrmDeviceEdit;
begin
  Application.CreateForm(TfrmDeviceEdit, frm);
  frm.Load(aitem);
  if frm.ShowModal = mrOk then
  begin
    // jump to the row
    UpdateGrid;
    SelectItem(frm.item);
  end;
  frm.Free;
end;

function TfrmDevices.SelectedItem : TUdoDevice;
begin
  if (grid.Row > 0) and (grid.Row <= itemlist.count)
  then
    result := candevlist[grid.Row - 1]
  else
    result := nil;
end;

procedure TfrmDevices.UpdateGrid;
begin
  grid.RowCount := 1 + itemlist.count;
end;

procedure TfrmDevices.SelectItem(aitem : TUdoDevice);
var
  i : integer;
begin
  for i := 1 to itemlist.Count do
  begin
    if itemlist[i-1] = aitem then
    begin
      grid.Row := i;
      exit;
    end;
  end;
end;

procedure TfrmDevices.btnCloseClick(Sender : TObject);
begin
  Close;
end;

procedure TfrmDevices.btnDeleteClick(Sender : TObject);
begin
  if MessageDlg('Are you sure to delete this record?', mtConfirmation, mbYesNo, 0) = mrYes then
  begin
    itemlist.Remove(SelectedItem);
    UpdateGrid;
  end;
end;

procedure TfrmDevices.btnEditClick(Sender : TObject);
begin
  EditItem(SelectedItem);
end;

procedure TfrmDevices.btnNewClick(Sender : TObject);
begin
  EditItem(nil);
end;

end.

