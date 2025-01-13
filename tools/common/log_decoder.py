#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os
import sys
import base64
import time
import uuid

default_log_file = "all_log_dump"
tz_offset_sec = int(time.mktime(time.localtime()) - time.mktime(time.gmtime()))
header_path = os.path.dirname(os.path.abspath(__file__)) + "/../../iot-core/src/include/"

log_version = 0
endian = ""
output_file = None


def get_timezone_string(timezone_sec):
    tz_min, tz_sec = divmod(abs(timezone_sec), 60)
    tz_hour, tz_min = divmod(tz_min, 60)
    if (timezone_sec >= 0):
        return "+" + format(tz_hour, "02d") + ":" + format(tz_min, "02d")
    else:
        return "-" + format(tz_hour, "02d") + ":" + format(tz_min, "02d")

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
    dip_version = 0
    os_name = ""
    os_version = ""
    bsp_name = ""
    bsp_version = ""
    dip_id = ""
    device_id = "********"
    mqtt_success_count = 0
    mqtt_try_count = 0
    log_time = 0
    def __init__(self, dumpState_line):
        self.stdk_version_code = int.from_bytes(dumpState_line[0:4], endian)
        self.clock_time = int.from_bytes(dumpState_line[4:8], endian)
        self.sequence_number = int.from_bytes(dumpState_line[8:12], endian)
        self.dip_version = int.from_bytes(dumpState_line[12:16], endian)
        self.os_name = dumpState_line[16:32].decode().replace("\x00", "")
        self.os_version = dumpState_line[32:48].decode().replace("\x00", "")
        self.bsp_name = dumpState_line[48:64].decode().replace("\x00", "")
        self.bsp_version = dumpState_line[64:80].decode().replace("\x00", "")
        self.firmware_version = dumpState_line[80:96].decode().replace("\x00", "")
        self.model_number = dumpState_line[96:112].decode().replace("\x00", "")
        self.manufacturer_name = dumpState_line[112:128].decode().replace("\x00", "")
        self.dip_id = dumpState_line[128:144]
        self.device_id = dumpState_line[144:152].decode().replace("\x00", "*")
        self.mqtt_success_count = int.from_bytes(dumpState_line[152:156], endian)
        self.mqtt_try_count = int.from_bytes(dumpState_line[156:160], endian)
        self.log_time = int.from_bytes(dumpState_line[160:164], endian)
    def printInfo(self):
        output = ""
        output += "stdk_version_code : " + hex(self.stdk_version_code) + "\n"
        output += "logtime : " + time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(self.log_time + tz_offset_sec)) + " (epoch : " + str(self.log_time) + ")\n"
        output += "uptime : " + str(self.clock_time) + "\n"
        output += "sequence_number : " + str(self.sequence_number) + "\n"
        output += "os_name : " + self.os_name.strip() + "\n"
        output += "os_version : " + self.os_version + "\n"
        output += "bsp_name : " + self.bsp_name + "\n"
        output += "bsp_version : " + self.bsp_version + "\n"
        output += "firmware_version : " + self.firmware_version + "\n"
        output += "model_number : " + self.model_number + "\n"
        output += "manufacturer_name : " + self.manufacturer_name + "\n"
        output += "DIP : " + str(uuid.UUID(bytes=self.dip_id)) + " (Ver:" + str(self.dip_version >> 16) + "." + str(self.dip_version & 0xffff) + ")\n"
        output += "DeviceID : " + self.device_id + "-****-****-****-************\n"
        output += "mqtt_success_count : " + str(self.mqtt_success_count) + "\n"
        output += "mqtt_try_count : " + str(self.mqtt_try_count) + "\n"
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
                comment = line[line.find("/*"):].strip().replace("/*", "").replace("*/","")
                return format(msg_enum, "42s") + " | " + comment
        return "unknown_id-/"+format(self.log_id, "04X")+"/"
    def get_arg_text(self):
        arg1_string = ""
        arg2_string = ""
        return arg1_string + format(self.arg1, "10d")+" (0x"+format(self.arg1 & (2**32-1), "08X") +")|" + \
               arg2_string + format(self.arg2, "10d")+" (0x"+format(self.arg2 & (2**32-1), "08X")+")"
    def get_time_text(self):
        return time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(self.time + tz_offset_sec))
    def printMessage(self):
        if (self.log_level == 1) : log_level_char = 'E'
        elif (self.log_level == 2): log_level_char = 'W'
        elif (self.log_level == 3): log_level_char = 'I'
        elif (self.log_level == 4): log_level_char = 'D'
        else: return
        output = log_level_char+"|"+self.get_time_text()+"|" + self.get_arg_text() + "| " + self.get_logid_text()
        print(output)
        output_file.write(output + "\n")

def printErrorAndExit():
    print("Error : need log file")
    print(" ex) ./log_decoder.py logfile")
    sys.exit()

if len(sys.argv) <= 1:
    printErrorAndExit()

if (not os.path.isfile(sys.argv[1])) and (sys.argv[1].startswith("CcH") or sys.argv[1].startswith("V9H")):
    print("Save input log to " + default_log_file)
    output_file = open(default_log_file, 'w')
    output_file.write(sys.argv[1])
    output_file.close()
    file_name = default_log_file
else:
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

if header.dumpState_size > 0:
    dumpState_line = log_file.read(header.dumpState_size)
    dumpState = dumpState_info(dumpState_line)
    dumpState.printInfo()
else:
    print("No dumpState info")
    output_file.write("No dumpState info\n")

timezone_info = "timezone : " + format(tz_offset_sec, "+d") + " (" + get_timezone_string(tz_offset_sec) + ")\n"
print(timezone_info)
output_file.write(timezone_info)

while True:
    message_line = log_file.read(16)
    if not message_line: break
    message = message_info(message_line)
    message.printMessage()

