# EventBinder

该模块作为事件的"粘合剂"，支持通过构造函数参数配置事件绑定关系。

## Required Hardware
- dr16

## Constructor Arguments
| 名称      | 类型       | 描述                          |
| --------- | ---------- | ----------------------------- |
| `modules`    | `std::initializer_list<ModuleInfo>`    | 模块信息列表，包含模块名称和事件处理器引用。    |
| `event_binding_groups` | `std::initializer_list<BindingGroup>` | 事件绑定组列表，定义源事件到目标事件的映射关系。 |

## Template Arguments
| 名称          | 默认值                   | 描述                       |
| ------------- | ------------------------ | -------------------------- |
| `ChassisType` | `Omni` | 底盘的具体实现类型。       |

## Depends
- qdu-future/DR16
- qdu-future/Chassis
- qdu-future/RMMotor
- qdu-future/CMD
