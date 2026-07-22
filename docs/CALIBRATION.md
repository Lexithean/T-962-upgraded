# Calibration & Tuning

Accurate reflow depends on two things: thermocouples that read the *true*
temperature, and a controller tuned to your oven's thermal behaviour. ReflowOS
automates both.

- [Control modes: PID vs bang-bang](#control-modes-pid-vs-bang-bang)
- [Thermocouple offset auto-calibration](#thermocouple-offset-auto-calibration)
- [Two-point calibration](#two-point-calibration)
- [Bang-bang auto-tune](#bang-bang-auto-tune)
- [PID auto-tune](#pid-auto-tune)
- [Operational modes](#operational-modes)

---

## Control modes: PID vs bang-bang

ReflowOS can drive the heater two ways, chosen by the **Bang-bang heat** setting
(#7):

| Mode | How it drives the heater | Best for |
|------|--------------------------|----------|
| **PID** (default) | Continuous PWM proportional to error, with integral/derivative terms | T-962 / T-962A with a responsive heater |
| **Bang-bang** | Full ON below target, full OFF above (with anticipatory offsets) | **T-962C** and other ovens whose heater responds poorly to PWM |

Each mode has its own auto-tune. Pick the mode first (Setup → *Bang-bang heat*),
then run the matching tune.

---

## Thermocouple offset auto-calibration

**When:** the oven is at room temperature and you have a working cold-junction
sensor (DS18B20/DS18S20/DS1822 on the TC terminal block).

**What it does:** at ambient, every thermocouple *should* read the same as the
cold-junction reference. `tccal` measures the difference and writes per-TC offset
corrections so both thermocouples read true.

Run it from the Setup menu (press **S** on a *TC offset* row) or over serial:

```
tccal
```

Typical output:

```
TC Cal: ref=24.5C L=26.1(err=+1.6) R=23.9(err=-0.6)
TC Cal: NV offset L=127->124 R=127->128
```

Notes:

- The oven **must be cool** — `tccal` refuses if it is too hot (the reading
  wouldn't be a valid ambient reference).
- It requires a **cold-junction sensor**; without one it reports an error.
- It calibrates against that reference sensor, so the reference must itself be
  trustworthy. This is an *ambient* (single-point) calibration; if you also set a
  high-temp offset it is carried along so the correction holds across the range.

---

## Two-point calibration

A thermocouple can read accurately at room temperature yet drift at reflow
temperatures. ReflowOS supports a **per-TC, temperature-dependent** offset:

- **Ambient offset** (settings #3 / #5) — referenced at 25 °C.
- **High-temp offset** (settings #17 / #18) — referenced at 200 °C.

The firmware **linearly interpolates** between them, so the correction applied at
100 °C is halfway between the two. If you leave the high-temp offset unset it
tracks the ambient one (constant correction).

To use it: run `tccal` (or set the ambient offset by hand) for room temperature,
then compare against a reference thermometer near your peak temperature and set
the high-temp offset (#17/#18) to close the remaining error.

---

## Bang-bang auto-tune

**Requires:** *Bang-bang heat* (#7) = **ON**.

Measures how far the oven overshoots when heating and undershoots when cooling,
then stores anticipatory **heat/cool offsets** (#8/#9) so the controller cuts
heat/cool early enough to hit the target.

<img src="images/16-bbtune-prompt.png" width="380" alt="Bang-bang tune prompt">
<img src="images/17-bbtune-running.png" width="380" alt="Bang-bang tune running">

1. Insert a scrap PCB (representative thermal mass).
2. Start it: Setup → a *Bang-bang* row → **S**, then **F1 = START**; or serial
   `bbtune`.
3. It runs **3 heat/cool cycles** between target temperatures, drawing a live
   graph with the target band dotted and your temperature traced.
4. On completion it prints and stores the measured offsets.

---

## PID auto-tune

**Requires:** *Bang-bang heat* (#7) = **OFF**.

Uses the **Ziegler-Nichols relay method**: it drives the heater as a relay around
a target, measures the natural oscillation period and amplitude over 3 cycles,
and computes optimal **Kp / Ki / Kd**, which it writes to settings #10–#12.

<img src="images/18-pidtune-prompt.png" width="380" alt="PID tune prompt">
<img src="images/19-pidtune-running.png" width="380" alt="PID tune running">

1. Insert a scrap PCB.
2. Start it: Setup → a *PID* row → **S**, then **F1 = START**; or serial `pidtune`.
3. Watch the live graph; on completion the new gains are stored and used.

To revert to the built-in tuning, set PID Kp/Ki/Kd back to `DEFAULT` (or
`factory reset`).

### Safety during tuning

Both tune routines have a **per-phase timeout**. If a phase stalls — for example
the oven can't reach the target, or has cooled to ambient with no rebound — the
tune **aborts cleanly with the heater off** and leaves your stored values
unchanged, instead of hanging indefinitely. The absolute over-temperature cutoff
is also active throughout.

---

## Operational modes

The **control source** (which thermocouple(s) drive the loop) is set over serial
with `set OpMode`:

| Mode | Value | Behaviour | Hardware |
|------|-------|-----------|----------|
| **AMBIENT** | 0 | Average of the two control thermocouples (default) | Any |
| **MAXTEMPOVERRIDE** | 1 | Control on the hottest TC when it exceeds average + threshold | Needs the 4-channel external TC interface — **no effect on stock 2-channel ADC hardware** |
| **SPLIT** | 2 | Control TCs until threshold, then switch to board-surface probes | Needs the external TC interface with surface probes |

```
set OpMode 2
set OpThresh 5
get OpMode
```

If you have a stock T-962 with only the two built-in thermocouples, leave this at
**AMBIENT** — MAXTEMPOVERRIDE and SPLIT are designed for the enhanced 4-probe
thermocouple interface described in the project wiki.
