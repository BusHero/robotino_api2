#!/bin/bash

srec_cat ../usb_bootloader/Memory.hex -intel keil/firmware.hex -intel -o ../combined.hex -intel
