#include <pebble.h>

static Window *s_window;
static Layer  *s_canvas;
static GFont s_font_time;
static GFont s_font_ampm;
static GFont s_font_date;
static GFont s_font_day;

// ── Settings ──────────────────────────────────────────────────────────────────
#define SETTINGS_KEY 1
// watermark: 0=off, 1=steamdeck, 2=valve
typedef struct { uint8_t watermark; } Settings;
static Settings s_settings;

static void prv_save_settings(void) {
  persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}
static void prv_load_settings(void) {
  s_settings.watermark = 1; // default: steamdeck
  persist_read_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

// ── Gamepad bitmap ────────────────────────────────────────────────────────────
#define GP_W 136
#define GP_H  57

static const uint8_t GAMEPAD_RUNS[] = {
  0,12,123,
  1,7,128,
  2,4,131,
  3,3,132,
  4,2,133,
  5,2,9,
  5,126,133,
  6,1,6,
  6,129,134,
  7,1,5,
  7,130,134,
  8,1,4,
  8,131,134,
  9,0,4,
  9,13,16,
  9,36,99,
  9,119,122,
  9,131,135,
  10,0,4,
  10,13,16,
  10,36,99,
  10,119,122,
  10,131,135,
  11,0,4,
  11,13,16,
  11,27,29,
  11,36,99,
  11,106,108,
  11,119,122,
  11,131,135,
  12,0,4,
  12,13,16,
  12,26,30,
  12,36,99,
  12,105,109,
  12,131,135,
  13,0,4,
  13,9,20,
  13,25,31,
  13,36,40,
  13,95,99,
  13,104,110,
  13,115,117,
  13,124,126,
  13,131,135,
  14,0,4,
  14,9,20,
  14,25,31,
  14,36,39,
  14,96,99,
  14,104,110,
  14,115,118,
  14,123,126,
  14,131,135,
  15,0,4,
  15,9,20,
  15,25,31,
  15,36,39,
  15,96,99,
  15,104,110,
  15,115,118,
  15,123,126,
  15,131,135,
  16,0,4,
  16,9,20,
  16,25,31,
  16,36,39,
  16,96,99,
  16,104,110,
  16,116,117,
  16,124,126,
  16,131,135,
  17,0,4,
  17,13,16,
  17,26,30,
  17,36,39,
  17,96,99,
  17,105,109,
  17,120,121,
  17,131,135,
  18,0,4,
  18,13,16,
  18,28,28,
  18,36,39,
  18,96,99,
  18,107,107,
  18,119,122,
  18,131,135,
  19,0,4,
  19,13,16,
  19,36,39,
  19,96,99,
  19,119,122,
  19,131,135,
  20,0,4,
  20,13,16,
  20,36,39,
  20,96,99,
  20,120,122,
  20,131,135,
  21,0,4,
  21,36,39,
  21,96,99,
  21,131,135,
  22,0,4,
  22,36,39,
  22,96,99,
  22,131,135,
  23,0,4,
  23,36,39,
  23,96,99,
  23,131,135,
  24,0,4,
  24,36,39,
  24,96,99,
  24,131,135,
  25,0,4,
  25,20,31,
  25,36,39,
  25,96,99,
  25,104,115,
  25,131,135,
  26,0,4,
  26,20,31,
  26,36,39,
  26,96,99,
  26,104,115,
  26,131,135,
  27,0,4,
  27,19,31,
  27,36,39,
  27,96,99,
  27,104,116,
  27,131,135,
  28,0,4,
  28,19,31,
  28,36,39,
  28,96,99,
  28,104,116,
  28,131,135,
  29,0,4,
  29,19,31,
  29,36,39,
  29,96,99,
  29,104,116,
  29,131,135,
  30,0,4,
  30,19,31,
  30,36,39,
  30,96,99,
  30,104,116,
  30,131,135,
  31,0,4,
  31,19,31,
  31,36,39,
  31,96,99,
  31,104,116,
  31,131,135,
  32,0,4,
  32,19,31,
  32,36,39,
  32,96,99,
  32,104,116,
  32,131,135,
  33,0,4,
  33,19,31,
  33,36,39,
  33,96,99,
  33,104,116,
  33,131,135,
  34,0,4,
  34,19,31,
  34,36,39,
  34,96,99,
  34,104,116,
  34,131,135,
  35,0,4,
  35,19,31,
  35,36,39,
  35,96,99,
  35,104,116,
  35,131,135,
  36,0,4,
  36,20,31,
  36,36,39,
  36,96,99,
  36,104,115,
  36,131,135,
  37,0,4,
  37,21,29,
  37,36,39,
  37,96,99,
  37,106,114,
  37,131,135,
  38,0,4,
  38,36,39,
  38,96,99,
  38,131,135,
  39,0,4,
  39,36,39,
  39,96,99,
  39,131,135,
  40,0,4,
  40,36,39,
  40,96,99,
  40,131,135,
  41,1,4,
  41,36,39,
  41,96,99,
  41,131,134,
  42,1,4,
  42,36,39,
  42,96,99,
  42,131,134,
  43,1,5,
  43,36,40,
  43,95,99,
  43,130,134,
  44,1,5,
  44,36,99,
  44,130,134,
  45,1,5,
  45,36,99,
  45,130,134,
  46,2,6,
  46,36,99,
  46,129,133,
  47,2,6,
  47,36,99,
  47,129,133,
  48,3,7,
  48,128,132,
  49,3,8,
  49,127,132,
  50,4,10,
  50,125,131,
  51,5,13,
  51,122,130,
  52,6,129,
  53,7,128,
  54,9,126,
  55,11,124,
  56,15,120,
  255,0,0
};

static void draw_gamepad(GContext *ctx, int ox, int oy) {
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  int i = 0;
  while (GAMEPAD_RUNS[i] != 255) {
    int row = GAMEPAD_RUNS[i];
    int x0  = GAMEPAD_RUNS[i+1];
    int x1  = GAMEPAD_RUNS[i+2];
    graphics_draw_line(ctx,
      GPoint(ox + x0, oy + row),
      GPoint(ox + x1, oy + row));
    i += 3;
  }
}

// ── AppMessage ────────────────────────────────────────────────────────────────
// Key 0 = watermark (0=off, 1=steamdeck, 2=valve, 3=battery white, 4=battery distressed)
static void prv_inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t0 = dict_find(iter, 0);
  if (t0) s_settings.watermark = (uint8_t)t0->value->int32;
  prv_save_settings();
  layer_mark_dirty(s_canvas);
}


