unit commh_udoip;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils, Sockets,
{$ifdef WINDOWS}
  Windows, WinSock,
{$else}
  BaseUnix,
{$endif}
  udo_comm, fpcfixes;

const
  UDOIP_MAX_DATALEN = 1024;
  UDOIP_MAX_RQ_SIZE = 1040; // 1024 byte payload + 16 byte header

  UDOIP_DEFAULT_PORT = 1221;

type
  TUdoIpRqHeader = packed record
  	rqid         : uint32;  // sequence id to detect repeated requests
  	len_cmd      : uint16;  // LEN, MLEN, RW
  	index        : uint16;
  	offset       : uint32;
  	metadata     : uint32;
  end;

  PUdoIpRqHeader = ^TUdoIpRqHeader;

type

  { TCommHandlerUdoIp }

  TCommHandlerUdoIp = class(TUdoCommHandler)
  public
    ipaddrstr : string;
    slaveid   : integer;

    max_tries : integer;

    timeout_ms : integer;

    cursqnum : uint16;

    constructor Create; override;
    destructor Destroy; override;

    procedure Open; override;
    procedure Close; override;
    function  Opened : boolean; override;
    function  ConnString : string; override;

    function  UdoRead(address : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer; override;
    procedure UdoWrite(address : uint16; offset : uint32; const dataptr; datalen : uint32); override;

  protected
    fdsocket      : integer;
    server_addr   : TSockAddr;
    response_addr : TSockAddr;
    rxpoll_fds    : Tfdset;

    rqlen  : uint32;
    rqbuf  : array[0..UDOIP_MAX_RQ_SIZE-1] of byte;
    ansbuf : array[0..UDOIP_MAX_RQ_SIZE-1] of byte;

    iswrite   : boolean;
    mindex  : uint16;
    moffset   : uint32;
    mdataptr  : PByte;
    mdataremaining : integer;
    mmaxdatalen  : integer;
    chunksize : integer;
    chunknum  : integer;
    mremaining : integer;
    mfullsize : integer;

    opstring : string;

    rsp_datalen : integer;

    procedure DoUdoReadWrite;

  end;

var
  udoip_commh : TCommHandlerUdoIp;

implementation

{ TCommHandlerUdoIp }

constructor TCommHandlerUdoIp.Create;
begin
  inherited;
  protocol := ucpIP;
  ipaddrstr := '127.0.0.1';
  slaveid := 0;

  cursqnum := 0; // always start at zero, and increment, the port number will be at every connection different
  fdsocket := -1;
  rqlen := 0;
  //timeout_ms := 50;
  timeout_ms := 500;  // ESP responds with 100 ms delay
  max_tries := 3;
end;

destructor TCommHandlerUdoIp.Destroy;
begin
  Close;
  inherited Destroy;
end;

procedure TCommHandlerUdoIp.Open;
var
  sarr : array of string;
begin
  if fdsocket >= 0 then
  begin
    closesocket(fdsocket);
  end;

  // Create UDP socket:
  fdsocket := fpSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  sarr := ipaddrstr.split(':');

  // Create UDP socket:
	server_addr.sin_family := AF_INET;
  server_addr.sin_addr.s_addr := htonl(StrToHostAddr(sarr[0]).s_addr);
  if length(sarr) > 1 then
  begin
  	server_addr.sin_port := htons(StrToInt(sarr[1]));
  end
  else
  begin
  	server_addr.sin_port := htons(UDOIP_DEFAULT_PORT);
  end;

{$ifdef WINDOWS}
  FD_ZERO(rxpoll_fds);
  FD_SET(fdsocket, rxpoll_fds);
{$else}
  fpFD_ZERO(rxpoll_fds);
  fpFD_SET(fdsocket, rxpoll_fds);
{$endif}

  cursqnum := 0;
end;

procedure TCommHandlerUdoIp.Close;
begin
  if fdsocket >= 0 then
  begin
    closesocket(fdsocket);
    fdsocket := -1;
  end;
end;

function TCommHandlerUdoIp.Opened : boolean;
begin
  Result := (fdsocket >= 0);
end;

function TCommHandlerUdoIp.ConnString : string;
begin
  Result := format('UDO-IP %s', [ipaddrstr]);
end;

function TCommHandlerUdoIp.UdoRead(address : uint16; offset : uint32; out dataptr; maxdatalen : uint32) : integer;
begin
  iswrite  := false;
  mindex := address;
  moffset  := offset;
  mdataptr := PByte(@dataptr);
  mmaxdatalen := maxdatalen;

  DoUdoReadWrite;

  if (mmaxdatalen <= 8) and (mfullsize < mmaxdatalen) then
  begin
    FillChar(PByte(mdataptr + mfullsize)^, mmaxdatalen - mfullsize, 0); // pad smaller responses, todo: sign extension
  end;

  result := mfullsize;
end;

procedure TCommHandlerUdoIp.UdoWrite(address : uint16; offset : uint32; const dataptr; datalen : uint32);
begin
  iswrite := true;
  mindex := address;
  moffset  := offset;
  mdataptr := PByte(@dataptr);
  mmaxdatalen := datalen;
  mfullsize := datalen;
  DoUdoReadWrite;
end;

procedure TCommHandlerUdoIp.DoUdoReadWrite;
var
  rqhead, anshead : PUdoIpRqHeader;
  headsize : integer;
  rsp_data_len : integer;
  rsp_addr_len : TSocklen;
  r : integer;
  trynum : integer;
  ecode : uint16;
  rqdatalen : integer;
  tv : TTimeVal;
label
  repeat_send, repeat_recv, next_chunk;
begin

  rqhead  := PUdoIpRqHeader(@rqbuf[0]);
  anshead := PUdoIpRqHeader(@ansbuf[0]);
  headsize := sizeof(TUdoIpRqHeader);

  rqhead^.index := mindex;
  rqhead^.offset := moffset;
  rqhead^.metadata := 0;  // no metadata support so far

  if iswrite then
  begin
    chunksize := sizeof(rqbuf) - headsize;
    if chunksize > mfullsize then chunksize := mfullsize;

    rqhead^.len_cmd := chunksize or (1 shl 15);
    mremaining := mfullsize;
    move(mdataptr^, rqbuf[headsize], chunksize);
    rqdatalen := chunksize;
    opstring := format('UdoWrite(%.4X, %d)', [mindex, moffset]);
  end
  else  // read
  begin
    chunksize := mmaxdatalen;
    if chunksize > UDOIP_MAX_DATALEN then chunksize := UDOIP_MAX_DATALEN;

    rqhead^.len_cmd := chunksize;  // bit15 = 0: read
    rqdatalen := 0;
    mremaining := 0;
    mfullsize := 0;
    opstring := format('UdoRead(%.4X, %d)', [mindex, moffset]);
  end;

next_chunk:
  // repeat for more chunks

  Inc(cursqnum); // increment sequence number
  rqhead^.rqid   := cursqnum;
  rqhead^.offset := moffset;

  trynum := 1;

repeat_send:

  r := fpsendto(fdsocket, @rqbuf[0], headsize + rqdatalen, 0, @server_addr, sizeof(server_addr));
  if r <= 0
  then
    raise EUdoAbort.Create(UDOERR_CONNECTION, '%s request error: %d', [opstring, r]);

repeat_recv:

  // timed wait for the response, letting the OS do something other
  tv.tv_sec := timeout_ms div 1000;
  tv.tv_usec := (timeout_ms mod 1000) * 1000;

{$ifdef WINDOWS}
  FD_ZERO(rxpoll_fds);
  FD_SET(fdsocket, rxpoll_fds);
  r := select(fdsocket + 1, @rxpoll_fds, nil, nil, @tv);
{$else}
  fpFD_ZERO(rxpoll_fds);
  fpFD_SET(fdsocket, rxpoll_fds);
  r := fpselect(fdsocket + 1, @rxpoll_fds, nil, nil, @tv);
{$endif}
  if r <= 0 then
  begin
    if trynum < max_tries then
    begin
      Inc(trynum);       // the only place where the trynum is incremented !
      goto repeat_send;  // re-send on timeout
    end;
    raise EUdoAbort.Create(UDOERR_TIMEOUT, '%s timeout', [opstring]);
  end;

  rsp_addr_len := sizeof(response_addr);
  r := fprecvfrom(fdsocket, @ansbuf[0], sizeof(ansbuf), 0, @response_addr, @rsp_addr_len);
  if r <= 0
  then
    raise EUdoAbort.Create(UDOERR_TIMEOUT, '%s response read error: %d', [opstring, r]);

  rsp_data_len := r - headsize; // data length
  if rsp_data_len < 0 then
  begin
    // something invalid received
    if trynum < max_tries
    then
        goto repeat_recv;

    raise EUdoAbort.Create(UDOERR_CONNECTION, '%s invalid response length: %d', [opstring, r]);
  end;

  if (anshead^.rqid <> cursqnum) or (anshead^.index <> mindex) or (anshead^.offset <> moffset) then
  begin
    if trynum < max_tries
    then
        goto repeat_recv;

    raise EUdoAbort.Create(UDOERR_CONNECTION, '%s unexpected response', [opstring]);
  end;

  if (anshead^.len_cmd and $7FF) = $7FF then // error response?
  begin
    if r < sizeof(TUdoIpRqHeader) + 2
    then
        raise EUdoAbort.Create(UDOERR_CONNECTION, '%s error response length: %d', [opstring, r]);

    ecode := PUint16(@ansbuf[headsize])^;
    raise EUdoAbort.Create(ecode, '%s result: %.4X', [opstring, ecode]);
  end;

  if iswrite then
  begin
    mremaining := mfullsize - moffset - chunksize;
    if mremaining > 0 then
    begin
      mdataptr += chunksize;
      moffset  += chunksize;

      chunksize := sizeof(rqbuf) - headsize;
      if chunksize > mremaining then chunksize := mremaining;

      move(mdataptr^, rqbuf[headsize], chunksize);

      rqdatalen := chunksize;

      goto next_chunk;
    end;
  end
  else
  begin
    if rsp_data_len > 0 then
    begin
      if moffset + rsp_data_len > mmaxdatalen then
      begin
        raise EUdoAbort.Create(UDOERR_DATA_TOO_BIG, '%s result data is too big: %d', [opstring, mfullsize]);
      end;

      move(ansbuf[headsize], mdataptr^, rsp_data_len);
      mfullsize += rsp_data_len;
      moffset   += rsp_data_len;
      mdataptr  += rsp_data_len;
    end;

    if (mfullsize < mmaxdatalen) and (rsp_data_len >= chunksize) then
    begin
      goto next_chunk;
    end;
  end;

end;

initialization
begin
  udoip_commh := TCommHandlerUdoIp.Create;
end;

end.

