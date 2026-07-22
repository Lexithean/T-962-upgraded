# T-962 ReflowOS

**A comprehensively improved, actively-maintained firmware for the T-962,
T-962A, and T-962C reflow ovens.** ReflowOS turns the notoriously mediocre stock
controller into a capable, safe, and scriptable reflow station — with auto-tuning,
thermocouple calibration, 40+ storable profiles, a full serial API, and layered
safety protections.

A maintained fork of [UnifiedEngineering/T-962-improvements](https://github.com/UnifiedEngineering/T-962-improvements),
funded and maintained by **[Schemara.com](https://schemara.com)** (AI-powered PCB
& schematic tools by **Lexithean**).

<p align="center">
  <img src="docs/images/01-main-menu.png" width="330" alt="Main menu">
  <img src="docs/images/12-reflow-ramp.png" width="330" alt="Reflow in progress">
</p>
<p align="center">
  <img src="docs/images/14-reflow-complete.png" width="330" alt="Post-reflow analytics">
  <img src="docs/images/15-thermal-runaway.png" width="330" alt="Thermal runaway protection">
</p>

> ⚠️ **Compatibility:** requires a T-962 with the **NXP LPC2134** MCU. Some 2024+
> (V2.0 board) models use a different processor and are **not compatible**.
> [Check your board](https://github.com/Lexithean/T-962_ReflowOS/wiki/Troubleshooting)
> before flashing.
>
> **T-962C users:** enable **bang-bang** heater control — see [Calibration](docs/CALIBRATION.md#control-modes-pid-vs-bang-bang).

---

## Documentation

| Guide | What's inside |
|-------|---------------|
| 📖 **[User Guide](docs/USER_GUIDE.md)** | Every on-oven screen and key, with screenshots — running a reflow, baking, editing profiles |
| 🌡️ **[Reflow Profiles](docs/PROFILES.md)** | Built-in, CUSTOM (EEPROM), and 32 flash profiles; editing, import/export, backup |
| 🎯 **[Calibration & Tuning](docs/CALIBRATION.md)** | Thermocouple auto-cal, two-point cal, bang-bang & PID auto-tune, operational modes |
| ⚙️ **[Settings Reference](docs/SETTINGS.md)** | Every setting, its range, default, and effect |
| 🔌 **[Serial Reference](docs/SERIAL.md)** | Full command set, CSV/JSON telemetry, headless control |
| 🛡️ **[Safety Features](docs/SAFETY.md)** | Runaway protection, absolute cutoff, heater-failure detection, watchdog |
| 🔧 **[Building & Flashing](docs/BUILDING.md)** | Toolchains, flashing the oven, first boot, safe updates |

---

## Highlights

### 🎯 Auto-calibration & tuning
- **Thermocouple offset auto-cal** (`tccal`) — zeroes both TCs against the cold-junction reference at ambient.
- **Two-point calibration** — separate ambient and 200 °C offsets per thermocouple, linearly interpolated for accuracy across the whole range.
- **Bang-bang auto-tune** — measures overshoot/undershoot over 3 cycles and stores anticipatory offsets, with a live graph.
- **PID auto-tune** — Ziegler-Nichols relay method computes optimal Kp/Ki/Kd automatically.
- All runnable from the front panel **or** over serial.

### 🌡️ Profiles
- **6 built-in** paste profiles (SAC305, AMTECH SynTECH-LF, NC-31 low-temp, 63/37 leaded, Loctite GC10/GC50).
- **2 editable CUSTOM** profiles in EEPROM, editable on-oven or via serial.
- **Up to 32 flash-stored profiles** — a whole paste library, selectable from the front panel.
- **Backup & restore** of every profile as pasteable serial text.

### 🛡️ Safety
- **Thermal runaway protection** with a configurable threshold, alarm, and abort.
- **Absolute 280 °C over-temperature cutoff** that fires even with a corrupt setpoint — cannot be disabled.
- **Heater-failure detection**, **cooling-rate limiting**, and a **hardware watchdog**.

### 🔧 Control
- **Bang-bang heater mode** — dramatically improves the poorly-behaved T-962C heater.
- **Configurable preheat** phase that soaks before the profile clock starts (and never skips the profile's own ramp).
- **PID or bang-bang**, your choice, each with its own auto-tune.
- **Operational modes** (AMBIENT / MAXTEMPOVERRIDE / SPLIT) for multi-probe setups.

### 📊 Monitoring & UI
- **Live reflow graph** with target curve, temperature trace, elapsed and remaining time.
- **Post-reflow analytics** — peak temperature, time-above-liquidus, and max ramp rate.
- **°C / °F** display toggle applied consistently across every screen.
- **Stage-transition buzzer alerts**, fan kickstart, cold-start logging, screensaver.

### 🔌 Serial & tooling
- **Full text console** (115200 8N1) — reflow, bake, profiles, settings, calibration, all scriptable.
- **CSV and JSON telemetry** streams at ~4 Hz for logging and graphing.
- **Binary profile upload** protocol and a text-based `import` for easy profile transfer.

See the [documentation table](#documentation) above for the details of each.

---

## Quick start

1. **Flash** the firmware — [`pio run -t upload`](docs/BUILDING.md), or hold F1 at
   power-on for ISP mode.
2. **Calibrate** — fit a cold-junction sensor, then run [`tccal`](docs/CALIBRATION.md#thermocouple-offset-auto-calibration).
3. **Tune** — pick [PID or bang-bang](docs/CALIBRATION.md#control-modes-pid-vs-bang-bang)
   (bang-bang for T-962C) and run the matching auto-tune.
4. **Select a profile** matching your solder paste (F4 on the oven).
5. **Run** — press **S**, watch the [live graph](docs/USER_GUIDE.md#running-a-reflow),
   and check the [analytics](docs/USER_GUIDE.md#peak-and-completion) at the end.

New here? Start with the **[User Guide](docs/USER_GUIDE.md)**.

---

## Serial at a glance

Connect any terminal at **115200 baud, 8N1** and type `help`. A few essentials:

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

Full command set, telemetry formats, and examples: **[Serial Reference](docs/SERIAL.md)**.

---

## Building

```bash
pio run                # build (PlatformIO pulls the ARM toolchain)
pio run -t upload      # build and flash
```

or with `make` + `gcc-arm-none-eabi`. Full instructions, flashing, and safe-update
steps: **[Building & Flashing](docs/BUILDING.md)**.

---

## Hardware improvements worth doing

These use the *existing* controller hardware:

- **Cold-junction sensor** — add a **DS18B20** (or DS18S20/DS1822) to the TC
  terminal block; the stock firmware otherwise assumes a fixed ambient. Enables
  accurate readings and `tccal`.
- **Kapton tape** — replace the failure-prone masking tape ([instructable](http://www.instructables.com/id/T962A-SMD-Reflow-Oven-FixHack/?ALLSTEPS)).
- **Mains earth** — confirm the protective earth contacts the chassis and both
  halves are bonded.
- **System-fan PWM** — speed-control the system fan via the spare `ADO` test point
  ([wiki](https://github.com/Lexithean/T-962_ReflowOS/wiki/System-fan-control)).

More in the [project wiki](https://github.com/Lexithean/T-962_ReflowOS/wiki).

---

## ⚠️ Disclaimer & support

**Use at your own risk.** This firmware controls a high-power heater and is
provided "as is" **without any warranty**. Schemara.com and Lexithean assume **no
liability** for injury, fire, property damage, or equipment failure. Read the
[Safety guide](docs/SAFETY.md), never leave a running oven unattended, and keep an
extinguisher nearby.

- **Commercial support:** [support@lexithean.com](mailto:support@lexithean.com)
- **Community help:** [GitHub Issues](https://github.com/Lexithean/T-962_ReflowOS/issues)

Runs on T-962, T-962A, and T-962C. Success/failure reports welcome. Released under
**GPLv3**.

---

## Acknowledgements

Built on years of community work:

- [Unified Engineering](https://github.com/UnifiedEngineering/T-962-improvements) — the original improved firmware this fork is based on
- [ImNoahDev](https://github.com/ImNoahDev) — T-962C bang-bang control, preheat, auto-tune, analytics, flash storage, fork maintenance
- [KLEYNOD](https://github.com/UnifiedEngineering/T-962-improvements/issues/267) — bang-bang heating concept & delta-rewiring research
- [Smashcat](https://github.com/Smashcat) — UI improvements, screensaver ([#159](https://github.com/UnifiedEngineering/T-962-improvements/pull/159))
- [radensb](https://github.com/radensb) — SPLIT/MAXTEMPOVERRIDE modes, binary command interface ([#136](https://github.com/UnifiedEngineering/T-962-improvements/pull/136))
- [ardiehl](https://github.com/ardiehl) — DS18S20/DS1822 sensor support ([#148](https://github.com/UnifiedEngineering/T-962-improvements/pull/148))
- [maxgerhardt](https://github.com/maxgerhardt) — PlatformIO support ([#207](https://github.com/UnifiedEngineering/T-962-improvements/pull/207))
- [mcapdeville](https://github.com/mcapdeville) — LCD buffer-overflow fix ([#245](https://github.com/UnifiedEngineering/T-962-improvements/pull/245))
- [georgeharker](https://github.com/georgeharker) — MAX31855 calibration ([#241](https://github.com/UnifiedEngineering/T-962-improvements/pull/241))
- [CoryCharlton](https://github.com/CoryCharlton) — finer TC offset steps ([#235](https://github.com/UnifiedEngineering/T-962-improvements/pull/235))
- [nica-f](https://github.com/nica-f) — LCD text-printing fixes ([#155](https://github.com/UnifiedEngineering/T-962-improvements/pull/155))
- [cinderblock](https://github.com/cinderblock) — URL fix ([#252](https://github.com/UnifiedEngineering/T-962-improvements/pull/252))
- [C PID Library](https://github.com/mblythe86/C-PID-Library) — v1.0.1, GPLv3

[wiki]: https://github.com/Lexithean/T-962_ReflowOS/wiki
