# EventBinder

该模块作为事件的“粘合剂”，将来自 DR16 遥控器的输入事件绑定到底盘（Chassis）模块的特定控制事件。

它负责监听遥控器开关位置的变化，并将其翻译成底盘的模式切换指令，例如将遥控器左侧拨码开关的“上”位置映射为底盘的“放松”模式。

## Required Hardware
无

## Constructor Arguments
| 名称      | 类型       | 描述                          |
| --------- | ---------- | ----------------------------- |
| `dr16`    | `@dr16`    | 对 `DR16` 模块实例的引用。    |
| `chassis` | `@chassis` | 对 `Chassis` 模块实例的引用。 |

## Template Arguments
| 名称          | 默认值                   | 描述                       |
| ------------- | ------------------------ | -------------------------- |
| `ChassisType` | `Omni<RMMotorContainer>` | 底盘的具体实现类型。       |
| `MotorType`   | `RMMotorContainer`       | 底盘所使用的电机容器类型。 |

## Depends
- `DR16`
- `Chassis`
