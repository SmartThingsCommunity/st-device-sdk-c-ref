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

#include <stddef.h>

#include "ventilation_sensor_source.h"

#define FILTER_LIFE_START_PERCENT 100
#define FILTER_LIFE_DROP_INTERVAL_TICKS 360
#define BASE_PATTERN_INTERVAL_TICKS 30
#define BASE_PATTERN_COUNT 12
#define MICRO_PATTERN_INTERVAL_TICKS 5
#define MINOR_VARIATION_COUNT 4
#define CO2_MAJOR_DELTA_COUNT 8
#define FINE_DUST_MAJOR_DELTA_COUNT 8

static unsigned int g_update_tick_count;
static unsigned int g_filter_life_tick_count;
static int g_carbon_dioxide_anchor_ppm;
static int g_fine_dust_anchor_pm25;

static int clamp_min(int value, int min_value)
{
    return (value < min_value) ? min_value : value;
}

static int interpolate_range(int value, int low_input, int high_input, int low_output, int high_output)
{
    if (value <= low_input) {
        return low_output;
    }
    if (value >= high_input) {
        return high_output;
    }

    return low_output + ((value - low_input) * (high_output - low_output) / (high_input - low_input));
}

static int get_pm25_air_quality_score(int fine_dust_pm25)
{
    if (fine_dust_pm25 <= 9) {
        return interpolate_range(fine_dust_pm25, 0, 9, 0, 50);
    }
    if (fine_dust_pm25 <= 35) {
        return interpolate_range(fine_dust_pm25, 10, 35, 51, 100);
    }
    if (fine_dust_pm25 <= 55) {
        return interpolate_range(fine_dust_pm25, 36, 55, 101, 150);
    }
    if (fine_dust_pm25 <= 125) {
        return interpolate_range(fine_dust_pm25, 56, 125, 151, 200);
    }
    if (fine_dust_pm25 <= 225) {
        return interpolate_range(fine_dust_pm25, 126, 225, 201, 300);
    }
    return interpolate_range(fine_dust_pm25, 226, 325, 301, 500);
}

static int get_co2_air_quality_score(int carbon_dioxide_ppm)
{
    if (carbon_dioxide_ppm <= 800) {
        return interpolate_range(carbon_dioxide_ppm, 400, 800, 0, 50);
    }
    if (carbon_dioxide_ppm <= 1000) {
        return interpolate_range(carbon_dioxide_ppm, 801, 1000, 51, 100);
    }
    if (carbon_dioxide_ppm <= 1500) {
        return interpolate_range(carbon_dioxide_ppm, 1001, 1500, 101, 150);
    }
    if (carbon_dioxide_ppm <= 2000) {
        return interpolate_range(carbon_dioxide_ppm, 1501, 2000, 151, 200);
    }
    if (carbon_dioxide_ppm <= 5000) {
        return interpolate_range(carbon_dioxide_ppm, 2001, 5000, 201, 300);
    }

    return interpolate_range(carbon_dioxide_ppm, 5001, 10000, 301, 500);
}

void ventilation_sensor_source_init(ventilation_sensor_values_t *values)
{
    static const int carbon_dioxide_base_pattern[BASE_PATTERN_COUNT] = {
        650, 700, 760, 820, 900, 980, 1100, 1300, 1600, 1200, 900, 740
    };
    static const int fine_dust_base_pattern[BASE_PATTERN_COUNT] = {
        8, 10, 12, 16, 20, 26, 34, 42, 28, 18, 12, 9
    };
    int pm25_score;
    int co2_score;

    if (!values) {
        return;
    }

    g_update_tick_count = 0;
    g_filter_life_tick_count = 0;
    g_carbon_dioxide_anchor_ppm = carbon_dioxide_base_pattern[0];
    g_fine_dust_anchor_pm25 = fine_dust_base_pattern[0];

    values->carbon_dioxide_ppm = g_carbon_dioxide_anchor_ppm;
    values->fine_dust_pm25 = g_fine_dust_anchor_pm25;
    values->filter_life_remaining = FILTER_LIFE_START_PERCENT;

    pm25_score = get_pm25_air_quality_score(values->fine_dust_pm25);
    co2_score = get_co2_air_quality_score(values->carbon_dioxide_ppm);
    values->air_quality_score = (pm25_score > co2_score) ? pm25_score : co2_score;
}

