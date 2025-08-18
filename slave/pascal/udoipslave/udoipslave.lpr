program udoipslave;

uses
  udo_slave;

type

  { TUdoSlave }

  TUdoSlave = class(TUdoIpSlaveBase)
  public
    testvar : int32;

    constructor Create(alisten_port : uint16 = udo_ip_default_port); override;

    function ParamReadWrite(var udorq : TUdoRequest) : boolean; override;
  end;

var
  udoslave : TUdoSlave;

{ TUdoSlave }

constructor TUdoSlave.Create(alisten_port : uint16);
begin
  inherited Create(alisten_port);

  testvar := 1;
end;

function TUdoSlave.ParamReadWrite(var udorq : TUdoRequest) : boolean;
begin
  if $2000 = udorq.index then
  begin
    testvar += 1;
    result := udo_rw_data(udorq, @testvar, sizeof(testvar));
  end
  else
  begin
    result := inherited ParamReadWrite(udorq);
  end;
end;

//-----------------------------------------------------------------------------

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

