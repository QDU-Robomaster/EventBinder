# EventBinder

将两个事件端点连接起来：触发源事件时，同步触发目标事件。端点由
`LibXR::Event&` 和事件 ID 组成，不要求事件来自模块，也不处理控制源或业务模式。

## C++

```cpp
LibXR::Event input_events;
LibXR::Event output_events;

EventBinder binder({
    {{input_events, 1}, {output_events, 2}},
});

input_events.Active(1);  // 同步触发 output_events 的事件 2
```

事件 ID 支持整数和枚举，内部转换为 `uint32_t`。枚举所属对象仍由调用方保证。
源、目标 `Event` 必须在绑定生效期间一直存活。绑定注册后不会随 binder
对象析构而解除。

## XRobot YAML

```yaml
- id: event_binder
  name: EventBinder
  constructor_args:
    bindings:
    - source:
        event: '@dr16.GetEvent()'
        id: DR16::SwitchPos::DR16_SW_R_POS_MID
      target:
        event: '@cmd.GetEvent()'
        id: CMD::Mode::CMD_OP_CTRL
```

`@` 后的内容直接作为 C++ 表达式。`GetEvent()` 只是调用方取得事件对象的一种方式，
绑定器本身不调用它，也不依赖 DR16 或 CMD。端点字段顺序使用 `event`、`id`，
绑定字段顺序使用 `source`、`target`，与生成器输出的聚合初始化顺序一致。
默认 `bindings: []`，无需硬件或其他模块。

## 配置约束

模块不检查重复绑定或环路。重复绑定会让目标回调重复执行；自绑定或多级环路在
事件触发时可能无限递归，直至栈溢出。配置者需要保证绑定关系无环。
EventBinder 本身不使用动态数组；底层 `LibXR::Event::Bind()` 在初始化时
为每条绑定分配记录。

## 执行上下文

实际转发使用 `LibXR::Event::Bind()`，没有新线程或队列。目标回调在源事件的
调用上下文中同步执行，并透传 `in_isr`。目标回调必须适合该上下文，尤其不能
在 ISR 路径阻塞。分支回调的顺序沿用 LibXR，不由本模块保证。

## 从旧配置迁移

删除 `modules` 和 `event_binding_groups`，把各组按原顺序展开为 `bindings`。
根据原模块别名对应的 `module_ref` 填入事件表达式，例如别名 `chassis` 对应
`@helm_chassis`，新端点使用 `@helm_chassis.GetEvent()`。保留原事件 ID 和边的顺序。

## 测试

使用模块内 `tests/CMakeLists.txt`，传入本地 LibXR 源码目录：

```sh
cmake -S Modules/EventBinder/tests -B build/event-binder-tests \
  -DLIBXR_ROOT="$PWD/Middlewares/Third_Party/LibXR"
cmake --build build/event-binder-tests
ctest --test-dir build/event-binder-tests --output-on-failure
```