// ── Valve logo bitmap ─────────────────────────────────────────────────────────
#define VALVE_W 136
#define VALVE_H  24

static const uint8_t VALVE_RUNS[] = {
  2,5,135,
  3,5,135,
  4,5,135,
  5,5,8,
  6,5,7,
  7,5,7,
  8,5,7,
  9,5,7,
  10,5,7,
  10,18,19,
  10,21,21,
  10,32,32,
  10,35,35,
  10,51,54,
  10,56,56,
  10,77,77,
  10,79,79,
  10,98,98,
  10,100,100,
  10,112,112,
  10,114,114,
  10,127,127,
  10,129,129,
  10,135,135,
  11,5,7,
  11,18,21,
  11,32,35,
  11,51,56,
  11,77,80,
  11,98,101,
  11,112,114,
  11,127,135,
  12,5,7,
  12,19,21,
  12,32,34,
  12,51,57,
  12,77,80,
  12,99,101,
  12,111,114,
  12,127,129,
  13,5,7,
  13,19,22,
  13,31,34,
  13,50,57,
  13,77,80,
  13,99,101,
  13,111,114,
  13,127,129,
  14,5,7,
  14,19,22,
  14,31,34,
  14,50,52,
  14,55,57,
  14,77,80,
  14,99,102,
  14,111,113,
  14,127,129,
  15,5,7,
  15,20,22,
  15,31,33,
  15,50,52,
  15,55,57,
  15,77,80,
  15,99,102,
  15,111,113,
  15,127,129,
  16,5,7,
  16,20,23,
  16,31,33,
  16,49,52,
  16,55,58,
  16,77,80,
  16,100,102,
  16,110,113,
  16,127,129,
  17,5,7,
  17,20,23,
  17,30,33,
  17,49,51,
  17,56,58,
  17,77,80,
  17,100,103,
  17,110,112,
  17,127,135,
  18,5,7,
  18,21,23,
  18,30,32,
  18,49,51,
  18,56,58,
  18,77,80,
  18,100,103,
  18,110,112,
  18,127,135,
  19,5,7,
  19,21,23,
  19,30,32,
  19,49,51,
  19,56,59,
  19,77,80,
  19,101,103,
  19,109,112,
  19,127,129,
  20,5,7,
  20,21,24,
  20,29,32,
  20,48,51,
  20,57,59,
  20,77,80,
  20,101,103,
  20,109,111,
  20,127,129,
  21,5,7,
  21,22,24,
  21,29,31,
  21,48,50,
  21,57,59,
  21,77,80,
  21,101,104,
  21,109,111,
  21,127,129,
  22,5,7,
  22,22,24,
  22,29,31,
  22,48,50,
  22,57,59,
  22,77,80,
  22,102,104,
  22,109,111,
  22,127,129,
  23,5,7,
  23,22,24,
  23,29,31,
  23,47,50,
  23,57,60,
  23,77,80,
  23,102,104,
  23,108,110,
  23,127,130,
  255,0,0
};

