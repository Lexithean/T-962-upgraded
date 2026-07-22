# Serial Reference

ReflowOS gives you a full text console over the oven's USB/UART port. You can run
reflows, bake, manage profiles, change every setting, calibrate, and stream
telemetry, all without touching the front panel.

- Port settings: 115200 baud, 8 data bits, no parity, 1 stop bit (8N1).
- Line ending: newline (`\n`). Commands run as soon as a full line arrives, so even
  one-character commands like `?` work right away.

Connect with any serial terminal (`screen`, `minicom`, PuTTY, the Arduino Serial
Monitor, or the [`serial-control.py`](../serial-control.py) helper).

## Command summary

Type `help` (or `?`) on the oven console to print this list:

| Command | Description |
|---------|-------------|
| `about` | Firmware version, part info, EEPROM dump, and sensor values |
| `help` / `?` | Show the command list |
| `values` | Dump the currently measured temperatures |
| `list profiles` | List all selectable profiles with their ids |
| `list settings` | List every setting with its id and value |
| `list flash` | List the flash-stored profiles |
| `select profile <id>` | Select a profile by id |
| `reflow` | Start a reflow with the selected profile |
| `bake <setpoint>` | Untimed bake at `<setpoint>` °C |
| `bake <setpoint> <time>` | Bake at `<setpoint>` °C for `<time>` seconds (counted after reaching temperature) |
| `stop` | Exit reflow or bake |
| `setting <id> <value>` | Set a menu setting (see [Settings](SETTINGS.md)) |
| `set OpMode <0-2>` | Set the control source mode |
| `set OpThresh <0-255>` | Set the mode threshold in °C |
| `dump profile <id>` | Print a profile's 48 temperature points |
| `export profile <id>` | Print a profile in import-compatible form |
| `import profile <1 or 2> t,t,...` | Import points into CUSTOM #1 or #2 (EEPROM) |
| `name profile <1 or 2> <name>` | Rename a CUSTOM profile (18 characters or fewer) |
| `save flash <N> t,t,...,Name` | Save a profile to flash slot N (0 to 31) |
| `delete flash <N>` | Delete flash profile slot N |
| `backup` | Dump all profiles as restorable text |
| `json` | Toggle JSON telemetry output |
| `quiet` | Toggle standby-mode logging |
| `bbtune` | Run bang-bang auto-tune |
| `pidtune` | Run PID auto-tune |
| `tccal` | Run thermocouple offset auto-calibration |
| `factory reset` | Restore all settings to defaults |
| `enter isp` | Back up profiles, shut down the heater and fan, and jump to the bootloader for flashing |

---

## Telemetry

While a reflow or bake runs (and in standby, unless you silence it with `quiet`),
the oven streams one telemetry line per control cycle, about 4 times a second.

### CSV (the default)

A header line prints on each mode change, followed by data rows:

```
# Time,  Temp0, Temp1, Temp2, Temp3,  Set,Actual, Heat, Fan,  ColdJ, Mode
  12.5,  148.2, 146.9,   0.0,   0.0,  150, 147.6,  212,   8,   29.4, REFLOW
```

| Column | Meaning |
|--------|---------|
| `Time` | Seconds since the current mode started |
| `Temp0` to `Temp3` | The four thermocouple channels (0 and 1 are the control TCs, 2 and 3 are extra) |
| `Set` | Current setpoint in °C |
| `Actual` | Control temperature in °C |
| `Heat` | Heater PWM, 0 to 255 |
| `Fan` | Fan PWM, 0 to 255 |
| `ColdJ` | Cold-junction (reference) temperature |
| `Mode` | `STANDBY`, `PREHEAT`, `REFLOW`, `BAKE`, and so on |

### JSON

Run `json` to switch to machine-readable output for logging and graphing tools:

```json
{"t":12.5,"tc0":148.2,"tc1":146.9,"tc2":0.0,"tc3":0.0,"set":150,"act":147.6,"heat":212,"fan":8,"cj":29.4,"mode":"REFLOW"}
```

Run `json` again to switch back to CSV.

> Telemetry temperatures follow your **Temp unit** setting. Set it to °C for
> logging unless you specifically want °F.

---

## Managing profiles over serial

### Inspect

```
list profiles          # ids and names of every profile
dump profile 0         # the 48 raw points of profile 0
export profile 6       # CUSTOM #1 as an "import profile 1 ..." line
```

### Import into a CUSTOM slot

CUSTOM #1 and #2 live in EEPROM and are editable. Import comma-separated
temperatures in °C, one per 10-second step, up to 48 of them:

```
import profile 1 50,90,130,150,160,170,180,200,230,245,240,210,170,120,80
name profile 1 My Leaded Paste
```

### Flash profile storage

Up to 32 more profiles live in MCU flash, in slots 0 to 31:

```
save flash 5 50,90,130,...,245,...,50,My Profile   # temps, then a name
list flash                                          # slots in use
delete flash 5
```

A name is kept even after a full 48-point list, and it may start with a digit.

### Backup and restore

Before a firmware update, which erases flash profiles, export everything:

```
backup
```

This prints both CUSTOM EEPROM profiles and every flash profile as ready-to-paste
`import` and `save flash` commands. Save the output, and after reflashing, paste it
back to restore. Both CUSTOM #1 and CUSTOM #2 are included.

> A word on pasting: the oven's receive buffer is small and drained about twice a
> second. When you paste a multi-line backup, send a few lines at a time (or use a
> terminal with a short inter-line delay) so lines are not dropped.

---

## Calibration and tuning over serial

```
tccal        # zero both thermocouples against the cold-junction sensor (oven must be cool)
bbtune       # bang-bang auto-tune (needs Bang-bang heat set to ON)
pidtune      # PID auto-tune, Ziegler-Nichols (needs Bang-bang heat set to OFF)
```

The [Calibration guide](CALIBRATION.md) explains what each one does and when to use
it.

---

## Operational mode

```
get OpMode              # print the current mode
set OpMode 2            # 0 is AMBIENT, 1 is MAXTEMPOVERRIDE, 2 is SPLIT
get OpThresh
set OpThresh 5          # threshold in °C
```

See the [operational-mode note](SETTINGS.md#operational-mode-serial-only) for the
hardware each mode needs.

---

## Firmware update (ISP)

`enter isp` gets the oven ready for flashing. It backs up flash profiles to the
console, turns the heater and fan off, and jumps to the LPC bootloader. After it
runs, reconnect your flashing tool, program the new firmware, and power-cycle the
oven. See [Building & Flashing](BUILDING.md).
