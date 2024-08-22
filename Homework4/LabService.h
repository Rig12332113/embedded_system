/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_BLE_LAB_SERVICE_H__
#define MBED_BLE_LAB_SERVICE_H__

#include "GattCharacteristic.h"
#include "ble/BLE.h"

#if BLE_FEATURE_GATT_SERVER

/**
 * BLE Heart Rate Service.
 *
 * @par purpose
 *
 * Fitness applications use the heart rate service to expose the heart
 * beat per minute measured by a heart rate sensor.
 *
 * Clients can read the intended location of the sensor and the last heart rate
 * value measured. Additionally, clients can subscribe to server initiated
 * updates of the heart rate value measured by the sensor. The service delivers
 * these updates to the subscribed client in a notification packet.
 *
 * The subscription mechanism is useful to save power; it avoids unecessary data
 * traffic between the client and the server, which may be induced by polling the
 * value of the heart rate measurement characteristic.
 *
 * @par usage
 *
 * When this class is instantiated, it adds a heart rate service in the GattServer.
 * The service contains the location of the sensor and the initial value measured
 * by the sensor.
 *
 * Application code can invoke updateHeartRate() when a new heart rate measurement
 * is acquired; this function updates the value of the heart rate measurement
 * characteristic and notifies the new value to subscribed clients.
 *
 * @note You can find specification of the heart rate service here:
 * https://www.bluetooth.com/specifications/gatt
 *
 * @attention The service does not expose information related to the sensor
 * contact, the accumulated energy expanded or the interbeat intervals.
 *
 * @attention The heart rate profile limits the number of instantiations of the
 * heart rate services to one.
 */
class LabService{
public:
    /**
     * Intended location of the heart rate sensor.
     */
    enum BodySensorLocation {
        /**
         * Other location.
         */
        LOCATION_OTHER = 0,

        /**
         * Chest.
         */
        LOCATION_CHEST = 1,

        /**
         * Wrist.
         */
        LOCATION_WRIST = 2,

        /**
         * Finger.
         */
        LOCATION_FINGER,

        /**
         * Hand.
         */
        LOCATION_HAND,

        /**
         * Earlobe.
         */
        LOCATION_EAR_LOBE,

        /**
         * Foot.
         */
        LOCATION_FOOT,
    };

public:
    /**
     * Construct and initialize a heart rate service.
     *
     * The construction process adds a GATT heart rate service in @p _ble
     * GattServer, sets the value of the heart rate measurement characteristic
     * to @p hrmCounter and the value of the body sensor location characteristic
     * to @p location.
     *
     * @param[in] _ble BLE device that hosts the heart rate service.
     * @param[in] hrmCounter Heart beats per minute measured by the heart rate
     * sensor.
     * @param[in] location Intended location of the heart rate sensor.
     */
    LabService(BLE &_ble, uint16_t hrmCounter) :
        ble(_ble),
        valueBytesx(hrmCounter),
        valueBytesy(hrmCounter),
        valueBytesz(hrmCounter),
        // hrmRate(
        //     // GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR,
        //     GattCharacteristic::BLE_GATT_UNIT_MAGNETIC_FIELD_STRENGTH_AMPERE_PER_METRE,
        //     valueBytes.getPointer(),
        //     valueBytes.getNumValueBytes(),
        //     HeartRateValueBytes::MAX_VALUE_BYTES,
        //     GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        // )
        magx(
            // GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR,
            GattCharacteristic::BLE_GATT_UNIT_MAGNETIC_FIELD_STRENGTH_AMPERE_PER_METRE,
            valueBytesx.getPointer(),
            valueBytesx.getNumValueBytes(),
            HeartRateValueBytes::MAX_VALUE_BYTES,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        ),
        magy(
            // GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR,
            GattCharacteristic::BLE_GATT_UNIT_MAGNETIC_FIELD_STRENGTH_AMPERE_PER_METRE+1,
            valueBytesy.getPointer(),
            valueBytesy.getNumValueBytes(),
            HeartRateValueBytes::MAX_VALUE_BYTES,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        ),
        magz(
            // GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR,
            GattCharacteristic::BLE_GATT_UNIT_MAGNETIC_FIELD_STRENGTH_AMPERE_PER_METRE+2,
            valueBytesz.getPointer(),
            valueBytesz.getNumValueBytes(),
            HeartRateValueBytes::MAX_VALUE_BYTES,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        )
    {
        setupService();
    }

