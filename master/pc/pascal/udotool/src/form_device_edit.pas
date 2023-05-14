unit form_device_edit;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, Buttons, udo_device;

type

  { TfrmDeviceEdit }

  TfrmDeviceEdit = class(TForm)
    btnCancel : TBitBtn;
    btnOk : TBitBtn;
    edID : TEdit;
    edJSON : TEdit;
    Label1 : TLabel;
    Label2 : TLabel;
    savedlg : TSaveDialog;
    btnSaveDlg : TSpeedButton;
    procedure btnOkClick(Sender : TObject);
    procedure btnSaveDlgClick(Sender : TObject);
  private

  public
    item : TUdoDevice;

    procedure Load(aitem : TUdoDevice);

  end;

var
  frmDeviceEdit : TfrmDeviceEdit;

implementation

{$R *.lfm}

{ TfrmDeviceEdit }

procedure TfrmDeviceEdit.btnOkClick(Sender : TObject);
var
  id : string;
  jf : string;
  litem : TUdoDevice;
begin
  ModalResult := mrNone; // default for error cases

  id := UpperCase(edID.Text);
  jf := edJSON.Text;

  litem := candevlist.Find(id);
  if (litem <> nil) and (litem <> item) then
  begin
    MessageDlg('Another device already exists this ID', mtError, [mbAbort], 0);
    edID.SetFocus;
    exit;
  end;

  if not DirectoryExists(ExtractFileDir(jf)) then
  begin
    MessageDlg('Directory for the JSON file does not exists.', mtError, [mbAbort], 0);
    edJSON.SetFocus;
    exit;
  end;

  // storing
  if item = nil then
  begin
    item := candevlist.Add(id, jf);
  end
  else
  begin
    item.id := id;
  end;

  item.jsonfile := jf;

  candevlist.Save;

  ModalResult := mrOk;
end;

procedure TfrmDeviceEdit.btnSaveDlgClick(Sender : TObject);
begin
  if edJSON.Text = '' then
  begin
    savedlg.FileName := ExpandFileName('devices/'+edID.Text+'.json');
  end
  else
  begin
    savedlg.FileName := ExpandFileName(edJSON.Text);
  end;

  if savedlg.Execute then
  begin
    if savedlg.FileName.IndexOf(GetCurrentDir) = 0 then
    begin
      edJSON.Text := savedlg.FileName.Substring(length(GetCurrentDir)+1);
    end
    else
    begin
      edJSON.Text := savedlg.FileName;
    end;
  end;
end;

procedure TfrmDeviceEdit.Load(aitem : TUdoDevice);
begin
  item := aitem;
  if item <> nil then
  begin
    edID.Text := item.id;
    edJSON.Text := item.jsonfile;
  end
  else
  begin
    Caption := 'New Device';
  end;
end;

end.

