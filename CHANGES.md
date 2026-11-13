# Changes

## Snoop37 2.8.5

- **SLOOP 2.8.5:** the UC25HQ80 flash chip (JEDEC 0xB36014, FM-1 units sold from late 2026) is known to the storage,
  USB RESCUE and the update loader. On the SMK-37 the accepted parts are 0xEB6014 (SMK-37 Pro), 0x856014 (Puya,
  SMK-37 Elite) and now 0xB36014, all in one place (firmware/hal/fm1_flash_id.h); any other chip stays RAM-only and
  the loader refuses, as before.
- About: `2.8.5 (SLOOP 2.8.5)`.

## Snoop37 2.8.4

- **SLOOP 2.8.4** (from 2.7.1, with 2.8, 2.8.1 and 2.8.3): the NOTES editor, the per-step sheet (OCT+ held), the
  playing section on TRACKS, FILTER RESO, the FX filter on the drums, the punch-in fade, MIDI Bank Select, held
  notes under others, drums that keep the kick, samples with their attack, the safer second core, clean note tails,
  UNISON apart, PHYS in tune, firm presses, REC READY's knobs; SLOOP's lists: README-SLOOP.md.
- **User samples:** SMP_INFO reports the SMK-37's two user slots (USR1, USR2), so the editor offers no USR3 / USR4
  (Samples, CHOP) and no USR4 / USR3+4 on its DRUM KIT page; ERASE of USR3 / USR4 answers done (always empty), so a
  restore of an FM-1 backup goes on; BEGIN there is still refused.
- **HOME menu:** AUDIO = SPEAKER LOWCUT, FILTER RESO, LIMITER (K5..K7); USB = USB AUDIO, USB RATE (a FELUCCA_CDC
  build: USB SERIAL between them); SEQ = STEP ADVANCE, DRUM PREVIEW. Four rows a section, as before.
- The LCD: 2.8's backlight helper for USB RESCUE drives PA2 on the FM-1 only (the SMK-37's backlight has no control).
- About: `2.8.4 (SLOOP 2.8.4)`.

## Snoop37 2.7.3

- **Sustain pedal** (the PEDAL jack): a footswitch on the jack holds the notes of the selected synth track
  until it is let up, as MIDI CC 64 in already did, and sends CC 64 (127 / 0) out over USB and the TRS jack on the
  track's channel (DAW mode: on CHANNEL). HOME menu > **PEDAL** > **SUSTAIN PEDAL** (K5): AUTO (default: the pedal's
  rest is learnt once it rests for 0.2 s, a middle reading is never taken), OPEN or CLOSED; the row shows UP, DOWN or
  WAITING. Kept with the SMK-37 settings (data of earlier builds reads as AUTO). The decoding is a small SMK-layer
  header (firmware/hal/smk_pedal.h) with a host test.
