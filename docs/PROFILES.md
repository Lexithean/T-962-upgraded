# Reflow Profiles

A **profile** is the temperature curve the oven follows during a reflow: 48
setpoints spaced 10 seconds apart, for a run of up to **480 seconds**. ReflowOS
gives you three kinds of profile storage.

<img src="images/06-select-profile.png" width="440" alt="A profile plotted on the oven">

- [Built-in profiles](#built-in-profiles)
- [CUSTOM profiles (EEPROM)](#custom-profiles-eeprom)
- [Flash profiles (up to 32)](#flash-profiles)
- [Editing on the oven](#editing-on-the-oven)
- [Editing over serial](#editing-over-serial)
- [Backup & restore](#backup--restore)

---

## Built-in profiles

These ship in firmware and are read-only. Select them with **F4** on the oven or
`select profile <id>` over serial.

| id | Name | Use |
|----|------|-----|
| 0 | **SAC305 Leadfree** | Standard lead-free SAC305 paste |
| 1 | **AMTECH SYNTECH-LF** | Amtech SynTECH-LF lead-free paste |
| 2 | **NC-31 LOW-TEMP LF** | Low-temperature lead-free |
| 3 | **4300 63SN/37PB** | Leaded 63/37 tin-lead |
| 4 | **LOCTITE GC 10** | Loctite GC 10 (SAC305-class) |
| 5 | **LOCTITE GC 50** | Loctite GC 50 |
| 6 | **CUSTOM #1** | Editable EEPROM slot (see below) |
| 7 | **CUSTOM #2** | Editable EEPROM slot |

> Always match the profile to the **paste you are actually using** — follow the
> paste manufacturer's reflow spec (soak time, peak temperature, time above
> liquidus). The built-in curves are sensible starting points, not guarantees.

---

## CUSTOM profiles (EEPROM)

**CUSTOM #1** and **CUSTOM #2** (ids 6 and 7) are stored in the oven's EEPROM,
so they persist across power cycles. You can:

- Edit them point-by-point [on the oven](#editing-on-the-oven).
- [Import](#editing-over-serial) them over serial.
- [Rename](#editing-over-serial) them (`name profile 1 …`).

EEPROM profiles **survive a firmware update**, but it is still good practice to
[back them up](#backup--restore).

---

## Flash profiles

Beyond the two EEPROM slots, ReflowOS stores up to **32 more profiles** in a
dedicated MCU flash sector (slots 0–31), managed entirely over serial:

```
save flash 5 50,90,130,...,245,...,50,My Profile
list flash
delete flash 5
```

Flash profiles appear in the on-oven profile selector alongside the built-ins,
so you can save a library of pastes and pick them from the front panel.

Two Loctite profiles are **preloaded** into slots 30 and 31 on first boot as
examples.

> ⚠️ **Flash profiles are erased when you reflash the firmware.** Run
> [`backup`](#backup--restore) first. (EEPROM CUSTOM profiles survive; flash
> profiles do not.)

### How it works

Flash storage uses the LPC's IAP (In-Application Programming) ROM to write a
reserved flash sector. Writes are atomic per profile from the user's point of
view, and out-of-range slots, over-long temperatures, and malformed input are
rejected. The profile **name is preserved** even when you provide the full
48-point list.

---

## Editing on the oven

From the profile selector (**F4**), highlight CUSTOM #1 or #2 and press **F3**:

<img src="images/09-edit-profile.png" width="440" alt="Profile editor">

| Key | Action |
|-----|--------|
| **F1 / F2** | Move to the previous / next time point (0:00 … 7:50) |
| **F3 / F4** | Lower / raise the setpoint at the cursor (0–300 °C, hold to accelerate) |
| **S** | Save to EEPROM and return |

The header shows the cursor's time and setpoint; the curve redraws as you edit.
Set a point (and all following points) to 0 to end the profile early.

---

## Editing over serial

Inspect any profile:

```
list profiles
dump profile 0            # raw 48 points
export profile 6          # as an "import profile 1 ..." line for round-tripping
```

Import into a CUSTOM slot — comma-separated °C values, one per 10-second step:

```
import profile 1 50,90,130,150,160,170,180,200,230,245,240,210,170,120,80
name profile 1 My Leaded Paste
```

Missing trailing points are treated as 0 (profile ends there). See the
[Serial Reference](SERIAL.md#profile-management-over-serial) for the full syntax.

---

## Backup & restore

Before **any** firmware update, dump everything to your terminal:

```
backup
```

Output looks like:

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

Save that text. After reflashing, paste it back into the console (a few lines at
a time — see the [paste caution](SERIAL.md#backup--restore)) to restore both
CUSTOM EEPROM profiles and all flash profiles, names included.