void ventilation_sensor_source_reset_filter_life(ventilation_sensor_values_t *values)
{
    if (!values) {
        return;
    }

    g_filter_life_tick_count = 0;
    values->filter_life_remaining = FILTER_LIFE_START_PERCENT;
}

void ventilation_sensor_source_update(bool power_on, ventilation_sensor_values_t *values)
{
    static const int carbon_dioxide_base_pattern[BASE_PATTERN_COUNT] = {
        650, 700, 760, 820, 900, 980, 1100, 1300, 1600, 1200, 900, 740
    };
    static const int fine_dust_base_pattern[BASE_PATTERN_COUNT] = {
        8, 10, 12, 16, 20, 26, 34, 42, 28, 18, 12, 9
    };
    static const int carbon_dioxide_minor_variation[MINOR_VARIATION_COUNT] = {
        3, -4, 6, -8
    };
    static const int carbon_dioxide_major_delta[CO2_MAJOR_DELTA_COUNT] = {
        12, -14, 11, -13, 15, -12, 14, -11
    };
    static const int fine_dust_minor_variation[MINOR_VARIATION_COUNT] = {
        1, -2, 3, -4
    };
    static const int fine_dust_major_delta[FINE_DUST_MAJOR_DELTA_COUNT] = {
        6, -7, 5, -6, 8, -5, 7, -8
    };
    unsigned int base_pattern_index;
    unsigned int micro_pattern_index;
    unsigned int major_delta_index;
    int base_carbon_dioxide_ppm;
    int base_fine_dust_pm25;
    int pm25_score;
    int co2_score;

    if (!values) {
        return;
    }

    (void)power_on;

    g_update_tick_count++;
    g_filter_life_tick_count++;

    if ((g_filter_life_tick_count % FILTER_LIFE_DROP_INTERVAL_TICKS) == 0 &&
            values->filter_life_remaining > 0) {
        values->filter_life_remaining--;
    }

    base_pattern_index = (g_update_tick_count / BASE_PATTERN_INTERVAL_TICKS) % BASE_PATTERN_COUNT;
    micro_pattern_index = (g_update_tick_count - 1) % MICRO_PATTERN_INTERVAL_TICKS;
    major_delta_index = ((g_update_tick_count - 1) / MICRO_PATTERN_INTERVAL_TICKS);

    base_carbon_dioxide_ppm = carbon_dioxide_base_pattern[base_pattern_index];
    base_fine_dust_pm25 = fine_dust_base_pattern[base_pattern_index];

    if (g_update_tick_count == 1 || ((g_update_tick_count - 1) % BASE_PATTERN_INTERVAL_TICKS) == 0) {
        g_carbon_dioxide_anchor_ppm = base_carbon_dioxide_ppm;
        g_fine_dust_anchor_pm25 = base_fine_dust_pm25;
    }

    if (micro_pattern_index < MINOR_VARIATION_COUNT) {
        values->carbon_dioxide_ppm = clamp_min(
                g_carbon_dioxide_anchor_ppm + carbon_dioxide_minor_variation[micro_pattern_index], 400);
        values->fine_dust_pm25 = clamp_min(
                g_fine_dust_anchor_pm25 + fine_dust_minor_variation[micro_pattern_index], 0);
    } else {
        g_carbon_dioxide_anchor_ppm = clamp_min(
                g_carbon_dioxide_anchor_ppm +
                carbon_dioxide_major_delta[major_delta_index % CO2_MAJOR_DELTA_COUNT], 400);
        g_fine_dust_anchor_pm25 = clamp_min(
                g_fine_dust_anchor_pm25 +
                fine_dust_major_delta[major_delta_index % FINE_DUST_MAJOR_DELTA_COUNT], 0);

        values->carbon_dioxide_ppm = g_carbon_dioxide_anchor_ppm;
        values->fine_dust_pm25 = g_fine_dust_anchor_pm25;
    }

    pm25_score = get_pm25_air_quality_score(values->fine_dust_pm25);
    co2_score = get_co2_air_quality_score(values->carbon_dioxide_ppm);
    values->air_quality_score = (pm25_score > co2_score) ? pm25_score : co2_score;
}
