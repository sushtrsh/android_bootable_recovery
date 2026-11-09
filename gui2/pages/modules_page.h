#ifndef GUI2_PAGES_MODULES_PAGE_H
#define GUI2_PAGES_MODULES_PAGE_H

#include <vector>

#include "backend/modules_backend.h"
#include "core/ui_metrics.h"
#include "i18n/i18n.h"
#include "lvgl.h"

namespace gui2_pages {

struct modules_page_options {
  lv_obj_t* content = nullptr;
  const gui2_core::ui_metrics* metrics = nullptr;
  const gui2_i18n::language_pack* strings = nullptr;
  const std::vector<gui2_backend::module_item>* modules = nullptr;
  bool data_mounted = true;
  lv_event_cb_t toggle_callback = nullptr;
  lv_event_cb_t delete_callback = nullptr;
  lv_event_cb_t mount_callback = nullptr;
  lv_event_cb_t press_guard_callback = nullptr;
};

void build_modules_page(const modules_page_options& options);

}  // namespace gui2_pages

#endif  // GUI2_PAGES_MODULES_PAGE_H
