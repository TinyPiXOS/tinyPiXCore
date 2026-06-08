# TpChart 示例

这个目录里现在放了两套独立的图表示例，但它们会分别生成两个可执行程序，运行后也是两个单独窗口：

- `TpChart`：折线图、柱状图测试
- `TpChartAdvanced`：散点图、饼图测试

## 生成

在本目录执行：

```bash
make
```

生成完成后，会得到两个程序：

```bash
./TpChart
./TpChartAdvanced
```

## 样式

两个程序共用同一个 `chart_style.css`，这样基础图表和扩展图表的颜色、边框、字体风格可以保持一致。

## 说明

- 两个程序是分开的，不会显示在同一个窗口里。
- `TpChart` 继续使用安装后的 `TpGUI` 库。
- `TpChartAdvanced` 继续编译图表组件源码，方便直接验证散点图和饼图的最新改动。
