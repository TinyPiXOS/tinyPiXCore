#include "TpApp.h"
#include <TpString.h>
#include <TpHash.h>
#include <iostream>
#include "TpCssParser.h"

#include "TpList.h"

// 根据线性渐变角度 计算线性渐变射线与矩形边界的交点
std::pair<TpPoint, TpPoint> calculateRayIntersections(float angleDeg, float width, float height)
{
    const float cx = width / 2.0f;
    const float cy = height / 2.0f;

    // 角度转弧度
    float angle_rad = angleDeg * M_PI / 180.0f;

    // 计算方向向量
    float dx = std::sin(angle_rad);
    float dy = -std::cos(angle_rad);

    // 处理零向量情况
    if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f)
    {
        return {TpPoint(cx, cy), TpPoint(cx, cy)};
    }

    // 计算正方向交点
    float t_positive = std::numeric_limits<float>::max();
    if (std::abs(dx) > 1e-6f)
    {
        float t_left = (0.0f - cx) / dx;
        float t_right = (width - cx) / dx;
        if (t_left > 0)
            t_positive = std::min(t_positive, t_left);
        if (t_right > 0)
            t_positive = std::min(t_positive, t_right);
    }
    if (std::abs(dy) > 1e-6f)
    {
        float t_top = (0.0f - cy) / dy;
        float t_bottom = (height - cy) / dy;
        if (t_top > 0)
            t_positive = std::min(t_positive, t_top);
        if (t_bottom > 0)
            t_positive = std::min(t_positive, t_bottom);
    }

    // 计算负方向交点
    float t_negative = std::numeric_limits<float>::max();
    if (std::abs(dx) > 1e-6f)
    {
        float t_left = (0.0f - cx) / (-dx);
        float t_right = (width - cx) / (-dx);
        if (t_left > 0)
            t_negative = std::min(t_negative, t_left);
        if (t_right > 0)
            t_negative = std::min(t_negative, t_right);
    }
    if (std::abs(dy) > 1e-6f)
    {
        float t_top = (0.0f - cy) / (-dy);
        float t_bottom = (height - cy) / (-dy);
        if (t_top > 0)
            t_negative = std::min(t_negative, t_top);
        if (t_bottom > 0)
            t_negative = std::min(t_negative, t_bottom);
    }

    // 计算交点坐标
    TpPoint endPoint(cx + t_positive * dx, cy + t_positive * dy);
    TpPoint startPoint(cx - t_negative * dx, cy - t_negative * dy);

    return {startPoint, endPoint};
}

int32_t main(int32_t argc, char *argv[])
{
    std::pair<TpPoint, TpPoint> pointList = calculateRayIntersections(30, 100, 100);

    std::cout << "起始点： " << pointList.first.x() << "  " << pointList.first.y() << std::endl;
    std::cout << "终止点： " << pointList.second.x() << "  " << pointList.second.y() << std::endl;

    // TpApp app(argc, argv);
    // app.setStyle(Tp::SmartDeviceGUIStyle);
    // tpShared<TpCssData> readData = app.cssParser()->readCss("TpButton", "", TpCssParser::Normal);
    // readData->color();

    // TpCssParser cssParser(TpString("./test.css"));

    return 0;
}
