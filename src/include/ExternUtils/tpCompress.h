#ifndef __TP_COMMPRESS_H
#define __TP_COMMPRESS_H

#include "tpUtils.h"

TP_DEF_VOID_TYPE_VAR(ItpCompressData);
class tpCompress
{
public:
	/// @brief 打包格式(适用于自定义的压缩),打包格式中，分为只有打包和打包加压缩两种
	enum tpCompressFormat
	{
		TP_FORMAT_NONE,
		TP_FORMAT_ZIP,
		TP_FORMAT_RAR,
		TP_FORMAT_7ZIP,
		TP_FORMAT_ISO,
		TP_FORMAT_PAX,
		TP_FORMAT_RAW,
		TP_FORMAT_TAR,
		TP_FORMAT_AR,
		TP_FORMAT_XAR,
		TP_FORMAT_WARC,
		TP_FORMAT_SHAR,
		TP_FORMAT_TARGZ,
		TP_FORMAT_TARBZ2
	};
	/// @brief 压缩格式(适用于自定义的压缩)
	enum tpCompressFilter
	{
		TP_FILTER_NONE,
		TP_FILTER_GZIP,
		TP_FILTER_BZIP2,
		TP_FILTER_XZ,
		TP_FILTER_LZMA,
		TP_FILTER_LZOP,
		TP_FILTER_LZ4,
		TP_FILTER_ZSTD
	};
	/// @brief 压缩类型，适用于简单常规的压缩，如果对压缩和打包笔记哦阿了解可以使用自定义的压缩和打包，否则悬着这种笔记哦阿简单的格式
	typedef enum {
		/// @brief tar格式打包
		TP_TAR,			
		/// @brief tzr.gz格式压缩
		TP_TAR_GZIP,
		/// @brief bzip格式压缩
		TP_TAR_BZIP2,
		/// @brief tag.xz格式
		TP_TAR_XZ,
		/// @brief tar.lzma格式
		TP_TAR_LZMA,
		/// @brief tar.lzop格式
		TP_TAR_LZOP,
		/// @brief tar.lz4格式
		TP_TAR_LZ4,
		/// @brief tar.zstd格式
		TP_TAR_ZSTD,
		/// @brief ar格式
		TP_AR,
		/// @brief ar.gz格式
		TP_AR_GZIP,
		/// @brief ar.gz格式
		TP_AR_BZIP2,
		/// @brief ar.xz格式
		TP_AR_XZ,
		/// @brief lzma格式
		TP_AR_LZMA,
        /// @brief lzop格式
        TP_AR_LZOP,
        /// @brief lz4格式
        TP_AR_LZ4,
        /// @brief zstd格式
        TP_AR_ZSTD,
		/// @brief xar格式
		TP_XAR,
		/// @brief gz格式
		TP_XAR_GZIP,
		/// @brief bz格式
		TP_XAR_BZIP2,
		/// @brief xz格式
		TP_XAR_XZ,
		/// @brief lzma格式
		TP_XAR_LZMA,
		/// @brief lz4格式
		TP_XAR_LZ4,
		/// @brief lzop格式
		TP_XAR_LZOP,
		/// @brief zstd格式
		TP_XAR_ZSTD,
		/// @brief zip格式
		TP_ZIP,
		/// @brief 7z格式
		TP_7ZIP
	}tpCompressType;

public:
	tpCompress();
	~tpCompress();

public:
	/// @brief 压缩文件
	/// @param path_s 源文件(末尾有/会遍历该目录下所有文件打包)
	/// @param path_t 目标文件(目标文件目录需要存在)
	/// @param type 压缩类型
	/// @return 压缩成功返回0，失败返回-1
	int addToCompress(const tpString &path_s, const tpString path_t, tpCompressType type=TP_ZIP);
	/// @brief 压缩文件
	/// @param path_s 源文件(末尾有/会遍历该目录下所有文件打包)
	/// @param path_t 目标文件(目标文件目录需要存在)目标文件(目标文件目录需要存在)
	/// @param pwd 压缩密码(当前默认没有密码)
	/// @param type 归档格式
	/// @param filter 压缩格式
	/// @return 压缩成功返回0，失败返回-1
	int addToCompress(const tpString &path_s, const tpString path_t, const tpString pwd , const tpCompressFormat &type, const tpCompressFilter &filter);
	/// @brief 解压
	/// @param path_s 源文件
	/// @param path_t 目标文件
	/// @return 解压成功返回0,失败返回-1
	int extractfromCompress(const tpString &path_s, const tpString &path_t);

private:
	ItpCompressData *data_;
};

#endif