static void draw_valve(GContext *ctx, int ox, int oy) {
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  int i = 0;
  while (VALVE_RUNS[i] != 255) {
    int row = VALVE_RUNS[i];
    int x0  = VALVE_RUNS[i+1];
    int x1  = VALVE_RUNS[i+2];
    graphics_draw_line(ctx,
      GPoint(ox + x0, oy + row),
      GPoint(ox + x1, oy + row));
    i += 3;
  }
}


// ── Half-Life lambda — battery indicator ─────────────────────────────────────
// 28x28, bottom-right corner. Filled = full battery, drains from top.
#define LAMBDA_W 28
#define LAMBDA_H 28

static const uint8_t LAMBDA_RUNS[] = {
  1,7,13,
  2,7,13,
  3,7,14,
  4,10,14,
  5,11,15,
  6,11,15,
  7,12,15,
  8,12,16,
  9,11,16,
  10,10,17,
  11,10,17,
  12,9,17,
  13,8,18,
  14,8,12,
  14,14,18,
  15,7,12,
  15,15,19,
  16,7,11,
  16,15,19,
  17,6,10,
  17,16,19,
  18,5,10,
  18,16,20,
  19,5,9,
  19,16,20,
  20,4,8,
  20,17,21,
  21,3,8,
  21,17,21,
  21,25,25,
  22,3,7,
  22,18,26,
  23,2,6,
  23,18,26,
  24,2,6,
  24,18,26,
  25,1,5,
  25,19,24,
  26,19,20,
  255,0,0
};

// Draw lambda battery: origin ox,oy (top-left of 28x28 box)
// battery_pct: 0-100
static void draw_lambda_battery(GContext *ctx, int ox, int oy, int battery_pct) {
  // How many rows from top are "drained" (dark gray shadow)
  int drain_rows = LAMBDA_H - (LAMBDA_H * battery_pct / 100);
  int i = 0;
  while (LAMBDA_RUNS[i] != 255) {
    int row = LAMBDA_RUNS[i];
    int x0  = LAMBDA_RUNS[i+1];
    int x1  = LAMBDA_RUNS[i+2];
    // Rows above drain line = dark (empty), below = white (charged)
    if (row < drain_rows) {
      graphics_context_set_stroke_color(ctx, GColorDarkGray);
    } else {
      graphics_context_set_stroke_color(ctx, GColorWhite);
    }
    graphics_draw_line(ctx,
      GPoint(ox + x0, oy + row),
      GPoint(ox + x1, oy + row));
    i += 3;
  }
}


// ── Distressed orange lambda — battery indicator ──────────────────────────────
// LCG seeded random for consistent distress pattern (no stdlib rand dependency)
static uint32_t s_lcg_state = 42;
static uint32_t prv_lcg_rand(void) {
  s_lcg_state = s_lcg_state * 1664525 + 1013904223;
  return s_lcg_state;
}

