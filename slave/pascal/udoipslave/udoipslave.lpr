program udoipslave;

uses
  udo_slave;

type
  TUdoSlave = class(TUdoIpSlaveBase)
  public
  end;

var
  udoslave : TUdoSlave;

procedure MainProc;
begin
  writeln('UDO-IP Slave (Pascal)');

  udoslave := TUdoSlave.Create;
  udoslave.InitListener;

  writeln('Starting main cycle...');
  while True do
  begin
    udoslave.WaitForEvents(1000);
  end;

  writeln('Test finished.');
end;

begin
  MainProc;
end.

