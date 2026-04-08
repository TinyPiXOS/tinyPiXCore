/*
 * 版权声明 (Copyright Declaration)
 * 作者 (Author)：张金圣
 * 邮箱 (Email)：3270141465@qq.com
 * 版权所有 (Copyright)：© 2026 张金圣. All rights reserved.
 * 描述 (Description)：该类实现逻辑数据与物理像素的映射映射及坐标轴布局计算
 */
 
#include "TpAxis.h"
#include <algorithm>
#include <cmath>

/// @brief 私有数据实现结构体
struct TpAxisData{
    TpAxis::AxisMode mode_ = TpAxis::AxisMode::Value;
    double min_ = 0.0;
    double max_ = 100.0;
    double step_ = 10.0;
    int32_t categoryCount_ = 1;
    TpVector<double> tickValues_;
    bool autoRange_ = true;

    int32_t tickCount_ = 0;   
    double fixedStep_ = 0.0;
    bool symmetric_ = false; // 是否强制对称标志
    bool autoRangeNice_ = true;   // 新增：默认允许美化范围
    bool rollingMode_ = false;   // 新增：滚动模式标志
    double xLeftPad_ = 0.0;
    double xRightPad_ = 0.0;
    double yTopPad_ = 0.0;
    double yBottomPad_ = 0.0;
};

TpAxis::TpAxis() {
    data_ = (ITpAxisData*)new TpAxisData();
}

TpAxis::~TpAxis() {
    delete (TpAxisData*)data_;
}

void TpAxis::setRange(double min, double max, bool includeZero) {
    TpAxisData *d = (TpAxisData*)data_;
    if (min > max) {
        std::swap(min, max);
    }

    d->min_ = min;
    d->max_ = max;

    if (includeZero) {
        if (d->min_ > 0) d->min_ = 0;
        if (d->max_ < 0) d->max_ = 0;
    }

    if (std::abs(d->max_ - d->min_) < 1e-9) {
        d->max_ = d->min_ + 1.0;
    }
}

void TpAxis::setMode(AxisMode mode) {
    ((TpAxisData*)data_)->mode_ = mode;
}

void TpAxis::setCategoryCount(int32_t count) {
    ((TpAxisData*)data_)->categoryCount_ = count;
}

int32_t TpAxis::mapToPixel(double value, int32_t length, int32_t offset, bool inverse) const {
    TpAxisData *d = (TpAxisData*)data_;
    if (length <= 0) return offset;
    
    double ratio = 0.0;

    if (d->mode_ == AxisMode::Value) {
        double range = d->max_ - d->min_;
        if (std::abs(range) < 1e-9) {
            ratio = 0.5;
        } else {
            ratio = (value - d->min_) / range;
        }
    } else {
        if (d->categoryCount_ <= 0) {
            ratio = 0.5;
        } else {
            int32_t index = static_cast<int32_t>(std::floor(value + 0.5));
            if (index < 0) index = 0;
            if (index >= d->categoryCount_) index = d->categoryCount_ - 1;
            ratio = (index + 0.5) / static_cast<double>(d->categoryCount_);
        }
    }

    if (inverse) {
        return offset + static_cast<int32_t>((1.0 - ratio) * length + 0.5);
    }
    return offset + static_cast<int32_t>(ratio * length + 0.5);
}

bool TpAxis::containsZero() const {
    TpAxisData *d = (TpAxisData*)data_;
    return d->min_ <= 0.0 && d->max_ >= 0.0;
}

int32_t TpAxis::ZeroPixel(int32_t length, int32_t offset, bool inverse) const {
    TpAxisData *d = (TpAxisData*)data_;
    if (d->mode_ == AxisMode::Category) {
        return mapToPixel(0, length, offset, inverse);
    }

    double target = 0.0;
    if (target < d->min_) target = d->min_;
    if (target > d->max_) target = d->max_;
    return mapToPixel(target, length, offset, inverse);
}

