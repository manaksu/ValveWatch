#include <pebble.h>

#define SCREEN_W   144
#define SCREEN_H   168

// Red rectangle: x=4, y=15, w=89, h=25
#define RECT_X     4
#define RECT_Y     15
#define RECT_W     89
#define RECT_H     25

// Time — FONT_LG_20 (League Gothic 20pt, tall condensed)
// Pebble adds ~4px top padding on custom fonts
#define TIME_X     (RECT_X + 3)
#define TIME_Y     (RECT_Y - 4)
#define TIME_W     60
#define TIME_H     22

// AM/PM superscript — FONT_LG_09
#define AMPM_X     (RECT_X + 55)
#define AMPM_Y     (RECT_Y + 1)
#define AMPM_W     14
#define AMPM_H     9

// Battery icon
#define BATT_X     (RECT_X + RECT_W - 17)
#define BATT_Y     (RECT_Y + 4)
#define BATT_W     12
#define BATT_H     6
#define BATT_NUB_W 2
#define BATT_NUB_H 3

// Date — FONT_LG_12
#define DATE_X     (RECT_X + 3)
#define DATE_Y     (RECT_Y + 13)
#define DATE_W     52
#define DATE_H     12

// Day — FONT_LG_09, right of date
#define DAY_X      (RECT_X + 48)
#define DAY_Y      (RECT_Y + 14)
#define DAY_W      38
#define DAY_H      10

// Watermark — bottom-right, peeking off-screen
#define WM_X       94
#define WM_Y       138
#define WM_W       58
#define WM_H       22

// ── Globals ───────────────────────────────────────────────────────────────────
static Window            *s_window;
static Layer             *s_canvas;
static GFont              s_font_lg20;
static GFont              s_font_lg12;
static GFont              s_font_lg09;
static char               s_time_buf[6];
static char               s_ampm_buf[3];
static char               s_date_buf[12];
static char               s_day_buf[10];
static BatteryChargeState s_battery;

static void update_time(struct tm *t) {
  int h = t->tm_hour;
  bool pm = (h >= 12);
  h = h % 12;
  if (h == 0) h = 12;
  snprintf(s_time_buf, sizeof(s_time_buf), "%02d:%02d", h, t->tm_min);
  snprintf(s_ampm_buf, sizeof(s_ampm_buf), "%s", pm ? "PM" : "AM");

  static const char *mo[] = {
    "JAN","FEB","MAR","APR","MAY","JUN",
    "JUL","AUG","SEP","OCT","NOV","DEC"
  };
  snprintf(s_date_buf, sizeof(s_date_buf), "%02d %s %02d",
           t->tm_mday, mo[t->tm_mon], t->tm_year % 100);

  static const char *dy[] = {
    "SUNDAY","MONDAY","TUESDAY","WEDNESDAY",
    "THURSDAY","FRIDAY","SATURDAY"
  };
  snprintf(s_day_buf, sizeof(s_day_buf), "%s", dy[t->tm_wday]);
}

static void draw_battery(GContext *ctx, int pct) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(BATT_X, BATT_Y, BATT_W, BATT_H));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx,
    GRect(BATT_X + BATT_W, BATT_Y + (BATT_H - BATT_NUB_H) / 2, BATT_NUB_W, BATT_NUB_H),
    0, GCornerNone);
  int fw = ((BATT_W - 2) * pct) / 100;
  if (fw > 0) {
    graphics_fill_rect(ctx,
      GRect(BATT_X + 1, BATT_Y + 1, fw, BATT_H - 2),
      0, GCornerNone);
  }
}

static void draw_watermark(GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_rect(ctx, GRect(WM_X, WM_Y, WM_W, WM_H));
  graphics_context_set_text_color(ctx, GColorDarkGray);
  graphics_draw_text(ctx, "VALV", s_font_lg12,
    GRect(WM_X + 2, WM_Y - 2, 46, 22),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, "E", s_font_lg09,
    GRect(WM_X + 46, WM_Y + 1, 10, 10),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  draw_watermark(ctx);

  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(RECT_X, RECT_Y, RECT_W, RECT_H), 0, GCornerNone);

  // Time
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buf, s_font_lg20,
    GRect(TIME_X, TIME_Y, TIME_W, TIME_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // AM/PM
  graphics_draw_text(ctx, s_ampm_buf, s_font_lg09,
    GRect(AMPM_X, AMPM_Y, AMPM_W, AMPM_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  draw_battery(ctx, s_battery.charge_percent);

  // Date
  graphics_draw_text(ctx, s_date_buf, s_font_lg12,
    GRect(DATE_X, DATE_Y, DATE_W, DATE_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // Day
  graphics_context_set_text_color(ctx, GColorDarkCandyAppleRed);
  graphics_draw_text(ctx, s_day_buf, s_font_lg09,
    GRect(DAY_X, DAY_Y, DAY_W, DAY_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  layer_mark_dirty(s_canvas);
}

static void battery_handler(BatteryChargeState charge) {
  s_battery = charge;
  layer_mark_dirty(s_canvas);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_font_lg20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LG_20));
  s_font_lg12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LG_12));
  s_font_lg09 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LG_09));

  s_canvas = layer_create(GRect(0, 0, SCREEN_W, SCREEN_H));
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);

  time_t now = time(NULL);
  update_time(localtime(&now));
  s_battery = battery_state_service_peek();
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  fonts_unload_custom_font(s_font_lg20);
  fonts_unload_custom_font(s_font_lg12);
  fonts_unload_custom_font(s_font_lg09);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
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
