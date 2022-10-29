/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include <stdio.h>

enum custom_keycodes {
    L_COMMAND,
    R_COMMAND,
};

#define _BASE 0
#define _LOWER 1
#define _RAISE 2
#define _ADJUST 3

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_3(
	KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                  KC_Y, KC_U, KC_I, KC_O, KC_P, KC_MINS,
	KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G,                 KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
	KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,                 KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_BSPC,
	      LALT_T(KC_ESC), L_COMMAND, LT(_LOWER,KC_SPC),     LT(_RAISE,KC_ENT), R_COMMAND, KC_RALT
  ),

	[_LOWER] = LAYOUT_split_3x6_3(
	KC_TAB, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC,                KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_TILD,
	KC_LCTL, KC_NO, KC_F5, KC_F6, KC_F7, KC_F8,                      KC_PLUS, KC_LCBR, KC_RCBR, KC_LT, KC_GT, KC_PIPE,
	KC_LSFT, KC_NO, KC_F9, KC_F10, KC_F11, KC_F12,                   KC_PEQL, KC_NO, KC_NO, KC_NO, KC_NO, KC_BSPC,
	     KC_LALT, KC_LGUI, KC_TRNS,          MO(_ADJUST), KC_LGUI, KC_RALT
  ),

	[_RAISE] = LAYOUT_split_3x6_3(
	KC_NO, KC_P1, KC_P2, KC_P3, KC_P4, KC_P5,              KC_NO, KC_NO, KC_UP, KC_NO, KC_NO, KC_NO,
	KC_NO, KC_P6, KC_P7, KC_P8, KC_P9, KC_P0,              KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, KC_NO,
	KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,              KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
	        KC_LALT, KC_LGUI, MO(_ADJUST),                  KC_TRNS, KC_LGUI, KC_RALT
  ),

	[_ADJUST] = LAYOUT_split_3x6_3(
	QK_BOOT, RGB_M_P, RGB_M_R, RGB_M_G, RGB_M_SN, RGB_M_K,            KC_NO, KC_BTN1, KC_MS_U, KC_BTN2, KC_NO, KC_NO,
	RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, KC_MUTE, KC_VOLU,             KC_NO, KC_MS_L, KC_MS_D, KC_MS_R, KC_NO, KC_NO,
	RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, KC_MSTP, KC_VOLD,             KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
	        KC_LGUI, KC_TRNS, KC_SPC,                              KC_ENT, KC_TRNS, KC_RALT
  )
};

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (!is_keyboard_master()) {
    return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
  }
  return rotation;
}

#define L_BASE 0
#define L_LOWER 2
#define L_RAISE 4
#define L_ADJUST 8

void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (layer_state) {
        case L_BASE:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case L_LOWER:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case L_RAISE:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case L_ADJUST:
        case L_ADJUST|L_LOWER:
        case L_ADJUST|L_RAISE:
        case L_ADJUST|L_LOWER|L_RAISE:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
    }
}


char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
  char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) { keycode = keycode & 0xFF; }
  if (keycode < 60) {
    name = code_to_name[keycode];
  }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "%dx%d, k%2d : %c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write(keylog_str, false);
}

void render_bootmagic_status(bool status) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
    if (status) {
        oled_write_ln_P(logo[0][0], false);
        oled_write_ln_P(logo[0][1], false);
    } else {
        oled_write_ln_P(logo[1][0], false);
        oled_write_ln_P(logo[1][1], false);
    }
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    oled_write_P(crkbd_logo, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}

static bool l_command_pressed = false;
static uint16_t l_command_pressed_time = 0;
static bool r_command_pressed = false;
static uint16_t r_command_pressed_time = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // 長押しでコマンド、タップで言語切り替えを実装
    // 参考: https://blog.magcho.com/2020/4/qmk_firmware%E3%81%A7ctrl%E3%81%A8%E3%81%AE%E5%90%8C%E6%99%82%E6%8A%BC%E3%81%97%E3%82%92%E3%82%AB%E3%82%B9%E3%82%BF%E3%83%9E%E3%82%A4%E3%82%BA%E3%81%99%E3%82%8B/
    // 参考: https://gist.github.com/okapies/5d13a174cbb13ce34dbd9faede9d0b71#file-keymap-c-L99-L164
    // 参考: https://okapies.hateblo.jp/entry/2019/02/02/133953
    switch (keycode) {
      case L_COMMAND:
          if (record->event.pressed) {
              l_command_pressed = true;
              l_command_pressed_time = record->event.time;
              register_code(KC_LGUI);
          } else {
              unregister_code(KC_LGUI);
              if (l_command_pressed && (TIMER_DIFF_16(record->event.time, l_command_pressed_time) < TAPPING_TERM)) {
                  register_code(KC_LANG2); // for macOS
                  register_code(KC_MHEN);
                  unregister_code(KC_MHEN);
                  unregister_code(KC_LANG2);
              }
              l_command_pressed = false;
          }
          return false;
          break;
      case R_COMMAND:
          if (record->event.pressed) {
              r_command_pressed = true;
              r_command_pressed_time = record->event.time;
              register_code(KC_LGUI);
          } else {
              unregister_code(KC_LGUI);
              if (r_command_pressed && (TIMER_DIFF_16(record->event.time, r_command_pressed_time) < TAPPING_TERM)) {
                  register_code(KC_LANG1); // for macOS
                  register_code(KC_HENK);
                  unregister_code(KC_HENK);
                  unregister_code(KC_LANG1);
              }
              r_command_pressed = false;
          }
          return false;
          break;
    }
    return true;
}
#endif // OLED_ENABLE
