#include "pages/modules_page.h"

#include <algorithm>
#include <string>

#include "components/flat_button.h"
#include "components/section_label.h"
#include "components/tip_card.h"
#include "core/ui_helpers.h"

namespace gui2_pages {

namespace {

int control_size(const gui2_core::ui_metrics& metrics) {
  const int header_height = std::max(1, metrics.text_font->line_height);
  return std::clamp(header_height * 3 / 2, gui2_core::ui_px(64), gui2_core::ui_px(88));
}

const char* framework_tag(gui2_backend::module_framework fw) {
  switch (fw) {
    case gui2_backend::module_framework::MAGISK:
      return "[Magisk]";
    case gui2_backend::module_framework::KERNELSU:
      return "[KernelSU]";
    case gui2_backend::module_framework::APATCH:
      return "[APatch]";
    default:
      return "[Module]";
  }
}

}  // namespace

void build_modules_page(const modules_page_options& options) {
  if (options.content == nullptr || options.metrics == nullptr) return;

  const auto& metrics = *options.metrics;
  const int card_width = metrics.content_width;
  const int padding = gui2_core::card_inner_padding();

  lv_obj_t* body = lv_obj_create(options.content);
  lv_obj_set_pos(body, metrics.content_left, 0);
  lv_obj_set_width(body, metrics.content_width);
  lv_obj_set_height(body, LV_SIZE_CONTENT);
  gui2_core::set_surface_style(body, metrics.background, LV_OPA_TRANSP);
  lv_obj_set_style_pad_all(body, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_row(body, metrics.card_gap, LV_PART_MAIN);
  lv_obj_set_layout(body, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(body, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(body, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  gui2_core::disable_scrolling(body);

  if (!options.data_mounted) {
    gui2_components::create_tip_card(
        body, metrics,
        "Data partition is not mounted or is encrypted. Please mount or decrypt Data to manage modules.",
        metrics.danger, metrics.danger_surface);
    if (options.mount_callback != nullptr) {
      gui2_components::create_flat_button(body, metrics, metrics.content_width, "Mount /data",
                                          options.mount_callback, options.press_guard_callback);
    }
    return;
  }

  if (options.modules == nullptr || options.modules->empty()) {
    gui2_components::create_tip_card(body, metrics,
                                     "No Magisk, KernelSU, or APatch modules found in /data/adb.",
                                     metrics.accent, metrics.accent_surface);
    return;
  }

  gui2_components::create_section_label(body, metrics, "Installed Modules");

  const int track_height = control_size(metrics) * 3 / 4;
  const int track_width = track_height * 7 / 4;

  for (size_t i = 0; i < options.modules->size(); ++i) {
    const auto& mod = (*options.modules)[i];

    lv_obj_t* card = lv_obj_create(body);
    lv_obj_set_size(card, card_width, LV_SIZE_CONTENT);
    gui2_core::set_surface_style(card, metrics.card_color);
    lv_obj_set_style_radius(card, gui2_core::ui_px(28), LV_PART_MAIN);
    lv_obj_set_style_pad_all(card, padding, LV_PART_MAIN);
    gui2_core::set_card_shadow(card);
    gui2_core::disable_scrolling(card);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(card, gui2_core::ui_px(10), LV_PART_MAIN);

    // Header row with module name, framework tag, and switch toggle
    lv_obj_t* header_row = lv_obj_create(card);
    lv_obj_set_size(header_row, lv_pct(100), LV_SIZE_CONTENT);
    gui2_core::set_surface_style(header_row, metrics.card_color, LV_OPA_TRANSP);
    lv_obj_set_style_pad_all(header_row, 0, LV_PART_MAIN);
    gui2_core::disable_scrolling(header_row);

    std::string title_text = framework_tag(mod.framework);
    title_text += " " + mod.name;
    if (!mod.version.empty()) {
      title_text += " (" + mod.version + ")";
    }

    const int title_w = card_width - padding * 2 - track_width - gui2_core::ui_px(20);
    lv_obj_t* title_label = lv_label_create(header_row);
    lv_label_set_text(title_label, title_text.c_str());
    lv_label_set_long_mode(title_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(title_label, std::max(1, title_w));
    lv_obj_set_style_text_color(title_label, metrics.primary_text, LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, metrics.text_font, LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* toggle = lv_switch_create(header_row);
    lv_obj_set_size(toggle, track_width, track_height);
    lv_obj_align(toggle, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_radius(toggle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(toggle,
                             lv_color_mix(metrics.secondary_text, metrics.card_color, 110),
                             LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(toggle, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(toggle, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(toggle, gui2_core::ui.accent,
                              LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_radius(toggle, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_bg_color(toggle, gui2_core::ui.on_accent, LV_PART_KNOB);
    lv_obj_set_style_pad_all(toggle, -gui2_core::ui_px(6), LV_PART_KNOB);
    lv_obj_set_style_shadow_width(toggle, 0, LV_PART_KNOB);

    if (mod.enabled) lv_obj_add_state(toggle, LV_STATE_CHECKED);
    if (options.toggle_callback != nullptr) {
      lv_obj_add_event_cb(toggle, options.toggle_callback, LV_EVENT_VALUE_CHANGED,
                          reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
    }

    // Description
    if (!mod.description.empty()) {
      lv_obj_t* desc = lv_label_create(card);
      lv_label_set_text(desc, mod.description.c_str());
      lv_label_set_long_mode(desc, LV_LABEL_LONG_WRAP);
      lv_obj_set_width(desc, lv_pct(100));
      lv_obj_set_style_text_color(desc, metrics.secondary_text, LV_PART_MAIN);
      lv_obj_set_style_text_font(desc, metrics.status_font, LV_PART_MAIN);
    }

    // Author
    if (!mod.author.empty()) {
      lv_obj_t* author = lv_label_create(card);
      std::string author_str = "Author: " + mod.author;
      lv_label_set_text(author, author_str.c_str());
      lv_obj_set_style_text_color(author, metrics.secondary_text, LV_PART_MAIN);
      lv_obj_set_style_text_font(author, metrics.status_font, LV_PART_MAIN);
    }

    // Delete button
    lv_obj_t* del_btn = lv_obj_create(card);
    lv_obj_set_size(del_btn, gui2_core::ui_px(160), gui2_core::ui_px(64));
    gui2_core::set_surface_style(del_btn, metrics.card_color);
    lv_obj_set_style_border_width(del_btn, gui2_core::ui_px(2), LV_PART_MAIN);
    lv_obj_set_style_border_color(del_btn, metrics.danger, LV_PART_MAIN);
    lv_obj_set_style_radius(del_btn, gui2_core::ui_px(16), LV_PART_MAIN);
    lv_obj_set_clickable(del_btn, true);
    gui2_core::disable_scrolling(del_btn);
    if (options.press_guard_callback != nullptr) {
      lv_obj_add_event_cb(del_btn, options.press_guard_callback, LV_EVENT_ALL, nullptr);
    }
    if (options.delete_callback != nullptr) {
      lv_obj_add_event_cb(del_btn, options.delete_callback, LV_EVENT_CLICKED,
                          reinterpret_cast<void*>(static_cast<uintptr_t>(i)));
    }

    lv_obj_t* del_label = lv_label_create(del_btn);
    lv_label_set_text(del_label, "Delete");
    lv_obj_set_style_text_color(del_label, metrics.danger, LV_PART_MAIN);
    lv_obj_set_style_text_font(del_label, metrics.status_font, LV_PART_MAIN);
    lv_obj_center(del_label);
  }
}

}  // namespace gui2_pages
