#include "tpRange.h"

struct tpRangeData
{
	int32_t min = 0;
	int32_t max = 0;
	double percent = 0;
	int32_t position = 0;
	int32_t length = 0;

	tpRangeData()
	{
	}
};

static inline int32_t range_priv_proper(int32_t v, int32_t min, int32_t max)
{
	if (v < min)
	{
		v = min;
	}
	else if (v > max)
	{
		v = max;
	}

	return v;
}

static inline void range_priv_adjust(tpRangeData *range, int32_t min, int32_t max)
{
	if (range)
	{
		if (min > max)
		{
			range->max = min;
			range->min = max;
		}
		else
		{
			range->max = max;
			range->min = min;
		}

		range->length = range->max - range->min + 1;
		range->position = (int32_t)(range->percent / 100 * range->length + range->min);
		range->position = range_priv_proper(range->position, range->min, range->max);
	}
}

tpRange::tpRange(int32_t min, int32_t max)
{
	tpRangeData *set = new tpRangeData();

	if (set)
	{
		this->data_ = set;
		this->setRange(min, max);
	}
}

tpRange::~tpRange()
{
	tpRangeData *set = (tpRangeData *)this->data_;

	if (set)
	{
		delete set;
	}
}

void tpRange::setRange(int32_t min, int32_t max)
{
	tpRangeData *set = (tpRangeData *)this->data_;

	if (set)
	{
		if (min > max)
		{
			set->max = min;
			set->min = max;
		}
		else
		{
			set->min = min;
			set->max = max;
		}

		set->percent = 0;
		set->position = set->min;
		set->length = set->max - set->min + 1;
	}
}

void tpRange::setPosition(int32_t position)
{
	tpRangeData *set = (tpRangeData *)this->data_;

	if (set)
	{
		if (position <= set->min)
		{
			position = set->min;
		}
		else if (position >= set->max)
		{
			position = set->max;
		}

		set->position = position;
		set->percent = (double)(set->position - set->min + 1) / set->length;
	}
}

void tpRange::setPercent(double percent)
{
	tpRangeData *set = (tpRangeData *)this->data_;

	if (set)
	{
		if (percent <= 0)
		{
			set->percent = 0;
			set->position = set->min;
		}
		else if (percent >= 1)
		{
			set->percent = 1;
			set->position = set->max;
		}
		else
		{
			set->percent = percent;
			set->position = (int32_t)(percent * set->length + set->min);
			set->position = range_priv_proper(set->position, set->min, set->max);
		}
	}
}

int32_t tpRange::position()
{
	tpRangeData *set = (tpRangeData *)this->data_;
	int32_t position = 0;

	if (set)
	{
		position = set->position;
	}

	return position;
}

double tpRange::percent()
{
	tpRangeData *set = (tpRangeData *)this->data_;
	double percent = 0;

	if (set)
	{
		percent = set->percent;
	}

	return percent;
}

int32_t tpRange::min()
{
	tpRangeData *set = (tpRangeData *)this->data_;
	int32_t min = 0;

	if (set)
	{
		min = set->min;
	}

	return min;
}

int32_t tpRange::max()
{
	tpRangeData *set = (tpRangeData *)this->data_;
	int32_t max = 0;

	if (set)
	{
		max = set->max;
	}

	return max;
}

int32_t tpRange::length()
{
	tpRangeData *set = (tpRangeData *)this->data_;
	int32_t length = 0;

	if (set)
	{
		length = set->length;
	}

	return length;
}
