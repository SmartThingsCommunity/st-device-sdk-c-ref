# Capability Gap Report

This report compares the capability samples in `apps/capability_sample` against the official SmartThings standard capability list.

Checked date: 2026-04-24

Official references:
- Production capabilities: https://developer.smartthings.com/docs/devices/capabilities/capabilities-reference
- Proposed capabilities: https://developer.smartthings.com/docs/devices/capabilities/proposed
- Raw capability data used for comparison: `https://developer.smartthings.com/capabilities`

Comparison rule:
- Local sample capability IDs were collected from `caps_{CAPABILITY}.c/.h` filenames in this directory.
- Official capability IDs were collected from the SmartThings raw capability JSON.
- Official statuses included in the main comparison: `live`, `proposed`

## Summary

- Local sample capabilities: 98
- Official `live` capabilities: 136
- Official `proposed` capabilities: 123
- Official `live + proposed` capabilities: 259
- Missing from local samples: 167
  - Missing `live`: 53
  - Missing `proposed`: 114
- Local capabilities that are no longer `live` or `proposed`: 6

## Missing Live Capabilities

```text
activitySensor
airConditionerFanMode
airConditionerMode
atmosphericPressureMeasurement
audioNotification
audioStream
audioTrackData
batteryLevel
bypassable
chime
currentMeasurement
demandResponseLoadControl
dewPoint
elevatorCall
filterState
formaldehydeHealthConcern
gasDetector
hardwareFault
healthCheck
humidifierMode
infraredLevel
keypadInput
laundryWasherRinseMode
laundryWasherSpinSpeed
lockCodes
mediaTrackControl
nitrogenDioxideHealthConcern
nitrogenDioxideMeasurement
notification
occupancySensor
ovenMode
ozoneHealthConcern
ozoneMeasurement
pestControl
powerConsumptionReport
precipitationSensor
radonMeasurement
riceCooker
robotCleanerCleaningMode
robotCleanerOperatingState
statelessPowerButton
statelessPowerToggleButton
temperatureLevel
temperatureSetpoint
tvChannel
videoCamera
videoCapture
videoStream
washerOperatingState
webrtc
windMode
windowShadeLevel
windowShadePreset
```

## Missing Proposed Capabilities

```text
alarmSensor
applianceUtilization
audioCapture
audioRecording
batchGasConsumptionReport
cameraEvent
cameraPreset
cameraPrivacyMode
cameraViewportSettings
carbonMonoxideHealthConcern
chargePointState
chargingState
coffeeMakerOperation
color
colorMode
consumable
consumableLife
containerState
cookTime
deliveryRobotCall
dishwasherMode
doorState
drivingStatus
dryerMode
endToEndEncryption
endToEndEncryptionState
estimatedTimeOfArrival
evseChargingSession
evseState
faceRecognition
fanDirection
fanMode
fanSpeedPercent
feederOperatingState
feederPortion
flowMeasurement
foodWasteDryingGrinder
gasConsumptionReport
geofence
geolocation
gridState
hdr
imageControl
knob
level
lightControllerMode
localMediaStorage
lockAlarm
lockAliro
lockCredentials
lockSchedules
lockUsers
massageIntensityChange
massageIntensityControl
massageOperating
massageOperatingState
massageTimeChange
massageTimeControl
mechanicalPanTiltZoom
mediaGroup
mediaPresets
motionBed
movementSensor
multipleZonePresence
nightVision
operationalState
petActivity
plantCultivation
pumpControlMode
pumpOperationMode
rainSensor
refrigerationSetpoint
relativeBrightness
safetySwitch
safetyValve
sceneActivity
scenes
scent
serviceArea
soilMoistureMeasurement
sounds
speechRecognition
speechSynthesis
statelessCurtainPowerButton
statelessScenes
switchState
thermostatWaterHeatingSetpoint
threadBorderRouter
threadNetwork
vehicleBattery
vehicleDoorState
vehicleEngine
vehicleFuelLevel
vehicleHvac
vehicleHvacRemoteSwitch
vehicleInformation
vehicleOdometer
vehicleRange
vehicleTirePressureMonitor
vehicleWarning
vehicleWindowState
videoCapture2
videoStreamSettings
washerMode
waterFlowAlarm
waterMeter
waterPressureMeasurement
waterTemperatureMeasurement
waterUsageMeter
wifiInformation
windSpeed
windowShadeTiltLevel
wirelessOperatingMode
zoneManagement
```

## Local Capabilities That Are Deprecated

These exist in `apps/capability_sample`, but the official SmartThings data marks them as `deprecated` rather than `live` or `proposed`.

```text
execute
garageDoorControl
operatingState
samsungTV
thermostatSetpoint
timedSession
```

## Notes

- `antiSnoringPillow`, `filterStatus`, `radonHealthConcern`, `rapidCooling`, `tvocHealthConcern` are already covered locally and are still in the official `proposed` set.
- This report only checks presence or absence of capability sample files. It does not verify whether each sample fully models every attribute, command, enum, or schema change in the latest SmartThings definition.
