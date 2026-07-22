# Safety Features

This firmware controls a ~1500 W heater with no human in the loop during a run.
ReflowOS layers several independent protections. **None of them replace
supervision** — never leave a running oven unattended, and keep an extinguisher
nearby.

- [Thermal runaway protection](#thermal-runaway-protection)
- [Absolute over-temperature cutoff](#absolute-over-temperature-cutoff)
- [Heater failure detection](#heater-failure-detection)
- [Cooling rate control](#cooling-rate-control)
- [Watchdog](#watchdog)
- [Safe shutdown paths](#safe-shutdown-paths)
- [What to check on your oven](#what-to-check-on-your-oven)

---

## Thermal runaway protection

During a reflow or bake, if the measured temperature exceeds the **setpoint plus
the runaway threshold** (Setting #14, *Runaway thresh*, default 30 °C), the
firmware immediately:

<img src="images/15-thermal-runaway.png" width="440" alt="Thermal runaway alert">

- turns the **heater off**,
- turns the **fan on**,
- sounds the alarm buzzer,
- shows the runaway alert and returns to standby.

Press any key to dismiss. Setting the threshold to `OFF` disables this
*relative* check — but the absolute cutoff below still applies.

---

## Absolute over-temperature cutoff

Independent of the setpoint-relative check, ReflowOS aborts if the measured
temperature ever exceeds an **absolute ceiling of 280 °C**. This is a hard
backstop: it fires even if the setpoint is corrupt or implausibly high, catching
a class of failure the relative check cannot. It cannot be disabled.

---

## Heater failure detection

If the heater is commanded to **full power for 30 seconds** but the temperature
has not risen by at least 5 °C, the firmware logs a heater-failure warning to the
serial console — a sign of a failed SSR, broken element, or disconnected heater.

---

## Cooling rate control

Cooling a board too fast causes thermal shock and cracked joints. If **Max cool
rate** (Setting #16) is set, the firmware limits the fan so the measured cooldown
does not exceed that rate (0.1–5.0 °C/s). `UNLIMIT` disables the limit.

---

## Watchdog

A hardware watchdog resets the MCU if the firmware ever stops servicing its main
loop, so a software hang cannot leave the heater stuck on. The firmware feeds the
watchdog on its normal schedule during all operations.

---

## Safe shutdown paths

- **Stop / abort** (`stop`, or **S** on the reflow/bake screen) always drives the
  heater to 0 before returning to standby.
- **Entering the bootloader** (`enter isp`, or holding F1 at boot) forces the
  heater and fan pins off before jumping to the ROM bootloader, so the oven does
  not heat while it waits to be reflashed.
- On any **mode change back to standby**, the heater is turned off.

---

## What to check on your oven

Firmware can only react to what it measures. The following are your
responsibility and matter more than any software feature:

- **Thermocouples** — mounted, intact, and reading the actual board/cavity
  temperature. A thermocouple that reads low while the oven is hot is the most
  dangerous failure mode; verify readings with an independent thermometer after
  installing/calibrating (see [Calibration](CALIBRATION.md)).
- **Cold-junction sensor** — fit a DS18B20 (or DS18S20/DS1822). Without it the
  firmware assumes a fixed ambient, hurting accuracy.
- **Mains earth** — confirm the protective earth contacts the chassis and both
  halves are bonded.
- **Wiring & SSR** — inspect for heat damage; a welded-closed SSR defeats every
  software cutoff (the fan-on + alarm still fire, but the element stays powered).
- **Ventilation & attendance** — run in a ventilated space and stay with the oven.

> Use at your own risk. This firmware is provided without warranty of any kind —
> see the disclaimer in the main [README](../README.md).
