# Settings Reference

Every configurable setting in ReflowOS, in the order it appears in the
**Setup / calibration** menu (press **F2** from the main menu). Each is stored
in non-volatile EEPROM and survives power cycles.

You can change any setting two ways:

- **On the oven** — Setup menu, F3/F4 to adjust (hold to accelerate).
- **Over serial** — `setting <id> <value>`, where `<id>` is the index in the
  table below and `<value>` is the **real-world value** (e.g. `setting 6 60`
  sets preheat to 60 °C). See [Serial Reference](SERIAL.md#setting).

The **Index** column is the id for the `setting` command. Out-of-range ids and
values are rejected/clamped.

| # | Setting | Range | Default | What it does |
|---|---------|-------|---------|--------------|
| 0 | **Min fan speed** | OFF–MAX (0–254) | 8 | Idle/standby fan PWM floor. Keeps airflow moving so the cavity and electronics don't heat-soak. |
| 1 | **Cycle done beep** | OFF–MAX, 0.1 s steps | 1.0 s | Length of the completion buzzer at the end of a reflow/bake. |
| 2 | **Left TC gain** | 0.10–1.90 | 1.00 | Multiplicative gain correction for the left thermocouple. 1.00 = no change. |
| 3 | **Left TC offset** | ±63.50 °C, 0.5 °C steps | +0.00 | Additive offset for the left thermocouple at ambient (low temp). |
| 4 | **Right TC gain** | 0.10–1.90 | 1.00 | Gain correction for the right thermocouple. |
| 5 | **Right TC offset** | ±63.50 °C, 0.5 °C steps | +0.00 | Ambient offset for the right thermocouple. |
| 6 | **Preheat temp** | 30–80 °C | 50 °C | Temperature the oven soaks at before the reflow profile clock starts. |
| 7 | **Bang-bang heat** | OFF / ON | OFF | Selects **bang-bang** (ON/OFF heater) control instead of PID. Strongly recommended for T-962C. See [Calibration](CALIBRATION.md#control-modes-pid-vs-bang-bang). |
| 8 | **BB heat offset** | 0–25 °C | 0 | Anticipatory shutoff margin for bang-bang heating — set by *bang-bang auto-tune*. |
| 9 | **BB cool offset** | 0–25 °C | 0 | Anticipatory margin for bang-bang cooling — set by auto-tune. |
| 10 | **PID Kp** | DEFAULT / 0–127.0 | DEFAULT | PID proportional gain. `DEFAULT` uses the built-in tuning; a set value overrides it. |
| 11 | **PID Ki** | DEFAULT / 0–0.508 | DEFAULT | PID integral gain. |
| 12 | **PID Kd** | DEFAULT / 0–127.0 | DEFAULT | PID derivative gain. |
| 13 | **Screensaver mins** | OFF–60 min | OFF | Blank the LCD after N minutes of inactivity. Any key wakes it. |
| 14 | **Runaway thresh** | OFF–50 °C | 30 °C | Thermal-runaway abort margin above setpoint. `OFF` disables the *relative* check (the absolute 280 °C ceiling still applies). See [Safety](SAFETY.md). |
| 15 | **Buzzer alerts** | OFF / ON | ON | Stage-transition beeps during a reflow (ramp, peak, cooling). |
| 16 | **Max cool rate** | UNLIMIT–5.0 °C/s, 0.1 steps | UNLIMIT | Caps the fan to limit the cooldown rate and reduce thermal shock. `UNLIMIT` = no limit. |
| 17 | **L TC hi-off** | ±63.50 °C, 0.5 °C steps | tracks #3 | Left thermocouple offset at high temp (200 °C reference) for two-point calibration. |
| 18 | **R TC hi-off** | ±63.50 °C, 0.5 °C steps | tracks #5 | Right thermocouple high-temp offset. |
| 19 | **Temp unit** | DEG C / DEG F | DEG C | Display unit. Affects on-screen and serial temperatures. Control is always internally Celsius. |
| 20 | **Fan kickstart** | OFF / ON | OFF | Fires a 250 ms full-power fan pulse when the fan first turns on, to break a low-speed stall. |
| — | **Factory Reset** | — | — | Virtual bottom entry. Press **S** to restore every setting above to its default. |

## Notes on specific settings

### Two-point thermocouple calibration (#3/#5 and #17/#18)

The offset applied to each thermocouple is **temperature-dependent**: the
firmware linearly interpolates between the **ambient offset** (#3/#5, referenced
at 25 °C) and the **high-temp offset** (#17/#18, referenced at 200 °C). If you
never set a high-temp offset, it tracks the ambient one, giving a constant
correction. Setting both lets you correct a thermocouple that reads accurately
at room temperature but drifts at reflow temperatures.

The offset encoding is `(NV − 127) × 0.5 °C`, so the on-screen value and the
correction actually applied are the same 0.5 °C-per-step scale.

### PID gains (#10–#12)

Leave these at `DEFAULT` unless you have run [PID auto-tune](CALIBRATION.md#pid-auto-tune)
or are tuning by hand. Auto-tune writes computed values here; `factory reset`
returns them to `DEFAULT`.

### Bang-bang offsets (#8/#9)

Only relevant when **Bang-bang heat** (#7) is ON. They tell the controller how
early to cut heat/cool to account for thermal lag. [Bang-bang auto-tune](CALIBRATION.md#bang-bang-auto-tune)
measures and sets them for you.

### Operational mode (serial only)

Beyond the menu settings, the control **source** can be changed over serial with
`set OpMode`:

| Mode | Value | Behaviour |
|------|-------|-----------|
| **AMBIENT** | 0 | Control on the average of the two main thermocouples (default). |
| **MAXTEMPOVERRIDE** | 1 | If any thermocouple reads more than the threshold above the average, control on that (hottest) sensor. Protects sensitive parts. *Requires the 4-channel external TC interface to be useful — on stock 2-channel ADC hardware it has no effect.* |
| **SPLIT** | 2 | Use the two control thermocouples until the threshold, then switch to the board-surface (extra) thermocouples for accurate reflow tracking. *Requires the external TC interface with board-surface probes.* |

Set the threshold with `set OpThresh <°C>`. See [Serial Reference](SERIAL.md#operational-mode).
