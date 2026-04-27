# SmartThings ESP32 Ventilation Test App

This ESP32 example is a Direct Connected test app for a ventilation product that can later be connected to a real device control interface.

The current implementation focuses on SmartThings integration only:

- `switch`: power on/off
- `mode`: operation mode
- `fanSpeed`: airflow level
- `airQualityHealthConcern`: derived dummy air quality health status
- `carbonDioxideMeasurement`: dummy CO2 value
- `fineDustSensor`: dummy PM2.5 value
- `filterState`: dummy filter life remaining and reset command
- `healthCheck`: automatically added by Developer Workspace / SmartThings

## Operation Modes

The `mode` capability is initialized with these product-oriented values:

- `auto`
- `clean_ventilation`
- `clean_circulation`
- `away`
- `sleep`

These values are intended to match the ventilation modes below:

- `auto` -> 자동
- `clean_ventilation` -> 청정환기
- `clean_circulation` -> 청정순환
- `away` -> 외출모드
- `sleep` -> 취침모드

## Test Behavior

- SmartThings commands update local state immediately.
- No real hardware control command is sent yet.
- The test stub updates dummy `fineDustSensor` and `carbonDioxideMeasurement` values every 10 seconds.
- The base air-quality pattern changes every 5 minutes, and each 10-second update adds a signed delta so threshold-based reporting can be tested with both ignored and reported changes.
- The stub is tuned so each sensor produces roughly a `4 ignored ticks : 1 reported tick` ratio during steady-state threshold testing.
- Dummy `filterState.filterLifeRemaining` starts at `100`, drops by `1` every hour, and returns to `100` after reboot.
- The same sensor-source interface can be reused later with a real physical sensor implementation.
- `filterState.resetFilter` restores the dummy filter life remaining value to `100%`.
- `airQualityHealthConcern.supportedAirQualityValues` and `filterState.supportedFilterCommands` are published once after boot.

## Local Button Shortcuts

- 1 press while onboarding confirmation is pending: confirm ownership
- 1 press after onboarding: toggle `switch`
- 5 short presses: clear onboarding / registered data and reboot
- Other short presses: LED feedback only
- Long press: clean up onboarding data and restart connection flow

## Suggested Device Profile

Create a Direct Connected device profile in SmartThings Console with the `main` component containing at least:

- `switch`
- `mode`
- `fanSpeed`
- `airQualityHealthConcern`
- `carbonDioxideMeasurement`
- `fineDustSensor`
- `filterState`
- `healthCheck`

`healthCheck` is added automatically by SmartThings and does not need a device-side handler.

SmartThings documentation generally recommends adding `refresh` for devices with readable attributes. This sample intentionally matches the capability set listed above and does not wire `refresh`.

## Next Step For Real Hardware

When the real hardware interface is defined, replace the state-only logic in `main/main.c` command callbacks with the actual device control and state-read path, then update the same capabilities from the resulting device state.
