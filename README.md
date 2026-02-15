# EventBinder

用于按配置把一个模块的事件映射到另一个模块事件的绑定模块。

它的目标是把“模块联动关系”从业务代码里抽离出来，避免硬编码耦合。

## 解决什么问题

EventBinder 主要解决三件事：

1. 统一描述跨模块事件映射（源事件 -> 目标事件）。
2. 支持一组模块、多组绑定关系集中管理。
3. 降低模式切换联动时的模块耦合度。

## 绑定模型

EventBinder 通过两类配置完成绑定：

1. `modules`：声明可参与绑定的模块事件入口。
2. `event_binding_groups`：声明每条映射规则。

每条映射规则包含：

1. `source_module`：源模块名。
2. `source_event`：源事件 ID。
3. `target_module`：目标模块名。
4. `target_event`：目标事件 ID。

核心结构与方法：

1. `ModuleInfo`：模块名 + `GetEvent()` 入口。
2. `EventBinding`：单条事件映射。
3. 构造函数：遍历配置并执行 `Bind(...)`。

## 最小接入示例

1. 添加模块：

```bash
xrobot_add_mod EventBinder --instance-id eventbinder
xrobot_gen_main
```

2. 典型配置（将 DR16 开关事件映射到 CMD 模式）：

```yaml
module: EventBinder
entry_header: Modules/EventBinder/EventBinder.hpp
constructor_args:
  - modules:
    - name: "dr16"
      module_ref: '@dr16'
    - name: "cmd"
      module_ref: '@cmd'
  - event_binding_groups:
    - bindings:
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_MID
        target_module: "cmd"
        target_event: CMD::Mode::CMD_OP_CTRL
      - source_module: "dr16"
        source_event: DR16::SwitchPos::DR16_SW_R_POS_BOT
        target_module: "cmd"
        target_event: CMD::Mode::CMD_OP_CTRL
template_args: []
```

## 使用约定

1. `modules` 中的 `name` 必须和 `event_binding_groups` 里引用完全一致。
2. 源/目标模块都必须提供 `GetEvent()`。
3. 事件 ID 推荐统一从枚举转换，避免裸数字。
4. 先验证关键联动链路，再逐步增加绑定条目。

## 模块信息

1. 代码入口：`Modules/EventBinder/EventBinder.hpp`
2. Required Hardware：`dr16`
3. Constructor Arguments：
   - `modules`
   - `event_binding_groups`
4. Template Arguments：None
5. Depends：
   - `qdu-future/DR16`
   - `qdu-future/CMD`
