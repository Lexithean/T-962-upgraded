# Calibration & Tuning

Accurate reflow comes down to two things: thermocouples that read the true
temperature, and a controller tuned to how your oven actually behaves. ReflowOS
automates both.

- [Control modes: PID vs bang-bang](#control-modes-pid-vs-bang-bang)
- [Thermocouple offset auto-calibration](#thermocouple-offset-auto-calibration)
- [Two-point calibration](#two-point-calibration)
- [Bang-bang auto-tune](#bang-bang-auto-tune)
- [PID auto-tune](#pid-auto-tune)
- [Operational modes](#operational-modes)

---

## Control modes: PID vs bang-bang

ReflowOS can drive the heater two ways. You pick between them with the
**Bang-bang heat** setting (#7):

| Mode | How it drives the heater | Best for |
|------|--------------------------|----------|
| **PID** (the default) | Continuous PWM proportional to the error, with integral and derivative terms | A T-962 or T-962A with a responsive heater |
| **Bang-bang** | Full on below target, full off above (with anticipatory offsets) | The T-962C and other ovens whose heater responds poorly to PWM |

Each mode has its own auto-tune. Pick the mode first (Setup, then *Bang-bang
heat*), then run the matching tune.

---

## Thermocouple offset auto-calibration

Use this when the oven is at room temperature and you have a working cold-junction
sensor (a DS18B20, DS18S20, or DS1822 on the TC terminal block).

At ambient, every thermocouple should read the same as the cold-junction
reference. `tccal` measures the difference and writes per-thermocouple offset
corrections so both read true. Run it from the Setup menu (press **S** on a *TC
offset* row) or over serial:

```
tccal
```

Typical output:

```
TC Cal: ref=24.5C L=26.1(err=+1.6) R=23.9(err=-0.6)
TC Cal: NV offset L=127->124 R=127->128
```

A few things to know:

- The oven must be cool. `tccal` refuses if it is too hot, because a hot reading is
  not a valid ambient reference.
- It needs a cold-junction sensor. Without one it reports an error.
- It calibrates against that reference sensor, so the reference itself has to be
  trustworthy. This is an ambient (single-point) calibration. If you have also set
  a high-temperature offset, it is carried along so the correction holds across the
  range.

---

## Two-point calibration

A thermocouple can read accurately at room temperature yet drift at reflow
temperatures. ReflowOS handles this with a per-thermocouple offset that changes
with temperature:

- The ambient offset (settings #3 and #5) is referenced at 25 °C.
- The high-temperature offset (settings #17 and #18) is referenced at 200 °C.

The firmware interpolates linearly between the two, so the correction it applies at
100 °C sits halfway between them. If you leave the high-temperature offset unset,
it tracks the ambient one and you get a constant correction.

To use it, run `tccal` (or set the ambient offset by hand) for room temperature.
Then compare against a reference thermometer near your peak temperature and set the
high-temperature offset (#17 or #18) to close the remaining error.

---

## Bang-bang auto-tune

This needs **Bang-bang heat** (#7) set to on.

It measures how far the oven overshoots when heating and undershoots when cooling,
then stores anticipatory heat and cool offsets (#8 and #9) so the controller cuts
heat and cool early enough to hit the target.

<img src="images/16-bbtune-prompt.png" width="380" alt="Bang-bang tune prompt">
<img src="images/17-bbtune-running.png" width="380" alt="Bang-bang tune running">

1. Insert a scrap PCB for representative thermal mass.
2. Start it from Setup (a *Bang-bang* row, then **S**, then **F1 (START)**), or with
   the serial command `bbtune`.
3. It runs three heat and cool cycles between target temperatures, drawing a live
   graph with the target band dotted and your temperature traced over it.
4. When it finishes, it prints and stores the measured offsets.

---

## PID auto-tune

This needs **Bang-bang heat** (#7) set to off.

It uses the Ziegler-Nichols relay method. The firmware drives the heater as a relay
around a target, measures the natural oscillation period and amplitude over three
cycles, and computes the optimal Kp, Ki, and Kd. It writes those to settings #10
through #12.

<img src="images/18-pidtune-prompt.png" width="380" alt="PID tune prompt">
<img src="images/19-pidtune-running.png" width="380" alt="PID tune running">

1. Insert a scrap PCB.
2. Start it from Setup (a *PID* row, then **S**, then **F1 (START)**), or with the
   serial command `pidtune`.
3. Watch the live graph. When it finishes, the new gains are stored and used.

To go back to the built-in tuning, set PID Kp, Ki, and Kd back to `DEFAULT` (or run
`factory reset`).

### Safety during tuning

Both tune routines have a per-phase timeout. If a phase stalls, say the oven cannot
reach the target, or it has cooled to ambient with no rebound, the tune aborts
cleanly with the heater off instead of hanging. The absolute over-temperature
cutoff stays active throughout.

---

## Operational modes

The control source, meaning which thermocouples drive the loop, is set over serial
with `set OpMode`:

| Mode | Value | Behaviour | Hardware |
|------|-------|-----------|----------|
| **AMBIENT** | 0 | Average of the two control thermocouples (the default) | Any |
| **MAXTEMPOVERRIDE** | 1 | Control on the hottest TC when it exceeds the average plus the threshold | Needs the 4-channel external TC interface. It has no effect on stock 2-channel ADC hardware. |
| **SPLIT** | 2 | Control TCs until the threshold, then switch to board-surface probes | Needs the external TC interface with surface probes |

```
set OpMode 2
set OpThresh 5
get OpMode
```

If you have a stock T-962 with only the two built-in thermocouples, leave this at
AMBIENT. MAXTEMPOVERRIDE and SPLIT are designed for the enhanced 4-probe
thermocouple interface described in the project wiki.
