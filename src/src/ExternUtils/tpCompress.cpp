/*///------------------------------------------------------------------------------------------------------------------------//
		压缩解压
说 明 :
日 期 : 2024.11.08

/*/
//------------------------------------------------------------------------------------------------------------------------//

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include "tpCompress.h"
#include "SystemLib/archive.h"
#include "SystemLib/archive_entry.h"

#define PATH_INCREMENT 256 // 扩展预留缓存区

struct tpCompressData
{
	tpCompress::tpCompressFormat format;
	tpCompress::tpCompressFilter filter;
	tpCompressData()
	{
	}
};

// 添加到打包文件
static int add_file_to_archive(struct archive *a, const char *file_path, const char *entry_name)
{
	struct archive_entry *entry;
	struct stat st;
	int fd;
	char buffer[8192];
	ssize_t len;

	// 打开文件
	fd = open(file_path, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "错误：源文件%s不存在或打不开 \n", file_path);
		return -1;
	}

	// 获取文件状态
	if (fstat(fd, &st) != 0)
	{
		perror("fstat error");
		close(fd);
		return -1;
	}
	// printf("打包%s文件到条目%s\n", file_path,entry_name);
	//  创建新的归档条目
	entry = archive_entry_new();
	archive_entry_set_pathname(entry, entry_name);
	archive_entry_set_size(entry, st.st_size);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);

	// 写入归档条目头部
	archive_write_header(a, entry);

	// 写入文件数据
	while ((len = read(fd, buffer, sizeof(buffer))) > 0)
	{
		archive_write_data(a, buffer, len);
	}

	// 释放资源
	archive_entry_free(entry);
	close(fd);
	return 0;
}

// 遍历文件夹进行打包
// path_source:打包的原始目录
// path_target:生成的安装包
static int appm_ergodic_source_dopack(struct archive *a, const char *path_source, const char *path_target)
{
	DIR *dir;
	struct stat entry_info;
	struct dirent *dirt;
	size_t size_path = PATH_INCREMENT;

	if ((dir = opendir(path_source)) == NULL)
	{
		// printf("源文件不存在或打不开，文件：%s\n",path_source);
		fprintf(stderr, "错误：源文件%s不存在或打不开 \n", path_source);
		return -1;
	}
	//	if(access(path_target, F_OK) == 0)
	//		return -1;

	char *path_next_s = (char *)malloc(size_path); // 动态分配 fullpath
	if (!path_next_s)
	{
		perror("Failed to allocate memory for fullpath");
		closedir(dir);
		return -2;
	}
	while ((dirt = readdir(dir)) != NULL) // 循环查看目录下所有文件
	{
		// char path_next_s[MAX_LEN_PATH];     //下一个要处理的源路径
		// char path_next_t[MAX_LEN_PATH];     //源路径对应的目标路径
		size_t size_path_need = strlen(path_next_s) + strlen(dirt->d_name); //
		if (size_path_need > size_path)
		{
			size_path = size_path_need + PATH_INCREMENT;
			path_next_s = (char *)realloc(path_next_s, size_path);
			if (!path_next_s)
			{
				closedir(dir);
				return -1;
			}
		}
		snprintf(path_next_s, size_path, "%s/%s", path_source, dirt->d_name);

		if ((strcmp(dirt->d_name, ".") == 0) || (strcmp(dirt->d_name, "..") == 0)) // 如果是..就直接跳过
			continue;
		if (stat(path_next_s, &entry_info) != 0)
		{
			perror("stat");
			continue;
		}

		size_t size_path_t = strlen(path_target) + strlen(dirt->d_name) + 2;
		char *path_next_t = (char *)malloc(size_path_t);
		snprintf(path_next_t, size_path_t, "%s/%s", path_target, dirt->d_name);
		if (S_ISDIR(entry_info.st_mode))
		{
			// 如果是目录，递归进入子目录
			appm_ergodic_source_dopack(a, path_next_s, path_next_t);
		}
		else if (S_ISREG(entry_info.st_mode))
		{
			// 如果是文件，添加到压缩包中
			add_file_to_archive(a, (const char *)path_next_s, (const char *)path_next_t);
		}
		free(path_next_t);
	}
	free(path_next_s);
	return 0;
}

// 新建文件目录
static void create_directories(const char *path)
{
	char temp[256];
	char *p = NULL;
	size_t len;

	snprintf(temp, sizeof(temp), "%s", path);
	len = strlen(temp);
	if (temp[len - 1] == '/')
	{
		temp[len - 1] = 0;
	}

	for (p = temp + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = 0;
			mkdir(temp, 0755);
			*p = '/';
		}
	}
	//   mkdir(temp, S_IRWXU);
}