TpAxis::BarGeometry TpAxis::mapBarToGeometry(int32_t catIndex, int32_t seriesCount, int32_t seriesIndex, 
                                            int32_t length, int32_t offset, bool inverse) const {
    TpAxisData *d = (TpAxisData*)data_;
    if (d->categoryCount_ <= 0 || seriesCount <= 0 || length <= 0) {
        return BarGeometry(offset, 0);
    }

    double slotWidth = static_cast<double>(length) / d->categoryCount_;
    double slotCenter;
    if (!inverse) {
        slotCenter = offset + (catIndex + 0.5) * slotWidth;
    } else {
        slotCenter = (offset + length) - (catIndex + 0.5) * slotWidth;
    }

    const double categoryGapFraction = 0.2; 
    double clusterWidth = slotWidth * (1.0 - categoryGapFraction);
    double barWidth = clusterWidth / seriesCount;
    double clusterStart = slotCenter - (clusterWidth / 2.0);
    double barStart = clusterStart + (seriesIndex * barWidth);

    return BarGeometry(
        static_cast<int32_t>(std::floor(barStart + 0.5)),
        static_cast<int32_t>(std::floor(barWidth + 0.5))
    );
}

double TpAxis::mapToValue(int32_t pixel, int32_t length, int32_t offset, bool inverse) const {
    TpAxisData *d = (TpAxisData*)data_;
    if (length <= 0) return d->min_;

    double ratio = static_cast<double>(pixel - offset) / length;
    if (inverse) ratio = 1.0 - ratio;

    if (d->mode_ == AxisMode::Value) {
        return d->min_ + ratio * (d->max_ - d->min_);
    } else {
        if (d->categoryCount_ <= 0) return 0.0;
        double index = ratio * static_cast<double>(d->categoryCount_) - 0.5;
        if (index < -0.5) index = -0.5;
        if (index > d->categoryCount_ - 0.5) index = d->categoryCount_ - 0.5;
        return index;
    }
}

double TpAxis::calculateNiceStep(double range, int32_t targetCount) {
    if (targetCount <= 1) return 1.0;
    if (range <= 1e-12) return 1.0; 

    double rawStep = range / (targetCount - 1);
    double exponent = std::floor(std::log10(rawStep));
    double fraction = rawStep / std::pow(10, exponent);

    double niceFraction;
    if (fraction < 1.5)      niceFraction = 1.0;
    else if (fraction < 3.0) niceFraction = 2.0;
    else if (fraction < 7.0) niceFraction = 5.0;
    else                     niceFraction = 10.0;

    return niceFraction * std::pow(10, exponent);
}

const TpVector<double>& TpAxis::getTickValues() const {
    return ((TpAxisData*)data_)->tickValues_;
}

double TpAxis::min() const {
    return ((TpAxisData*)data_)->min_;
}

double TpAxis::max() const {
    return ((TpAxisData*)data_)->max_;
}

