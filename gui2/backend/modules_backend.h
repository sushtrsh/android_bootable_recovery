#ifndef GUI2_BACKEND_MODULES_BACKEND_H
#define GUI2_BACKEND_MODULES_BACKEND_H

#include <string>
#include <vector>

namespace gui2_backend {

enum class module_framework {
  MAGISK,
  KERNELSU,
  APATCH,
  UNKNOWN
};

struct module_item {
  std::string id;
  std::string name;
  std::string version;
  std::string author;
  std::string description;
  std::string path;
  module_framework framework = module_framework::UNKNOWN;
  bool enabled = true;
};

class modules_backend {
 public:
  virtual ~modules_backend() = default;

  virtual bool is_data_mounted() = 0;
  virtual bool mount_data() = 0;
  virtual std::vector<module_item> get_modules() = 0;
  virtual bool toggle_module(const std::string& path, bool enable) = 0;
  virtual bool delete_module(const std::string& path) = 0;
};

}  // namespace gui2_backend

#endif  // GUI2_BACKEND_MODULES_BACKEND_H
