# Troubleshooting, FAQ & Glossary

- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [Glossary](#glossary)

---

## Troubleshooting

### "No cold-junction sensor on PCB" / cold junction shows ERR
The firmware could not find a 1-wire cold-junction sensor. Fit a DS18B20 (or
DS18S20/DS1822) to the TC terminal block. Without it the firmware falls back to a
fixed ambient assumption, which hurts accuracy, and `tccal` will not run.

### `tccal` says the oven is too hot
TC offset auto-calibration needs an ambient (cool) oven to have a valid reference.
Let the oven cool to near room temperature and try again.

### Reflow aborts with THERMAL RUNAWAY unexpectedly
The measured temperature passed either setpoint plus the runaway threshold, or the
absolute 280 °C ceiling. Common causes: a runaway threshold set too tight for your
oven's overshoot (raise *Runaway thresh*, setting #14), a mis-calibrated
thermocouple reading high, or a genuinely overpowered/over-driven oven. Check your
[calibration](CALIBRATION.md) and consider [bang-bang mode](CALIBRATION.md#control-modes-pid-vs-bang-bang)
if PID is overshooting badly.

### "[WARNING] HEATER FAILURE?" in the serial log
The heater was at full power for 30 seconds without the temperature rising 5 °C.
Suspect a failed SSR, a broken heating element, a disconnected heater, or a
thermocouple that is not sensing the cavity. Stop and inspect before running
again.

### The oven overshoots or oscillates around the setpoint
Run [PID auto-tune](CALIBRATION.md#pid-auto-tune) (PID mode) or
[bang-bang auto-tune](CALIBRATION.md#bang-bang-auto-tune) (bang-bang mode) with a
scrap board inside. On the T-962C, prefer bang-bang mode; its heater responds
poorly to PWM.

### "Reflow can't keep up with desired PID_TIMEBASE!" on serial
The control loop missed its timing budget for a cycle. An occasional message under
heavy serial traffic is harmless. If it is constant, reduce what else is polling
the console.

### Pasting a backup drops profiles
The oven's serial receive buffer is small and is drained about twice a second.
Paste your backup a few lines at a time, or use a terminal that inserts a short
delay between lines. See [Backup and restore](PROFILES.md#backup-and-restore).

### The temperature unit setting does not change the serial output
That is expected. **Temp unit** (setting #19) changes the on-oven LCD only. Serial
telemetry, `values`, and `about` are always Celsius.

### A custom profile runs the wrong curve, or "starts in the middle"
Make sure you selected the profile you meant (`list profiles`, then
`select profile <id>`). If you recently changed control mode or preheat, note that
preheat soaks *before* the profile starts; the profile itself always begins at its
first point.

### Firmware will not flash / oven not detected in ISP mode
Enter ISP mode by holding **F1** at power-on, or with `enter isp`. The bootloader
runs at **57600 baud**, not the console's 115200. Confirm your board actually has
an **LPC2134** (some 2024+ V2.0 boards do not, and are not compatible). Check the
serial adapter is 3.3 V and wired to the correct pins (see the project wiki).

---

## FAQ

**Which profile should I use?**
Match it to your solder paste and follow the paste maker's reflow spec. SAC305
Leadfree (id 0) is a common lead-free default; 4300 63SN/37PB (id 3) is a leaded
option. See [Profiles](PROFILES.md).

**PID or bang-bang?**
PID for a T-962/T-962A with a responsive heater; bang-bang for the T-962C. Each
has its own auto-tune. See [Calibration](CALIBRATION.md#control-modes-pid-vs-bang-bang).

**Do my profiles survive a firmware update?**
The two CUSTOM EEPROM profiles do. The 32 flash profiles do not. Run `backup`
before updating and paste it back afterward.

**Can I control the oven entirely from a PC?**
Yes. The [serial console](SERIAL.md) exposes reflow, bake, profiles, settings, and
calibration, and streams CSV or JSON telemetry.

**How long is a reflow profile?**
48 points, 10 seconds apart, so up to 7:50 (470 seconds).

**What is the absolute temperature limit?**
The firmware hard-aborts above 280 °C regardless of setpoint. This cannot be
disabled. See [Safety](SAFETY.md).

**Why does turning off "Buzzer alerts" make the oven silent at the end?**
That setting gates the stage beeps, the completion melody, and the runaway alarm
melody. The runaway screen still emits a plain warning beep.

**Can I leave the oven running unattended?**
No. Never leave a running reflow oven unattended, regardless of the software
protections. See [Safety](SAFETY.md).

---

## Glossary

- **Bang-bang**: On/off heater control (full power or none) instead of continuous
  PWM. Better for ovens whose heater responds poorly to PWM.
- **Cold junction**: The reference point where the thermocouple wires meet the
  measuring circuit. Its temperature must be known (via the DS18B20 sensor) to
  convert a thermocouple voltage into an accurate temperature.
- **IAP**: In-Application Programming. LPC ROM routines the firmware uses to write
  flash (here, to store flash profiles).
- **ISP**: In-System Programming. The LPC bootloader mode used to flash new
  firmware over serial.
- **Kp / Ki / Kd**: The proportional, integral, and derivative gains of the PID
  controller.
- **Liquidus**: The temperature at which solder fully melts (217 °C for SAC305).
- **PID**: Proportional-Integral-Derivative control, a feedback loop that adjusts
  heater power based on the temperature error and its trend.
- **Preheat**: An optional soak at a set temperature before the reflow profile
  begins.
- **Reflow**: Heating a board through a temperature profile to melt solder paste
  and form joints.
- **Soak**: A period of near-constant temperature that lets the board equalise and
  the flux activate before the peak.
- **SSR**: Solid-State Relay, the switch that drives the heater.
- **TAL**: Time Above Liquidus, seconds spent above the solder's melting point
  (measured above 217 °C).
- **Thermocouple (TC)**: The temperature sensor probe that reads the oven/board
  temperature.
- **Ziegler-Nichols**: A classic method for computing PID gains from an oven's
  measured oscillation. Used by PID auto-tune.
