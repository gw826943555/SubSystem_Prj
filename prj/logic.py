import sys
import os
os.chdir(sys.path[0])

from time import sleep
from iapIntf import CIapDev
from charDev import CCharDev, CUdpCharDev

flashMap_F4 = [0x08000000, 0x0800c000, 0x08010000]
udpCharDev = CUdpCharDev(('192.168.192.4',5003), ('192.168.192.4',5003))
# udpCharDev = CUdpCharDev(('127.0.0.1', 9999),('127.0.0.1', 9999))
udpIapDev = CIapDev(udpCharDev, flashMap_F4)

udpIapDev.jumpToBootloader()
udpIapDev.getBootLoaderVersion()
udpIapDev.loadBin('../Output/Project.bin')
udpIapDev.writeBootParam(CIapDev.byteBootParam_APP)
udpIapDev.jumpToApp()
os.system('pause')
sys.exit()





