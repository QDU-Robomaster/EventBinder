#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: 事件绑定模块，支持通过构造函数参数配置事件绑定关系
constructor_args:
  - modules:
    # List of modules involved in event binding
    - name: "dr16"
      module_ref: '@dr16'
    - name: "cmd"
      module_ref: '@cmd'
    - name: "launcher"
      module_ref: '@Launcher_0'
    - name: "chassis"
      module_ref: '@Chassis_0'
  - event_binding_groups:
    # CMD control mode switching
    - bindings:
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_MID
        target_module: "cmd"
        target_event: CMD::Mode::CMD_OP_CTRL
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_BOT
        target_module: "cmd"
        target_event: CMD::Mode::CMD_OP_CTRL
    # Chassis mode switching
    - bindings:
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_L_POS_TOP
        target_module: "chassis"
        target_event: ChassisEvent::SET_MODE_RELAX
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_L_POS_MID
        target_module: "chassis"
        target_event: ChassisEvent::SET_MODE_INDEPENDENT
    # Launcher mode switching
    - bindings:
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_MID
        target_module: "launcher"
        target_event: DartEvent::SET_MODE_FRIC_STOP
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_BOT
        target_module: "launcher"
        target_event: DartEvent::SET_MODE_FRIC_START
template_args:
  - ChassisType: Mecanum
required_hardware:
  - dr16
depends:
  - qdu-future/DR16
  - qdu-future/Chassis
  - qdu-future/RMMotor
  - qdu-future/CMD
=== END MANIFEST === */
// clang-format on

#include <cstdint>
#include <initializer_list>

#include "CMD.hpp"
#include "Chassis.hpp"
#include "DR16.hpp"
#include "app_framework.hpp"

class EventBinder : public LibXR::Application {
 public:
  struct EventBinding {
    const char* source_module;  // 源模块名称 ("dr16", "cmd", "chassis")
    uint32_t source_event;      // 源事件ID
    const char* target_module;  // 目标模块名称 ("cmd", "chassis", "dr16")
    uint32_t target_event;      // 目标事件ID

    template <typename T1, typename T2>
    constexpr EventBinding(const char* src_mod, T1 src_evt,
                           const char* target_mod, T2 target_evt)
        : source_module(src_mod),
          source_event(static_cast<uint32_t>(src_evt)),
          target_module(target_mod),
          target_event(static_cast<uint32_t>(target_evt)) {}
  };

  struct BindingGroup {
    std::initializer_list<EventBinding> bindings;
  };

  struct ModuleInfo {
    const char* name;
    LibXR::Event* event_ptr;

    template <typename T>
    constexpr ModuleInfo(const char* n, T& module)
        : name(n), event_ptr(&module.GetEvent()) {}
    constexpr ModuleInfo(const char* n, LibXR::Event* e)
        : name(n), event_ptr(e) {}
  };

  EventBinder(LibXR::HardwareContainer& hw, LibXR::ApplicationManager& app,
              std::initializer_list<ModuleInfo> modules,
              std::initializer_list<BindingGroup> event_binding_groups) {
    UNUSED(hw);
    UNUSED(app);

    auto find_event = [&](const char* name) -> LibXR::Event* {
      for (const auto& mod : modules) {
        if (strcmp(mod.name, name) == 0) {
          return mod.event_ptr;
        }
      }
      return nullptr;
    };

    for (const auto& group : event_binding_groups) {
      for (const auto& binding : group.bindings) {
        LibXR::Event* source_event = find_event(binding.source_module);
        LibXR::Event* target_event = find_event(binding.target_module);

        if (source_event && target_event) {
          target_event->Bind(*source_event,
                             static_cast<uint32_t>(binding.source_event),
                             static_cast<uint32_t>(binding.target_event));
        }
      }
    }
  }

  void OnMonitor() override {}

 private:
};
