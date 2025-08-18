program udoiptest;

uses
  sysutils, udo_comm, commh_udoip;

var
  test_arr : array of integer;

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

  SetLength(test_arr, 65536);

  writeln('Testing blob read');
  rlen := udocomm.ReadBlob(2, 0, test_arr[0], length(test_arr) * 4);
  writeln('rlen = ', rlen);

  for i := 0 to 512 - 1 do
  begin
    if (i > 0) and ((i mod 8) = 0) then writeln;
    write(format(' %5d', [test_arr[i]]));

  end;

  writeln;
  writeln('test finished.');

end;

begin
  MainProc;
  {$ifdef WINDOWS}
  readln;
  {$endif}
end.

