#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: 事件绑定模块，支持通过构造函数参数配置事件绑定关系
depends:
- id: QDU-Robomaster/DR16
  ref: same-or-dev
- id: QDU-Robomaster/CMD
  ref: same-or-dev
=== END MANIFEST === */
// clang-format on

#include <cstdint>
#include <initializer_list>

#include "CMD.hpp"
#include "DR16.hpp"

/**
 * @brief 事件绑定模块
 * @details 按配置将源模块事件转发绑定到目标模块事件，实现跨模块事件映射。
 */
class EventBinder
{
 public:
  /**
   * @brief 单条事件绑定描述
   */
  struct EventBinding
  {
    const char* source_module;  // 源模块名称 ("dr16", "cmd", "chassis")
    uint32_t source_event;      // 源事件ID
    const char* target_module;  // 目标模块名称 ("cmd", "chassis", "dr16")
    uint32_t target_event;      // 目标事件ID

    template <typename T1, typename T2>
    constexpr EventBinding(const char* src_mod, T1 src_evt, const char* target_mod,
                           T2 target_evt)
        : source_module(src_mod),
          source_event(static_cast<uint32_t>(src_evt)),
          target_module(target_mod),
          target_event(static_cast<uint32_t>(target_evt))
    {
    }
  };

  struct BindingGroup
  {
    std::initializer_list<EventBinding> bindings;
  };

  /**
   * @brief 模块事件入口描述
   */
  struct ModuleInfo
  {
    const char* name;
    LibXR::Event* event_ptr;

    template <typename T>
    constexpr ModuleInfo(const char* n, T& module)
        : name(n), event_ptr(&module.GetEvent())
    {
    }
    constexpr ModuleInfo(const char* n, LibXR::Event* e) : name(n), event_ptr(e) {}
  };

  /**
   * @brief 构造 EventBinder 并注册全部绑定关系
   * @param modules 可绑定的模块事件列表
   * @param event_binding_groups 分组事件绑定配置
   */
  EventBinder(std::initializer_list<ModuleInfo> modules,
              std::initializer_list<BindingGroup> event_binding_groups = {})
  {
    auto find_event = [&](const char* name) -> LibXR::Event*
    {
      for (const auto& mod : modules)
      {
        if (strcmp(mod.name, name) == 0)
        {
          return mod.event_ptr;
        }
      }
      return nullptr;
    };

    for (const auto& group : event_binding_groups)
    {
      for (const auto& binding : group.bindings)
      {
        LibXR::Event* source_event = find_event(binding.source_module);
        LibXR::Event* target_event = find_event(binding.target_module);

        if (source_event && target_event)
        {
          target_event->Bind(*source_event, static_cast<uint32_t>(binding.source_event),
                             static_cast<uint32_t>(binding.target_event));
        }
      }
    }
  }

  /**
   * @brief 监控回调
   */
  void OnMonitor() {}

 private:
};
