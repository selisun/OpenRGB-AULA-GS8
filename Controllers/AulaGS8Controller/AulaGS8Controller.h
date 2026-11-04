/*---------------------------------------------------------*\
| AulaGS8Controller.h                                       |
|                                                           |
|   Driver for AULA GS8 USB speaker RGB lighting            |
|                                                           |
|   Protocol reverse engineered from USB captures of the    |
|   AULA "Shenguang" sync application                       |
|                                                           |
|   selisun（蓝胖纸）                           04 Nov 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <hidapi.h>
#include "RGBController.h"

/*---------------------------------------------------------*\
| Device identification                                     |
\*---------------------------------------------------------*/
#define AULA_GS8_VID                    0x3654
#define AULA_GS8_PID                    0x0517
#define AULA_GS8_INTERFACE              3

/*---------------------------------------------------------*\
| Protocol constants                                        |
|                                                           |
| Every command is a 16 byte HID output report:             |
|   byte 0     report ID (0xFF)                             |
|   byte 1     command                                      |
|   byte 2..3  command specific                             |
|   byte 4..15 payload                                      |
\*---------------------------------------------------------*/
#define AULA_GS8_REPORT_ID              0xFF
#define AULA_GS8_REPORT_SIZE            16

#define AULA_GS8_CMD_COMMIT             0x01
#define AULA_GS8_CMD_PREPARE            0x02
#define AULA_GS8_CMD_CONFIGURE          0x03
#define AULA_GS8_CMD_SET_COLORS         0x04

/*---------------------------------------------------------*\
| Lighting layout                                           |
|                                                           |
| 22 LEDs are written in 6 packets of 4 LEDs.  The last     |
| packet only carries 2 LEDs, the remaining slots are zero. |
\*---------------------------------------------------------*/
#define AULA_GS8_LED_COUNT              22
#define AULA_GS8_LEDS_PER_PACKET        4
#define AULA_GS8_PACKET_COUNT           6

class AulaGS8Controller
{
public:
    AulaGS8Controller(hid_device* dev_handle, const char* path);
    ~AulaGS8Controller();

    std::string     GetDeviceLocation();
    std::string     GetSerialString();

    void            SendColors(std::vector<RGBColor>& colors);

private:
    hid_device*     dev;
    std::string     location;

    void            SendPacket(unsigned char* packet);
};
