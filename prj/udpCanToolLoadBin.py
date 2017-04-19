import sys
import os
os.chdir(sys.path[0])

from time import sleep
from iapIntf import CIapDev
from charDev import CCharDev, CUdpCharDev

flashMap_F4 = [0x08000000, 0x0800c000, 0x08040000]
udpCharDev = CUdpCharDev(('192.168.192.4',5003), ('192.168.192.4',5003))
udpIapDev = CIapDev(udpCharDev, flashMap_F4)

# udpIapDev.jumpToBootloader()
udpIapDev.getBootLoaderVersion()
udpIapDev.jumpToAddress(0x08040000)

# udpIapDev.jumpToBootloader()
udpIapDev.loadBin('../../SubSystem_Prj/prj/out/test1.bin', 0x08008000)
udpIapDev.writeBootParam(CIapDev.byteBootParam_BL, 0x08007800)
udpIapDev.jumpToAddress(0x08008000)
os.system('pause')
sys.exit()





