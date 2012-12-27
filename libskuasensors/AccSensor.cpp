/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <math.h>
#include <cutils/log.h>

#include "AccSensor.h"

/*****************************************************************************/

#define LIS3DH_SYSFS_PATH           "/sys/class/input"
#define LIS3DH_SYSFS_ATTR_ENABLE    "enable"
#define LIS3DH_SYSFS_ATTR_DISABLE   "disable"
#define LIS3DH_SYSFS_ATTR_CALIBRATE "calibrate"
#define LIS3DH_SYSFS_ATTR_RATE      "rate"
#define LIS3DH_DATA_NAME            "kxtik"
#define LIS3DH_SYSFS_ATTR_DELAY     "poll"

#define MAKE_SYSFS_ATTR_PATH(attr) (LIS3DH_SYSFS_PATH "/" attr)

#define LIS3DH_GRAVITY_EARTH GRAVITY_EARTH
#define LIS3DH_GRAVITY_UNIT  3.90625f /* 1g/256LSB -> 3.90625mg/1LSB */
#define LIS3DH_UNIT_CONVERSION(value) (float)((float)((short)value) * (GRAVITY_EARTH / 1024) )

#define LIS3DH_ENABLE_BITMASK_P 0x01 /* primary logical driver. */
#define LIS3DH_ENABLE_BITMASK_S 0x02 /* secondary logical driver. */

/*****************************************************************************/
LIS3DHSensor::LIS3DHSensor()
: SensorBase(NULL, LIS3DH_DATA_NAME),
      mEnabled(0),
      mPendingMask(0),
      mInputReader(32),
      mDelay(0)
{
    memset(&mPendingEvent, 0, sizeof(mPendingEvent));
	memset(mClassPath, '\0', sizeof(mClassPath));
	
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor  = ID_A_P;
    mPendingEvent.type    = SENSOR_TYPE_ACCELEROMETER;
    mPendingEvent.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;

	if(sensor_get_class_path(mClassPath))
	{
		LOGE("Can`t find the LIS3DH sensor!");
	}
    // read the actual value of all sensors if they're enabled already
/*
    struct input_absinfo absinfo;
    if (is_sensor_enabled())  {
        mEnabled |= LIS3DH_ENABLE_BITMASK_P;
        if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_X), &absinfo)) {
            mPendingEvent.acceleration.x = LIS3DH_UNIT_CONVERSION(absinfo.value);
        }
        if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Y), &absinfo)) {
            mPendingEvent.acceleration.y = LIS3DH_UNIT_CONVERSION(absinfo.value);
        }
        if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Z), &absinfo)) {
            mPendingEvent.acceleration.z = LIS3DH_UNIT_CONVERSION(absinfo.value);
        }
	}
*/
}

LIS3DHSensor::~LIS3DHSensor()
{
}

int LIS3DHSensor::enable(int32_t handle, int en)
{
    uint32_t mask;
	int err = 0;
    uint32_t newState  = en;

    if (mEnabled != newState) {
        if (newState && !mEnabled)
            err = enable_sensor();
        else if (!newState)
            err = disable_sensor();
		LOGI("change G sensor state \"%d -> %d\"", mEnabled, newState);
        LOGE_IF(err, "Could not change sensor state \"%d -> %d\" (%s).", mEnabled, newState, strerror(-err));
        if (!err) {
            mEnabled = newState;
            update_delay();
        }
    }
    return err;
}

int LIS3DHSensor::setDelay(int32_t handle, int64_t ns)
{
    if (ns < 0)
        return -EINVAL;

    mDelay = ns;
    return update_delay();
}

int LIS3DHSensor::update_delay()
{
    if (mEnabled) {
        return set_delay(mDelay);
    }
    else
	    return 0;
}

int LIS3DHSensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if ((type == EV_ABS) || (type == EV_REL) || (type == EV_KEY)) {
            processEvent(event->code, event->value);
            mInputReader.next();
        } else if (type == EV_SYN) {
            int64_t time = timevalToNano(event->time);
			if (mPendingMask) {
				mPendingMask = 0;
				mPendingEvent.timestamp = time;
				if (mEnabled) {
					*data++ = mPendingEvent;
					count--;
					numEventReceived++;
				}
			}
            if (!mPendingMask) {
                mInputReader.next();
            }
        } else {
            LOGE("LIS3DHSensor: unknown event (type=%d, code=%d)",
                    type, event->code);
            mInputReader.next();
        }
    }

    return numEventReceived;
}

