/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_SENSORS_H
#define ANDROID_SENSORS_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/input.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

__BEGIN_DECLS

/*****************************************************************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define ID_A_P  (0)
#define ID_A_S  (1)
#define ID_M    (2)
#define ID_O    (3)
#define ID_L    (4)
#define ID_P    (5)
#define ID_GY   (6)

/*****************************************************************************/

/*
 * The SENSORS Module
 */

/* the GP2A is a binary proximity sensor that triggers around 5 cm on
 * this hardware */
#define PROXIMITY_THRESHOLD_GP2A  5.0f

/*****************************************************************************/

#define AKM_DEVICE_NAME     "/dev/akm8973_aot"
/*
#define EVENT_TYPE_ACCEL_X          ABS_Y
#define EVENT_TYPE_ACCEL_Y          ABS_X
#define EVENT_TYPE_ACCEL_Z          ABS_Z
*/
#define EVENT_TYPE_ACCEL_STATUS     ABS_WHEEL

#define EVENT_TYPE_YAW              ABS_RX
#define EVENT_TYPE_PITCH            ABS_RY
#define EVENT_TYPE_ROLL             ABS_RZ
#define EVENT_TYPE_ORIENT_STATUS    ABS_RUDDER

/* For AK8975 */
#define EVENT_TYPE_MAGV_X           ABS_HAT0X
#define EVENT_TYPE_MAGV_Y           ABS_HAT0Y
#define EVENT_TYPE_MAGV_Z           ABS_BRAKE
#define EVENT_TYPE_MAGV_STATUS      ABS_GAS

#define EVENT_TYPE_PROXIMITY        ABS_DISTANCE
#define EVENT_TYPE_LIGHT            ABS_MISC

#define EVENT_TYPE_GYRO_X           REL_RX
#define EVENT_TYPE_GYRO_Y           REL_RY
#define EVENT_TYPE_GYRO_Z           REL_RZ 

/* For ADXL34X */
/*
#define EVENT_TYPE_ACCEL_X      REL_X
#define EVENT_TYPE_ACCEL_Y      REL_Y
#define EVENT_TYPE_ACCEL_Z      REL_Z
*/
#define EVENT_TYPE_ACCEL_X      ABS_X
#define EVENT_TYPE_ACCEL_Y      ABS_Y
#define EVENT_TYPE_ACCEL_Z      ABS_Z

#define EVENT_TYPE_ACCEL_FREE_FALL  0

#define EVENT_TYPE_ACCEL_ACTIVITY_CHANGED 0
/*
#define EVENT_TYPE_ACCEL_SINGLE_TAP_X     BTN_TOUCH
#define EVENT_TYPE_ACCEL_SINGLE_TAP_Y     BTN_TOUCH
#define EVENT_TYPE_ACCEL_SINGLE_TAP_Z     BTN_TOUCH
#define EVENT_TYPE_ACCEL_DOUBLE_TAP_X     BTN_TOUCH
#define EVENT_TYPE_ACCEL_DOUBLE_TAP_Y     BTN_TOUCH
#define EVENT_TYPE_ACCEL_DOUBLE_TAP_Z     BTN_TOUCH
*/
#define EVENT_TYPE_ACCEL_SINGLE_TAP       BTN_TOUCH
#define EVENT_TYPE_ACCEL_DOUBLE_TAP       BTN_TOUCH

// 720 LSG = 1G
#define LSG                         (720.0f)
#define NUMOFACCDATA                8

// conversion of acceleration data to SI units (m/s^2)
#define RANGE_A                     (2*GRAVITY_EARTH)
#define RESOLUTION_A                (RANGE_A/(256*NUMOFACCDATA))
#define CONVERT_A                   (GRAVITY_EARTH / LSG / NUMOFACCDATA)

/* conversion of magnetic data to uT units (1/5 * 0.3) */
#define CONVERT_M                   (0.06f)

/* conversion of orientation data to degree units (1/64) */
#define CONVERT_O                   (0.015625f)

// conversion of gyro data to SI units (radian/sec)
#define RANGE_GYRO                  (2000.0f*(float)M_PI/180.0f)
#define CONVERT_GYRO                ((70.0f / 1000.0f) * ((float)M_PI / 180.0f))
#define CONVERT_GYRO_X              (CONVERT_GYRO)
#define CONVERT_GYRO_Y              (CONVERT_GYRO)
#define CONVERT_GYRO_Z              (CONVERT_GYRO)

#define SENSOR_STATE_MASK           (0x7FFF)

/*****************************************************************************/

__END_DECLS

#endif  // ANDROID_SENSORS_H
