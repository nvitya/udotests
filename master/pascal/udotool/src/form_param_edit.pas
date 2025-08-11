unit form_param_edit;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  Spin, Buttons, udo_device;

type

  { TfrmParamEdit }

  TfrmParamEdit = class(TForm)
    Bevel1 : TBevel;
    btnCancel : TBitBtn;
    btnOk : TBitBtn;
    edIndex : TEdit;
    edDescr : TEdit;
    edScale : TEdit;
    edUnit : TEdit;
    edName : TEdit;
    Label1 : TLabel;
    Label2 : TLabel;
    edOffset : TSpinEdit;
    Label3 : TLabel;
    Label4 : TLabel;
    Label5 : TLabel;
    Label6 : TLabel;
    Label7 : TLabel;
    cbDType : TComboBox;
    procedure btnOkClick(Sender : TObject);
  private

  public
    item : TUdoParam;
    itemlist : TUdoParamList; // filled externally

    function DtypeToIndex(adtype : byte) : integer;
    function IndexToDtype(aindex : integer) : byte;

    procedure Load(apar : TUdoParam);

  end;

var
  frmParamEdit : TfrmParamEdit = nil;

implementation

{$R *.lfm}

{ TfrmParamEdit }

procedure TfrmParamEdit.btnOkClick(Sender : TObject);
var
  i, pind, poffs : integer;
  s : string;
  lpar : TUdoParam;
  lscale : double;
begin
  ModalResult := mrNone;

  // validate the inputs
  pind := StrToIntDef('$'+edIndex.Text, -1);
  if (pind < 0) or (pind >= $FFFF) then
  begin
    MessageDlg('Invalid index, it must be a proper hexadecimal number between 0000 and FFFF',
        mtError, [mbAbort], 0);
    edIndex.SetFocus;
    exit;
  end;
  // subindex validity already forced by the spinedit.
  poffs := edOffset.Value;

  lpar := itemlist.Find(pind, poffs);
  if (lpar <> nil) and (lpar <> item) then
  begin
    MessageDlg(format('Another parameter ("%s") already exists this index', [lpar.name]),
        mtError, [mbAbort], 0);
    edIndex.SetFocus;
    exit;
  end;

  s := UpperCase(edName.Text);
  if length(s) > sizeof(item.name) then
  begin
    MessageDlg('Name is too long. Max 16 chars are allowed.', mtError, [mbAbort], 0);
    edName.SetFocus;
    exit;
  end;

  // ensure that the name is unique
  if s <> '' then
  begin
    for i := 0 to itemlist.count-1 do
    begin
      lpar := itemlist[i];
      if (lpar <> item) and (lpar.name = s) then
      begin
        MessageDlg(
           format('Parameter %.4X.%d already has the name "%s"', [lpar.index, lpar.offset, lpar.name]),
           mtError, [mbAbort], 0);
        edName.SetFocus;
        exit;
      end;
    end;
  end;

  lscale := StrToFloatDef(edScale.Text, -1);
  if lscale <= 0 then
  begin
    MessageDlg('Invalid scale', mtError, [mbAbort], 0);
    edScale.SetFocus;
    exit;
  end;

  // storing
  if item = nil then
  begin
    item := itemlist.Add(pind, poffs);
  end
  else
  begin
    item.index  := pind;
    item.offset := poffs;
  end;

  item.name := UpperCase(edName.Text);
  item.vunit := edUnit.Text;
  item.dtype := IndexToDtype(cbDType.ItemIndex);
  item.descr := edDescr.Text;
  item.scale := lscale;

  itemlist.Save;

  ModalResult := mrOK;
end;

const
  dtype_to_index_arr : array of byte = (
    CDTYPE_INT + 3,  // i32
    CDTYPE_INT + 1,  // i16
    CDTYPE_INT + 0,  // i8
    CDTYPE_UINT + 3, // u32
    CDTYPE_UINT + 1, // u16
    CDTYPE_UINT + 0, // u8
    CDTYPE_FLOAT + 3, // f32
    CDTYPE_FLOAT + 7, // f64
    CDTYPE_STRING,    // str
    CDTYPE_BLOB      // blo
  );

function TfrmParamEdit.DtypeToIndex(adtype : byte) : integer;
var
  i : integer;
begin
  for i := 0 to High(dtype_to_index_arr) do
  begin
    if dtype_to_index_arr[i] = adtype then
    begin
      EXIT(i);
    end;
  end;
  result := 0;
end;

function TfrmParamEdit.IndexToDtype(aindex : integer) : byte;
begin
  if (aindex >= 0) and (aindex <= High(dtype_to_index_arr))
  then
      result := dtype_to_index_arr[aindex]
  else
      result := CDTYPE_UINT; // byte by default
end;

procedure TfrmParamEdit.Load(apar : TUdoParam);
begin
  item := apar;
  if item <> nil then
  begin
    edIndex.Text := IntToHex(item.index, 4);
    edOffset.Value := item.offset;
    edName.Text := item.name;
    cbDType.ItemIndex := DtypeToIndex(item.dtype);
    edDescr.Text := item.descr;
    edUnit.Text := item.vunit;
    edScale.Text := format('%.6f', [item.scale]);
  end
  else
  begin
    Caption := 'New Parameter';
  end;
end;

end.

