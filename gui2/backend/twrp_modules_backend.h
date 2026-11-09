#ifndef GUI2_BACKEND_TWRP_MODULES_BACKEND_H
#define GUI2_BACKEND_TWRP_MODULES_BACKEND_H

#include "modules_backend.h"

namespace gui2_backend {

class twrp_modules_backend final : public modules_backend {
 public:
  bool is_data_mounted() override;
  bool mount_data() override;
  std::vector<module_item> get_modules() override;
  bool toggle_module(const std::string& path, bool enable) override;
  bool delete_module(const std::string& path) override;

 private:
  void scan_directory(const std::string& base_path, module_framework framework,
                      std::vector<module_item>& out);
  bool parse_module_prop(const std::string& prop_path, module_item& item);
};

}  // namespace gui2_backend

#endif  // GUI2_BACKEND_TWRP_MODULES_BACKEND_H
