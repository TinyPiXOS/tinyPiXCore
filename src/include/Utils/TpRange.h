#include "TpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpRangeData);
class TpRange
{
public:
	TpRange(int32_t min = 0, int32_t max = 100);

public:
	~TpRange();

public:
	virtual void setRange(int32_t min = 0, int32_t max = 0);
	virtual void setPosition(int32_t position);
	virtual void setPercent(double percent);

public:
	virtual int32_t position();
	virtual double percent();
	virtual int32_t min();
	virtual int32_t max();
	virtual int32_t length();

private:
	ItpRangeData *data_;
};
