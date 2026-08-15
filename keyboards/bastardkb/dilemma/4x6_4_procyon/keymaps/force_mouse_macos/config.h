/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 * Copyright 2023 casuanoob <casuanoob@hotmail.com> (@casuanoob)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 8
/* VIA custom-config eeprom: bytes for the Trackpad settings pane
 * (drag trigger keycode). */
#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 8
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE

#define ENCODER_RESOLUTION 4

/* At-rest jitter tuning. George's procyon.h already sets TOUCH_HYST=10,
 * MOVE_HYSTERESIS_NEXT=16, CONFTHR=5 for Procyon boards - the deltas
 * that remain worth raising vs his defaults: */
/* threshold override removed: back to procyon.h stock 20 - our 28 made
 * resting contact marginal (measured amp 18-32) and fed the flicker */
#define MXT_LOW_PASS_FILTER_COEFFICIENT 4   /* default 0 = off */

/* THE real anti-jitter knob: the chip's movement smoothing filter,
 * hardcoded off in the stock driver (patched to be configurable).
 * Smooths slow motion hard, tails off at speed. */
#define MXT_MOVE_SMOOTHING 255

/* Raw sensor event trace over QMK console - re-enable for debugging */
#define MAXTOUCH_EVENT_TRACE

/* Charger-noise campaign verdict (measured vs a third-party GaN
 * charger, the reproducible noise source): chrgtime 1 -> 2 HALVED
 * phantom contacts, re-registrations and teleports with amplitudes
 * unchanged - kept. Further raises plateaued; syncsperx averaging
 * REGRESSED (noise is synchronous with the charger's switching, p5
 * amp collapsed); T72 blind-enable regressed (register map is NDA).
 * The residual fix is electrical: grounded charger, ferrites. */
#define MXT_CHARGE_TIME 2

/* Analog gain: stock Procyon 10. The measured contact amplitudes
 * (18-40 vs threshold 20) are razor-thin for weakly-coupled or dry
 * fingers - the user's palm-grounding trick proved failures scale
 * with signal strength. Raising gain widens every margin. */
#define MXT_GAIN 13 /* sweep: born-merged stroke rate vs saturation */

/* Contact geometry instrumentation for merged-finger detection: enables
 * ONLY the sensor's width/height aux bytes and their trace print - NOT
 * DIGITIZER_REPORT_FINGER_SIZE, whose liftoff-buffer path causes
 * at-rest jitter. */
#define MXT_REPORT_CONTACT_SIZE

/* Merge threshold: measured DIRECTION IS INVERTED vs intuition -
 * HIGHER value = MORE merge-eager. Close-finger two-contact share:
 * 25-47% at 5+gain10, 73.6% at 25+gain12, 16.3% at 40+gain12 (fingers
 * had to be spread wide to scroll at all). Back to stock 5; gain 12
 * appears to be the real de-merge lever. */
#define MXT_MERGE_THRESHOLD 1 /* experiment: minimum practical - maximum finger separation; watch for phantom twins */
#define MXT_MERGE_HYSTERESIS 5 /* stock 10: lower = a split pair re-fuses less easily (stacked-finger scroll) */

/* Pointer 10% slower per user preference; precision unaffected. */
#define DIGITIZER_MOUSE_POINTER_SCALE_PCT 70

/* Edge-cell inclusion tuning: lower internal threshold so cells at the
 * finger's leading/trailing edge join the centroid gradually while
 * ramping, instead of popping in at full weight (the measured ~1-row
 * lurch during drags). */
#define MXT_INTERNAL_TOUCH_THRESHOLD 6
#define MXT_INTERNAL_TOUCH_HYST 3

/* Finer sensor motion steps: stock hysteresis (10/16) pre-quantizes slow
 * drags into 0.1-0.16mm chunks. Report finer; upstream filters absorb
 * the extra noise. */
#define MXT_MOVE_HYSTERESIS_INITIAL 6
#define MXT_MOVE_HYSTERESIS_NEXT 4

/* 1-euro filter tuning (digitizer_mouse_fallback.c). Official procedure:
 * 1) finger at rest: LOWER mincutoff until the wiggle dies;
 * 2) fast swipes:    RAISE beta until the lag disappears. */
#define ONE_EURO_MINCUTOFF 1.0f
#define ONE_EURO_BETA 0.01f

/* Scroll resolution, declared in the HID descriptor so no host-side
 * software is needed to get a fine scroll quantum.
 *
 * macOS reads the wheel's physical-vs-logical extents to work out
 * counts-per-inch and falls back to 9 when a descriptor omits them -
 * that fallback is the chunky "one click = about three lines" wheel
 * behaviour. Apple's own trackpad declares 400, so declare the same.
 *
 * This is NOT the Resolution Multiplier feature report, which was
 * tried and is a measured dead end: macOS reads the multiplied value
 * raw and scroll ends up ~120x too fast.
 *
 * Scroll: travel/divisor wheel clicks per 16ms tick, magnitude intact.
 * The divisor is the single source of truth for scroll speed, and it
 * is PAIRED with the resolution above - raising counts-per-inch makes
 * each click move less, so the divisor has to come down by roughly the
 * same factor to keep the speed. 112 with the old fallback resolution
 * of 9 (and a host tap rendering 13px per click) works out at about 9
 * here; expect to trim this by feel. */
/* MEASURED CEILING - the sensor reports at about 40Hz where an Apple
 * trackpad runs at 120Hz. Halving the interval from 16ms to 8ms did not
 * move the observed event gap at all (23.8/30.4/25.4ms before and
 * after), so the interval is not the limit and nothing in firmware can
 * raise the rate. Scroll will always arrive in ~3x fewer, ~3x larger
 * steps than Apple's.
 *
 * That inverts the resolution choice. With only ~40 events/s the pixels
 * per event have to be LARGER, not finer, to reach the same speed, and
 * at 400 CPI a click is only ~0.38px - so a fast flick hit the sv>100
 * click clamp and peaked at exactly 38px in every measured burst, while
 * Apple peaked at 162px. Halving the resolution doubles the pixels per
 * click, which closes the ~2x median speed gap and leaves headroom
 * before the clamp. Still ~44x finer than the macOS fallback of 9. */
#define WHEEL_RESOLUTION_CPI 200
#define DIGITIZER_SCROLL_DIVISOR 3
#define DIGITIZER_SCROLL_INTERVAL_MS 8

/* Measured against the built-in Apple trackpad: it flings at roughly
 * 780 px/s, and one coast click is now about a pixel, so the coast needs
 * a batch of about a dozen per tick to fling at the same speed. */
#define DIGITIZER_SCROLL_COAST_CLICKS 12

/* Natural scroll as the BOOT default - not dependent on OS detection
 * (which sometimes never fires); NatScrl still toggles at runtime. */
#define DIGITIZER_NATURAL_SCROLL

/* Three-finger swipes = Apple gestures, via macOS's own shortcuts
 * (the route every non-Apple trackpad takes): left/right switch
 * Spaces in Apple's direction (fingers left -> the space on the
 * right slides in), up = Mission Control, down = App Expose.
 * Distance lowered from 500 to a ~2cm flick. */
#define DIGITIZER_SWIPE_LEFT_KC C(KC_RGHT)
#define DIGITIZER_SWIPE_RIGHT_KC C(KC_LEFT)
#define DIGITIZER_SWIPE_UP_KC C(KC_UP)
#define DIGITIZER_SWIPE_DOWN_KC C(KC_DOWN)
#define DIGITIZER_MOUSE_SWIPE_DISTANCE 300
