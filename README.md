# EventBinder

事件绑定模块：按构造参数把一个模块的事件映射到另一个模块的事件，把“模块联动关系”
（例如遥控器拨杆切换 CMD 控制模式、切换云台 / 底盘模式）从业务代码中抽离到配置里。

- `modules` 声明可参与绑定的模块：每项是一个名字和一个提供 `LibXR::Event& GetEvent()` 的模块
  实例（`ModuleInfo(name, module)` 取 `&module.GetEvent()`；也可直接给 `LibXR::Event*`）。
- `event_binding_groups` 是若干组绑定，每组的 `bindings` 是若干条 `EventBinding`：
  `{源模块名, 源事件 ID, 目标模块名, 目标事件 ID}`。事件 ID 可以直接写枚举值，
  构造时统一转换为 `uint32_t`。
- 构造函数遍历全部绑定，按名字在 `modules` 中查找源和目标，执行
  `target_event->Bind(*source_event, source_event_id, target_event_id)`：此后源模块激活
  源事件时，目标模块的目标事件随之被激活。名字找不到的绑定会被静默跳过。
- 绑定只在构造时完成一次，模块本身没有线程、Topic 或运行期逻辑。

## 依赖

- `QDU-Robomaster/DR16`：遥控器模块，常见的事件源（`DR16::SwitchPos`、`DR16::Key`）。
- `QDU-Robomaster/CMD`：常见的事件目标（`CMD::Mode`）。

两者都只在头文件层面被包含，绑定对象可以是任何提供 `GetEvent()` 的模块。
无外部软件包，仅使用 LibXR。

## 构造接口

```cpp
EventBinder(std::initializer_list<ModuleInfo> modules,
            std::initializer_list<BindingGroup> event_binding_groups = {});
```

配置：

- `modules`：`std::initializer_list<EventBinder::ModuleInfo>`，没有默认值，必须填写。
  每项 `{"名字", 实例}`，名字是 C++ 字符串字面量，实例是某个在本实例之前列出的模块实例 id。
- `event_binding_groups`：`std::initializer_list<EventBinder::BindingGroup>`，默认 `{}`（不绑定）。
  每个 `BindingGroup` 只有一个成员 `bindings`，即 `EventBinding` 列表；每条 `EventBinding` 为
  `{"源模块名", 源事件, "目标模块名", 目标事件}`，名字必须与 `modules` 中的名字完全一致。

## 使用

```sh
xrobot module add QDU-Robomaster/EventBinder
xrobot setup
xrobot instance add QDU-Robomaster/EventBinder
```

`xrobot instance add` 在 `User/xrobot.yaml` 中写入一个实例：`modules` 留空（必须填写），
`event_binding_groups` 按源码默认值写为 `'{}'`。下例把 DR16 右拨杆映射到 CMD 控制模式，
把左拨杆映射到云台模式。YAML 列表按位置生成花括号初始化：`['"dr16"', dr16]` 生成
`{"dr16", dr16}`；`event_binding_groups` 的三层列表依次是“组列表 / 一组（只含 `bindings`）/
绑定列表”，每条绑定是四元素列表：

```yaml
modules:
  - module: QDU-Robomaster/EventBinder
    id: eventbinder_0
    args:
      - modules:
          - ['"dr16"', dr16]
          - ['"cmd"', cmd]
          - ['"gimbal"', gimbal]
      - event_binding_groups:
          - - - ['"dr16"', 'DR16::SwitchPos::DR16_SW_R_POS_MID', '"cmd"', 'CMD::Mode::CMD_OP_CTRL']
              - ['"dr16"', 'DR16::SwitchPos::DR16_SW_R_POS_BOT', '"cmd"', 'CMD::Mode::CMD_AUTO_CTRL']
              - ['"dr16"', 'DR16::SwitchPos::DR16_SW_L_POS_TOP', '"gimbal"', 'GimbalEvent::SET_MODE_RELAX']
              - ['"dr16"', 'DR16::SwitchPos::DR16_SW_L_POS_MID', '"gimbal"', 'GimbalEvent::SET_MODE_COMMON']
```

同样的值也可以直接写成一条 C++ 表达式，例如
`- modules: '{{"dr16", dr16}, {"cmd", cmd}, {"gimbal", gimbal}}'`。

`dr16`、`cmd`、`gimbal` 是其他模块实例的 id，须在本实例之前列出：分别由
`QDU-Robomaster/DR16`、`QDU-Robomaster/CMD`、`QDU-Robomaster/Gimbal` 提供。本例没有需要
BSP 用 `XR_REGISTER` 注册的对象。通常把本实例放在 `modules:` 的最后。

填好后再次运行 `xrobot setup`，生成 `User/xrobot_main.hpp`。

`xrobot module show .`（在本仓库中）或 `xrobot module show Modules/QDU-Robomaster/EventBinder`
（在 BSP 中）打印当前的构造函数。
