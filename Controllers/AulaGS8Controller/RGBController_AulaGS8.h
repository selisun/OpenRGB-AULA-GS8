/*---------------------------------------------------------*\
| RGBController_AulaGS8.h                                   |
|                                                           |
|   RGBController for AULA GS8 USB speaker                  |
|                                                           |
|   selisun                                     04 Nov 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "AulaGS8Controller.h"

class RGBController_AulaGS8 : public RGBController
{
public:
    RGBController_AulaGS8(AulaGS8Controller* controller_ptr);
    ~RGBController_AulaGS8();

    void        SetupZones();

    void        DeviceUpdateLEDs();
    void        DeviceUpdateZoneLEDs(int zone);
    void        DeviceUpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    AulaGS8Controller*  controller;
};