void LIS3DHSensor::processEvent(int code, int value)
{

    switch (code) {
        case ABS_X:
            mPendingMask = 1;
            mPendingEvent.acceleration.x = LIS3DH_UNIT_CONVERSION(value);
            break;
        case ABS_Y:
            mPendingMask = 1;
            mPendingEvent.acceleration.y = LIS3DH_UNIT_CONVERSION(value);
            break;
        case ABS_Z:
            mPendingMask = 1;
            mPendingEvent.acceleration.z = LIS3DH_UNIT_CONVERSION(value);
            break;
		default:
			/* TODO: implement if needed. */
			break;
    }
	
//	LOGE("the acc code is %d value is %d" ,code, value);
//	LOGE("the acc data is x= %f , y = %f , z = %f",mPendingEvent.acceleration.x,
//							mPendingEvent.acceleration.y,mPendingEvent.acceleration.z);
}

int LIS3DHSensor::writeDisable(int isDisable) {
	char attr[PATH_MAX] = {'\0'};
	if(mClassPath[0] == '\0')
		return -1;

	strcpy(attr, mClassPath);
	strcat(attr,"/");
	strcat(attr,LIS3DH_SYSFS_ATTR_ENABLE);

	int fd = open(attr, O_RDWR);
	if (0 > fd) {
		LOGE("Could not open (write-only) SysFs attribute \"%s\" (%s).", attr, strerror(errno));
		return -errno;
	}

	char buf[2];

	if (isDisable) {
		buf[0] = '0';
	} else {
		buf[0] = '1';
	}
	buf[1] = '\0';

	int err = 0;
	err = write(fd, buf, sizeof(buf));

	if (0 > err) {
		err = -errno;
		LOGE("Could not write SysFs attribute \"%s\" (%s).", attr, strerror(errno));
	} else {
		err = 0;
	}

	close(fd);

	return err;
}

int LIS3DHSensor::writeDelay(int64_t ns) {
	char attr[PATH_MAX] = {'\0'};
	if(mClassPath[0] == '\0')
		return -1;

	strcpy(attr, mClassPath);
	strcat(attr,"/");
	strcat(attr,LIS3DH_SYSFS_ATTR_DELAY);

	int fd = open(attr, O_RDONLY);
	if (0 > fd) {
		LOGE("Could not open (write-only) SysFs attribute \"%s\" (%s).", attr, strerror(errno));
		return -errno;
	}
	if (ns > 10240000000LL) {
		ns = 10240000000LL; /* maximum delay in nano second. */
	}
	if (ns < 312500LL) {
		ns = 312500LL; /* minimum delay in nano second. */
	}

    char buf[80];
    sprintf(buf, "%lld", ns);
    write(fd, buf, strlen(buf)+1);
    close(fd);
    return 0;

}

int LIS3DHSensor::enable_sensor() {
	return writeDisable(0);
}

int LIS3DHSensor::disable_sensor() {
	return writeDisable(1);
}

int LIS3DHSensor::set_delay(int64_t ns) {
	return writeDelay(ns);
}

bool LIS3DHSensor::isEnabled(int32_t handle) {
	return (0 != (mEnabled)) ? true : false;
}

int LIS3DHSensor::sensor_get_class_path(char *class_path)
{
	char *dirname = LIS3DH_SYSFS_PATH;
	char buf[256];
	int res;
	DIR *dir;
	struct dirent *de;
	int fd = -1;
	int found = 0;

	dir = opendir(dirname);
	if (dir == NULL)
		return -1;

	while((de = readdir(dir))) {
		if (strncmp(de->d_name, "input", strlen("input")) != 0) {
		    continue;
        	}

		sprintf(class_path, "%s/%s/device", dirname, de->d_name);
		snprintf(buf, sizeof(buf), "%s/name", class_path);

		fd = open(buf, O_RDONLY);
		if (fd < 0) {
		    continue;
		}
		if ((res = read(fd, buf, sizeof(buf))) < 0) {
		    close(fd);
		    continue;
		}
		buf[res - 1] = '\0';
		if (strcmp(buf, LIS3DH_DATA_NAME) == 0) {
		    found = 1;
		    close(fd);
		    break;
		}

		close(fd);
		fd = -1;
	}
	closedir(dir);
	LOGE("the G sensor dir is %s",class_path);

	if (found) {
		return 0;
	}else {
		*class_path = '\0';
		return -1;
	}
}

