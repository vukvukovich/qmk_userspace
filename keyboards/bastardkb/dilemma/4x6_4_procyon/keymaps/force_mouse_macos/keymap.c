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
#ifdef VIA_ENABLE
#    include "via.h"
#endif

/* Hold-to-drag trigger, chosen from a curated dropdown (VIA's keycode
 * picker can't suggest layer/custom keys - hardcoded app whitelist).
 * The dropdown stores an INDEX into this table. */
static const uint16_t tp_drag_key_table[] = {KC_NO, TL_LOWR, TL_UPPR, MO(1), MO(2), MO(3), KC_LSFT, KC_RSFT, KC_LCTL, KC_LALT, KC_LGUI};
/* Hold-to-drag armed by default on TL_LOWR, the left lower-layer thumb
 * key: hold it and the pad selects and drags, release and it points
 * again. Index 1 in the table above. A VIA setting still overrides it. */
#define TP_DRAG_KEY_DEFAULT_IDX 1
static uint8_t  tp_drag_key_idx  = TP_DRAG_KEY_DEFAULT_IDX;
static uint16_t tp_drag_custom   = KC_NO; /* free-entry keycode, used when the dropdown says Custom */
static uint16_t tp_drag_key      = KC_NO;
static bool     tp_drag_key_held = false;
#define TP_DRAG_KEY_TABLE_LEN (sizeof(tp_drag_key_table) / sizeof(tp_drag_key_table[0]))
#define TP_DRAG_CUSTOM_IDX TP_DRAG_KEY_TABLE_LEN /* dropdown entry after the table = Custom */

static void tp_drag_key_resolve(void) {
    tp_drag_key      = tp_drag_key_idx == TP_DRAG_CUSTOM_IDX ? tp_drag_custom : tp_drag_key_table[tp_drag_key_idx];
    tp_drag_key_held = false;
}

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
                         TL_LOWR, KC_SPC, KC_LGUI, MEH(KC_NO), HYPR(KC_NO), TL_UPPR, KC_ENT, KC_RALT
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
    /* drag trigger key: observed, never consumed - it keeps its own
     * function while also arming select & drag */
    if (tp_drag_key != KC_NO && keycode == tp_drag_key) {
        tp_drag_key_held = record->event.pressed;
    }
    uint16_t swapped;
    switch (keycode) {
        case MS_WHLU: swapped = MS_WHLD; break;
        case MS_WHLD: swapped = MS_WHLU; break;
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

// VIA "Trackpad" settings pane (custom menu channel): per-user,
// EEPROM-persisted, no code edits. Two backends:
//   kb-eeconfig word (byte 0 = DPI config, untouched):
//     byte 1: bit 4 settings-written marker
//             bit 5 natural-scroll value, bit 6 gesture-trace value
//     byte 2: pointer scale percent (valid 30-130; else firmware default)
//   VIA custom-config region (offsets 0-6): drag-key index (0),
//     custom drag keycode (1-2), pinch-in kc (3-4), pinch-out kc (5-6)
extern bool    digitizer_select_drag;
extern uint8_t digitizer_pointer_scale_pct;
extern bool    digitizer_gesture_trace;
extern uint16_t digitizer_pinch_in_kc;
extern uint16_t digitizer_pinch_out_kc;



static void trackpad_settings_apply(void) {
    const uint32_t ee = eeconfig_read_kb();
    const uint8_t  b1 = (ee >> 8) & 0xff;
    const uint8_t  b2 = (ee >> 16) & 0xff;
#ifdef VIA_ENABLE
    via_read_custom_config(&tp_drag_key_idx, 0, sizeof(tp_drag_key_idx));
    via_read_custom_config(&tp_drag_custom, 1, sizeof(tp_drag_custom));
    if (tp_drag_key_idx > TP_DRAG_CUSTOM_IDX) tp_drag_key_idx = TP_DRAG_KEY_DEFAULT_IDX; /* fresh eeprom */
    if (tp_drag_custom == 0xffff) tp_drag_custom = KC_NO;
    tp_drag_key_resolve();
    uint16_t kc;
    via_read_custom_config(&kc, 3, sizeof(kc));
    if (kc != 0xffff && kc != KC_NO) digitizer_pinch_in_kc = kc;
    via_read_custom_config(&kc, 5, sizeof(kc));
    if (kc != 0xffff && kc != KC_NO) digitizer_pinch_out_kc = kc;
#endif
    /* a freshly wiped eeprom reads 0xff - only trust values inside
     * their valid ranges (255% pointer speed taught us this) */
    if (b1 != 0xff && (b1 & 0x10)) {
        digitizer_natural_scroll = (b1 >> 5) & 1;
        digitizer_gesture_trace  = (b1 >> 6) & 1;
    }
    if (b2 >= 30 && b2 <= 130) digitizer_pointer_scale_pct = b2;
}

static void trackpad_settings_save(void) {
    const uint8_t  b1 = 0x10 | (digitizer_natural_scroll ? 0x20 : 0) | (digitizer_gesture_trace ? 0x40 : 0);
    const uint8_t  b2 = digitizer_pointer_scale_pct;
    const uint32_t ee = (eeconfig_read_kb() & 0x000000ff) | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16);
    eeconfig_update_kb(ee);
#ifdef VIA_ENABLE
    via_update_custom_config(&tp_drag_key_idx, 0, sizeof(tp_drag_key_idx));
    via_update_custom_config(&tp_drag_custom, 1, sizeof(tp_drag_custom));
    via_update_custom_config(&digitizer_pinch_in_kc, 3, sizeof(digitizer_pinch_in_kc));
    via_update_custom_config(&digitizer_pinch_out_kc, 5, sizeof(digitizer_pinch_out_kc));
#endif
}

