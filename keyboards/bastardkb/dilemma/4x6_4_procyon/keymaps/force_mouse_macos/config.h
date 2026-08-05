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
// #define MAXTOUCH_EVENT_TRACE

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

/* Hi-res wheel experiment failed on macOS (2026-08-04): QMK scales
 * wheel units unconditionally but macOS never activates the HID
 * resolution multiplier, so everything arrived 120x too fast. The fix
 * that worked instead: wheel events throttled to physical-wheel
 * cadence (DIGITIZER_SCROLL_INTERVAL_MS in the fallback), which took
 * macOS out of its saturated-acceleration regime and made both the
 * divisor and the system slider effective again. */
/* With the host-side pixel conversion (LineScroll.spoon) the pipeline
 * is linear: divisor = finger travel per line, the single source of
 * truth for scroll speed. 112 = calibrated match to an Apple trackpad
 * with the macOS Mouse scrolling slider at its middle default.
 * Interval is just the emission tick - smooth, not a knob. */
#define DIGITIZER_SCROLL_DIVISOR 112
/* 16ms: with the host-side pixel conversion there is no acceleration
 * to saturate, so the old wheel-plausibility cap is obsolete - the
 * divisor alone rules the speed. */
#define DIGITIZER_SCROLL_INTERVAL_MS 16

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
