/*---------------------------------------------------------*\
| AulaGS8ControllerDetect.cpp                               |
|                                                           |
|   Detector for AULA GS8 USB speaker                       |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <hidapi.h>
#include <algorithm>
#include <cctype>
#include <string>
#include "AulaGS8Controller.h"
#include "DetectionManager.h"
#include "RGBController_AulaGS8.h"

DetectedControllers DetectAulaGS8Controllers(hid_device_info* info, const std::string&)
{
    DetectedControllers detected_controllers;
    hid_device*         dev;

    /*---------------------------------------------------------*\
    | Interface 3 exposes two consumer control collections. Only |
    | COL02 carries the 32 byte output report used for lighting, |
    | COL01 is the media key collection and cannot be written.   |
    \*---------------------------------------------------------*/
#ifdef _WIN32
    /*---------------------------------------------------------*\
    | hidapi returns the Windows device path with mixed case,    |
    | i.e. "...&MI_03&Col02#...", so compare case-insensitively. |
    \*---------------------------------------------------------*/
    std::string path = std::string(info->path);

    std::transform(path.begin(), path.end(), path.begin(),
                   [](unsigned char c) { return(static_cast<char>(std::tolower(c))); });

    if(path.find("col02") == std::string::npos)
    {
        return(detected_controllers);
    }
#endif

    dev = hid_open_path(info->path);

    if(dev)
    {
        AulaGS8Controller*      controller     = new AulaGS8Controller(dev, info->path);
        RGBController_AulaGS8*  rgb_controller = new RGBController_AulaGS8(controller);

        detected_controllers.push_back(rgb_controller);
    }

    return(detected_controllers);
}

REGISTER_HID_DETECTOR_I("AULA GS8", DetectAulaGS8Controllers, AULA_GS8_VID, AULA_GS8_PID, AULA_GS8_INTERFACE);