/* Bindings drive the drag flag only on CHANGES, so the SelDrag toggle
 * key keeps working when no binding is active. */
void housekeeping_task_user(void) {
    static bool bound = false;
    const bool  want  = tp_drag_key_held;
    if (want != bound) {
        bound                       = want;
        digitizer_select_drag = want;
    }
}

#ifdef VIA_ENABLE
enum trackpad_value_id {
    id_tp_pointer_scale = 2,
    id_tp_natural_scroll = 3,
    id_tp_swipe_left = 4,
    id_tp_swipe_right = 5,
    id_tp_swipe_up = 6,
    id_tp_swipe_down = 7,
    id_tp_drag_key = 9,
    id_tp_drag_custom = 10,
    id_tp_trace = 11,
    id_tp_pinch_in = 12,
    id_tp_pinch_out = 13,
};

/* The swipe keycode pickers proxy into the spare-matrix slots: same
 * storage, same persistence, same full-pipeline execution (macros and
 * layer keys included) as assigning the slot keys directly. */
static const digitizer_swipe_dir_t swipe_value_dir[4] = {DIGITIZER_SWIPE_DIR_LEFT, DIGITIZER_SWIPE_DIR_RIGHT, DIGITIZER_SWIPE_DIR_UP, DIGITIZER_SWIPE_DIR_DOWN};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);
    if (*channel_id != id_custom_channel) {
        *command_id = id_unhandled;
        return;
    }
    const uint8_t value_id = value_id_and_data[0];
    uint8_t      *value    = &value_id_and_data[1];
    switch (*command_id) {
        case id_custom_set_value:
            switch (value_id) {
                case id_tp_pointer_scale: digitizer_pointer_scale_pct = value[0] ? value[0] : 1; break;
                case id_tp_natural_scroll: digitizer_natural_scroll = value[0]; break;
                case id_tp_trace: digitizer_gesture_trace = value[0]; break;
                case id_tp_pinch_in: digitizer_pinch_in_kc = (value[0] << 8) | value[1]; break;
                case id_tp_pinch_out: digitizer_pinch_out_kc = (value[0] << 8) | value[1]; break;
                case id_tp_drag_key:
                    tp_drag_key_idx = value[0] <= TP_DRAG_CUSTOM_IDX ? value[0] : 0;
                    tp_drag_key_resolve();
                    break;
                case id_tp_drag_custom:
                    tp_drag_custom = (value[0] << 8) | value[1];
                    tp_drag_key_resolve();
                    break;
                case id_tp_swipe_left ... id_tp_swipe_down: {
                    const uint8_t d = swipe_value_dir[value_id - id_tp_swipe_left];
                    dynamic_keymap_set_keycode(0, swipe_slot[d][0], swipe_slot[d][1], (value[0] << 8) | value[1]);
                    break;
                }
            }
            break;
        case id_custom_get_value:
            switch (value_id) {
                case id_tp_pointer_scale: value[0] = digitizer_pointer_scale_pct; break;
                case id_tp_natural_scroll: value[0] = digitizer_natural_scroll; break;
                case id_tp_trace: value[0] = digitizer_gesture_trace; break;
                case id_tp_pinch_in:
                    value[0] = digitizer_pinch_in_kc >> 8;
                    value[1] = digitizer_pinch_in_kc & 0xff;
                    break;
                case id_tp_pinch_out:
                    value[0] = digitizer_pinch_out_kc >> 8;
                    value[1] = digitizer_pinch_out_kc & 0xff;
                    break;
                case id_tp_drag_key:
                    value[0] = tp_drag_key_idx;
                    break;
                case id_tp_drag_custom:
                    value[0] = tp_drag_custom >> 8;
                    value[1] = tp_drag_custom & 0xff;
                    break;
                case id_tp_swipe_left ... id_tp_swipe_down: {
                    const uint8_t d = swipe_value_dir[value_id - id_tp_swipe_left];
                    const uint16_t kc = dynamic_keymap_get_keycode(0, swipe_slot[d][0], swipe_slot[d][1]);
                    value[0] = kc >> 8;
                    value[1] = kc & 0xff;
                    break;
                }
            }
            break;
        case id_custom_save:
            trackpad_settings_save();
            break;
        default:
            *command_id = id_unhandled;
            break;
    }
}
#endif

// This keymap exists for macOS, which cannot consume digitizer reports:
// mouse mode is forced unconditionally at boot. Without this, mouse mode
// hangs on two fragile things - OS detection succeeding, and no host ever
// sending the PTP input-mode feature request (which silently switches the
// firmware to digitizer reporting).
void keyboard_post_init_user(void) {
    force_digitizer_send_mouse_reports = true;
    trackpad_settings_apply();
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
