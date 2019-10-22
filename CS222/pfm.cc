#include "pfm.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
	if(_pf_manager){
		delete _pf_manager;
		_pf_manager = NULL;

	}
}


RC PagedFileManager::createFile(const string &fileName)
{
	FILE * fp;

		if ((fp = fopen(fileName.c_str(), "r")) != NULL) {
			fclose(fp);
			printf("the file has already exits");

			return -1;
		}

		fp = fopen(fileName.c_str(), "w+b");

		if (fp == NULL) {
			printf("file fail to create");
			return -1;
		}

		fclose(fp);
		return 0;
}


RC PagedFileManager::destroyFile(const string &fileName)
{
    if(remove(fileName.c_str()) != 0 ){
    	printf("fail to remove the file");
    	return -1;
    }else{
    	printf("file removed");

    	return 0;
    }
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
 
	return fileHandle.initFileHandle(fileName);

}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
	return fileHandle.closeFileHandle();
}


FileHandle::FileHandle()
{
    readPageCounter = 0;
    writePageCounter = 0;
    appendPageCounter = 0;
    filePointer = NULL;
    pageNumber = 0;


 }


FileHandle::~FileHandle()
{
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
	if(filePointer == NULL){
		return -1;
	}
	unsigned temp_page = getNumberOfPages();

	if(temp_page < pageNum ){
		return -1;
	}else{


	fseek(filePointer, pageNum * PAGE_SIZE, SEEK_SET);


	if(fread(data, PAGE_SIZE, 1, filePointer)!= 1){
		return-1;
	}
	readPageCounter++;
	}

    return 0;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{


	if(filePointer == NULL){
			return -1;
		}
		unsigned temp_page = getNumberOfPages();

		if(pageNum > temp_page-1 ){
			return -1;
		}else{

	if(fseek(filePointer, pageNum * PAGE_SIZE, SEEK_SET) != 0){
		return -1;
	}

	if(fwrite(data, PAGE_SIZE, 1, filePointer) !=1){
		return -1;
	}
		}
	writePageCounter++;

    return 0;

}


RC FileHandle::appendPage(const void *data)
{
	fseek(filePointer, 0, SEEK_END);
	fwrite(data, PAGE_SIZE, 1, filePointer);
	appendPageCounter++;
	fseek(filePointer, 0, SEEK_END);
	pageNumber = ftell(filePointer) / PAGE_SIZE;
	rewind(filePointer);

    return 0;
}


unsigned FileHandle::getNumberOfPages()
{
	fseek(filePointer, 0, SEEK_END);
	pageNumber = ftell(filePointer) / PAGE_SIZE;
	rewind(filePointer);

    return pageNumber;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
	readPageCount = readPageCounter;
	writePageCount = writePageCounter;
	appendPageCount = appendPageCounter;
	return 0;
}

RC FileHandle::initFileHandle(const string & fileName){
if ((filePointer = fopen(fileName.c_str(), "r+b")) == NULL) {
		cout<<"fail to open file"<<endl;
		return -1;
	}

fseek(filePointer, 0, SEEK_END);
	pageNumber = ftell(filePointer) / PAGE_SIZE;
	rewind(filePointer);

	return 0;

		
}
RC FileHandle:: closeFileHandle(){

	return fclose(filePointer);
}



