# TpChart组件提交日志

## 本次变更
- 统一 `TpChart`、`TpAxis`、`TpSeries` 的私有数据封装方式，公开头文件仅保留不透明指针声明。
- 移除 `TpChart` 自身重复维护的 CSS 缓存，改为复用 `TpWidget` 的状态样式读取逻辑。
- 清理图表组件公开接口中的实现细节，避免把内部 helper 和私有数据结构暴露到对外头文件中。
- 规范示例程序成员命名，统一为后缀下划线风格，减少 `m_` 命名混用。
- 保持图表绘制、交互和示例运行逻辑不变，仅做封装与一致性整理。

## 涉及文件
- `src/include/TpGUI/Widgets/TpChart.h`
- `src/include/TpGUI/Widgets/TpAxis.h`
- `src/include/TpGUI/Widgets/TpSeries.h`
- `src/include_p/TpGUI/Widgets/TpChart_p.h`
- `src/src/TpGUI/Widgets/TpChart.cpp`
- `src/src/TpGUI/Widgets/TpSeries.cpp`
- `examples/TpGUI/TpChart/MainWindowService.h`
- `examples/TpGUI/TpChart/MainWindowService.cpp`
- `examples/TpGUI/TpChart/MainWindowServiceAdvanced.h`
- `examples/TpGUI/TpChart/MainWindowServiceAdvanced.cpp`

## 说明
- 本次修改不引入新的对外业务接口。
- 现有图表示例仍可继续用于折线图、柱状图、散点图和饼图验证。