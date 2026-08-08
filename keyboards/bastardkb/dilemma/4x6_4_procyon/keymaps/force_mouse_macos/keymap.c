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
#include QMK_KEYBOARD_H
#include "digitizer_mouse_fallback.h"
#include "dynamic_keymap.h"

enum dilemma_keymap_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_POINTER,
};

// Automatically enable sniping-mode on the pointer layer.
// #define DILEMMA_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
       KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
       KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS,
       KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
       KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RCTL,
                         TL_LOWR, KC_SPC, KC_LGUI, XXXXXXX, HYPR(KC_NO), TL_UPPR, KC_ENT, KC_RALT
  ),

  [LAYER_LOWER] = LAYOUT(
       KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
       RM_NEXT, RM_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, C(KC_LEFT), G(KC_LBRC), G(KC_RBRC), C(KC_RGHT), KC_RBRC, KC_BSLS,
       _______, C(KC_LEFT), KC_LALT, KC_LCTL, C(KC_RGHT), XXXXXXX, KC_MINS, KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV,
       RM_PREV, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, S(KC_MINS), S(KC_EQL), S(KC_LBRC), S(KC_RBRC), S(KC_BSLS), S(KC_GRV),
                         XXXXXXX, XXXXXXX, XXXXXXX, _______, XXXXXXX, _______, XXXXXXX, XXXXXXX
  ),

  [LAYER_RAISE] = LAYOUT(
       KC_BRID, KC_BRIU, KC_MCTL, KC_LPAD, C(KC_DOWN), GU_ON, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,
       KC_GRV, QK_KB_1, QK_KB_0, XXXXXXX, XXXXXXX, XXXXXXX, MS_WHLL, MS_WHLD, MS_WHLU, MS_WHLR, KC_HOME, KC_DEL,
       XXXXXXX, C(KC_LEFT), C(KC_RGHT), MS_BTN2, MS_BTN1, QK_KB_4, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_HOME, KC_PGUP,
       C(G(KC_SPC)), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_END, KC_DEL,
                         XXXXXXX, _______, _______, KC_LALT, _______, XXXXXXX, XXXXXXX, XXXXXXX
  ),

  [LAYER_POINTER] = LAYOUT(
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_KB_0, QK_KB_2, QK_KB_2, QK_KB_0, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
       XXXXXXX, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX, XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI, XXXXXXX,
       XXXXXXX, _______, QK_KB_6, QK_KB_4, EE_CLR, QK_BOOT, QK_BOOT, EE_CLR, QK_KB_4, QK_KB_6, _______, XXXXXXX,
                         XXXXXXX, MS_BTN2, MS_BTN1, MS_BTN3, MS_BTN3, MS_BTN1, MS_BTN2, XXXXXXX
  ),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef DILEMMA_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    dilemma_set_pointer_sniping_enabled(layer_state_cmp(state, DILEMMA_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // DILEMMA_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLEE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif // RGB_MATRIX_ENABLE

#ifdef ENCODER_MAP_ENABLE
// clang-format off
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE]       = {ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_LOWER]      = {ENCODER_CCW_CW(KC_UP, KC_DOWN), ENCODER_CCW_CW(KC_LEFT, KC_RGHT)},
    [LAYER_RAISE]      = {ENCODER_CCW_CW(KC_PGUP, KC_PGDN), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_POINTER]    = {ENCODER_CCW_CW(RM_HUED, RM_HUEU), ENCODER_CCW_CW(RM_SATD, RM_SATU)},
};

// clang-format on
#endif // ENCODER_MAP_ENABLE

extern bool force_digitizer_send_mouse_reports;
extern bool digitizer_natural_scroll;

// macOS "natural scrolling" inverts wheel events system-wide, which
// would make a key labeled wheel-down scroll up. While natural scroll
// is active, swap the wheel keycodes (keys and encoder alike) so they
// keep their labeled meaning.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint16_t swapped;
    switch (keycode) {
        case MS_WHLU: swapped = MS_WHLD; break;
        case MS_WHLD: swapped = MS_WHLU; break;
        case MS_WHLL: swapped = MS_WHLR; break;
        case MS_WHLR: swapped = MS_WHLL; break;
        default: return true;
    }
    if (!digitizer_natural_scroll) return true;
    if (record->event.pressed) {
        register_code16(swapped);
    } else {
        unregister_code16(swapped);
    }
    return false;
}

// Trackpad swipes are VIA-assignable: the four matrix positions with no
// physical switch behind them appear in the sideloaded VIA layout as
// Swipe keys. A swipe injects a key event at its position, so whatever
// VIA assigned there fires through the normal pipeline (macros, layer
// keys, custom keycodes included). An unassigned slot falls back to the
// macOS gesture shortcuts - except on a positively detected non-Apple
// host, where it stays quiet (Ctrl+arrows is word-jump on Windows).
// Assign on the base layer.
static const uint8_t swipe_slot[4][2] = {
    [DIGITIZER_SWIPE_DIR_LEFT]  = {4, 0},
    [DIGITIZER_SWIPE_DIR_UP]    = {4, 5},
    [DIGITIZER_SWIPE_DIR_DOWN]  = {9, 0},
    [DIGITIZER_SWIPE_DIR_RIGHT] = {9, 5},
};

void digitizer_swipe_action(digitizer_swipe_dir_t dir) {
    const uint8_t row = swipe_slot[dir][0];
    const uint8_t col = swipe_slot[dir][1];
    if (dynamic_keymap_get_keycode(0, row, col) != KC_NO) {
        keyevent_t press = MAKE_KEYEVENT(row, col, true);
        action_exec(press);
        keyevent_t release = MAKE_KEYEVENT(row, col, false);
        action_exec(release);
        return;
    }
    switch (detected_host_os()) {
        case OS_WINDOWS:
        case OS_LINUX:
            return;
        default:
            break;
    }
    switch (dir) {
        case DIGITIZER_SWIPE_DIR_RIGHT: tap_code16(DIGITIZER_SWIPE_RIGHT_KC); break;
        case DIGITIZER_SWIPE_DIR_LEFT: tap_code16(DIGITIZER_SWIPE_LEFT_KC); break;
        case DIGITIZER_SWIPE_DIR_DOWN: tap_code16(DIGITIZER_SWIPE_DOWN_KC); break;
        case DIGITIZER_SWIPE_DIR_UP: tap_code16(DIGITIZER_SWIPE_UP_KC); break;
    }
}

// This keymap exists for macOS, which cannot consume digitizer reports:
// mouse mode is forced unconditionally at boot. Without this, mouse mode
// hangs on two fragile things - OS detection succeeding, and no host ever
// sending the PTP input-mode feature request (which silently switches the
// firmware to digitizer reporting).
void keyboard_post_init_user(void) {
    force_digitizer_send_mouse_reports = true;
}

// NAT_TOG (keyboard keycode) toggles scroll direction; natural is also
// the compile-time boot default (DIGITIZER_NATURAL_SCROLL).
bool process_detected_host_os_kb(os_variant_t detected_os) {
    if (!process_detected_host_os_user(detected_os)) {
        return false;
    }
    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            // Force mouse mode
            force_digitizer_send_mouse_reports = true;
            digitizer_natural_scroll           = true;
            break;
        case OS_WINDOWS:
        case OS_LINUX:
            // Non-Apple host: standard wheel direction expected
            digitizer_natural_scroll = false;
            break;
        case OS_UNSURE:
            // Rely on autodetection of mouse mode
            break;
    }
    return true;
}