    /**
     * Update the heart rate that the service exposes.
     *
     * The server sends a notification of the new value to clients that have
     * subscribed to updates of the heart rate measurement characteristic; clients
     * reading the heart rate measurement characteristic after the update obtain
     * the updated value.
     *
     * @param[in] hrmCounter Heart rate measured in BPM.
     *
     * @attention This function must be called in the execution context of the
     * BLE stack.
     */
    void updateHeartRate(uint16_t x, uint16_t y, uint16_t z) {
        // valueBytes.updateHeartRate(hrmCounter);
        // ble.gattServer().write(
        //     hrmRate.getValueHandle(),
        //     valueBytes.getPointer(),
        //     valueBytes.getNumValueBytes()
        // );
        valueBytesx.updateHeartRate(x);
        ble.gattServer().write(
            magx.getValueHandle(),
            valueBytesx.getPointer(),
            valueBytesx.getNumValueBytes()
        );
        valueBytesy.updateHeartRate(y);
        ble.gattServer().write(
            magy.getValueHandle(),
            valueBytesy.getPointer(),
            valueBytesy.getNumValueBytes()
        );
        valueBytesz.updateHeartRate(z);
        ble.gattServer().write(
            magz.getValueHandle(),
            valueBytesz.getPointer(),
            valueBytesz.getNumValueBytes()
        );
    }

protected:
    /**
     * Construct and add to the GattServer the heart rate service.
     */
    void setupService(void) {
        GattCharacteristic *charTable[] = {
            &magx, &magy, &magz
        };
        GattService hrmService(
            GattService::UUID_HEALTH_THERMOMETER_SERVICE,
            charTable,
            sizeof(charTable) / sizeof(GattCharacteristic*)
        );

        ble.gattServer().addService(hrmService);
    }

protected:
    /*
     * Heart rate measurement value.
     */
    struct HeartRateValueBytes {
        /* 1 byte for the Flags, and up to two bytes for heart rate value. */
        static const unsigned MAX_VALUE_BYTES = 3;
        static const unsigned FLAGS_BYTE_INDEX = 0;

        static const unsigned VALUE_FORMAT_BITNUM = 0;
        static const uint8_t  VALUE_FORMAT_FLAG = (1 << VALUE_FORMAT_BITNUM);

        HeartRateValueBytes(uint16_t hrmCounter) : valueBytes()
        {
            updateHeartRate(hrmCounter);
        }

        void updateHeartRate(uint16_t hrmCounter)
        {
            if (hrmCounter <= 255) {
                valueBytes[FLAGS_BYTE_INDEX] &= ~VALUE_FORMAT_FLAG;
                valueBytes[FLAGS_BYTE_INDEX + 1] = hrmCounter;
            } else {
                valueBytes[FLAGS_BYTE_INDEX] |= VALUE_FORMAT_FLAG;
                valueBytes[FLAGS_BYTE_INDEX + 1] = (uint8_t)(hrmCounter & 0xFF);
                valueBytes[FLAGS_BYTE_INDEX + 2] = (uint8_t)(hrmCounter >> 8);
            }
        }

        uint8_t *getPointer(void)
        {
            return valueBytes;
        }

        const uint8_t *getPointer(void) const
        {
            return valueBytes;
        }

        unsigned getNumValueBytes(void) const
        {
            if (valueBytes[FLAGS_BYTE_INDEX] & VALUE_FORMAT_FLAG) {
                return 1 + sizeof(uint16_t);
            } else {
                return 1 + sizeof(uint8_t);
            }
        }

    private:
        uint8_t valueBytes[MAX_VALUE_BYTES];
    };

protected:
    BLE &ble;
    // HeartRateValueBytes valueBytes;
    HeartRateValueBytes valueBytesx;
    HeartRateValueBytes valueBytesy;
    HeartRateValueBytes valueBytesz;
    // GattCharacteristic hrmRate;
    GattCharacteristic magx;
    GattCharacteristic magy;
    GattCharacteristic magz;
};

#endif // BLE_FEATURE_GATT_SERVER

#endif /* #ifndef MBED_BLE_HEART_RATE_SERVICE_H__*/