static void draw_lambda_battery_distressed(GContext *ctx, int ox, int oy, int battery_pct) {
  int drain_rows = LAMBDA_H - (LAMBDA_H * battery_pct / 100);
  s_lcg_state = 42; // reset seed each draw for consistent pattern
  int i = 0;
  while (LAMBDA_RUNS[i] != 255) {
    int row = LAMBDA_RUNS[i];
    int x0  = LAMBDA_RUNS[i+1];
    int x1  = LAMBDA_RUNS[i+2];
    for (int x = x0; x <= x1; x++) {
      uint32_t r = prv_lcg_rand() % 100;
      if (r < 18) { i += 0; continue; } // skip pixel = scratch
      // On Basalt: orange closest = GColorChromeYellow, dim = GColorDarkGray
      if (row < drain_rows) {
        graphics_context_set_stroke_color(ctx, GColorDarkGray);
      } else {
        // alternate between orange shades for texture
        GColor col = (prv_lcg_rand() % 10 < 7) ? GColorChromeYellow : GColorOrange;
        graphics_context_set_stroke_color(ctx, col);
      }
      graphics_draw_pixel(ctx, GPoint(ox + x, oy + row));
    }
    i += 3;
  }
}

// ── Draw ──────────────────────────────────────────────────────────────────────
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char time_buf[6];
  char ampm_buf[3];
  char date_buf[12];
  char day_buf[10];
  int hour = t->tm_hour % 12;
  if (hour == 0) hour = 12;
  snprintf(time_buf, sizeof(time_buf), "%d:%02d", hour, t->tm_min);
  snprintf(ampm_buf, sizeof(ampm_buf), "%s", t->tm_hour < 12 ? "AM" : "PM");
  static const char *months[] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
  snprintf(date_buf, sizeof(date_buf), "%d %s", t->tm_mday, months[t->tm_mon]);
  static const char *days[] = {"SUNDAY","MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY"};
  snprintf(day_buf, sizeof(day_buf), "%s", days[t->tm_wday]);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  GSize time_size = graphics_text_layout_get_content_size(
    time_buf, s_font_time, GRect(0, 0, 144, 40),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  GSize ampm_size = graphics_text_layout_get_content_size(
    ampm_buf, s_font_ampm, GRect(0, 0, 30, 20),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);

  int pad_left=8, pad_right=8, pad_top=-3, pad_bottom=5, gap=2;
  int box_x=4, box_y=15;
  int box_w = pad_left + time_size.w + gap + ampm_size.w + pad_right;
  int box_h = pad_top + time_size.h + pad_bottom;

  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(box_x, box_y, box_w, box_h), 0, GCornerNone);

  int time_x = box_x + pad_left;
  int time_y = box_y + pad_top;

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, time_buf, s_font_time,
    GRect(time_x, time_y, time_size.w + 2, time_size.h),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  int ampm_x = time_x + time_size.w + gap;
  int ampm_y = time_y + 5;
  graphics_draw_text(ctx, ampm_buf, s_font_ampm,
    GRect(ampm_x, ampm_y, ampm_size.w + 2, ampm_size.h + 4),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  int date_x = box_x + box_w + 6;
  int date_y = box_y + pad_top;
  int date_w = 144 - date_x;

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, date_buf, s_font_date,
    GRect(date_x, date_y, date_w, 18),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  graphics_context_set_text_color(ctx, GColorLightGray);
  graphics_draw_text(ctx, day_buf, s_font_day,
    GRect(date_x, date_y + 16, date_w, 18),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  if (s_settings.watermark == 1) {
    draw_gamepad(ctx, (144 - GP_W) / 2, 168 - GP_H - 8);
  } else if (s_settings.watermark == 2) {
    draw_valve(ctx, (144 - VALVE_W) / 2, 168 - VALVE_H - 6);
  } else if (s_settings.watermark == 3) {
    BatteryChargeState bat = battery_state_service_peek();
    draw_lambda_battery(ctx, 144 - LAMBDA_W - 4, 168 - LAMBDA_H - 4, bat.charge_percent);
  } else if (s_settings.watermark == 4) {
    BatteryChargeState bat = battery_state_service_peek();
    draw_lambda_battery_distressed(ctx, 144 - LAMBDA_W - 4, 168 - LAMBDA_H - 4, bat.charge_percent);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_font_time = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_font_ampm = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  s_font_date = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  s_font_day  = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
}

static void prv_battery_handler(BatteryChargeState state) {
  layer_mark_dirty(s_canvas);
}

static void init(void) {
  prv_load_settings();
  app_message_register_inbox_received(prv_inbox_received);
  app_message_open(64, 64);
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(prv_battery_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