- **Battery bars** (reported as "always 1 bar"): the battery is read again right after its first conversion, on the
  same ADC channel, as the stock firmware reads it (the first came straight after a pad's on another channel and may
  read low); the stock thresholds (3 bars from 603, 2 from 561, 1 from 530); the smoothing no longer stops up to 31
  counts short. SYSTEM > ABOUT shows the reading (BAT now / the old way's).
- **Delay:** SMK37.md says how to turn it off (FX > DLY > MIX to 0 for every sound; a preset brings its own send).

## Snoop37 2.7.2

- **PAD LIGHT** (HOME menu > LIGHTS, K8): the pads at 100 % (default, as before), 75 %, 50 % or 25 %. The bright
  states follow the setting, the dim ones keep most of their light, so a dim pad never goes dark and dim and lit stay
  apart at every setting. Kept with the SMK-37 settings; data of earlier builds reads as 100 %.

## Snoop37 2.7.1

- **SLOOP 2.7.1:** a late second core is held with the bus-invalid traps off (after Felucca 1.6.1): no FELUCCA CRASH
  screen and restart, the sound goes on on one core; the crash screen shows the stack and the second core's state, 10 s.

## Snoop37 2.7.0

- **SLOOP 2.7:** the sound on both of the chip's cores (HOME menu > SYSTEM > 2ND CORE), the supplies at M-Vave's own
  levels for two cores, fifteen arp modes, KBD HOLD, drum TUNE / DECAY, favourite sounds, looping samples, QUANT off,
  GLO > MIDI 3, CC 74 per engine, KNOB ACCEL, USB RATE, your own screen colours, the editor's LIVE page
  ([README-SLOOP.md](README-SLOOP.md#whats-new-in-27)).
- The SMK-37 settings (KEY VELOCITY, TRS MIDI OUT, BEND UP / DOWN, LIMITER, KNOB LAYOUT, BOOT LOGO) in an object of
  their own (SLOOP 2.7 uses the bits they were kept in); 2.6.x values are read once from the old places.
- HOME menu: KNOB ACCEL on PLAY (K8), USB RATE on AUDIO.
- The volume knob (K1, or K5 with KNOB LAYOUT = SMK) shows MASTER and its level in the top bar while it turns (#1).

## Snoop37 2.6.6

- **DAW mode:** the keyboard as a MIDI controller (USB, and the TRS jack). **`<` + `>`** together (or HOME menu > MIDI >
  **MODE**) switches SYNTH ↔ DAW, with a screen that says which; kept over power-off. The keys play notes on
  **CHANNEL**; K1–K8, the faders, the pads, 13 buttons and the wheels send CC, notes, MMC or pitch bend as the map says.
  KNOB BANK / FADER BANK / both: banks A / B of the knobs, faders, pads (as the stock firmware). OCTAVE− / +: the
  octave. PATCH: **LOCAL** ON / OFF (the keys also play the selected track, NOTE pads the drum kit; OFF by default).
  The screen shows the last control's message and value and the eight knobs as bars; the pads light by kind. SLOOP's
  sequencer stops; MIDI IN still plays SLOOP. Default map and controls: [SMK37.md](SMK37.md#daw-mode).
- **Your own map (CC MAP USER):** PATCH held + OCTAVE+ (or HOME menu > DAW > EDIT MAP): touch a control, then KNOB 1
  TYPE, KNOB 2 NUMBER, KNOB 3 CHANNEL; OCTAVE+ the next, OCTAVE− done. Or the web editor's new **DAW MAP** tab (read,
  send, save / open a file). HOME menu > DAW: CC MAP, KNOBS (ABSOLUTE / RELATIVE), RESET MAP.
- **The way back to the M-Vave firmware keeps working:** the sample slots USR1 / USR2 share their flash with the
  stock firmware's key, knob and fader mapping, which it trusts as long as its end marker is there (after a return to
  stock, keys sent one note). Now that marker is erased before the first sample is stored there, and at power-on when
  it sits over sample data, so the stock firmware rebuilds its factory mapping; an untouched stock mapping stays. The
  web editor's Settings tab can erase it on request (**Prepare the return to M-Vave**).
- HOME menu: new sections **PLAY** (KEY VELOCITY on K5, BEND UP K6, BEND DOWN K7: they leave MIDI) and **DAW**; MIDI is
  TRS MIDI OUT, MODE, CHANNEL, LOCAL. The section tabs scroll.

## Snoop37 2.6.5

- **LIMITER:** HOME menu > AUDIO > **LIMITER** (K8): SOFT (default) / CLASSIC. SOFT gives the mix 9 dB more headroom
  at the power-on MASTER position and lets the limiter act only near full scale, so a held chord no longer turns the
  whole mix down ("one note loud, a chord quieter"). MASTER's top is as loud as before (louder peaks pass): turn
  MASTER up a few clicks after updating. CLASSIC is SLOOP's limiter and MASTER curve.
- **BEND UP / BEND DOWN** replace BEND RANGE: HOME menu > MIDI > **BEND UP** (K6), **BEND DOWN** (K7), each OFF, 1, 2,
  3, 4, 5, 7 or 12 semitones, default 7 / 7 (e.g. +2 up, −12 down). A 2.6.4 BEND RANGE comes back as both.
- **KNOB LAYOUT:** HOME menu > SCREEN > **KNOB LAYOUT** (K7): FM-1 (default, as before) / SMK: K1–K4 the four dials on
  the screen, K5 master volume, K6 SELECT (tempo / pages), K7 PRESETS (sound / kit), K8 ALGORITHM (track). Menu rows,
  help lines and the calibration screen name the knobs as they are; the hardware calibration stays per printed knob.
- **Pads: DRUM KIT by default.** On the PLAY screen the pads play the drum kit whatever track is selected (a pad a
  sound, with velocity; the keys play the selected track). **KNOB BANK + FADER BANK** (the stock PAD BANK combo)
  switches the pads to **STEPS** (the selected track's 16 steps of the page: tap to set / clear, `<` `>` the page;
  drums: the sound last played) and back. The pads no longer mirror the keys there. Layers keep their pads (SEQ, FX,
  GLO, SAVE grids, SCALE piano, EDIT / ARP held: the notes). While recording, the DRUM KIT pads record into the drum
  track whatever track is armed (the keys into the armed one).
- SEQ PLAY and SEQ REC work while the HOME menu is up (start / stop, record arm).
- HOME menu: KEY VELOCITY moves to MIDI (still K8); AUDIO's K8 is LIMITER.

## Snoop37 2.6.4

- **TRS MIDI OUT:** the 3.5 mm jack sends what goes out over USB MIDI (the keys and pads, the sequencer with
  GLO > SYSTEM > MIDI = SEQ, on each track's channel), with or without a computer. HOME menu > MIDI >
  **TRS MIDI OUT** (K5): ON (default) / OFF. Use a TRS-to-DIN MIDI adapter (type A; if a synth does not respond, try
  type B).
- **BEND RANGE:** HOME menu > MIDI > **BEND RANGE** (K6) sets the pitch wheel's range on the internal sounds: ±1, 2,
  3, 4, 5, 7 or 12 semitones, default **7** (the range it had). Pitch bend from MIDI IN
  keeps its own range (GLO > MIDI 2 > BEND).
- **Wheel calibration:** HARDWARE CALIBRATION (OCTAVE− + OCTAVE+ at power-on, or HOME menu > SYSTEM) now also
  teaches the wheels after the buttons and knobs: the pitch wheel at rest, full down, full up; the mod wheel at the
  bottom, full up (OCTAVE+ takes each step). The full travel then gives the full range; at power-on a resting pitch
  wheel is re-centred. Without it the wheels calibrate themselves as before. The calibration screen names the
  knobs as printed (K2 SELECT … K5–K8).

## Snoop37 2.6.3

- HOME menu > AUDIO > VELOCITY is now **KEY VELOCITY**, and it is set with **K8** (2.6.2 said K4).
- The HOME menu's rows name the SMK-37's knobs: **K5–K8** (the FM-1's KNOB 1–4; K1–K4 are MASTER, SELECT,
  ALGORITHM, PRESETS). The ARP mode is on K5.
- The visualiser no longer has the SNOOP and WATERFALL styles: it has SLOOP's 12 again (the last one: SLOOP).
  A style kept from 2.6.2 reads as SLOOP (was SNOOP) or OSCILLOSCOPE (was WATERFALL).

## Snoop37 2.6.2

- HOME menu > AUDIO > **VELOCITY** (K4): DYNAMIC (as played, the default), SOFT (a light touch plays louder), HARD
  (loud needs a hard hit), FIXED 64, FIXED 100, FIXED 127. It applies to the 37 keys and the pads: the notes, their
  MIDI out and the drum levels (FIXED 64: soft, 100: normal, 127: hard; OCT− / OCT+ held still play ghost / hard).
  Kept with the settings; after updating it is DYNAMIC, as before.

## Snoop37 2.6.1

- About screen without the repository link (it names SLOOP's repository, as SLOOP does).
- The SMK-37 hardware support now matches the upstream pull request
  [#112](https://github.com/isod89/sloop-fm1/pull/112); the look matches the branch `smk37-snoop-look`.
- Snoop37 lives in the SLOOP fork, <https://github.com/genkigenki/sloop-fm1> (branch `snoop37`).

## Snoop37 2.6

From here on Snoop37's version number follows the SLOOP release it is based on (the release before was 0.1.x).

- **Based on SLOOP 2.6** (was 2.5): 27 punch-in effects (the new ones on the black keys F#3–F#5 with FX held),
  REV / ECHO THROW, an INSERT effect per track, LFO 2 with tempo sync, ENV SYNC, ANALOG filter types, note LEN and
  step velocity, MIDI LEARN (GLOBE + F#3), live drum mutes, TEXTURE, a MIDI channel per track, MIDI IN on one
  channel, pitch bend and sustain from MIDI IN, step entry from a MIDI keyboard, Program Change, project ERASE,
  the bold screen font, the look-ahead limiter. SLOOP's list: [README-SLOOP.md](README-SLOOP.md#whats-new-in-26).
- The SMK-37 pitch wheel keeps its own ±7 semitones; MIDI pitch bend from outside uses GLO > MIDI 2 > BEND.
- SYSTEM's about screen shows `2.6 (SLOOP 2.6)`.
- FADER BANK: pressed again, `<` `>` or K2 change the bank; the screen closes after 4 s. KNOB BANK and FADER BANK
  close each other, so only one bank screen is up (before, `<` `>` went to the hidden pad picker).
- HOME menu > SYSTEM gains MIDI LEARN CLEAR next to BOOT LOGO.
- The installers (web and `tools/fm1_install.py`) keep the SMK-37 model rules and port names and take SLOOP 2.6's
  additions (CLIPPER / SLOOP port names, "not found" lists the MIDI ports there are).
- The intro ends on the SNOOP37 title alone (no port credit line under it).
- Snoop37 is now SLOOP's SMK-37 support (the pull request to SLOOP: every SMK-37 feature, also in a plain SLOOP
  build) plus the Snoop37 look behind the build flag `SNOOP37=1` (intro, jingle, update screens, name,
  visualisers, BOOT LOGO). The source is based on SLOOP's current main (firmware 2.6; the editor's audio preview
  and the offline app came with it).
- Installers: the official packages of the whole SMK-37 range are accepted for the way back (by their structure, so
  M-Vave's next release too); more port names probed (MAKU, RAINBOW).
- Kept from 0.1.x: the pitch wheel fix, the Elite / MKE-P37 builds (`MODEL=elite|mke`), the installer's model check
  on resume.

**Settings: check once after updating from 0.1.x.** SLOOP 2.6 keeps the MIDI IN channel (GLO > MIDI 2 > IN) in the
settings bits where 0.1.x kept the BOOT LOGO. A 0.1.x BOOT LOGO of SLOOP or NONE therefore reads once as
IN = CH 1 or CH 2, and BOOT LOGO goes back to SNOOP37. After updating, check HOME menu > SYSTEM > BOOT LOGO and
GLO > MIDI 2 > IN once; set them, and they stay. Projects saved in 2.6 (project format 6) do not open in 0.1.x.

## Snoop37 0.1.x

- SLOOP 2.5 on the SMK-37 Pro, Elite and MKE-P37, with the Snoop37 intro, jingle and look (0.1.1: the pitch wheel
  fix, the Elite / MKE-P37 builds).
