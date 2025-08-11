program udoiptest2;

uses
  sysutils, udo_comm, commh_udoip;

var
  test_arr : array of integer;

type
  TNamedParInfo = packed record // 64 bytes
  	index            : uint16;
  	bin_storage_type : uint8; // $28 = f64, $24 = f32, $12 = int16, $14 = int32, $02 = uint16, $04 = uint32
  	_reserved        : uint8;
  	scale            : single; // usually not 1.0 when bin_storage_type is int16
    name             : array[0..39] of char;
  	dataunit         : array[0..15] of char;
  end;


var
  np_list : array[0..15] of TNamedParInfo;

procedure ShowNamedPars;
var
  rlen : integer;
  i : integer;
  pn, pu : string;
  pnpi : ^TNamedParInfo;
begin
  writeln('Listing named parameters...');
  rlen := udocomm.UdoRead($7F00, $3020, np_list, sizeof(np_list));
  writeln('rlen=', rlen);
  for i := 0 to rlen div sizeof(np_list[0]) - 1 do
  begin
    pnpi := @np_list[i];
    pn := PChar(@pnpi^.name[0]);
    pu := PChar(@pnpi^.dataunit[0]);
    writeln(i,'.="',pn,'" [',pu,'], bst=', IntToHex(pnpi^.bin_storage_type, 2));
  end;

end;

procedure MainProc;
var
  rlen : integer;
  rint : integer;
  i : integer;
begin
  writeln('UDOIP Test');

  udoip_commh.ipaddrstr := '127.0.0.1:1221';
  udocomm.SetHandler(udoip_commh);

  udocomm.Open;

  writeln('Reading test object:');
  rlen := udocomm.UdoRead(0, 0, rint, 4);
  writeln('  = ', format('%.8X', [rint]));

  writeln('Reading test object again:');
  rlen := udocomm.UdoRead(0, 0, rint, 4);
  writeln('  = ', format('%.8X', [rint]));

  writeln('Reading max chunk length:');
  rlen := udocomm.UdoRead(1, 0, rint, 4);
  writeln('  = ', rint);

{$if 0}

  SetLength(test_arr, 65536);

  writeln('Testing blob read');
  rlen := udocomm.ReadBlob(2, 0, test_arr[0], length(test_arr) * 4);
  writeln('rlen = ', rlen);

  for i := 0 to 512 - 1 do
  begin
    if (i > 0) and ((i mod 8) = 0) then writeln;
    write(format(' %5d', [test_arr[i]]));
  end;

{$endif}

  ShowNamedPars;

  writeln;
  writeln('test finished.');

end;

begin
  MainProc;
  {$ifdef WINDOWS}
  readln;
  {$endif}
end.

