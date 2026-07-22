# Safety Features

This firmware controls a roughly 1500 W heater with no human in the loop during a
run, so ReflowOS layers on several independent protections. None of them replace
supervision. Never leave a running oven unattended, and keep an extinguisher
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

During a reflow or bake, if the measured temperature climbs past the setpoint plus
your runaway threshold (setting #14, *Runaway thresh*, 30 °C by default), the
firmware acts at once.

<img src="images/15-thermal-runaway.png" width="440" alt="Thermal runaway alert">

It turns the heater off, turns the fan on, sounds the alarm buzzer, shows the
runaway alert, and returns to standby. Press any key to dismiss it. Setting the
threshold to `OFF` disables this relative check, but the absolute cutoff below
still applies.

---

## Absolute over-temperature cutoff

Separate from the setpoint-relative check, ReflowOS aborts if the measured
temperature ever passes an absolute ceiling of 280 °C. This is a hard backstop. It
fires even when the setpoint is corrupt or implausibly high, which catches a class
of failure the relative check cannot. You cannot disable it.

---

## Heater failure detection

If the heater is commanded to full power for 30 seconds but the temperature has not
risen by at least 5 °C, the firmware logs a heater-failure warning to the serial
console. That points to a failed SSR, a broken element, or a disconnected heater.

---

## Cooling rate control

Cooling a board too fast causes thermal shock and cracked joints. If you set
**Max cool rate** (setting #16), the firmware limits the fan so the measured
cooldown does not exceed that rate (0.1 to 5.0 °C/s). `UNLIMIT` turns the limit
off.

---

## Watchdog

A hardware watchdog resets the MCU if the firmware ever stops servicing its main
loop, so a software hang cannot leave the heater stuck on. The firmware feeds the
watchdog on its normal schedule throughout every operation.

---

## Safe shutdown paths

- Stopping or aborting (`stop`, or **S** on the reflow or bake screen) always drives
  the heater to 0 before returning to standby.
- Entering the bootloader (`enter isp`, or holding F1 at boot) forces the heater and
  fan pins off before jumping to the ROM bootloader, so the oven does not heat while
  it waits to be reflashed.
- Any mode change back to standby turns the heater off.

---

## What to check on your oven

Firmware can only react to what it measures. The following are your job, and they
matter more than any software feature:

- **Thermocouples.** They should be mounted, intact, and reading the real board or
  cavity temperature. A thermocouple that reads low while the oven is hot is the
  most dangerous failure mode there is. Verify readings against an independent
  thermometer after you install and calibrate (see [Calibration](CALIBRATION.md)).
- **Cold-junction sensor.** Fit a DS18B20 (or DS18S20/DS1822). Without it the
  firmware assumes a fixed ambient, which hurts accuracy.
- **Mains earth.** Confirm the protective earth contacts the chassis and that both
  halves are bonded.
- **Wiring and SSR.** Inspect for heat damage. A welded-closed SSR defeats every
  software cutoff. The fan-on and alarm still fire, but the element stays powered.
- **Ventilation and attendance.** Run in a ventilated space and stay with the oven.

> Use at your own risk. This firmware is provided without warranty of any kind. See
> the disclaimer in the main [README](../README.md).
