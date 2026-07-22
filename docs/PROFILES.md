# Reflow Profiles

A profile is the temperature curve the oven follows during a reflow. It is 48
setpoints spaced 10 seconds apart, so it runs from 0:00 to 7:50 (470 seconds).
ReflowOS gives you three kinds of profile storage.

<img src="images/06-select-profile.png" width="440" alt="A profile plotted on the oven">

- [Built-in profiles](#built-in-profiles)
- [CUSTOM profiles in EEPROM](#custom-profiles-in-eeprom)
- [Flash profiles](#flash-profiles)
- [Editing on the oven](#editing-on-the-oven)
- [Editing over serial](#editing-over-serial)
- [Backup and restore](#backup-and-restore)

---

## Built-in profiles

These ship in the firmware and are read-only. Select them with **F4** on the oven,
or `select profile <id>` over serial.

| id | Name | Use |
|----|------|-----|
| 0 | **SAC305 Leadfree** | Standard lead-free SAC305 paste |
| 1 | **AMTECH SYNTECH-LF** | Amtech SynTECH-LF lead-free paste |
| 2 | **NC-31 LOW-TEMP LF** | Low-temperature lead-free |
| 3 | **4300 63SN/37PB** | Leaded 63/37 tin-lead |
| 4 | **LOCTITE GC 10** | Loctite GC 10 (SAC305 class) |
| 5 | **LOCTITE GC 50** | Loctite GC 50 |
| 6 | **CUSTOM #1** | An editable EEPROM slot (see below) |
| 7 | **CUSTOM #2** | An editable EEPROM slot |

> Always match the profile to the paste you are actually using, and follow the
> paste maker's reflow spec for soak time, peak temperature, and time above
> liquidus. The built-in curves are sensible starting points, not guarantees.

---

## CUSTOM profiles in EEPROM

CUSTOM #1 and CUSTOM #2 (ids 6 and 7) are stored in the oven's EEPROM, so they
persist across power cycles. You can edit them point by point
[on the oven](#editing-on-the-oven), [import](#editing-over-serial) them over
serial, and rename them with `name profile 1 ...`.

EEPROM profiles survive a firmware update, but it is still good practice to
[back them up](#backup-and-restore).

---

## Flash profiles

Beyond the two EEPROM slots, ReflowOS keeps up to 32 more profiles in a dedicated
MCU flash sector (slots 0 to 31). You manage them entirely over serial:

```
save flash 5 50,90,130,...,245,...,50,My Profile
list flash
delete flash 5
```

Flash profiles show up in the on-oven profile selector alongside the built-ins, so
you can save a library of pastes and pick them from the front panel. Two Loctite
profiles are preloaded into slots 30 and 31 on first boot as examples. (These same
two curves also ship as the built-in profiles GC 10 and GC 50 at ids 4 and 5, so
you may see them listed twice, once as built-ins and once as flash profiles.)

> ⚠️ Flash profiles are erased when you reflash the firmware. Run
> [`backup`](#backup-and-restore) first. EEPROM CUSTOM profiles survive an update;
> flash profiles do not.

### How it works

Flash storage uses the LPC's IAP (In-Application Programming) ROM to write a
reserved flash sector. From your point of view each profile writes atomically, and
out-of-range slots, over-long temperatures, and malformed input are all rejected.
The profile name is preserved even when you provide the full 48-point list.

Each stored block carries a checksum. A block whose data is corrupt (for example
from a power cut mid-write) fails the checksum and is ignored on the next boot, so
the oven never runs a garbage profile. Writes are also read back and verified.
Storage is a single reserved sector, though, so a power cut during the brief
rewrite can still lose profiles; `backup` remains the way to guard against that.

---

## Editing on the oven

From the profile selector (**F4**), highlight CUSTOM #1 or #2 and press **F3**:

<img src="images/09-edit-profile.png" width="440" alt="Profile editor">

| Key | Action |
|-----|--------|
| **F1 / F2** | Move to the previous or next time point (0:00 up to 7:50, 48 points) |
| **F3 / F4** | Lower or raise the setpoint at the cursor (0 to 300 °C, hold to accelerate) |
| **S** | Save to EEPROM and return |

The header shows the cursor's time and setpoint, and the curve redraws as you edit.
Set a point (and every point after it) to 0 to end the profile early.

---

## Editing over serial

Inspect any profile:

```
list profiles
dump profile 0            # the raw 48 points
export profile 6          # as an "import profile 1 ..." line for round-tripping
```

Import into a CUSTOM slot with comma-separated °C values, one per 10-second step:

```
import profile 1 50,90,130,150,160,170,180,200,230,245,240,210,170,120,80
name profile 1 My Leaded Paste
```

Any points you leave off are treated as 0, and the profile ends there. The
[Serial Reference](SERIAL.md#managing-profiles-over-serial) has the full syntax.

---

## Backup and restore

Before any firmware update, dump everything to your terminal:

```
backup
```

The output looks like this:

```
# T-962 Profile Backup
# --- EEPROM profiles ---
import profile 1 50,90,130,...
import profile 2 60,100,140,...
# --- Flash profiles (lost on update) ---
save flash 30 50,70,90,...,LOCTITE GC 10
save flash 31 50,70,90,...,LOCTITE GC 50
# --- End of backup ---
```

Save that text. After reflashing, paste it back into the console a few lines at a
time (see the [note on pasting](SERIAL.md#backup-and-restore)) to restore both
CUSTOM EEPROM profiles and all flash profiles, names included.
