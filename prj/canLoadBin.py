import ctypes
import sys
import time
import io
import os
sys.stdout = io.TextIOWrapper(sys.stdout.buffer,encoding='utf8')
ACK = b'\x79'
NACK = b'\x1F'
mcuUpwardCanId = 0x5004
mcuDownwardCanId = 0x5005

oriDownloadAddress = 0x08008000
byteStart = bytearray(b'\x31\xCE')
byteReset = bytearray(b'\x09\x00\x00\x00\xff\xff\xff\xff')
byteEnterBL = bytearray(b'\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f')
SEND_DATA_LEN = 256

DEV_TYPE = 4
DEV_IDX = 0
DEV_CH = 0
# typedef struct _INIT_CONFIG {
# DWORD AccCode;
# DWORD AccMask;
# DWORD Reserved;
# UCHAR Filter;
# UCHAR Timing0;
# UCHAR Timing1;
# UCHAR Mode;
# } VCI_INIT_CONFIG, *PVCI_INIT_CONFIG;
from ctypes import *

canInitArrayType = c_byte*16
canInitArray = canInitArrayType()
# canInitArray[0] = 0x08
# canInitArray[1] = 0x00
# canInitArray[2] = 0x00
# canInitArray[3] = 0x80

# canInitArray[4] = 0xFF
# canInitArray[5] = 0xFF
# canInitArray[6] = 0xFF
# canInitArray[7] = 0xFF

canInitArray[0] = 0x20
canInitArray[1] = 0x80
canInitArray[2] = 0x02
canInitArray[3] = 0x00

canInitArray[4] = 0x00
canInitArray[5] = 0x00
canInitArray[6] = 0x00
canInitArray[7] = 0x00

canInitArray[8] = 0x00
canInitArray[9] = 0x00
canInitArray[10] = 0x00
canInitArray[11] = 0x00

canInitArray[12] = 0x00
canInitArray[13] = 0x01
canInitArray[14] = 0x1c
canInitArray[15] = 0x00


# typedef struct _VCI_CAN_OBJ {
# UINT ID;
# UINT TimeStamp;
# BYTE TimeFlag;
# BYTE SendType;
# BYTE RemoteFlag;
# BYTE ExternFlag;
# BYTE DataLen;
# BYTE Data[8];
# BYTE Reserved[3];
# }VCI_CAN_OBJ, *PVCI_CAN_OBJ;

class _VCI_CAN_OBJ(Structure):
    _fields_ = [("ID", c_uint32),
                ("TimeStamp", c_uint32),
                ("TimeFlag", c_byte),
                ("SendType", c_byte),
                ("RemoteFlag", c_byte),
                ("ExternFlag", c_byte),
                ("DataLen", c_byte),
                ("Data", c_byte * 8),
                ("Reserved", c_byte * 3)]

BUFF_LEN = 512
_VCI_CAN_OBJ_ARRAY = _VCI_CAN_OBJ*BUFF_LEN
rxbuff = _VCI_CAN_OBJ_ARRAY()

send_frame = _VCI_CAN_OBJ()
send_frame.ID = mcuDownwardCanId
send_frame.SendType = 1
send_frame.RemoteFlag = 0
send_frame.ExternFlag = 1
send_frame.DataLen = 8

rx_que = []

def sendto_stm32(byte_msg):
    count = 0
    global rx_que
    rx_que.clear()
    dll.VCI_ClearBuffer(DEV_TYPE, DEV_IDX, DEV_CH)
    for byte in byte_msg:
        send_frame.Data[count] = byte
        count = count + 1
        if (count == 8):
            count = 0
            send_frame.DataLen = 8
            dll.VCI_Transmit(DEV_TYPE, DEV_IDX, DEV_CH, byref(send_frame),1)
    if(count > 0):
        send_frame.DataLen = count
        dll.VCI_Transmit(DEV_TYPE, DEV_IDX, DEV_CH, byref(send_frame),1)

def send_enterBlByte():
    send_frame.DataLen = 8
    for i in range(0,8):
        send_frame.Data[i] = 0x7F
    dll.VCI_Transmit(DEV_TYPE, DEV_IDX, DEV_CH, byref(send_frame),1)

def receivefrom_stm32(len):
    timeup = 0
    global rx_que
    while rx_que.__len__() < len:
        frame_num = dll.VCI_Receive(DEV_TYPE, DEV_IDX, DEV_CH, rxbuff, BUFF_LEN, 0)
        if(frame_num > 0):
            for i in range(0, frame_num):
                if(rxbuff[i].ID != mcuUpwardCanId):
                    continue
                for j in range(0,rxbuff[i].DataLen):
                    rx_que += bytes([rxbuff[i].Data[j]])
        elif(timeup > 50):
            print('time out')
            return b''
        else:
            time.sleep(0.01)
            timeup = timeup + 1
        sys.stdout.flush()
    
    ret = rx_que[0:len]
    rx_que[0:len] = []
    
    return ret

