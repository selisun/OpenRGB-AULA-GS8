/*---------------------------------------------------------*\
| RGBController_AulaGS8.cpp                                 |
|                                                           |
|   RGBController for AULA GS8 USB speaker                  |
|                                                           |
|   selisun（蓝胖纸）                           04 Nov 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "RGBController_AulaGS8.h"

/**------------------------------------------------------------------*\
    @name AULA GS8
    @category Speaker
    @type USB
    @save :x:
    @direct :white_check_mark:
    @effects :x:
    @detectors DetectAulaGS8Controllers
    @comment The AULA GS8 is a USB gaming soundbar with a 22 LED light
    bar. Lighting is controlled through 16 byte HID output reports on
    interface 3. Only direct per-LED color is supported; the dynamic
    effects built into the speaker are driven by its own firmware and
    cannot be selected from the host.
\*-------------------------------------------------------------------*/

RGBController_AulaGS8::RGBController_AulaGS8(AulaGS8Controller* controller_ptr)
{
    controller  = controller_ptr;

    name        = "AULA GS8";
    vendor      = "AULA";
    type        = DEVICE_TYPE_SPEAKER;
    description = "AULA GS8 USB Gaming Speaker";
    location    = controller->GetDeviceLocation();
    serial      = controller->GetSerialString();

    mode Direct;
    Direct.name         = "Direct";
    Direct.value        = 0;
    Direct.flags        = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode   = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_AulaGS8::~RGBController_AulaGS8()
{
    Shutdown();

    delete controller;
}

void RGBController_AulaGS8::SetupZones()
{
    zone light_bar;
    light_bar.name          = "Light Bar";
    light_bar.type          = ZONE_TYPE_LINEAR;
    light_bar.leds_min      = AULA_GS8_LED_COUNT;
    light_bar.leds_max      = AULA_GS8_LED_COUNT;
    light_bar.leds_count    = AULA_GS8_LED_COUNT;
    zones.push_back(light_bar);

    for(unsigned int led_idx = 0; led_idx < AULA_GS8_LED_COUNT; led_idx++)
    {
        led new_led;

        new_led.name    = "LED " + std::to_string(led_idx + 1);
        new_led.value   = led_idx;

        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_AulaGS8::DeviceUpdateLEDs()
{
    controller->SendColors(colors);
}

void RGBController_AulaGS8::DeviceUpdateZoneLEDs(int)
{
    /*---------------------------------------------------------*\
    | The whole light bar is rewritten on every update, so a    |
    | single zone update is the same as a full update.          |
    \*---------------------------------------------------------*/
    DeviceUpdateLEDs();
}

void RGBController_AulaGS8::DeviceUpdateSingleLED(int)
{
    DeviceUpdateLEDs();
}

void RGBController_AulaGS8::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
