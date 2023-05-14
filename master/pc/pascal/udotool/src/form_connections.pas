unit form_connections;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, Grids, ExtCtrls,
  Buttons, connections, Types;

type

  { TfrmConnect }

  TfrmConnect = class(TForm)
    btnClose : TBitBtn;
    btnConnect : TBitBtn;
    btnCopy : TBitBtn;
    btnDelete : TBitBtn;
    btnEdit : TBitBtn;
    btnNew : TBitBtn;
    grid : TDrawGrid;
    pnlButtons : TPanel;
    procedure btnCloseClick(Sender : TObject);
    procedure btnConnectClick(Sender : TObject);
    procedure btnCopyClick(Sender : TObject);
    procedure btnDeleteClick(Sender : TObject);
    procedure btnEditClick(Sender : TObject);
    procedure btnNewClick(Sender : TObject);
    procedure FormClose(Sender : TObject; var CloseAction : TCloseAction);
    procedure FormCreate(Sender : TObject);
    procedure FormShow(Sender : TObject);
    procedure gridDblClick(Sender : TObject);
    procedure gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
  private

  public

    itemlist : TConnList;

    procedure EditItem(aitem : TConnection; acopy : TConnection = nil);

    function SelectedItem : TConnection;
    procedure UpdateGrid;

    procedure SelectItem(aitem : TConnection);

  end;

var
  frmConnect : TfrmConnect = nil;

implementation

uses
  form_conn_edit;

{$R *.lfm}

{ TfrmConnect }

procedure TfrmConnect.btnCloseClick(Sender : TObject);
begin
  Close;
end;

procedure TfrmConnect.btnConnectClick(Sender : TObject);
begin
  if SelectedItem <> nil then
  begin
    SelectedItem.Connect;
    Close;
  end;
end;

procedure TfrmConnect.btnCopyClick(Sender : TObject);
begin
  EditItem(nil, SelectedItem);
end;

procedure TfrmConnect.btnDeleteClick(Sender : TObject);
begin
  if MessageDlg('Are you sure to delete this item?', mtConfirmation, mbYesNo, 0) = mrYes then
  begin
    itemlist.Remove(SelectedItem);
    UpdateGrid;
  end;
end;

procedure TfrmConnect.btnEditClick(Sender : TObject);
begin
  EditItem(SelectedItem, nil);
end;

procedure TfrmConnect.btnNewClick(Sender : TObject);
begin
  EditItem(nil, nil);
end;

procedure TfrmConnect.FormClose(Sender : TObject; var CloseAction : TCloseAction);
begin
  CloseAction := caFree;
  frmConnect := nil;
end;

procedure TfrmConnect.FormCreate(Sender : TObject);
begin
  itemlist := connlist;
end;

procedure TfrmConnect.FormShow(Sender : TObject);
begin
  UpdateGrid;
end;

procedure TfrmConnect.gridDblClick(Sender : TObject);
begin
  btnConnect.Click;
end;

procedure TfrmConnect.gridDrawCell(Sender : TObject; aCol, aRow : Integer; aRect : TRect; aState : TGridDrawState);
var
  s : string;
  item : TConnection;
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

  if      0 = acol then s := item.id
  else if 1 = acol then
  begin
    if item.device <> nil then s := item.device.id
                          else s := '???';
  end
  else if 2 = acol then s := GetConnTypeName(item.conntype)
  else if 3 = acol then s := item.addr
  else s := '?';

  c.TextStyle := ts;
  c.TextRect(aRect, arect.Left, arect.top, s);
end;

procedure TfrmConnect.EditItem(aitem : TConnection; acopy : TConnection);
var
  frm : TfrmConnEdit;
begin
  Application.CreateForm(TfrmConnEdit, frm);
  if acopy <> nil then
  begin
    frm.Load(acopy);
    frm.item := nil;
    frm.Caption := 'Copy Parameter';
  end
  else
  begin
    frm.Load(aitem);
  end;
  if frm.ShowModal = mrOk then
  begin
    // jump to the row
    UpdateGrid;
    SelectItem(frm.item);
  end;
  frm.Free;
end;

function TfrmConnect.SelectedItem : TConnection;
begin
  if (grid.Row > 0) and (grid.Row <= connlist.count)
  then
    result := connlist[grid.Row - 1]
  else
    result := nil;
end;

procedure TfrmConnect.UpdateGrid;
begin
  grid.RowCount := 1 + connlist.count;
end;

procedure TfrmConnect.SelectItem(aitem : TConnection);
var
  i : integer;
begin
  for i := 1 to connlist.Count do
  begin
    if connlist[i-1] = aitem then
    begin
      grid.Row := i;
      exit;
    end;
  end;
end;

end.

