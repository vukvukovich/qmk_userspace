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

/* Diagnostic build: raw sensor event trace over QMK console */
#define MAXTOUCH_EVENT_TRACE

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
