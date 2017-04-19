import sys
import os

os.chdir(sys.path[0])

from time import sleep
from iapIntf import CIapDev
from charDev import CCharDev, CCanCharDev

flashMap_F1 = [0x08000000, 0x08007800, 0x08008000]
# canCharDev = CCanCharDev((0x5006, 0x5005), (0x5006, 0x5005))
canCharDev = CCanCharDev((0x5004, 0x5005), (0x5004, 0x5005))
canCharDev.ioctl("readTimeout", 1)

canIapDev = CIapDev(canCharDev, flashMap_F1)
canIapDev.resetToBootloader()
#canIapDev.resetBoard()
ver = canIapDev.getBootLoaderVersion()
canIapDev.loadBin('.\\out\\test1.bin')
canIapDev.writeBootParam(CIapDev.byteBootParam_APP)
canIapDev.jumpToApp()

# canIapDev.resetBoard()
canCharDev.close()
os.system('pause')
sys.exit()