// --- 核心修改部分：大数据降采样与缩放支持 ---
TpVector<TpAxis::SamplePoint> TpAxis::downsample(const TpVector<double>& rawData, int32_t startIndex, int32_t count, int32_t pixelWidth) {
    TpVector<SamplePoint> result;
    int32_t totalDataSize = rawData.size();

    // 安全边界处理
    if (totalDataSize == 0 || pixelWidth <= 0 || count <= 0) return result;
    if (startIndex < 0) startIndex = 0;
    if (startIndex >= totalDataSize) return result;
    if (startIndex + count > totalDataSize) count = totalDataSize - startIndex;

    // 计算每个像素跨越的数据点数
    double samplesPerPixel = static_cast<double>(count) / pixelWidth;

    for (int32_t i = 0; i < pixelWidth; ++i) {
        // 计算当前像素覆盖的数据点索引范围 [fStart, fEnd)
        double fStart = startIndex + i * samplesPerPixel;
        double fEnd = startIndex + (i + 1) * samplesPerPixel;

        int32_t iStart = static_cast<int32_t>(fStart);
        int32_t iEnd = static_cast<int32_t>(std::ceil(fEnd)); // 确保覆盖浮点边缘

        if (iStart >= totalDataSize) break;
        if (iEnd > totalDataSize) iEnd = totalDataSize;

        SamplePoint sp;
        sp.firstVal = rawData[iStart];
        sp.lastVal = rawData[std::max(iStart, iEnd - 1)];
        
        double sum = 0, sumSq = 0;
        int32_t minIdx = iStart, maxIdx = iStart;
        sp.minVal = sp.maxVal = rawData[iStart];

        // 遍历区间内所有点，捕捉所有极值
        for (int32_t j = iStart; j < iEnd; ++j) {
            double val = rawData[j];
            if (val < sp.minVal) { sp.minVal = val; minIdx = j; }
            if (val > sp.maxVal) { sp.maxVal = val; maxIdx = j; }
            sum += val;
            sumSq += val * val;
        }

        // 计算亚像素相对位置，处理缩放后的精确对齐
        int32_t bucketSize = iEnd - iStart;
        if (bucketSize > 1) {
            sp.minRelX = static_cast<double>(minIdx - iStart) / (bucketSize - 1);
            sp.maxRelX = static_cast<double>(maxIdx - iStart) / (bucketSize - 1);
        } else {
            sp.minRelX = 0.5; sp.maxRelX = 0.5;
        }

        sp.meanVal = sum / bucketSize;
        double variance = (sumSq / bucketSize) - (sp.meanVal * sp.meanVal);
        sp.stdDev = std::sqrt(std::max(0.0, variance));

        result.append(sp);
    }
    return result;
}


TpVector<int32_t> TpAxis::mapAllToPixels(const TpAxis& axis, const TpVector<double>& rawData, int32_t startIndex, int32_t count, int32_t length, int32_t offset,  bool inverse) 
{
    TpVector<int32_t> pixelResults;
    int32_t totalSize = rawData.size();
    
    // 边界检查
    if (startIndex < 0) startIndex = 0;
    if (startIndex + count > totalSize) count = totalSize - startIndex;
    if (count <= 0) return pixelResults;

    // 预分配内存，避免多次扩容带来的拷贝开销
    pixelResults.reserve(count);

    for (int32_t i = 0; i < count; ++i) {
        double val = rawData[startIndex + i];
        // 直接调用单点转换逻辑
        int32_t pix = axis.mapToPixel(val, length, offset, inverse);
        pixelResults.append(pix);
    }

    return pixelResults;
}



//  设置是否自动计算范围
void TpAxis::setAutoRange(bool autoRange) {
    ((TpAxisData*)data_)->autoRange_ = autoRange;
}

//  获取当前是否是自动范围
bool TpAxis::isAutoRange() const {
    return ((TpAxisData*)data_)->autoRange_;
}

void TpAxis::setTickCount(int32_t count) {
    ((TpAxisData*)data_)->tickCount_ = count;
}

void TpAxis::setTickStep(double step) {
    ((TpAxisData*)data_)->fixedStep_ = step;
}

void TpAxis::setSymmetric(bool enabled) {
    ((TpAxisData*)data_)->symmetric_ = enabled;
}

bool TpAxis::isSymmetric() const {
    return ((TpAxisData*)data_)->symmetric_;
}

