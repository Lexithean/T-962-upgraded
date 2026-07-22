# Settings Reference

This is every configurable setting in ReflowOS, in the order it appears in the
Setup / calibration menu (press **F2** from the main menu). Each one is stored in
non-volatile EEPROM and survives power cycles.

You can change any setting two ways:

- On the oven, through the Setup menu, using F3/F4 to adjust (hold to accelerate).
- Over serial, with `setting <id> <value>`, where `<id>` is the index in the table
  below and `<value>` is the real-world value. For example, `setting 6 60` sets
  preheat to 60 °C. See the [Serial Reference](SERIAL.md#command-summary).

The **#** column is the id you use with the `setting` command. Out-of-range ids and
values are rejected or clamped.

| # | Setting | Range | Default | What it does |
|---|---------|-------|---------|--------------|
| 0 | **Min fan speed** | OFF to MAX (0 to 254) | 8 | The idle/standby fan PWM floor. Keeps air moving so the cavity and electronics do not heat-soak. |
| 1 | **Cycle done beep** | OFF to MAX, 0.1 s steps | 1.0 s | How long the completion buzzer sounds at the end of a reflow or bake. |
| 2 | **Left TC gain** | 0.10 to 1.90 | 1.00 | Multiplicative gain correction for the left thermocouple. 1.00 means no change. |
| 3 | **Left TC offset** | plus or minus 63.50 °C, 0.5 °C steps | +0.00 | Additive offset for the left thermocouple at ambient (low temperature). |
| 4 | **Right TC gain** | 0.10 to 1.90 | 1.00 | Gain correction for the right thermocouple. |
| 5 | **Right TC offset** | plus or minus 63.50 °C, 0.5 °C steps | +0.00 | Ambient offset for the right thermocouple. |
| 6 | **Preheat temp** | 30 to 80 °C | 50 °C | The temperature the oven soaks at before the reflow profile clock starts. |
| 7 | **Bang-bang heat** | OFF / ON | OFF | Selects bang-bang (on/off heater) control instead of PID. Strongly recommended for the T-962C. See [Calibration](CALIBRATION.md#control-modes-pid-vs-bang-bang). |
| 8 | **BB heat offset** | 0 to 25 °C | 0 | How early bang-bang cuts the heater to account for lag. Set by bang-bang auto-tune. |
| 9 | **BB cool offset** | 0 to 25 °C | 0 | The equivalent anticipatory margin for cooling. Set by auto-tune. |
| 10 | **PID Kp** | DEFAULT, or 0 to 127.0 | DEFAULT | PID proportional gain. `DEFAULT` uses the built-in tuning; any value you set overrides it. |
| 11 | **PID Ki** | DEFAULT, or 0 to 0.508 | DEFAULT | PID integral gain. |
| 12 | **PID Kd** | DEFAULT, or 0 to 127.0 | DEFAULT | PID derivative gain. |
| 13 | **Screensaver mins** | OFF to 60 min | OFF | Blank the LCD after this many minutes of inactivity. Any key wakes it. |
| 14 | **Runaway thresh** | OFF to 50 °C | 30 °C | The thermal-runaway abort margin above setpoint. `OFF` disables the relative check, but the absolute 280 °C ceiling still applies. See [Safety](SAFETY.md). |
| 15 | **Buzzer alerts** | OFF / ON | ON | Stage-transition beeps during a reflow (ramp, peak, cooling). |
| 16 | **Max cool rate** | UNLIMIT to 5.0 °C/s, 0.1 steps | UNLIMIT | Caps the fan to limit the cooldown rate and reduce thermal shock. `UNLIMIT` means no limit. |
| 17 | **L TC hi-off** | plus or minus 63.50 °C, 0.5 °C steps | tracks #3 | The left thermocouple offset at high temperature (200 °C reference), for two-point calibration. |
| 18 | **R TC hi-off** | plus or minus 63.50 °C, 0.5 °C steps | tracks #5 | The right thermocouple high-temperature offset. |
| 19 | **Temp unit** | DEG C / DEG F | DEG C | The display unit. It affects the on-screen and serial temperatures. Control is always Celsius underneath. |
| 20 | **Fan kickstart** | OFF / ON | OFF | Fires a 250 ms full-power fan pulse when the fan first turns on, to break a low-speed stall. |
| . | **Factory Reset** | . | . | The virtual bottom entry. Press **S** to restore every setting above to its default. |

## Notes on specific settings

### Two-point thermocouple calibration (#3/#5 and #17/#18)

The offset applied to each thermocouple depends on temperature. The firmware
linearly interpolates between the ambient offset (#3/#5, referenced at 25 °C) and
the high-temperature offset (#17/#18, referenced at 200 °C). If you never set a
high-temperature offset, it tracks the ambient one and you get a constant
correction. Setting both lets you correct a thermocouple that reads accurately at
room temperature but drifts at reflow temperatures.

The offset encoding is `(NV minus 127) times 0.5 °C`, so the value shown on screen
and the correction actually applied are on the same 0.5 °C-per-step scale.

### PID gains (#10 to #12)

Leave these at `DEFAULT` unless you have run [PID auto-tune](CALIBRATION.md#pid-auto-tune)
or are tuning by hand. Auto-tune writes computed values here, and `factory reset`
returns them to `DEFAULT`.

### Bang-bang offsets (#8/#9)

These only matter when **Bang-bang heat** (#7) is on. They tell the controller how
early to cut heat and cool to account for thermal lag.
[Bang-bang auto-tune](CALIBRATION.md#bang-bang-auto-tune) measures and sets them
for you.

### Operational mode (serial only)

Beyond the menu settings, you can change the control source over serial with
`set OpMode`:

| Mode | Value | Behaviour |
|------|-------|-----------|
| **AMBIENT** | 0 | Control on the average of the two main thermocouples. This is the default. |
| **MAXTEMPOVERRIDE** | 1 | If any thermocouple reads more than the threshold above the average, control on that hottest sensor. Protects sensitive parts. This needs the 4-channel external TC interface to do anything; on stock 2-channel ADC hardware it has no effect. |
| **SPLIT** | 2 | Use the two control thermocouples until the threshold, then switch to the board-surface thermocouples for accurate reflow tracking. This needs the external TC interface with board-surface probes. |

Set the threshold with `set OpThresh <°C>`. See the
[Serial Reference](SERIAL.md#operational-mode).
