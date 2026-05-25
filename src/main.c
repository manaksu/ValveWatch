#include <pebble.h>

static Window *s_window;
static Layer  *s_canvas;

static GFont s_font_time;   // 60px  — HH:MM
static GFont s_font_ampm;   // 18px  — am/pm superscript

// ─── Draw ────────────────────────────────────────────────────────────────────
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  char time_buf[6];  // "12:34"
  char ampm_buf[3];  // "am" / "pm"

  // 12-hour format
  int hour = t->tm_hour % 12;
  if (hour == 0) hour = 12;

  snprintf(time_buf, sizeof(time_buf), "%d:%02d", hour, t->tm_min);
  snprintf(ampm_buf, sizeof(ampm_buf), "%s", t->tm_hour < 12 ? "am" : "pm");

  // ── Background ──
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  // ── HH:MM ── starts at x=4, y=15
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(
    ctx,
    time_buf,
    s_font_time,
    GRect(4, 15, 140, 72),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL
  );

  // ── Measure time text width to place am/pm right after ──
  GSize time_size = graphics_text_layout_get_content_size(
    time_buf,
    s_font_time,
    GRect(4, 15, 144, 72),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft
  );

  // Superscript: x = 4 + text_width + 2px gap, y near cap-top
  int ampm_x = 4 + time_size.w + 2;
  int ampm_y = 18;

  graphics_draw_text(
    ctx,
    ampm_buf,
    s_font_ampm,
    GRect(ampm_x, ampm_y, 30, 24),
    GTextOverflowModeWordWrap,
    GTextAlignmentLeft,
    NULL
  );
}

// ─── Tick ─────────────────────────────────────────────────────────────────────
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas);
}

// ─── Window lifecycle ────────────────────────────────────────────────────────
static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_font_time = fonts_load_custom_font(
    resource_get_handle(RESOURCE_ID_FONT_LG_60));
  s_font_ampm = fonts_load_custom_font(
    resource_get_handle(RESOURCE_ID_FONT_LG_18));

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  fonts_unload_custom_font(s_font_time);
  fonts_unload_custom_font(s_font_ampm);
}

// ─── Init / Deinit ───────────────────────────────────────────────────────────
static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
