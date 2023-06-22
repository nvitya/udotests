from udo_comm import *
from commh_udoip import udoip_commh
from commh_udosl import udosl_commh

#udoip_commh.ipaddrstr = '192.168.0.76:1221'  # this was an ESP32 UDO test device
#udoip_commh.ipaddrstr = '127.0.0.1:1221'  # pc udo slave test
#udocomm.SetHandler(udoip_commh)

udosl_commh.devstr = '/dev/ttyACM0'
udocomm.SetHandler(udosl_commh)

udocomm.Open()

print('Reading test object:')
i = udocomm.ReadU32(0, 0)
print(f'  = {i:8X}')

print('Reading test object again:')
i = udocomm.ReadU32(0, 0)
print(f'  = {i:8X}')

print('Reading max chunk length:')
i = udocomm.ReadU32(1, 0)
print(f'  = {i}')

print('testing invalid object read:')
try:
    i = udocomm.ReadU32(9999, 0)
    print(f'  = {i}')
except Exception as e:
    print('Exception: ', str(e))

print('reading blob...')
bd = udocomm.ReadBlob(2, 0, 2048)
print('result length: ', len(bd))
#print(bd)

print('test finished.')
