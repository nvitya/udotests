unit udo_comm;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils;

const
  UDOERR_CONNECTION             = $1001;  // not connected, send / receive error
  UDOERR_CRC                    = $1002;
  UDOERR_TIMEOUT                = $1003;
  UDOERR_DATA_TOO_BIG           = $1004;

  UDOERR_WRONG_INDEX            = $2000;  // index / object does not exists
  UDOERR_WRONG_OFFSET           = $2001;  // like the offset must be divisible by the 4
  UDOERR_WRONG_ACCESS           = $2002;
  UDOERR_READ_ONLY              = $2010;
  UDOERR_WRITE_ONLY             = $2011;
  UDOERR_WRITE_BOUNDS           = $2012;  // write is out ouf bounds
  UDOERR_WRITE_VALUE            = $2020;  // invalid value
  UDOERR_RUN_MODE               = $2030;  // config mode required
  UDOERR_UNITSEL                = $2040;  // the referenced unit is not existing
  UDOERR_BUSY                   = $2050;

  UDOERR_NOT_IMPLEMENTED        = $9001;
  UDOERR_INTERNAL               = $9002;  // internal implementation error
  UDOERR_APPLICATION            = $9003;  // internal implementation error

type

  { EUdoAbort }

  EUdoAbort = class(Exception)
  public
    ecode : uint16;
    constructor Create(acode : uint16; amsg : string; const args : array of const);
  end;

  TUdoCommProtocol = (ucpNone, ucpSerial, ucpIP);

  { TUdoCommHandler }

  TUdoCommHandler = class
  public
    default_timeout : single;
    timeout : single;
    protocol : TUdoCommProtocol;

    constructor Create; virtual;

  public // virtual functions
    procedure Open; virtual;
    procedure Close; virtual;
    function  Opened : boolean; virtual;
    function  ConnString : string; virtual;

    function  UdoRead(index : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer; virtual;
    procedure UdoWrite(index : uint16; offset : uint32; const dataptr; datalen : uint32); virtual;
  end;

  { TUdoComm }

  TUdoComm = class
  public
    commh : TUdoCommHandler;

    constructor Create;

    procedure SetHandler(acommh : TUdoCommHandler);
    procedure Open;
    procedure Close;
    function  Opened : boolean;

    function  UdoRead(index : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer;
    procedure UdoWrite(index : uint16; offset : uint32; const dataptr; datalen : uint32);

    function  UdoReadInt(index : uint16; offset : uint32) : int32;
    procedure UdoWriteInt(index : uint16; offset : uint32; avalue : int32);

  end;

function UdoAbortText(abortcode : word) : string;

var
  commh_none : TUdoCommHandler = nil;

var
  udocomm : TUdoComm;

implementation

function UdoAbortText(abortcode : word) : string;
begin
  if      abortcode = UDOERR_CONNECTION       then result := 'connection error'
  else if abortcode = UDOERR_CRC              then result := 'CRC error'
  else if abortcode = UDOERR_TIMEOUT          then result := 'Timeout'
  else if abortcode = UDOERR_DATA_TOO_BIG     then result := 'Data does not fit'
  else if abortcode = UDOERR_WRONG_INDEX      then result := 'Object does not exists'
  else if abortcode = UDOERR_WRONG_OFFSET     then result := 'Invalid offset'
  else if abortcode = UDOERR_WRONG_ACCESS     then result := 'Invalid Access'
  else if abortcode = UDOERR_READ_ONLY        then result := 'Writing read-only object'
  else if abortcode = UDOERR_WRITE_ONLY       then result := 'Reading write-only object'
  else if abortcode = UDOERR_WRITE_BOUNDS     then result := 'Write out of bounds'
  else if abortcode = UDOERR_WRITE_VALUE      then result := 'invalid value'
  else if abortcode = UDOERR_RUN_MODE         then result := 'config mode required'
  else if abortcode = UDOERR_UNITSEL          then result := 'Unit not existing'
  else if abortcode = UDOERR_BUSY             then result := 'Busy'
  else if abortcode = UDOERR_NOT_IMPLEMENTED  then result := 'Not Implemented'
  else if abortcode = UDOERR_INTERNAL         then result := 'Internal error'
  else if abortcode = UDOERR_APPLICATION      then result := 'Application error'
  else
      result := format('Error 0x%04X', [abortcode]);
end;

{ EUdoAbort }

constructor EUdoAbort.Create(acode : uint16; amsg : string; const args : array of const);
begin
  inherited CreateFmt(amsg, args);
  ecode := acode;
end;

{ TUdoCommHandler }

constructor TUdoCommHandler.Create;
begin
  default_timeout := 1;
  timeout := default_timeout;
  protocol := ucpNone;
end;

procedure TUdoCommHandler.Open;
begin
  raise EUdoAbort.Create(UDOERR_APPLICATION, 'Open: Invalid commhandler', []);
end;

procedure TUdoCommHandler.Close;
begin
  // does nothing
end;

function TUdoCommHandler.Opened : boolean;
begin
  result := false;
end;

function TUdoCommHandler.ConnString : string;
begin
  result := 'NONE';
end;

function TUdoCommHandler.UdoRead(index : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer;
begin
  result := 0;
  raise EUdoAbort.Create(UDOERR_APPLICATION, 'UdoRead: Invalid commhandler', []);
end;

procedure TUdoCommHandler.UdoWrite(index : uint16; offset : uint32; const dataptr; datalen : uint32);
begin
  raise EUdoAbort.Create(UDOERR_APPLICATION, 'UdoWrite: Invalid commhandler', []);
end;

{ TUdoComm }

constructor TUdoComm.Create;
begin
  commh := commh_none;
end;

procedure TUdoComm.SetHandler(acommh : TUdoCommHandler);
begin
  if acommh <> nil then commh := acommh
                   else commh := commh_none;
end;

procedure TUdoComm.Open;
begin
  if not commh.Opened
  then
      commh.Open();
end;

procedure TUdoComm.Close;
begin
  commh.Close();
end;

function TUdoComm.Opened : boolean;
begin
  result := commh.Opened();
end;

function TUdoComm.UdoRead(index : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer;
begin
  result := commh.UdoRead(index, offset, dataptr, maxdatalen);
end;

procedure TUdoComm.UdoWrite(index : uint16; offset : uint32; const dataptr; datalen : uint32);
begin
  commh.UdoWrite(index, offset, dataptr, datalen);
end;

function TUdoComm.UdoReadInt(index : uint16; offset : uint32) : int32;
var
  resultbuf : array[0..16] of byte;
begin
  resultbuf[0] := 0; // fpc warning fix
  fillchar(resultbuf[0], sizeof(resultbuf), 0);
  commh.UdoRead(index, offset, resultbuf[0], sizeof(resultbuf));
  result := PInt32(@resultbuf[0])^;
end;

procedure TUdoComm.UdoWriteInt(index : uint16; offset : uint32; avalue : int32);
var
  lvalue : int32;
begin
  lvalue := avalue;
  commh.UdoWrite(index, offset, lvalue, 4);
end;


initialization
begin
  commh_none := TUdoCommHandler.Create;
  udocomm := TUdoComm.Create;
end;

end.

