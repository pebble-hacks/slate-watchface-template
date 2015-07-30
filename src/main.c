#include <pebble.h>

#define KEY_BACKGROUND_COLOR 0
#define KEY_TWENTY_FOUR_HOUR_FORMAT 1

static Window *s_main_window;
static TextLayer *s_text_layer;
static bool twenty_four_hour_format = false;

static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
 
  static char buffer[] = "00:00";

  if (clock_is_24h_style() == twenty_four_hour_format) {
    strftime(buffer, sizeof("00:00"), "20:19", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "08:19", tick_time);
  }
 
  text_layer_set_text(s_text_layer, buffer);
}

static void set_background_and_text_color(int color) {
  GColor background_color = GColorFromHEX(color);
  window_set_background_color(s_main_window, background_color);
  text_layer_set_text_color(s_text_layer, gcolor_legible_over(background_color));
}


static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR);
  Tuple *twenty_four_hour_format_t = dict_find(iter, KEY_TWENTY_FOUR_HOUR_FORMAT);

  if (background_color_t) {
    int background_color = background_color_t->value->int32;

    persist_write_int(KEY_BACKGROUND_COLOR, background_color);

    set_background_and_text_color(background_color);
  }

  if (twenty_four_hour_format_t) {
    twenty_four_hour_format = twenty_four_hour_format_t->value->int8;

    persist_write_int(KEY_TWENTY_FOUR_HOUR_FORMAT, twenty_four_hour_format);

    update_time();
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 55, bounds.size.w, 100));
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_text_layer, GColorClear);

  if (persist_read_int(KEY_BACKGROUND_COLOR)) {
    int background_color = persist_read_int(KEY_BACKGROUND_COLOR);
    set_background_and_text_color(background_color);
  }

  if (persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT)) {
    twenty_four_hour_format = persist_read_bool(KEY_TWENTY_FOUR_HOUR_FORMAT);
  }

  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  update_time();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
