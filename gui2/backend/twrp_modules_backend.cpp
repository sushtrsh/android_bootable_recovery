#include "twrp_modules_backend.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <sstream>

#include "partitions.hpp"
#include "twrp_functions.hpp"
#include "twrp_operation_executor.h"

namespace gui2_backend {

bool twrp_modules_backend::is_data_mounted() {
  return PartitionManager.IsMountedByPath("/data");
}

bool twrp_modules_backend::mount_data() {
  if (is_data_mounted()) return true;
  twrp_operation_executor::guard transaction(twrp_operation_executor::instance().mutex());
  if (!PartitionManager.PrepareDataForDe(true)) {
    if (!PartitionManager.MountByPath("/data", true)) {
      return false;
    }
  }
  return is_data_mounted();
}

bool twrp_modules_backend::parse_module_prop(const std::string& prop_path, module_item& item) {
  std::ifstream file(prop_path);
  if (!file.is_open()) return false;

  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty() || line[0] == '#') continue;

    size_t eq_pos = line.find('=');
    if (eq_pos == std::string::npos) continue;

    std::string key = line.substr(0, eq_pos);
    std::string val = line.substr(eq_pos + 1);

    if (key == "id" && !val.empty()) {
      item.id = val;
    } else if (key == "name" && !val.empty()) {
      item.name = val;
    } else if (key == "version" && !val.empty()) {
      item.version = val;
    } else if (key == "author" && !val.empty()) {
      item.author = val;
    } else if (key == "description" && !val.empty()) {
      item.description = val;
    }
  }
  return true;
}

void twrp_modules_backend::scan_directory(const std::string& base_path, module_framework framework,
                                          std::vector<module_item>& out) {
  if (!TWFunc::IsPathExists(base_path)) return;

  DIR* dir = opendir(base_path.c_str());
  if (dir == nullptr) return;

  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    if (entry->d_name[0] == '.') continue;

    std::string full_path = base_path + "/" + entry->d_name;
    struct stat st;
    if (stat(full_path.c_str(), &st) != 0 || !S_ISDIR(st.st_mode)) continue;

    module_item item;
    item.id = entry->d_name;
    item.name = entry->d_name;
    item.path = full_path;
    item.framework = framework;

    std::string prop_file = full_path + "/module.prop";
    parse_module_prop(prop_file, item);

    std::string disable_file = full_path + "/disable";
    item.enabled = !TWFunc::IsPathExists(disable_file);

    out.push_back(std::move(item));
  }
  closedir(dir);
}

std::vector<module_item> twrp_modules_backend::get_modules() {
  std::vector<module_item> modules;
  if (!is_data_mounted()) {
    mount_data();
  }
  if (!is_data_mounted()) {
    return modules;
  }

  // Magisk modules: /data/adb/modules
  scan_directory("/data/adb/modules", module_framework::MAGISK, modules);

  // KernelSU modules: /data/adb/ksu/modules
  scan_directory("/data/adb/ksu/modules", module_framework::KERNELSU, modules);

  // APatch modules: /data/adb/ap/modules
  scan_directory("/data/adb/ap/modules", module_framework::APATCH, modules);

  std::sort(modules.begin(), modules.end(), [](const module_item& a, const module_item& b) {
    return a.name < b.name;
  });

  return modules;
}

bool twrp_modules_backend::toggle_module(const std::string& path, bool enable) {
  std::string disable_file = path + "/disable";
  std::string remove_file = path + "/remove";

  if (enable) {
    if (TWFunc::IsPathExists(disable_file)) unlink(disable_file.c_str());
    if (TWFunc::IsPathExists(remove_file)) unlink(remove_file.c_str());
    return true;
  } else {
    int fd = creat(disable_file.c_str(), 0644);
    if (fd >= 0) {
      close(fd);
      return true;
    }
    return false;
  }
}

bool twrp_modules_backend::delete_module(const std::string& path) {
  if (path.empty() || path == "/" || path == "/data" || path == "/data/adb") return false;
  return TWFunc::RemoveDir(path, false) == 0;
}

}  // namespace gui2_backend
