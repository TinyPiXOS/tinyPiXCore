#ifndef __TP_PAINTER_PATH_PRIVATE_H
#define __TP_PAINTER_PATH_PRIVATE_H

#include <algorithm>
#include <cmath>
#include "TpVector.h"
#include <limits>

// 路径元素类型
enum TpPathElementType
{
    MoveTo,
    LineTo,
    CubicTo,
    CloseSubpath
};

// 路径元素结构
struct TpPathElement
{
    TpPathElementType type;
    TpVector<TpPoint> points;

    TpPathElement()
    {
    }
    TpPathElement(TpPathElementType type, const TpVector<TpPoint> &points)
        : type(type), points(points)
    {
    }
};

// 路径数据实现
struct TpPainterPathData
{
    TpVector<TpPathElement> elements;
    TpPoint currentPoint;
    TpPoint startPoint; // 当前子路径的起点
    bool isClosed = false;

    // 添加元素并更新当前点
    void addElement(TpPathElementType type, const TpVector<TpPoint> &pts)
    {
        elements.emplace_back(TpPathElement(type, pts));

        if (!pts.empty())
        {
            switch (type)
            {
            case TpPathElementType::MoveTo:
                currentPoint = pts[0];
                startPoint = currentPoint;
                isClosed = false;
                break;
            case TpPathElementType::LineTo:
                currentPoint = pts[0];
                break;
            case TpPathElementType::CubicTo:
                if (pts.size() >= 3)
                {
                    currentPoint = pts[2];
                }
                break;
            case TpPathElementType::CloseSubpath:
                currentPoint = startPoint;
                isClosed = true;
                break;
            }
        }
    }
};

#endif
