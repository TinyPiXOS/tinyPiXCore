#ifndef __TP_CONFIG_H
#define __TP_CONFIG_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(IPitpConfig);

/// @brief 读取INI格式配置文件
class tpConfig
{
public:
	tpConfig();
	virtual ~tpConfig();

public:
	virtual bool readFrom(const char *cfgFile);

	virtual bool saveAs(const char *cfgFile);
	virtual bool save();

public:
	virtual const char *getComment();
	virtual void setComment(const char *comment);

public:
	virtual int32_t getNbrSections();
	virtual const char **getAllSections();
	virtual bool hasSections(const char *section);

public:
	virtual const char *getSectionComment(const char *section);
	virtual void setSectionComment(const char *section, const char *comment);

public:
	virtual int32_t getNbrKeys(const char *section);
	virtual const char **getKeys(const char *section);
	virtual bool hasKeys(const char *section, const char *key);

public:
	virtual bool deleteSeciton(const char *section);
	virtual bool deleteKey(const char *section, const char *key);

public:
	virtual bool deleteVaule(const char *section, const char *key);

public:
	virtual const char *getKeyComment(const char *section, const char *key);
	virtual void setKeyComment(const char *section, const char *key, const char *comment);

public:
	virtual const char *keyValueAsString(const char *section, const char *key);
	virtual int32_t keyValueAsInteger(const char *section, const char *key);
	virtual float keyValueAsFloat(const char *section, const char *key);
	virtual bool keyValueAsBool(const char *section, const char *key);

public:
	virtual void setKeyValueAsString(const char *section, const char *key, const char *value);
	virtual void setKeyValueAsInteger(const char *section, const char *key, int32_t value);
	virtual void setKeyValueAsFloat(const char *section, const char *key, float value);
	virtual void setKeyAsBool(const char *section, const char *key, bool value);

private:
	IPitpConfig *tpConfigSet;
};

#endif