def confirm_ack():
    stmback = receivefrom_stm32(1)
    if stmback.__len__() < 1:
        return False
    elif stmback[0] != ACK[0]:
        print('not ack:'+ str(stmback))
        return False
    else:
        return True

def getXor(val):
    xor = 0
    for byte in val:
        xor ^= byte
    return xor

def getBytesFromUint32(val):
    val0 = val & 0xFF
    val1 = (val & 0xFF00) >> 8
    val2 = (val & 0xFF0000) >> 16
    val3 = (val & 0xFF000000) >> 24
    return bytearray([val3,val2,val1,val0])

if len(sys.argv) < 2:
    print('no binary file specified.')
    sys.exit()

fname = sys.argv[1]

#get extension name
tail = fname[-4:]
if tail != ".bin":
    print("the extension is not .bin")
    os.system('pause')
    sys.exit()

# get file, read only, binary
f = open(fname,'rb')
data = f.read()

print('start')
sys.stdout.flush()
dll = ctypes.windll.LoadLibrary('ControlCAN.dll')
if(dll.VCI_OpenDevice(DEV_TYPE, DEV_IDX, DEV_CH) == 1):
    print('UsbCanTool Open Success')
else:
    print('UsbCanTool Open falid')
    sys.stdout.flush()
    os.system('pause')
    quit()

if(dll.VCI_InitCAN(DEV_TYPE, DEV_IDX, DEV_CH, byref(canInitArray)) == 1):
    print('device init ok!')
else:
    print('device init failed...')
    sys.stdout.flush()
    os.system('pause')
    quit()

if(dll.VCI_StartCAN(DEV_TYPE, DEV_IDX, DEV_CH) == 1):
    print('Channel start ok!')
else:
    print('Channel start failed...')
    sys.stdout.flush()
    os.system('pause')
    quit()

sys.stdout.flush()

isInApp = True

sendto_stm32(byteReset)
stmback = receivefrom_stm32(1)
if(stmback[0] == NACK[0]):
    print("already in bootloader")
    isInApp = False
elif (stmback[0] == byteReset[0]):
    print("jump from app")
else:
    print("don't know where it is, get", stmback[0])
# if(dll.VCI_Receive(DEV_TYPE, DEV_IDX, DEV_CH, rxbuff, BUFF_LEN, 0) > 0):
#     if(rxbuff[0].Data[0] == 0x1F):
#         time.sleep(0.5)
#         dll.VCI_ClearBuffer(DEV_TYPE, DEV_IDX, DEV_CH)
#         print("already in bootloader")
#         isInApp = False
sys.stdout.flush()
while(isInApp):
    send_enterBlByte()
    time.sleep(0.02)
    if(dll.VCI_Receive(DEV_TYPE, DEV_IDX, DEV_CH, rxbuff, BUFF_LEN, 0) > 0):
        if(rxbuff[0].Data[0] == NACK[0]):
            print('jump success')
            dll.VCI_ClearBuffer(DEV_TYPE, DEV_IDX, DEV_CH)
            break
        else:
            print('error , get', rxbuff[0].Data[0])
            dll.VCI_ClearBuffer(DEV_TYPE, DEV_IDX, DEV_CH)
        sys.stdout.flush()

time.sleep(0.1)

i = 0
length = len(data)
while i < length:
    nowDownloadAddress = oriDownloadAddress + i
    print("write address 0x%X" % nowDownloadAddress)
    sys.stdout.flush()
    j = i + SEND_DATA_LEN
    if j > length:
        j = length
    slip = data[i:j]
    slipLen = j - i
    slipArray = bytearray(slip)
    slipArray.insert(0,slipLen - 1)
    slipArray.append(getXor(slipArray))

    # send head
    sendto_stm32(byteStart)
    if confirm_ack() != True:
        continue

    #send address
    byteAddress = getBytesFromUint32(nowDownloadAddress)
    byteAddress.append(getXor(byteAddress))
    sendto_stm32(byteAddress)
    if confirm_ack() != True:
        continue

    #send data
    sendto_stm32(slipArray)
    if confirm_ack() != True:
        continue

    i = j

time.sleep(0.1)
print('write boot param')
while True:
    sendto_stm32(b'\x31\xce')
    if confirm_ack() == False:
        print('WM cmd NACK')
        continue
    sendto_stm32(b'\x08\x00\x78\x00\x70')
    if confirm_ack() == False:
        print('WM address NACK')
        continue
    sendto_stm32(b'\x03\xaa\xaa\x55\x55\x03')
    if confirm_ack() == False:
        print('WM data NACK')
        continue
    break
    sys.stdout.flush()

print('jump to app')

while True:
    sendto_stm32(b'\x21\xde')
    if confirm_ack() == False:
        print('jump cmd NACK')
        continue
    sendto_stm32(b'\x08\x00\x80\x00\x88')
    
    if confirm_ack() == False:
        print('jump addr NACK')
        continue
    break
time.sleep(0.1)
dll.VCI_CloseDevice(DEV_TYPE, 0)
print('UsbCanTool Close')
sys.stdout.flush()
os.system('pause')
