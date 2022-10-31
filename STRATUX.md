# Stratux Release

This version of SoftRF follow's changes to the origional SoftRF with modification to the following items:

- Add $PSOFT to indicate to stratux how to handle this device
- Add miliseconds to GGA and RMC, still need to decide if this is correct or not. During initial testing I found the SoftRF device about 300ft flying behind me
- Baro source for stratux when hardware is available


## Add modifications to the GPS setup to supper multiGNS, eg more accurate GPS

- u-blox GPS configurations (for **T-Beam**):
    - enable GSA, GSV, VTG
    - enable GPS, GALILEO, BEIDOU and SBAS
    - enable NMEA extended
- L76K GPS configuration (for **T-Echo**)
    - enable GSA, GSV, VTG
    - enable GPS, GLONASS and BEIDOU
- disable LK8EX1 and LEGACY traffic messages

**IMPORTANT**: after compiling/flashing, SoftRF needs to be configured as follows, using the respective method:
- disable "NMEA sentences - Legacy"
- when connecting SoftRF via USB with Stratux:
    - set "NMEA output" to USB
- when connecting SoftRF via Bluetooth LE with Stratux (**WORK IN PROGRESS, NOT IMPLEMENTED IN STRATUX YET**):
    - set "NMEA output" to Bluetooth (default)

Follow the normal firmware update after downloading of this release, instructions for flashing can be found here : (https://github.com/lyusupov/SoftRF/tree/master/software/firmware/binaries)[Instructions how to flash]


Commit hash : 