#!/usr/bin/env python3

import platform
import sys
import os
import json

def generate_qrcode(app_path, output_file):
    try:
       import qrcode
    except:
       print("WARN : Fail to find qrcode module. please install qrcode module")
       return

    with open(os.path.join(app_path, "main", "onboarding_config.json")) as json_file:
        json_data = json.load(json_file)
        mnId = json_data["onboardingConfig"]["mnId"]
        setupId = json_data["onboardingConfig"]["setupId"]

    with open(os.path.join(app_path, "main", "device_info.json")) as json_file:
        json_data = json.load(json_file)
        serialNumber = json_data["deviceInfo"]["serialNumber"]

    if (mnId == "MNID") or (serialNumber == "serialNumber_here") :
        print("WARN : json file is default. fail to generate QR code")
        return

    url = 'https://qr.samsungiots.com/?m=' + mnId + '&s=' + setupId + '&r=' + serialNumber
    img = qrcode.make(url)
    img.save(output_file)

   
APP_PATH = sys.argv[1]
if len(sys.argv) > 2 :
    output_file = sys.argv[2]
else:
    output_file = 'qrcode.png'

    
generate_qrcode(APP_PATH, output_file)
