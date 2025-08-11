unit form_conn_edit;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  Buttons, udo_device, connections;

type

  { TfrmConnEdit }

  TfrmConnEdit = class(TForm)
    Bevel1 : TBevel;
    btnTest : TBitBtn;
    btnCancel : TBitBtn;
    btnOk : TBitBtn;
    cbDevice : TComboBox;
    cbConnType : TComboBox;
    edID : TEdit;
    edAddress : TEdit;
    Label1 : TLabel;
    Label2 : TLabel;
    Label3 : TLabel;
    Label4 : TLabel;
    pnlButtons : TPanel;
    btnDevices : TSpeedButton;
    procedure btnDevicesClick(Sender : TObject);
    procedure btnOkClick(Sender : TObject);
  private

  public

    item : TConnection;

    procedure Load(aitem : TConnection);

    procedure UpdateDevices;
  end;

var
  frmConnEdit : TfrmConnEdit;

implementation

uses
  form_devices;

{$R *.lfm}

{ TfrmConnEdit }

procedure TfrmConnEdit.Load(aitem : TConnection);
begin
  item := aitem;
  if item = nil then Exit;

  UpdateDevices;

  edID.Text := item.id;
  if item.device <> nil then cbDevice.ItemIndex := candevlist.GetIndex(item.device);
  cbConnType.ItemIndex := ord(item.conntype);
  edAddress.Text := item.addr;
end;

procedure TfrmConnEdit.btnOkClick(Sender : TObject);
var
  id : string;
  dev : TUdoDevice;
begin
  ModalResult := mrNone;

  id := edID.Text;

  dev := candevlist[cbDevice.ItemIndex];
  if dev = nil then
  begin
    MessageDlg('Select a valid Device', mtError, [mbAbort], 0);
    cbDevice.SetFocus;
    exit;
  end;

  if item = nil then
  begin
    item := connlist.Add(id);
  end
  else
  begin
    item.id := id;
  end;

  item.device := dev;
  item.conntype := TConnType(cbConnType.ItemIndex);
  item.addr := edAddress.text;

  connlist.Save;

  ModalResult := mrOK;
end;

procedure TfrmConnEdit.btnDevicesClick(Sender : TObject);
var
  oldidx : integer;
begin
  oldidx := cbDevice.ItemIndex;
  FormDevicesShowModal;
  UpdateDevices;
  cbDevice.ItemIndex := oldidx;
end;

procedure TfrmConnEdit.UpdateDevices;
var
  i : integer;
  oldidx : integer;
begin
  oldidx := cbDevice.ItemIndex;
  cbDevice.Items.Clear;
  for i := 0 to candevlist.Count-1 do cbDevice.Items.Add(candevlist[i].id);
  cbDevice.ItemIndex := oldidx;
end;

end.

