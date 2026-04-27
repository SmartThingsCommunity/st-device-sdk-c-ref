/* ***************************************************************************
 *
 * Copyright 2019-2026 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef VENTILATION_SENSOR_SOURCE_H
#define VENTILATION_SENSOR_SOURCE_H

#include <stdbool.h>

typedef struct ventilation_sensor_values {
    int air_quality_score;
    int carbon_dioxide_ppm;
    int fine_dust_pm25;
    int filter_life_remaining;
} ventilation_sensor_values_t;

void ventilation_sensor_source_init(ventilation_sensor_values_t *values);
void ventilation_sensor_source_update(bool power_on, ventilation_sensor_values_t *values);
void ventilation_sensor_source_reset_filter_life(ventilation_sensor_values_t *values);

#endif /* VENTILATION_SENSOR_SOURCE_H */
