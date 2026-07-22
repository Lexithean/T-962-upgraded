# User Guide — On-Oven Interface

Everything the T-962 ReflowOS firmware does from the front panel: every screen,
every key, and how to run a reflow, bake a board, edit a profile, and calibrate.

All screenshots on this page are rendered from the **real firmware** running in
the [host simulator](BUILDING.md#simulator), so they match the pixels you see on
the oven's 128×64 LCD.

- [The keypad](#the-keypad)
- [Main menu](#main-menu)
- [Running a reflow](#running-a-reflow)
- [Bake / manual mode](#bake--manual-mode)
- [Selecting a profile](#selecting-a-profile)
- [Editing a custom profile](#editing-a-custom-profile)
- [Setup / calibration menu](#setup--calibration-menu)
- [Auto-tune screens](#auto-tune-screens)
- [About screen](#about-screen)
- [Screensaver](#screensaver)

---

## The keypad

The oven has five keys. Their meaning changes per screen and is always labelled
on-screen in an inverted (white) box next to the relevant option.

| Key | Typical role |
|-----|--------------|
| **F1** | Left / previous / decrease |
| **F2** | Right / next / increase / toggle display |
| **F3** | Adjust down (context) |
| **F4** | Adjust up (context) |
| **S** | Select / Start / Stop / Back |

Holding a key **auto-repeats with acceleration** — the longer you hold, the
faster the value changes. This applies to setpoint, timer, and profile-value
adjustments.

---

## Main menu

<img src="images/01-main-menu.png" width="440" alt="Main menu">

The home screen. Each row maps to a key:

| Key | Action |
|-----|--------|
| **F1** | [About](#about-screen) |
| **F2** | [Setup / calibration](#setup--calibration-menu) |
| **F3** | [Bake / manual mode](#bake--manual-mode) |
| **F4** | [Select profile](#selecting-a-profile) |
| **S** | [Run reflow](#running-a-reflow) with the selected profile |

The bottom two lines show the **currently selected profile** (highlighted) and
the **live oven temperature** in your chosen unit (°C or °F).

---

## Running a reflow

Press **S** from the main menu to start a reflow with the selected profile.

### Preheat

If a **preheat temperature** is configured (Setup → *Preheat temp*), the oven
first heats to that temperature and holds before the profile clock starts. The
screen header shows `PREHEAT` during this phase. The profile always begins at
**index 0** the moment preheat completes — the initial ramp and soak are never
skipped.

### Live reflow view

<img src="images/12-reflow-ramp.png" width="440" alt="Reflow running — graph view">

- **SET** — the current profile setpoint (target). Blinks if the oven is more
  than 5 °C below target.
- **ACTUAL** — the measured oven temperature. Blinks if more than 5 °C above target.
- **Top-right** — elapsed time (`MM:SS`) and, below it, time remaining
  (`-MM:SS`, the leading dash marks it as a countdown).
- **Graph** — the target profile (dotted) with your live temperature trace drawn
  over it, left-to-right.
- **F2 = HOLD** pauses the profile clock (lets heat soak at the current setpoint).
- **S = STOP** aborts the reflow and returns to standby with the heater off.

Press **F1** to toggle to the **big-number view**, which shows setpoint, actual
temperature, and remaining time as large digits with bar graphs — easier to read
across the room.

### Peak and completion

<img src="images/13-reflow-peak.png" width="440" alt="Reflow at peak">

When the profile finishes, the oven turns the heater off, sounds the completion
buzzer, and shows the **post-reflow analytics**:

<img src="images/14-reflow-complete.png" width="440" alt="Reflow complete with analytics">

| Metric | Meaning |
|--------|---------|
| **PEAK** | Highest temperature reached during the run |
| **TAL (>217)** | Time Above Liquidus — seconds spent above 217 °C (the SAC305 liquidus point) |
| **MAX RAMP** | Fastest heating rate observed, in °C/s |

Press **F4** for the menu or **S** to stop. Use these numbers to judge whether
the run matched your solder paste's reflow spec.

### If something goes wrong

<img src="images/15-thermal-runaway.png" width="440" alt="Thermal runaway alert">

If the temperature exceeds the setpoint by more than the configured runaway
threshold, **or** exceeds the absolute ceiling (280 °C) regardless of setpoint,
the firmware aborts immediately: heater **off**, fan **on**, alarm buzzer, and
this screen. Press any key to dismiss. See [Safety](SAFETY.md) for the full list
of protections.

---

## Bake / manual mode

Press **F3** from the main menu. Bake mode holds a fixed temperature — useful for
drying boards, curing, or preheating.

<img src="images/10-bake-celsius.png" width="440" alt="Bake mode (Celsius)">

| Key | Action |
|-----|--------|
| **F1 / F2** | Decrease / increase the setpoint (30–300 °C) |
| **F3 / F4** | Decrease / increase the timer |
| **S** | Stop and return to the menu |

- The **timer** counts down only *after* the oven reaches setpoint (it shows
  `PREHEAT` until then). When it expires, the oven beeps and returns to standby.
- Set the timer to zero for an **untimed** bake that holds until you stop it.
- Timers up to **36 hours** are supported and will reliably expire (an overnight
  bake shuts itself off).

The display honours your **temperature unit** setting. In Fahrenheit mode the
setpoint and all readings are shown in °F, while the oven is still controlled
correctly internally:

<img src="images/11-bake-fahrenheit.png" width="440" alt="Bake mode (Fahrenheit)">

---

## Selecting a profile

Press **F4** from the main menu.

<img src="images/06-select-profile.png" width="440" alt="Select profile — SAC305">

- **F1 / F2** cycle through every available profile (built-in, custom, and
  flash-stored), wrapping around at both ends.
- The selected profile's **curve is plotted** so you can eyeball the ramp, soak,
  peak, and cooldown before committing.
- **S** selects the profile and returns to the menu.

For the two editable **CUSTOM** profiles, an **F3 = EDIT** button also appears:

<img src="images/08-select-custom.png" width="440" alt="Select a custom profile">

See [Profiles](PROFILES.md) for the full list of built-in profiles and how the
custom/flash storage works.

---

## Editing a custom profile

From the profile selector, choose CUSTOM #1 or CUSTOM #2 and press **F3**.

<img src="images/09-edit-profile.png" width="440" alt="Profile editor">

A profile is **48 setpoints spaced 10 seconds apart** (up to 480 s total).

| Key | Action |
|-----|--------|
| **F1 / F2** | Move the cursor to the previous / next time point |
| **F3 / F4** | Lower / raise the setpoint at the cursor (0–300 °C) |
| **S** | Save to EEPROM and return |

The header shows the cursor's time (`M:SS`) and setpoint. The curve redraws live
as you edit. Setting a point (and everything after it) to 0 ends the profile
early. Edits are written to EEPROM, so they survive power cycles — but **not**
firmware updates (use [`backup`](SERIAL.md#backup--restore) first).

---

## Setup / calibration menu

Press **F2** from the main menu.

<img src="images/03-setup-top.png" width="440" alt="Setup menu top">

| Key | Action |
|-----|--------|
| **F1 / F2** | Previous / next setting (the list scrolls) |
| **F3 / F4** | Decrease / increase the highlighted value (hold to accelerate) |
| **S** | On most rows: exit to menu. On calibration/tune rows: launch that routine (see below). |

Scroll down for the thermocouple calibration and control-tuning rows:

<img src="images/04-setup-calibration.png" width="440" alt="Setup calibration rows">

At the very bottom is a **Factory Reset** entry — press **S** on it to restore
all settings to defaults:

<img src="images/05-setup-factory-reset.png" width="440" alt="Factory reset entry">

Every setting is documented in the [Settings Reference](SETTINGS.md).

### Launching calibration from the menu

Pressing **S** while certain rows are highlighted starts a routine instead of
exiting:

- **Left/Right TC offset** rows → [TC offset auto-calibration](CALIBRATION.md#thermocouple-offset-auto-calibration)
- **Bang-bang** rows (when bang-bang mode is ON) → [Bang-bang auto-tune](CALIBRATION.md#bang-bang-auto-tune)
- **PID** rows (when bang-bang mode is OFF) → [PID auto-tune](CALIBRATION.md#pid-auto-tune)

---

## Auto-tune screens

Both auto-tune routines open with a confirmation prompt:

<img src="images/16-bbtune-prompt.png" width="440" alt="Bang-bang tune prompt">

Insert a scrap PCB for representative thermal mass, then press **F1 = START**
(or **S = BACK** to cancel). The tune then runs several heat/cool cycles with a
live temperature graph:

<img src="images/17-bbtune-running.png" width="440" alt="Bang-bang tune running">

The header shows the cycle count and current phase (`HEAT`, `COOL`, etc.), the
dotted lines mark the target band, and the solid trace is the live temperature.
PID tuning (Ziegler-Nichols relay method) works the same way:

<img src="images/18-pidtune-prompt.png" width="440" alt="PID tune prompt">
<img src="images/19-pidtune-running.png" width="440" alt="PID tune running">

Auto-tune has a built-in **timeout**: if any phase stalls (e.g. the oven can't
reach the target), it aborts cleanly with the heater off rather than hanging.
Full details in [Calibration](CALIBRATION.md).

---

## About screen

Press **F1** from the main menu.

<img src="images/02-about.png" width="440" alt="About screen">

Shows the firmware name, maintainer, build version, and upstream credit. Press
**S** (or any key) to return. On a real build the `VER` line shows the git
version stamped in at compile time.

---

## Screensaver

If **Screensaver mins** (Setup) is non-zero, the display blanks after that many
minutes of inactivity to protect the LCD. Any keypress wakes it. Set it to
`OFF` to disable.
