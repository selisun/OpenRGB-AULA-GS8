/*---------------------------------------------------------*\
| AulaGS8Controller.cpp                                     |
|                                                           |
|   Driver for AULA GS8 USB speaker RGB lighting            |
|                                                           |
|   selisun（蓝胖纸）                           04 Nov 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <cstring>
#include "AulaGS8Controller.h"
#include "StringUtils.h"

AulaGS8Controller::AulaGS8Controller(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;
}

AulaGS8Controller::~AulaGS8Controller()
{
    if(dev != nullptr)
    {
        hid_close(dev);
    }
}

std::string AulaGS8Controller::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string AulaGS8Controller::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    return(StringUtils::wstring_to_string(serial_string));
}

void AulaGS8Controller::SendPacket(unsigned char* packet)
{
    if(dev == nullptr)
    {
        return;
    }

    hid_write(dev, packet, AULA_GS8_REPORT_SIZE);
}

void AulaGS8Controller::SendColors(std::vector<RGBColor>& colors)
{
    unsigned char packet[AULA_GS8_REPORT_SIZE];

    /*---------------------------------------------------------*\
    | Prepare transaction                                       |
    |   0xFF, 0x02, 0x00 ... 0x00                               |
    \*---------------------------------------------------------*/
    memset(packet, 0, AULA_GS8_REPORT_SIZE);

    packet[0]   = AULA_GS8_REPORT_ID;
    packet[1]   = AULA_GS8_CMD_PREPARE;

    SendPacket(packet);

    /*---------------------------------------------------------*\
    | Configure transaction                                     |
    |   0xFF, 0x03, 0x04, <LED count>, 0x00 ...                 |
    \*---------------------------------------------------------*/
    memset(packet, 0, AULA_GS8_REPORT_SIZE);

    packet[0]   = AULA_GS8_REPORT_ID;
    packet[1]   = AULA_GS8_CMD_CONFIGURE;
    packet[2]   = 0x04;
    packet[3]   = (unsigned char)AULA_GS8_LED_COUNT;

    SendPacket(packet);

    /*---------------------------------------------------------*\
    | Color transaction                                         |
    |   0xFF, 0x04, <offset>, 0x00, then four RGB triplets      |
    \*---------------------------------------------------------*/
    for(unsigned int packet_idx = 0; packet_idx < AULA_GS8_PACKET_COUNT; packet_idx++)
    {
        memset(packet, 0, AULA_GS8_REPORT_SIZE);

        packet[0]   = AULA_GS8_REPORT_ID;
        packet[1]   = AULA_GS8_CMD_SET_COLORS;
        packet[2]   = (unsigned char)(packet_idx * AULA_GS8_LEDS_PER_PACKET);
        packet[3]   = 0x00;

        for(unsigned int led_idx = 0; led_idx < AULA_GS8_LEDS_PER_PACKET; led_idx++)
        {
            unsigned int color_idx = (packet_idx * AULA_GS8_LEDS_PER_PACKET) + led_idx;

            if(color_idx < colors.size())
            {
                packet[4 + (led_idx * 3) + 0] = (unsigned char)RGBGetRValue(colors[color_idx]);
                packet[4 + (led_idx * 3) + 1] = (unsigned char)RGBGetGValue(colors[color_idx]);
                packet[4 + (led_idx * 3) + 2] = (unsigned char)RGBGetBValue(colors[color_idx]);
            }
        }

        SendPacket(packet);
    }

    /*---------------------------------------------------------*\
    | Commit transaction                                        |
    |   0xFF, 0x01, 0x00 ... 0x00                               |
    \*---------------------------------------------------------*/
    memset(packet, 0, AULA_GS8_REPORT_SIZE);

    packet[0]   = AULA_GS8_REPORT_ID;
    packet[1]   = AULA_GS8_CMD_COMMIT;

    SendPacket(packet);
}
