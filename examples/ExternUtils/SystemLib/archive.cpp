

#include <iostream>
#include "tpCompress.h"

int main()
{
	tpCompress file;
	//打包文件夹下所有子文件：/home/485_transform/
	//打包文件夹：/home/485_transform
	tpString file_s("/home/485_transform/");		
	tpString file_t("/home/mytest.zip");
	
	file.addToCompress(file_s,tpString("/home/mytest.7z"),tpCompress::TP_7ZIP);
	file.addToCompress(file_s,tpString("/home/mytest.zip"),tpCompress::TP_ZIP);
	file.addToCompress(file_s,tpString("/home/mytest.tar.gz"),tpCompress::TP_TAR_GZIP);

	file.extractfromCompress(tpString("/home/usb3.23.zip"),tpString("/home/usb3.23_zip"));
	file.extractfromCompress(tpString("/home/usb3.23.rar"),tpString("/home/usb3.23_rar"));
	file.extractfromCompress(tpString("/home/usb3.23.7z"),tpString("/home/usb3.23_7z"));


	/*file.addToZipCompress(file_s,file_t);
	file.addTo7zipCompress(file_s,std::string("/home/mytest.7z"));
	file.addToIsoCompress(file_s,std::string("/home/mytest.iso"));
	file.addToZipCompress(file_s,file_t);

	file.setCompressFormatTar();
	file.setCompressFilterXz();
	file.addToCompress(file_s,std::string("/home/mytest.tar.xz"));
	file.extractfromCompress(std::string("/home/usb3.23.zip"),std::string("/home/usb3.23_zip"));
	file.extractfromCompress(std::string("/home/usb3.23.rar"),std::string("/home/usb3.23_rar"));
	file.extractfromCompress(std::string("/home/usb3.23.7z"),std::string("/home/usb3.23_7z"));*/
	return 0;
}