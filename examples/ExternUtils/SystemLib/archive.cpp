

#include <iostream>
#include "TpCompress.h"

int main()
{
	TpCompress file;
	//打包文件夹下所有子文件：/home/485_transform/
	//打包文件夹：/home/485_transform
	TpString file_s("/home/485_transform/");		
	TpString file_t("/home/mytest.zip");
	
	file.addToCompress(file_s,TpString("/home/mytest.7z"),TpCompress::TP_7ZIP);
	file.addToCompress(file_s,TpString("/home/mytest.zip"),TpCompress::TP_ZIP);
	file.addToCompress(file_s,TpString("/home/mytest.tar.gz"),TpCompress::TP_TAR_GZIP);

	file.extractfromCompress(TpString("/home/usb3.23.zip"),TpString("/home/usb3.23_zip"));
	file.extractfromCompress(TpString("/home/usb3.23.rar"),TpString("/home/usb3.23_rar"));
	file.extractfromCompress(TpString("/home/usb3.23.7z"),TpString("/home/usb3.23_7z"));


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