#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: 将任意两个 LibXR 事件端点连接起来
constructor_args:
  - bindings: []
required_hardware: []
depends: []
=== END MANIFEST === */
// clang-format on

#include <cstdint>
#include <initializer_list>
#include <type_traits>

#include "app_framework.hpp"
#include "event.hpp"
#include "libxr_def.hpp"

class EventBinder : public LibXR::Application {
 public:
  struct EventPoint {
    LibXR::Event& event;
    uint32_t id;

    template <typename Id>
    constexpr EventPoint(LibXR::Event& event, Id id)
        : event(event), id(static_cast<uint32_t>(id)) {
      static_assert(std::is_integral_v<Id> || std::is_enum_v<Id>,
                    "Event IDs must be integers or enums");
    }
  };

  struct EventBinding {
    EventPoint source;
    EventPoint target;
  };

  explicit EventBinder(std::initializer_list<EventBinding> bindings) {
    for (const auto& BINDING : bindings) {
      BINDING.target.event.Bind(BINDING.source.event, BINDING.source.id,
                                BINDING.target.id);
    }
  }

  EventBinder(LibXR::HardwareContainer& hw, LibXR::ApplicationManager& app,
              std::initializer_list<EventBinding> bindings)
      : EventBinder(bindings) {
    UNUSED(hw);
    UNUSED(app);
  }

  void OnMonitor() override {}
};
