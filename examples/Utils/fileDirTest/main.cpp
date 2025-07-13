#include "tpFile.h"
#include "tpSignalSlot.h"
#include <memory>
#include <cstring>
#include "tpDir.h"

int32_t main(int32_t argc, char *argv[])
{
	// tpFile testFile;

	// tpString testFilePath = "/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/demo/Utils/fileDirTest/testFile";
	// tpString testCopyFilePath = "/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/demo/Utils/fileDirTest/testFileCopy";

	// bool testFileExists = tpFile::exists(testFilePath);
	// std::cout << "testFileExists : " << testFileExists << std::endl;

	// bool copyRest = tpFile::copy(testFilePath, testCopyFilePath);
	// std::cout << "copyRest : " << copyRest << std::endl;

	// bool renameRest = tpFile::rename(testCopyFilePath, testCopyFilePath + "2");
	// std::cout << "renameRest : " << renameRest << std::endl;

	// bool removeRest = tpFile::remove(testCopyFilePath + "2");
	// std::cout << "removeRest : " << removeRest << std::endl;

	// testFile.setFileName(testFilePath);

	// testFile.open(tpFile::Append);
	// if (!testFile.isOpen())
	// {
	// 	std::cout << "OpenError !" << std::endl;
	// 	return 0;
	// }

	// uint64_t fileSize = testFile.size();
	// std::cout << "fileSize :" << fileSize << std::endl;

	// std::cout << "isReadable :" << testFile.isReadable() << std::endl;
	// std::cout << "isWritable :" << testFile.isWritable() << std::endl;

	// ****************************************************************************
	// char *readBuff = new char[5];
	// testFile.read(readBuff, 5);

	// tpString readBuffStr(readBuff);
	// std::cout << "readBuffStr :" << readBuffStr << std::endl;

	// tpString readBufString = testFile.read(3);
	// std::cout << "readBufString :" << readBufString << std::endl;

	// tpString readLineStr = testFile.readLine();
	// std::cout << "readLineStr :" << readLineStr << std::endl;

	// char *readLineBuff = new char[5];
	// testFile.readLine(readLineBuff, 5);

	// tpString readLineString(readLineBuff);
	// std::cout << "readLineString :" << readLineString << std::endl;

	// tpString readAllString = testFile.readAll();
	// std::cout << "readAllString :" << readAllString << std::endl;

	// ****************************************************************************
	// char *writeBuff = new char[5];
	// memcpy(writeBuff, "abcde", 5);
	// testFile.write(writeBuff, 3);

	// tpString writeString = "1122334455667788\n";
	// testFile.write(writeString);

	// testFile.write(writeBuff);

	// testFile.close();

	// +++++++++++++++++++++++++DirTest+++++++++++++++++++++++++++++
	// tpDir testDir;
	// testDir.setPath("/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/demo/Utils/fileDirTest/");

	// tpString pathStr = testDir.path();
	// std::cout << "pathStr :" << pathStr << std::endl;

	// tpString absolutePathStr = testDir.absolutePath();
	// std::cout << "absolutePathStr :" << absolutePathStr << std::endl;

	// tpString canonicalPathStr = testDir.canonicalPath();
	// std::cout << "canonicalPath :" << canonicalPathStr << std::endl;

	// tpString dirNameStr = testDir.dirName();
	// std::cout << "dirName :" << dirNameStr << std::endl;

	// tpString filePathStr = testDir.filePath("/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/DeskTop/bottomBar.cpp");
	// std::cout << "filePath :" << filePathStr << std::endl;

	// tpString absoluteFilePathStr = testDir.absoluteFilePath("/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/DeskTop/bottomBar.cpp");
	// std::cout << "absoluteFilePath :" << absoluteFilePathStr << std::endl;

	// tpString relativeFilePathStr = testDir.relativeFilePath("/home/hawk/Public/pix-singlegui/tinyPiX-V7.0.2/DeskTop/bottomBar.cpp");
	// std::cout << "relativeFilePath :" << relativeFilePathStr << std::endl;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// bool mkdirRes = testDir.mkdir("testSonDir");
	// std::cout << "mkdirRes :" << mkdirRes << std::endl;

	// bool mkpathRes = testDir.mkpath("/home/hawk/Public/testParentDir/testSonDir");
	// std::cout << "mkpathRes :" << mkpathRes << std::endl;

	// tpDir testDir2("/home/hawk/Public/testParentDir/");
	// bool dirExists = testDir2.exists();
	// std::cout << "dirExists :" << dirExists << std::endl;

	// bool removeRes = testDir2.remove("testSonDir");
	// std::cout << "removeRes :" << removeRes << std::endl;

	// bool removeRecursivelyRes = testDir2.removeRecursively();
	// std::cout << "removeRecursivelyRes :" << removeRecursivelyRes << std::endl;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// tpFileInfo fileInfo(applicationDirPath() + "/test.txt");
	// tpFileInfo fileInfo(applicationDirPath() );

    // std::cout << "File Path: " << fileInfo.filePath() << std::endl;
    // std::cout << "Absolute File Path: " << fileInfo.absoluteFilePath() << std::endl;
    // std::cout << "Canonical File Path: " << fileInfo.canonicalFilePath() << std::endl;
    // std::cout << "File Name: " << fileInfo.fileName() << std::endl;
    // std::cout << "Base Name: " << fileInfo.baseName() << std::endl;
    // std::cout << "Suffix: " << fileInfo.suffix() << std::endl;

	// std::cout << "--------------------------------------------------" << std::endl;

    // std::cout << "Directory Path: " << fileInfo.path() << std::endl;
    // std::cout << "Absolute Path: " << fileInfo.absolutePath() << std::endl;
    // std::cout << "Canonical Path: " << fileInfo.canonicalPath() << std::endl;

	// std::cout << "Is Readable: " << fileInfo.isReadable() << std::endl;
    // std::cout << "Is Writable: " << fileInfo.isWritable() << std::endl;
    // std::cout << "Is Executable: " << fileInfo.isExecutable() << std::endl;
    // std::cout << "Is Hidden: " << fileInfo.isHidden() << std::endl;
    // std::cout << "Is Native Path: " << fileInfo.isNativePath() << std::endl;
    // std::cout << "Is Relative Path: " << fileInfo.isRelative() << std::endl;

    // std::cout << "isFile: " << fileInfo.isFile() << std::endl;
    // std::cout << "isDir: " << fileInfo.isDir() << std::endl;
    // std::cout << "isSymLink: " << fileInfo.isSymLink() << std::endl;
    // std::cout << "isRoot: " << fileInfo.isRoot() << std::endl;

    // std::cout << "size: " << fileInfo.size() << std::endl;
    // // std::cout << "created: " << fileInfo.created() << std::endl;
    // std::cout << "lastModified: " << fileInfo.lastModified() << std::endl;

	tpDir showDir("/home/hawk/Public/tinyPiXOS/tinyPiXApp/fileManagement");

    // for (tpFileInfo fileInfo : showDir.entryInfoList(tpDir::NoDotAndDotDot, tpDir::Name))
    for (const tpFileInfo& fileInfo : showDir.entryInfoList())
    {

        std::cout << std::endl << std::endl;
        std::cout << "absoluteFilePath :" << fileInfo.absoluteFilePath() << std::endl;
        std::cout << "absoluteFilePath :" << fileInfo.canonicalFilePath() << std::endl;

        std::cout << "path             :" << fileInfo.path() << std::endl;
        std::cout << "absolutePath     :" << fileInfo.absolutePath() << std::endl;
        std::cout << "canonicalPath    :" << fileInfo.canonicalPath() << std::endl;

    }

	return 0;
}
