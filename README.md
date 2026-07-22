# T-962 ReflowOS

A comprehensively improved, actively maintained firmware for the T-962, T-962A,
and T-962C reflow ovens. ReflowOS turns the notoriously mediocre stock controller
into a capable, safe, and scriptable reflow station. You get auto-tuning,
thermocouple calibration, room for 40+ stored profiles, a full serial API, and
several layers of safety protection.

It is a maintained fork of [UnifiedEngineering/T-962-improvements](https://github.com/UnifiedEngineering/T-962-improvements),
funded and maintained by [Schemara.com](https://schemara.com), the AI-powered PCB
and schematic tools by Lexithean.

<p align="center">
  <img src="docs/images/01-main-menu.png" width="330" alt="Main menu">
  <img src="docs/images/12-reflow-ramp.png" width="330" alt="Reflow in progress">
</p>
<p align="center">
  <img src="docs/images/14-reflow-complete.png" width="330" alt="Post-reflow analytics">
  <img src="docs/images/15-thermal-runaway.png" width="330" alt="Thermal runaway protection">
</p>

> ⚠️ **Compatibility.** This firmware requires a T-962 with the NXP LPC2134 MCU.
> Some 2024+ models (the V2.0 board) use a different processor and will not work.
> [Check your board](https://github.com/Lexithean/T-962_ReflowOS/wiki/Troubleshooting)
> before you flash.
>
> **T-962C users:** turn on bang-bang heater control. The
> [Calibration guide](docs/CALIBRATION.md#control-modes-pid-vs-bang-bang) explains why.

---

## Documentation

| Guide | What's inside |
|-------|---------------|
| 📖 **[User Guide](docs/USER_GUIDE.md)** | Every on-oven screen and key, with screenshots. Running a reflow, baking, editing profiles. |
| 🌡️ **[Reflow Profiles](docs/PROFILES.md)** | Built-in, CUSTOM (EEPROM), and 32 flash profiles. Editing, import/export, backup. |
| 🎯 **[Calibration & Tuning](docs/CALIBRATION.md)** | Thermocouple auto-cal, two-point cal, bang-bang and PID auto-tune, operational modes. |
| ⚙️ **[Settings Reference](docs/SETTINGS.md)** | Every setting, its range, default, and effect. |
| 🔌 **[Serial Reference](docs/SERIAL.md)** | The full command set, CSV/JSON telemetry, headless control. |
| 🛡️ **[Safety Features](docs/SAFETY.md)** | Runaway protection, absolute cutoff, heater-failure detection, watchdog. |
| 🔧 **[Building & Flashing](docs/BUILDING.md)** | Toolchains, flashing the oven, first boot, safe updates. |

---

## Highlights

### 🎯 Auto-calibration and tuning
Run `tccal` to zero both thermocouples against the cold-junction reference at
room temperature. For accuracy across the whole range, set a separate offset at
ambient and at 200 °C per thermocouple, and the firmware interpolates between
them. Bang-bang auto-tune measures overshoot and undershoot over three cycles and
stores the anticipatory offsets it finds, all with a live graph. PID auto-tune
uses the Ziegler-Nichols relay method to compute Kp, Ki, and Kd for you. Every
one of these runs from the front panel or over serial.

### 🌡️ Profiles
Six built-in paste profiles cover SAC305, AMTECH SynTECH-LF, NC-31 low-temp, 63/37
leaded, and Loctite GC10/GC50. Two CUSTOM profiles live in EEPROM and are editable
on the oven or over serial. Up to 32 more profiles fit in flash, so you can keep a
whole paste library and pick any of them from the front panel. The `backup`
command dumps everything as pasteable serial text.

### 🛡️ Safety
Thermal runaway protection aborts the run if the temperature climbs past your
threshold above setpoint. On top of that, an absolute 280 °C cutoff fires even
when the setpoint is corrupt, and it cannot be switched off. There is also
heater-failure detection, cooling-rate limiting, and a hardware watchdog.

### 🔧 Control
Bang-bang heater mode dramatically improves the poorly behaved T-962C heater. A
configurable preheat phase soaks the oven before the profile clock starts, and it
never skips the profile's own opening ramp. Choose PID or bang-bang, each with its
own auto-tune. Operational modes (AMBIENT, MAXTEMPOVERRIDE, SPLIT) support
multi-probe setups.

### 📊 Monitoring and UI
The live reflow graph shows the target curve, your temperature trace, and both
elapsed and remaining time. When a run finishes you get analytics: peak
temperature, time above liquidus, and the fastest ramp rate. A °C/°F toggle
applies consistently on every screen. Rounding out the UI are stage-transition
buzzer alerts, a fan kickstart pulse, cold-start logging, and a screensaver.

### 🔌 Serial and tooling
The text console (115200 8N1) exposes everything: reflow, bake, profiles,
settings, and calibration, all scriptable. Telemetry streams as CSV or JSON at
about 4 Hz for logging and graphing. A binary upload protocol and a text-based
`import` command make it easy to move profiles around.

The [documentation table](#documentation) above covers each of these in detail.

---

## Quick start

1. Flash the firmware with [`pio run -t upload`](docs/BUILDING.md), or hold F1 at
   power-on to enter ISP mode.
2. Fit a cold-junction sensor, then run [`tccal`](docs/CALIBRATION.md#thermocouple-offset-auto-calibration)
   to calibrate.
3. Pick [PID or bang-bang](docs/CALIBRATION.md#control-modes-pid-vs-bang-bang)
   (bang-bang for the T-962C) and run the matching auto-tune.
4. Select the profile that matches your solder paste (F4 on the oven).
5. Press **S**, watch the [live graph](docs/USER_GUIDE.md#running-a-reflow), and
   read the [analytics](docs/USER_GUIDE.md#peak-and-completion) at the end.

New here? Start with the [User Guide](docs/USER_GUIDE.md).

---

## Serial at a glance

Connect any terminal at 115200 baud, 8N1, and type `help`. A few essentials:

```
list profiles          # see all profiles and ids
select profile 0       # pick SAC305
reflow                 # start a reflow
bake 120 600           # bake at 120 C for 600 s
stop                   # abort
tccal                  # calibrate thermocouples
backup                 # dump all profiles before a firmware update
json                   # toggle machine-readable telemetry
```

The full command set, telemetry formats, and examples are in the
[Serial Reference](docs/SERIAL.md).

---

## Building

```bash
pio run                # build (PlatformIO pulls the ARM toolchain)
pio run -t upload      # build and flash
```

You can also use `make` with `gcc-arm-none-eabi`. Full instructions, flashing, and
safe-update steps are in [Building & Flashing](docs/BUILDING.md).

---

## Hardware improvements worth doing

These all use the controller hardware you already have:

- **Add a cold-junction sensor.** Fit a DS18B20 (or DS18S20/DS1822) to the TC
  terminal block. The stock firmware otherwise assumes a fixed ambient, and
  without one you cannot run `tccal`.
- **Replace the masking tape** with Kapton tape ([instructable](http://www.instructables.com/id/T962A-SMD-Reflow-Oven-FixHack/?ALLSTEPS)).
- **Check the mains earth.** Confirm the protective earth contacts the chassis and
  that both halves are bonded.
- **Add system-fan PWM.** Speed-control the system fan through the spare `ADO` test
  point ([wiki](https://github.com/Lexithean/T-962_ReflowOS/wiki/System-fan-control)).

There is more in the [project wiki](https://github.com/Lexithean/T-962_ReflowOS/wiki).

---

## ⚠️ Disclaimer and support

Use at your own risk. This firmware controls a high-power heater and is provided
"as is" without any warranty. Schemara.com and Lexithean assume no liability for
injury, fire, property damage, or equipment failure. Read the
[Safety guide](docs/SAFETY.md), never leave a running oven unattended, and keep an
extinguisher nearby.

- Commercial support: [support@lexithean.com](mailto:support@lexithean.com)
- Community help: [GitHub Issues](https://github.com/Lexithean/T-962_ReflowOS/issues)

Runs on the T-962, T-962A, and T-962C. Success and failure reports are both
welcome. Released under GPLv3.

---

## Acknowledgements

Built on years of community work:

- [Unified Engineering](https://github.com/UnifiedEngineering/T-962-improvements) for the original improved firmware this fork is based on
- [ImNoahDev](https://github.com/ImNoahDev) for T-962C bang-bang control, preheat, auto-tune, analytics, flash storage, and fork maintenance
- [KLEYNOD](https://github.com/UnifiedEngineering/T-962-improvements/issues/267) for the bang-bang heating concept and delta-rewiring research
- [Smashcat](https://github.com/Smashcat) for UI improvements and the screensaver ([#159](https://github.com/UnifiedEngineering/T-962-improvements/pull/159))
- [radensb](https://github.com/radensb) for the SPLIT/MAXTEMPOVERRIDE modes and the binary command interface ([#136](https://github.com/UnifiedEngineering/T-962-improvements/pull/136))
- [ardiehl](https://github.com/ardiehl) for DS18S20/DS1822 sensor support ([#148](https://github.com/UnifiedEngineering/T-962-improvements/pull/148))
- [maxgerhardt](https://github.com/maxgerhardt) for PlatformIO support ([#207](https://github.com/UnifiedEngineering/T-962-improvements/pull/207))
- [mcapdeville](https://github.com/mcapdeville) for the LCD buffer-overflow fix ([#245](https://github.com/UnifiedEngineering/T-962-improvements/pull/245))
- [georgeharker](https://github.com/georgeharker) for MAX31855 calibration ([#241](https://github.com/UnifiedEngineering/T-962-improvements/pull/241))
- [CoryCharlton](https://github.com/CoryCharlton) for finer TC offset steps ([#235](https://github.com/UnifiedEngineering/T-962-improvements/pull/235))
- [nica-f](https://github.com/nica-f) for LCD text-printing fixes ([#155](https://github.com/UnifiedEngineering/T-962-improvements/pull/155))
- [cinderblock](https://github.com/cinderblock) for a URL fix ([#252](https://github.com/UnifiedEngineering/T-962-improvements/pull/252))
- The [C PID Library](https://github.com/mblythe86/C-PID-Library), v1.0.1, GPLv3

[wiki]: https://github.com/Lexithean/T-962_ReflowOS/wiki
