#include <pebble.h>

// ── Screen ────────────────────────────────────────────────────────────────────
#define SCREEN_W   144
#define SCREEN_H   168

// ── Red rectangle: x=4, y=15, w=89, h=25 ────────────────────────────────────
#define RECT_X     4
#define RECT_Y     15
#define RECT_W     89
#define RECT_H     25

// ── Time row (inside rect, top) ───────────────────────────────────────────────
#define TIME_X     (RECT_X + 4)
#define TIME_Y     (RECT_Y + 2)
#define TIME_W     56
#define TIME_H     14

// ── AM/PM superscript ─────────────────────────────────────────────────────────
#define AMPM_X     (TIME_X + TIME_W + 1)
#define AMPM_Y     (RECT_Y + 2)
#define AMPM_W     10
#define AMPM_H     8

// ── Battery icon ──────────────────────────────────────────────────────────────
#define BATT_X     (RECT_X + RECT_W - 18)
#define BATT_Y     (RECT_Y + 3)
#define BATT_W     13
#define BATT_H     7
#define BATT_NUB_W 2
#define BATT_NUB_H 3

// ── Date row (inside rect, bottom) ───────────────────────────────────────────
#define DATE_X     (RECT_X + 4)
#define DATE_Y     (RECT_Y + 15)
#define DATE_W     52
#define DATE_H     9

// ── Day name (inside rect, right of date) ─────────────────────────────────────
#define DAY_X      (DATE_X + DATE_W + 2)
#define DAY_Y      (RECT_Y + 15)
#define DAY_W      40
#define DAY_H      9

// ── Valve watermark: bottom-right, partially off-screen ──────────────────────
// Drawn with GColorDarkGray — faint ghost effect on Pebble's 1-bit display
#define WM_W       90
#define WM_H       32
#define WM_X       (SCREEN_W - WM_W + 18)   // clip right edge
#define WM_Y       (SCREEN_H - WM_H + 12)   // clip bottom edge

// VALV text inside watermark
#define WM_TEXT_X  (WM_X + 4)
#define WM_TEXT_Y  (WM_Y + 4)
#define WM_TEXT_W  66
#define WM_TEXT_H  22

// Small E superscript
#define WM_E_X     (WM_X + 68)
#define WM_E_Y     (WM_Y + 4)
#define WM_E_W     14
#define WM_E_H     10

// ── Globals ───────────────────────────────────────────────────────────────────
static Window              *s_window;
static Layer               *s_canvas;
static GFont                s_font_time;
static GFont                s_font_date;
static GFont                s_font_small;
static char                 s_time_buf[6];   // "07:26"
static char                 s_ampm_buf[3];   // "PM"
static char                 s_date_buf[12];  // "24 MAY 26"
static char                 s_day_buf[10];   // "SUNDAY"
static BatteryChargeState   s_battery;

// ── Helpers ───────────────────────────────────────────────────────────────────
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
    "SUNDAY","MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY"
  };
  snprintf(s_day_buf, sizeof(s_day_buf), "%s", dy[t->tm_wday]);
}

static void draw_battery(GContext *ctx, int pct) {
  // Outline
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(BATT_X, BATT_Y, BATT_W, BATT_H));
  // Nub
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx,
    GRect(BATT_X + BATT_W,
          BATT_Y + (BATT_H - BATT_NUB_H) / 2,
          BATT_NUB_W, BATT_NUB_H),
    0, GCornerNone);
  // Fill
  int fw = ((BATT_W - 2) * pct) / 100;
  if (fw > 0) {
    graphics_fill_rect(ctx,
      GRect(BATT_X + 1, BATT_Y + 1, fw, BATT_H - 2),
      0, GCornerNone);
  }
}

static void draw_watermark(GContext *ctx) {
  // Outer border
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_rect(ctx, GRect(WM_X, WM_Y, WM_W, WM_H));
  // VALV text
  graphics_context_set_text_color(ctx, GColorDarkGray);
  graphics_draw_text(ctx, "VALV", s_font_date,
    GRect(WM_TEXT_X, WM_TEXT_Y, WM_TEXT_W, WM_TEXT_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // Small E
  graphics_draw_text(ctx, "E", s_font_small,
    GRect(WM_E_X, WM_E_Y, WM_E_W, WM_E_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

// ── Draw ──────────────────────────────────────────────────────────────────────
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Black background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_W, SCREEN_H), 0, GCornerNone);

  // Watermark (behind rect)
  draw_watermark(ctx);

  // Red rectangle
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, GRect(RECT_X, RECT_Y, RECT_W, RECT_H), 0, GCornerNone);

  // Time
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buf, s_font_time,
    GRect(TIME_X, TIME_Y, TIME_W, TIME_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // AM/PM
  graphics_draw_text(ctx, s_ampm_buf, s_font_small,
    GRect(AMPM_X, AMPM_Y, AMPM_W, AMPM_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // Battery
  draw_battery(ctx, s_battery.charge_percent);

  // Date
  graphics_draw_text(ctx, s_date_buf, s_font_date,
    GRect(DATE_X, DATE_Y, DATE_W, DATE_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  // Day (darker — GColorDarkCandyAppleRed reads as muted on Pebble display)
  graphics_context_set_text_color(ctx, GColorDarkCandyAppleRed);
  graphics_draw_text(ctx, s_day_buf, s_font_small,
    GRect(DAY_X, DAY_Y, DAY_W, DAY_H),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

// ── Handlers ──────────────────────────────────────────────────────────────────
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  layer_mark_dirty(s_canvas);
}

static void battery_handler(BatteryChargeState charge) {
  s_battery = charge;
  layer_mark_dirty(s_canvas);
}

// ── Window ────────────────────────────────────────────────────────────────────
static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_font_time  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_14));
  s_font_date  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_10));
  s_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SMALL_8));

  s_canvas = layer_create(GRect(0, 0, SCREEN_W, SCREEN_H));
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);

  time_t now = time(NULL);
  update_time(localtime(&now));
  s_battery = battery_state_service_peek();
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  fonts_unload_custom_font(s_font_time);
  fonts_unload_custom_font(s_font_date);
  fonts_unload_custom_font(s_font_small);
}

// ── Init ──────────────────────────────────────────────────────────────────────
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
