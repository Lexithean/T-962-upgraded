# Building & Flashing

ReflowOS targets the NXP LPC2134/01 ARM7 MCU found in the original T-962, T-962A,
and T-962C ovens. It has 128 kB of flash, 16 kB of RAM, and runs at 55.296 MHz.

> ⚠️ **Compatibility.** Some 2024+ "V2.0 board" ovens ship a different processor and
> will not work. Confirm your board has an LPC2134 before you flash. See the
> project wiki's Troubleshooting page.

- [Build with PlatformIO](#build-with-platformio-recommended)
- [Build with make](#build-with-make)
- [Flashing the oven](#flashing-the-oven)
- [First boot](#first-boot)
- [Updating firmware safely](#updating-firmware-safely)
- [For developers: the host simulator](#for-developers-the-host-simulator)

---

## Build with PlatformIO (recommended)

[PlatformIO](https://platformio.org/) pulls the ARM toolchain for you.

```bash
pio run                 # build, producing .pio/build/lpc2134_01/firmware.hex
pio run -t upload       # build and flash (via lpc21isp)
```

The build prints flash and RAM usage when it finishes. The image should sit
comfortably under the 128 kB flash budget.

---

## Build with make

Install `gcc-arm-none-eabi`, then:

```bash
make                    # build the firmware
make flash              # download/build lpc21isp and flash
```

On Linux the `flash` target fetches and compiles the
[lpc21isp](http://sourceforge.net/projects/lpc21isp/) programmer for you.
[`COMPILING.md`](../COMPILING.md) has the original toolchain notes.

---

## Flashing the oven

The LPC2134 is programmed over its serial port in ISP (In-System Programming) mode.
You need a 3.3 V USB-to-serial adapter wired to the oven's serial header. The
wiki's flashing guide has the pinout.

To put the oven into ISP mode, either hold the F1 key while you power it on, or run
`enter isp` on the serial console. The `enter isp` route also backs up your flash
profiles and turns the heater and fan off first.

The bootloader communicates at **57600 baud** (not the 115200 the normal console
uses). The upload tools below already know this. Then flash with your tool of
choice:

```bash
pio run -t upload
# or
make flash
```

When you are done, power-cycle the oven. It boots the new firmware and the console
returns to the normal 115200 baud.

---

## First boot

On the first boot after flashing, ReflowOS initialises EEPROM settings to their
[defaults](SETTINGS.md), preloads two example Loctite profiles into flash slots 30
and 31, prints boot and diagnostic info to the serial console, and lands on the
[main menu](USER_GUIDE.md#main-menu).

Before your first real reflow:

1. Fit and [calibrate](CALIBRATION.md) your thermocouples with `tccal`.
2. Choose your [control mode](CALIBRATION.md#control-modes-pid-vs-bang-bang)
   (bang-bang for the T-962C) and run the matching [auto-tune](CALIBRATION.md).
3. Verify readings against an independent thermometer.
4. Read through the [safety features](SAFETY.md).

---

## Updating firmware safely

A firmware update erases the 32 flash profile slots. EEPROM CUSTOM profiles
survive, but back up everything anyway:

```
backup
```

Save the output, reflash, then paste it back to restore. See
[Backup and restore](PROFILES.md#backup-and-restore).

---

## For developers: the host simulator

This repository is developed against a host-side simulator that compiles the real
firmware sources and runs them on your computer against an emulated oven. It models
the thermal behaviour and emulates the LCD (rendered to PNG), keypad, serial,
EEPROM, and IAP flash. It is how every screenshot in these docs was produced, and
how changes are regression-tested without a physical oven.

The simulator is local development tooling and is not part of the shipped firmware.
If you are contributing, see the build and usage notes bundled with it.
