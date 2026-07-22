# Building & Flashing

ReflowOS targets the **NXP LPC2134/01** ARM7 MCU found in the original T-962,
T-962A, and T-962C ovens (128 kB flash / 16 kB RAM, 55.296 MHz).

> ⚠️ **Compatibility:** Some 2024+ "V2.0 board" ovens ship a *different*
> processor and are **not** compatible. Confirm your board has an LPC2134 before
> flashing. See the project wiki's Troubleshooting page.

- [Build with PlatformIO](#build-with-platformio-recommended)
- [Build with make](#build-with-make)
- [Flashing the oven](#flashing-the-oven)
- [First boot](#first-boot)
- [Updating firmware safely](#updating-firmware-safely)
- [For developers: the host simulator](#for-developers-the-host-simulator)

---

## Build with PlatformIO (recommended)

[PlatformIO](https://platformio.org/) pulls the ARM toolchain automatically.

```bash
pio run                 # build -> .pio/build/lpc2134_01/firmware.hex
pio run -t upload       # build and flash (via lpc21isp)
```

The build reports flash/RAM usage on completion; the image should sit well under
the 128 kB flash budget.

---

## Build with make

Install `gcc-arm-none-eabi`, then:

```bash
make                    # build firmware
make flash              # download/build lpc21isp and flash
```

On Linux the `flash` target fetches and compiles the
[lpc21isp](http://sourceforge.net/projects/lpc21isp/) programmer for you. See
[`COMPILING.md`](../COMPILING.md) for the original toolchain notes.

---

## Flashing the oven

The LPC2134 is programmed over its serial port in ISP (In-System Programming)
mode. You need a **3.3 V USB-to-serial adapter** wired to the oven's serial
header (see the wiki's flashing guide for the pinout).

To put the oven in ISP mode, either:

- **Hold the F1 key while powering on** the oven, or
- run **`enter isp`** on the serial console (this also backs up your flash
  profiles and turns the heater/fan off first).

Then flash with your tool of choice:

```bash
pio run -t upload
# or
make flash
```

Reconnect at the normal 115200 baud and power-cycle the oven when done.

---

## First boot

On first boot after flashing, ReflowOS:

- initialises EEPROM settings to their [defaults](SETTINGS.md),
- preloads two example Loctite profiles into flash slots 30/31,
- prints boot/diagnostic info to the serial console,
- lands on the [main menu](USER_GUIDE.md#main-menu).

Before your first real reflow:

1. Fit and [calibrate](CALIBRATION.md) your thermocouples (`tccal`).
2. Choose your [control mode](CALIBRATION.md#control-modes-pid-vs-bang-bang)
   (bang-bang for T-962C) and run the matching [auto-tune](CALIBRATION.md).
3. Verify readings against an independent thermometer.
4. Review the [safety features](SAFETY.md).

---

## Updating firmware safely

A firmware update **erases flash profiles** (the 32 flash slots). EEPROM CUSTOM
profiles survive, but back up everything anyway:

```
backup
```

Save the output, reflash, then paste it back to restore. See
[Backup & restore](PROFILES.md#backup--restore).

---

## For developers: the host simulator

This repository is developed against a **host-side simulator** that compiles the
real firmware sources and runs them on your computer against an emulated oven —
thermal model, LCD (rendered to PNG), keypad, serial, EEPROM, and IAP flash. It's
how every screenshot in these docs was produced and how changes are regression-
tested without a physical oven.

The simulator is local development tooling and is not part of the shipped
firmware. If you're contributing, see the build/usage notes bundled with it.
