#include "stdafx.h"
#include "FileUtils.h"
#include "CommenHeaders.h"

using std::vector;
using std::string;

//获得文件夹下所有文件名
void getFileNames(const string & path, vector<string> & fileNames) 
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName, exdName;
	if ((hFile = _findfirst(pathName.assign(path).
		append("\\*").c_str(), &fileInfo)) == -1) {
		throw "Directory doesn't exits!";
	}
	do {
		if (fileInfo.attrib&_A_SUBDIR) {
			string fname = string(fileInfo.name);
			if (fname != ".." && fname != ".") {
				getFileNames(path + "\\" + fname, fileNames);
			}
		}
		else {
			//std::cout << fileInfo.name << std::endl;
			fileNames.push_back(fileInfo.name);
		}

	} while(_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return;
}

// 创建指定文件名和内容的新文件
void creatFile(const string & fileName, const string & content) {
	std::ofstream fout(fileName, std::ios_base::out|std::ios_base::trunc);
	if (!fout.is_open()) {
		std::cout << "Failed to open files" << std::endl;
		exit(-1);
	}
	fout << content << std::endl;
	fout.close();
}