// 常规解包
// filename:原始包
// dest_dir:解压路径
static int extract_archive_file(const char *filename, const char *dest_dir)
{
	struct archive_entry *entry;
	size_t size_path = PATH_INCREMENT;
	int r;
	struct archive *a;
	struct archive *ext;
	int flags;
	char *full_path = (char *)malloc(size_path);
	if (full_path == NULL)
		return -1;

	// 设置解压选项（提取所有文件）
	flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

	// 创建archive对象用于读取归档文件
	a = archive_read_new();
	archive_read_support_format_all(a);
	archive_read_support_filter_all(a);

	if ((r = archive_read_open_filename(a, filename, 10240)))
	{
		fprintf(stderr, "Could not open file: %s\n", archive_error_string(a));
		return -1;
	}

	// 创建目标目录（如果不存在）
	if ((strlen(dest_dir) + 1) > size_path)
	{
		size_path = strlen(dest_dir) + PATH_INCREMENT;
		full_path = (char *)realloc(full_path, size_path);
		if (full_path == NULL)
			return -1;
	}
	snprintf(full_path, size_path, "%s", dest_dir);
	if (mkdir(full_path, 0755) != 0)
	{
		printf("creat error %s \n", full_path);
		return -1;
	}
	// 读取归档条目
	while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK)
	{
		const char *current_file;
		FILE *output_file;
		char *buffer;
		ssize_t len;

		// 获取文件名
		current_file = archive_entry_pathname(entry);

		size_t size_need = strlen(dest_dir) + strlen(current_file) + 2;
		if (size_need > size_path)
		{
			size_path = size_need + PATH_INCREMENT;
			full_path = (char *)realloc(full_path, size_path);
			if (full_path == NULL)
				return -1;
		}
		snprintf(full_path, size_path, "%s/%s", dest_dir, current_file);
		// 处理目录
		if (archive_entry_filetype(entry) == AE_IFDIR)
		{ // 如果当前条目是目录就创建文件夹否则创建文件
			mkdir(full_path, 0755);
		}
		else
		{
			// 创建目录
			create_directories((const char *)full_path);
			// 创建文件
			output_file = fopen(full_path, "wb");
			if (output_file == NULL)
			{
				printf("open %s error\n", full_path);
				perror("fopen");
				archive_read_free(a);
				return -1;
			}
			// 写入文件数据
			buffer = (char *)malloc(8192);
			while ((len = archive_read_data(a, buffer, 8192)) > 0)
			{
				//				printf("read %ld bytes\n", len);
				/*for(int i=0;i<len;i++)
				{
					printf("%02X ", (unsigned char)buffer[i]);
				}
				printf("\n");*/
				fwrite(buffer, 1, len, output_file);
			}

			free(buffer);
			fclose(output_file);
		}
	}

	// 处理错误
	if (r != ARCHIVE_EOF)
	{
		fprintf(stderr, "Error reading archive: %s\n", archive_error_string(a));
	}

	archive_read_close(a);
	archive_read_free(a);
	free(full_path);
	return 0;
}

tpCompress::tpCompress()
{
	data_ = new tpCompressData();
	tpCompressData *compressData = static_cast<tpCompressData *>(data_);
	
	compressData->format = TP_FORMAT_NONE;
	compressData->filter = TP_FILTER_NONE;
}

tpCompress::~tpCompress()
{
	tpCompressData *compressData = static_cast<tpCompressData *>(data_);
	if (compressData)
	{
		delete compressData;
		compressData = nullptr;
		data_ = nullptr;
	}
}


// //.zip
// int tpCompress::addToZipCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_ZIP;
// 	compressData->filter = TP_FILTER_NONE;
// 	return addToCompress(path_s, path_t, pwd);
// }

// //.rar
// int tpCompress::addToRarCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_RAR;
// 	compressData->filter = TP_FILTER_NONE;
// 	return addToCompress(path_s, path_t, pwd);
// }

// //.7z
// int tpCompress::addTo7zipCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_7ZIP;
// 	compressData->filter = TP_FILTER_NONE;
// 	return addToCompress(path_s, path_t, pwd);
// }

// //.ar（归档）
// int tpCompress::addToArCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_AR;
// 	compressData->filter = TP_FILTER_NONE;
// 	return addToCompress(path_s, path_t, pwd);
// }

// //.tar（归档）
// int tpCompress::addToTarCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_TAR;
// 	compressData->filter = TP_FILTER_NONE;
// 	return addToCompress(path_s, path_t, pwd);
// }

// // tar.gz
// int tpCompress::addToTargzCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_TARGZ;
// 	compressData->filter = TP_FILTER_GZIP;
// 	return addToCompress(path_s, path_t, pwd);
// }

// // tar.bz2
// int tpCompress::addToTarbz2Compress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_TAR;
// 	compressData->filter = TP_FILTER_BZIP2;
// 	return addToCompress(path_s, path_t, pwd);
// }

// //.iso
// int tpCompress::addToIsoCompress(tpString &path_s, tpString path_t, tpString pwd)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

