#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: No description provided
constructor_args:
  - dr16: '@dr16'
  - chassis: '@omni_chassis'
template_args:
  - ChassisType: Omni<RMMotorContainer>
  - MotorType: RMMotorContainer
required_hardware:
  - dr16
depends:
  - qdu-future/DR16
  - qdu-future/Chassis
  - qdu-future/Motor
  - qdu-future/CMD
=== END MANIFEST === */
// clang-format on

#include <sys/_intsup.h>
#include "DR16.hpp"
#include "Chassis.hpp"
#include "app_framework.hpp"
#include "libxr_def.hpp"
#include "thread.hpp"

template <typename ChassisType, typename MotorType>
class EventBinder : public LibXR::Application {
 public:
  EventBinder(LibXR::HardwareContainer &hw, LibXR::ApplicationManager &app,
              DR16 &dr16, Chassis<ChassisType, MotorType> &chassis)
      : dr16_(dr16), chassis_(chassis) {
    UNUSED(hw);
    UNUSED(app);

    thread_.Create(this, ThreadFunction, "EventBinderThread", 1024,
                   LibXR::Thread::Priority::LOW);
  }

  static void ThreadFunction(EventBinder<ChassisType, MotorType> *binder) {
    auto &dr16_event_source = binder->dr16_.GetEvent();
    auto &chassis_event_handler = binder->chassis_.GetEvent();
    chassis_event_handler.Bind(
        dr16_event_source,
        static_cast<uint32_t>(DR16::SwitchPos::DR16_SW_L_POS_TOP),
        static_cast<uint32_t>(ChassisEvent::SET_MODE_RELAX));

    chassis_event_handler.Bind(
        dr16_event_source,
        static_cast<uint32_t>(DR16::SwitchPos::DR16_SW_L_POS_MID),
        static_cast<uint32_t>(ChassisEvent::SET_MODE_FOLLOW));
  }

  void OnMonitor() override {}

 private:
  LibXR::Thread thread_;
  DR16 &dr16_;
  Chassis<ChassisType, MotorType> &chassis_;
};
