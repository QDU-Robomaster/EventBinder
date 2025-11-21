#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: 事件绑定模块，支持通过构造函数参数配置事件绑定关系
constructor_args:
  - dr16: '@dr16'
  - chassis: '@omni_chassis'
  - cmd: '@cmd'
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
        target_event: CMD::Mode::CMD_AUTO_CTRL
    # Chassis mode switching
    - bindings:
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_L_POS_TOP
        target_module: "chassis"
        target_event: ChassisEvent::SET_MODE_RELAX
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_L_POS_MID
        target_module: "chassis"
        target_event: ChassisEvent::SET_MODE_FOLLOW
template_args:
  - ChassisType: Omni
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

template <typename ChassisType>
class EventBinder : public LibXR::Application {
 public:
  struct EventBinding {
    const char* source_module;  // 源模块名称 ("dr16", "cmd", "chassis")
    uint32_t source_event;      // 源事件ID
    const char* target_module;  // 目标模块名称 ("cmd", "chassis", "dr16")
    uint32_t target_event;      // 目标事件ID

    template <typename T1, typename T2>
    constexpr EventBinding(const char* src_mod, T1 src_evt, const char* target_mod, T2 target_evt)
        : source_module(src_mod),
          source_event(static_cast<uint32_t>(src_evt)),
          target_module(target_mod),
          target_event(static_cast<uint32_t>(target_evt)) {}
  };

  struct BindingGroup {
    std::initializer_list<EventBinding> bindings;
  };

  EventBinder(LibXR::HardwareContainer& hw, LibXR::ApplicationManager& app,
              DR16& dr16, Chassis<ChassisType>& chassis, CMD& cmd,
              std::initializer_list<BindingGroup> event_binding_groups)
      : dr16_(dr16), chassis_(chassis), cmd_(cmd) {
    UNUSED(hw);
    UNUSED(app);

    // 遍历每个绑定组
    for (const auto& group : event_binding_groups) {
      // 遍历组内的每个绑定
      for (const auto& binding : group.bindings) {
        LibXR::Event* source_event = GetEventByName(binding.source_module);
        LibXR::Event* target_event = GetEventByName(binding.target_module);

        // 执行绑定
        if (source_event && target_event) {
          target_event->Bind(*source_event, static_cast<uint32_t>(binding.source_event),
                             static_cast<uint32_t>(binding.target_event));
        }
      }
    }
  }

  void OnMonitor() override {}

 private:
  LibXR::Thread thread_;
  DR16& dr16_;
  Chassis<ChassisType>& chassis_;
  CMD& cmd_;

  LibXR::Event* GetEventByName(const char* module_name) {
    if (strcmp(module_name, "dr16") == 0) {
      return &dr16_.GetEvent();
    } else if (strcmp(module_name, "chassis") == 0) {
      return &chassis_.GetEvent();
    } else if (strcmp(module_name, "cmd") == 0) {
      return &cmd_.GetEvent();
    }
    return nullptr;
  }
};