// 	compressData->format = TP_FORMAT_ISO;
// 	compressData->filter = TP_FILTER_NONE;
// 	return -1;
// }
int tpCompress::addToCompress(const tpString &path_s, const tpString path_t,tpCompressType type)
{
	tpCompressFilter filter = TP_FILTER_NONE;
	tpCompressFormat format = TP_FORMAT_NONE;
	switch(type)
	{
		case TP_TAR:	
		case TP_TAR_GZIP:
			filter=TP_FILTER_XZ;
		case TP_TAR_BZIP2:
			filter=TP_FILTER_XZ;
		case TP_TAR_XZ:
			filter=TP_FILTER_XZ;
		case TP_TAR_LZMA:
			filter=TP_FILTER_LZMA;
		case TP_TAR_LZOP:
			filter=TP_FILTER_LZOP;
		case TP_TAR_LZ4:
			filter=TP_FILTER_LZ4;
		case TP_TAR_ZSTD:
			filter=TP_FILTER_ZSTD;
			format=TP_FORMAT_TAR;
			break;
		case TP_AR:
		case TP_AR_GZIP:
			filter=TP_FILTER_XZ;
		case TP_AR_BZIP2:
			filter=TP_FILTER_XZ;
		case TP_AR_XZ:
			filter=TP_FILTER_XZ;
		case TP_AR_LZMA:
			filter=TP_FILTER_LZMA;
        case TP_AR_LZOP:
			filter=TP_FILTER_LZOP;
        case TP_AR_LZ4:
			filter=TP_FILTER_LZ4;
        case TP_AR_ZSTD:
			filter=TP_FILTER_ZSTD;
			format=TP_FORMAT_AR;
			break;
		case TP_XAR:
		case TP_XAR_GZIP:
			filter=TP_FILTER_XZ;
		case TP_XAR_BZIP2:
			filter=TP_FILTER_XZ;
		case TP_XAR_XZ:
			filter=TP_FILTER_XZ;
		case TP_XAR_LZMA:
			filter=TP_FILTER_LZMA;
		case TP_XAR_LZ4:
			filter=TP_FILTER_LZ4;
		case TP_XAR_LZOP:
			filter=TP_FILTER_LZOP;
		case TP_XAR_ZSTD:
			format=TP_FORMAT_XAR;
			break;
		case TP_ZIP:
			format=TP_FORMAT_ZIP;
			break;
		case TP_7ZIP:
			format=TP_FORMAT_7ZIP;
			break;
		default:
			break;
	}
	return addToCompress(path_s, path_t, "", format, filter);
}

int tpCompress::addToCompress(const tpString &path_s, const tpString path_t, const tpString pwd, const tpCompressFormat &type, const tpCompressFilter &filter)
{
	tpCompressData *compressData = static_cast<tpCompressData *>(data_);

	compressData->format = type;
	compressData->filter = filter;

	if (compressData->format == TP_FORMAT_NONE)
	{
		fprintf(stderr, "至少需要设置归档格式，使用setFormat...\n");
		return -1;
	}

	struct archive *a = archive_write_new();

	switch (compressData->format)
	{
	case TP_FORMAT_ZIP:
		archive_write_set_format_zip(a);
		break;
	case TP_FORMAT_RAR: // 暂时不支持
		break;
	case TP_FORMAT_7ZIP: //
		archive_write_set_format_7zip(a);
		break;
	case TP_FORMAT_AR:
		archive_write_set_format_ar_bsd(a);
		break;
	case TP_FORMAT_XAR:
		break;
	case TP_FORMAT_TAR:
		archive_write_set_format_pax_restricted(a); // 使用 tar 格式
		break;
	default:
		archive_write_set_format_pax_restricted(a); // 不支持的格式默认按照tar归档
		break;
	}

	switch (compressData->filter)
	{
	case TP_FILTER_NONE:
		break;
	case TP_FILTER_LZMA:
		archive_write_add_filter_lzma(a);
		break;
	case TP_FILTER_XZ:
		archive_write_add_filter_xz(a);
		break;
	case TP_FILTER_BZIP2:
		archive_write_add_filter_bzip2(a);
		break;
	case TP_FILTER_GZIP:
		archive_write_add_filter_gzip(a);
		break;
	case TP_FILTER_LZOP:
		archive_write_add_filter_lzop(a);
		break;
	case TP_FILTER_LZ4:
		archive_write_add_filter_lz4(a);
		break;
	case TP_FILTER_ZSTD:
		archive_write_add_filter_zstd(a);
		break;
	default:
		break;
	}

	if (archive_write_open_filename(a, path_t.c_str()) != ARCHIVE_OK)
	{
		archive_write_free(a);
		return -1;
	}

	// 提取
	const char *pik = strrchr(path_s.c_str(), '/');
	if (pik == NULL)
		pik = path_s.c_str();
	if (strlen(pik) == 1)
		appm_ergodic_source_dopack(a, path_s.c_str(), ".");
	else
		appm_ergodic_source_dopack(a, path_s.c_str(), pik);

	archive_write_close(a);
	archive_write_free(a);
	return 0;
}



// path_s:压缩包
// path_t:解压位置
int tpCompress::extractfromCompress(const tpString &path_s, const tpString &path_t)
{
	return extract_archive_file(path_s.c_str(), path_t.c_str());
}

// void tpCompress::setCompressType(const tpCompressFormat &type)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);
// 	compressData->format = type;
// }

// void tpCompress::setCompressFilter(const tpCompressFilter & filter)
// {
// 	tpCompressData *compressData = static_cast<tpCompressData *>(data_);
// 	compressData->filter = filter;
// }

