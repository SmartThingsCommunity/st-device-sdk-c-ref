#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os
import sys
import base64
import time

header_path = os.path.dirname(os.path.abspath(__file__)) + "/../../iot-core/src/include/"
print(header_path)
log_version = 0
endian = ""
output_file = None

def printAndWrite(*args, **kwargs):
    print(*args, **kwargs)
    if output_file:
        output_file.write(*args, **kwargs)

class header_info:
    magicnumber = 0
    log_version = 0
    dumpState_size = 0
    def __init__(self, header_line):
        self.magicnumber = int.from_bytes(header_line[0:4], endian)
        self.log_version = int.from_bytes(header_line[4:8], endian)
        self.dumpState_size = int.from_bytes(header_line[8:12], endian)
    def checkMagicnumber(self):
        if not self.magicnumber == 0x57d1c109:
            print("Error: magicnumber is wrong: ", hex(self.magicnumber))
            sys.exit()
    def printInfo(self):
        output = ""
        output += "Log_version : " + hex(self.log_version) + "\n"
        output += "dumpState_size : " + str(self.dumpState_size) + "\n"
        print(output)
        output_file.write(output)

class dumpState_info:
    stdk_version_code = 0
    clock_time = 0
    sequence_number = 0
    os_name = ""
    os_version = ""
    bsp_name = ""
    bsp_version = ""
    def __init__(self, dumpState_line):
        self.stdk_version_code = int.from_bytes(dumpState_line[0:4], endian)
        self.clock_time = int.from_bytes(dumpState_line[4:8], endian)
        self.sequence_number = int.from_bytes(dumpState_line[8:12], endian)
        self.os_name = dumpState_line[16:32].decode().replace("\x00", "")
        self.os_version = dumpState_line[32:48].decode().replace("\x00", "")
        self.bsp_name = dumpState_line[48:64].decode().replace("\x00", "")
        self.bsp_version = dumpState_line[64:80].decode().replace("\x00", "")
    def printInfo(self):
        output = ""
        output += "stdk_version_code : " + hex(self.stdk_version_code) + "\n"
        output += "clock_time : " + str(self.clock_time) + "\n"
        output += "sequence_number : " + str(self.sequence_number) + "\n"
        output += "os_name : " + self.os_name.strip() + "\n"
        output += "os_version : " + self.os_version + "\n"
        output += "bsp_name : " + self.bsp_name + "\n"
        output += "bsp_version : " + self.bsp_version + "\n"
        print(output)
        output_file.write(output)

class message_info:
    log_level = 0
    log_id = 0
    time = 0
    arg1 = 0
    arg2 = 0
    def __init__(self, message_line):
        first_byte = int.from_bytes(message_line[0:4], endian)
        self.log_level = (first_byte & 0xf0000000) >> 28
        self.log_id = (first_byte & 0xffff)
        self.time = int.from_bytes(message_line[4:8], endian)
        self.arg1 = int.from_bytes(message_line[8:12], endian, signed=True)
        self.arg2 = int.from_bytes(message_line[12:16], endian, signed=True)
    def get_logid_text(self):
        for line in header_file_lines:
            if "0x"+format(self.log_id, "04x").lower() in line.lower() :
                msg_enum = line.split('=')[0].replace("IOT_DUMP_","").strip() #.replace("_","] ",1)
                comment = line[line.find("/*"):].replace("/*", ":").strip().replace("*/","")
                return msg_enum+" "+comment
        return "unknown_id-/"+format(self.log_id, "04X")+"/"
    def get_arg_text(self):
        arg1_string = ""
        arg2_string = ""
        return arg1_string + format(self.arg1, "10d")+" (0x"+format(self.arg1 & (2**32-1), "08X") +") | " + \
               arg2_string + format(self.arg2, "10d")+" (0x"+format(self.arg2 & (2**32-1), "08X")+")"
    def get_time_text(self):
        return time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(self.time))
    def printMessage(self):
        if (self.log_level == 1) : log_level_char = 'E'
        elif (self.log_level == 2): log_level_char = 'W'
        elif (self.log_level == 3): log_level_char = 'I'
        elif (self.log_level == 4): log_level_char = 'D'
        else: return
        output = log_level_char+" |"+self.get_time_text()+"| " + format(self.get_logid_text(), "50s") + " | " + self.get_arg_text()
        print(output)
        output_file.write(output + "\n")

def printErrorAndExit():
    print("Error : need log file")
    print(" ex) ./log_decoder.py logfile")
    sys.exit()

if len(sys.argv) <= 1:
    printErrorAndExit()

file_name = sys.argv[1]
input_file = open(file_name, 'rb')
if not input_file:
    printErrorAndExit()
magicnumber = input_file.read(4)

isLittleEndian = False
isEncoded = False

print("First 4 : " + str(magicnumber))
print("First 4(hex) : " + magicnumber.hex())
if (magicnumber.hex() == "43634852"):
    isEncoded = True
    endian = 'little'
elif (magicnumber.hex() == "56394842"):
    isEncoded = True
    endian = 'big'
elif (magicnumber.hex() == "09c1d157"):
    isEncoded = False
    endian = 'little'
elif (magicnumber.hex() == "57d1c109"):
    isEncoded = False
    endian = 'big'
else:
    print("unknown magic number!")
    sys.exit()

if isEncoded:
    binary_file = open(file_name+"_d", 'wb')
    input_file.seek(0)
    binary_file.write(base64.b64decode(input_file.read()))
    binary_file.close()
    log_file_path = file_name+"_d"
else:
    log_file_path = file_name
input_file.close()

log_file = open(log_file_path, 'rb')
output_file = open(file_name + "_output", 'w')

header_file = open(header_path+"iot_dump_log.h", 'r')
header_file_lines = header_file.readlines()
header_file.close()

header_line = log_file.read(16)
header = header_info(header_line)
header.checkMagicnumber()

print("--------------------")
header.printInfo()
log_version = header.log_version

dumpState_line = log_file.read(header.dumpState_size)
dumpState = dumpState_info(dumpState_line)
dumpState.printInfo()

while True:
    message_line = log_file.read(16)
    if not message_line: break
    message = message_info(message_line)
    message.printMessage()