void TpAxis::updateNiceTicks(int32_t targetCount) {
    TpAxisData *d = (TpAxisData*)data_;

     if (d->mode_ == AxisMode::Category) {
        d->tickValues_.clear();
        for (int32_t i = 0; i < d->categoryCount_; ++i) {
            d->tickValues_.append(static_cast<double>(i));
        }
        return;
    }

    // 保存原始范围（如果需要恢复）
    double origMin = d->min_;
    double origMax = d->max_;
    bool restoreRange = !d->autoRangeNice_;

    // --- 对称性处理：仅在允许美化范围时才修改范围 ---
    if (d->symmetric_ && d->autoRangeNice_) {
        double maxAbs = std::max(std::abs(d->min_), std::abs(d->max_));
        if (maxAbs < 1e-9) maxAbs = 1.0;
        d->min_ = -maxAbs;
        d->max_ = maxAbs;
    }

    // 1. 固定刻度个数（不修改范围）
    if (d->tickCount_ > 1) {
        d->tickValues_.clear();
        d->step_ = (d->max_ - d->min_) / (d->tickCount_ - 1);
        for (int32_t i = 0; i < d->tickCount_; ++i) {
            double val = d->min_ + i * d->step_;
            if (std::abs(val) < (d->step_ * 1e-10)) val = 0.0;
            d->tickValues_.append(val);
        }
        if (restoreRange) {
            d->min_ = origMin;
            d->max_ = origMax;
        }
        return;
    }

    // 2. 固定步长
    if (d->fixedStep_ > 0) {
        d->step_ = d->fixedStep_;
        double eps = d->step_ * 1e-10;
        double niceMin = std::floor((d->min_ + eps) / d->step_) * d->step_;
        double niceMax = std::ceil((d->max_ - eps) / d->step_) * d->step_;
        int32_t tickCount = static_cast<int32_t>((niceMax - niceMin) / d->step_ + 0.5);

        d->tickValues_.clear();
        for (int32_t i = 0; i <= tickCount; ++i) {
            double val = niceMin + i * d->step_;
            if (std::abs(val) < eps) val = 0.0;
            d->tickValues_.append(val);
        }

        if (d->autoRangeNice_) {
            d->min_ = niceMin;
            d->max_ = niceMax;
        } else {
            // 不修改范围，但可能后续需要恢复（以防前面对称性修改过）
            if (restoreRange) {
                d->min_ = origMin;
                d->max_ = origMax;
            }
        }
        return;
    }

    // 3. 自动计算步长
    if (targetCount <= 1) {
        d->tickValues_.clear();
        d->tickValues_.append(d->min_);
        if (restoreRange) {
            d->min_ = origMin;
            d->max_ = origMax;
        }
        return;
    }

    double range = d->max_ - d->min_;
    d->step_ = calculateNiceStep(range, targetCount);
    double eps = d->step_ * 1e-10;
    double niceMin = std::floor((d->min_ + eps) / d->step_) * d->step_;
    double niceMax = std::ceil((d->max_ - eps) / d->step_) * d->step_;
    int32_t tickCount = static_cast<int32_t>((niceMax - niceMin) / d->step_ + 0.5);

    d->tickValues_.clear();
    for (int32_t i = 0; i <= tickCount; ++i) {
        double val = niceMin + i * d->step_;
        if (std::abs(val) < eps) val = 0.0;
        d->tickValues_.append(val);
    }

    if (d->autoRangeNice_) {
        d->min_ = niceMin;
        d->max_ = niceMax;
    } else {
        // 恢复原始范围
        d->min_ = origMin;
        d->max_ = origMax;
    }
}

void TpAxis::setAutoRangeNice(bool enabled) {
    ((TpAxisData*)data_)->autoRangeNice_ = enabled;
}

bool TpAxis::isAutoRangeNice() const {
    return ((TpAxisData*)data_)->autoRangeNice_;
}

void TpAxis::setRollingMode(bool enabled) {
    ((TpAxisData*)data_)->rollingMode_ = enabled;
}

bool TpAxis::isRollingMode() const {
    return ((TpAxisData*)data_)->rollingMode_;
}

void TpAxis::setXPadding(double leftRatio, double rightRatio) {
    TpAxisData* d = (TpAxisData*)data_;
    d->xLeftPad_ = leftRatio;
    d->xRightPad_ = rightRatio;
}

void TpAxis::setYPadding(double topRatio, double bottomRatio) {
    TpAxisData* d = (TpAxisData*)data_;
    d->yTopPad_ = topRatio;
    d->yBottomPad_ = bottomRatio;
}

double TpAxis::xLeftPaddingRatio() const {
    return ((TpAxisData*)data_)->xLeftPad_;
}
double TpAxis::xRightPaddingRatio() const {
    return ((TpAxisData*)data_)->xRightPad_;
}
double TpAxis::yTopPaddingRatio() const {
    return ((TpAxisData*)data_)->yTopPad_;
}
double TpAxis::yBottomPaddingRatio() const {
    return ((TpAxisData*)data_)->yBottomPad_;
}
