import serial
from udo_comm import *

class TCommHandlerUdoSl(TUdoCommHandler):
    def __init__(self):
        super().__init__()
        self.protocol = UCP_SERIAL
        self.comm = serial.Serial()

    def Open(self):
        raise EUdoAbort(UDOERR_APPLICATION, "Open: Invalid comm. handler")

    def Close(self):
        pass

    def Opened(self) -> bool:
        return False

    def UdoRead(self, index : int, offset : int, maxdatalen : int) -> bytearray:
        raise EUdoAbort(UDOERR_APPLICATION, "Open: Invalid comm. handler")

    def UdoWrite(self, index : int, offset : int, avalue : bytearray):
        raise EUdoAbort(UDOERR_APPLICATION, "Open: Invalid comm. handler")


udosl_commh = TCommHandlerUdoSl()