#include "rbfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
    pf_manager = PagedFileManager::instance();

}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) {
	    return pf_manager->createFile(fileName);
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
    return this->pf_manager->destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
    return this->pf_manager->openFile(fileName , fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
    return this->pf_manager->closeFile(fileHandle);
}




RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid){


unsigned  int dataSize;

void * tempData = malloc(PAGE_SIZE);


unsigned int fieldNum = recordDescriptor.size();

unsigned int nullFieldLength = ceil((double)fieldNum / 8);

unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldLength);

memcpy(nullsIndicator ,data,nullFieldLength);

 unsigned  int * fieldPosition = new unsigned  int [fieldNum];

unsigned short int offsetForField = 0;

unsigned  int offset=0;


rid.pageNum = 0;
rid.slotNum = 0;
memcpy((byte*)tempData+offset ,&rid.pageNum,sizeof(rid.pageNum) );
offset+=sizeof(rid.pageNum);

memcpy((byte*)tempData+offset, &rid.slotNum,sizeof(rid.slotNum));
offset+=sizeof(rid.slotNum);





memcpy((byte*)tempData+offset+recordDescriptor.size() * sizeof(unsigned  int),data,nullFieldLength);
offsetForField=offset;


offset += recordDescriptor.size() * sizeof(unsigned  int)+nullFieldLength;

unsigned int dataOffset =nullFieldLength;
for(int i =0; i < fieldNum;i++){
     fieldPosition[i] = offset;
	memcpy((byte*)tempData+offsetForField,&fieldPosition[i],sizeof(unsigned  int));
	bool nullBit = nullsIndicator[i / 8]
					& (1 << (8 - 1 - i % 8));
			if (!nullBit) { // not null
				AttrType type = recordDescriptor[i].type;
				if (type == TypeInt) {
					memcpy((byte*)tempData+offset ,(byte*)data+dataOffset ,sizeof(int));

					offset += sizeof(int);
					dataOffset += sizeof(int);

				} else if (type == TypeReal) {

					memcpy((byte*)tempData+offset ,(byte*)data+dataOffset ,sizeof(float));

					dataOffset += sizeof(float);
					offset +=sizeof(float);
				} else {
					int length = 0;
					memcpy(&length, (byte*) data + dataOffset, sizeof(int));
					memcpy((byte*)tempData+offset,(byte*)data+dataOffset, sizeof(int)+length);
					offset +=sizeof(int)+length;
					dataOffset +=sizeof(int)+length;

				}
			} else {


			}
      offsetForField += sizeof(unsigned  int );
	}

dataSize = offset;

free(nullsIndicator);

unsigned int totalPageNum = fileHandle.getNumberOfPages();

unsigned  int availableIndex ;
bool needAppend = true;
unsigned int pageIndex = 1;
unsigned  int slotNumber ;





if(totalPageNum < 1){



	unsigned  int init = 0;
	rid.pageNum = 1;
	rid.slotNum = 2;
	 availableIndex = offset;

	 slotNumber = 2;

	memcpy((byte*) tempData + PAGE_SIZE - 2 * sizeof(unsigned  int),
				&availableIndex, sizeof(unsigned  int));




	memcpy((byte*) tempData + PAGE_SIZE - sizeof(unsigned  int),
				&slotNumber, sizeof(unsigned  int));

	memcpy((byte*) tempData + PAGE_SIZE - 4 * sizeof(unsigned  int),
					&init, sizeof(unsigned  int));

	memcpy((byte*) tempData + PAGE_SIZE - 3 * sizeof(unsigned  int),
					&offset, sizeof(unsigned  int));

	memcpy((byte*) tempData ,
						&rid.pageNum, sizeof(rid.pageNum));
	memcpy((byte*) tempData + sizeof(rid.pageNum),
						&rid.slotNum, sizeof(rid.slotNum));


	fileHandle.appendPage(tempData);


	needAppend = false;


}else{


	void * tempPageData = malloc(PAGE_SIZE);


		while(pageIndex <=  totalPageNum){

			fileHandle.readPage(pageIndex -1, tempPageData);



			memcpy(&availableIndex,
						(byte*) tempPageData + PAGE_SIZE - 2 * sizeof(unsigned  int),
						sizeof(unsigned  int));
			memcpy(&slotNumber,(byte*) tempPageData + PAGE_SIZE - sizeof(unsigned  int),
						sizeof(unsigned  int));

			if (availableIndex * sizeof(byte)
						+ (slotNumber + 1) * 2 * sizeof(unsigned  int)
						+ dataSize < PAGE_SIZE) {

					needAppend = false;
				} else {


				}


			if(!needAppend){

				 


				memcpy(&slotNumber,(byte*) tempPageData + PAGE_SIZE - sizeof(unsigned  int),
										sizeof(unsigned  int));

				for (int i = 1; i <=slotNumber; i++)
				{
					int pagelen;
					memcpy(&pagelen, (byte*)tempPageData + PAGE_SIZE - 2 * sizeof(unsigned int)*i+sizeof(unsigned int), sizeof(unsigned int));
					if (pagelen == 0)
					{
						slotNumber = i;
						memcpy((byte *)tempData, &pageIndex, sizeof(pageIndex));
						memcpy((byte*)tempData + sizeof(pageIndex), &slotNumber, sizeof(slotNumber));

						memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned  int), &availableIndex, sizeof(unsigned  int));
						memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned int) + sizeof(unsigned  int), &offset, sizeof(unsigned  int));
						memcpy((byte*)tempPageData + availableIndex, tempData, offset);
						
						availableIndex += offset;

						memcpy((byte*)tempPageData + PAGE_SIZE - 2 * sizeof(unsigned  int), &availableIndex, sizeof(unsigned  int));



						fileHandle.writePage(pageIndex - 1, tempPageData);

						free(tempPageData);


						rid.pageNum = pageIndex;
						rid.slotNum = slotNumber;
						return 0;


					}

				}
				slotNumber++;

				memcpy((byte*)tempPageData + PAGE_SIZE - sizeof(unsigned  int),&slotNumber,
										sizeof(unsigned  int));


				memcpy((byte *)tempData, &pageIndex , sizeof(pageIndex));
				memcpy((byte*)tempData+sizeof(pageIndex),&slotNumber,sizeof(slotNumber));

				memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned  int),&availableIndex , sizeof(unsigned  int));
				memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned int) + sizeof(unsigned  int),&offset , sizeof(unsigned  int));
				memcpy((byte*)tempPageData+availableIndex ,tempData,offset);

				
				availableIndex +=offset;

				memcpy((byte*) tempPageData + PAGE_SIZE - 2* sizeof(unsigned  int),&availableIndex,sizeof(unsigned  int));



				fileHandle.writePage(pageIndex-1,tempPageData);

			 


				  rid.pageNum = pageIndex;
				  rid.slotNum =slotNumber;

				break;




			}
			pageIndex++;

		}



		free(tempPageData);



}




if(needAppend){


	   unsigned  int init = 0;
		rid.pageNum = pageIndex ;
		rid.slotNum = 2;
		 availableIndex = dataSize;

		 slotNumber = 2;

		memcpy((byte*) tempData + PAGE_SIZE - 2 * sizeof(unsigned  int),
					&availableIndex, sizeof(unsigned  int));
		memcpy((byte*) tempData + PAGE_SIZE - sizeof(unsigned  int),
					&slotNumber, sizeof(unsigned  int));
		memcpy((byte*) tempData + PAGE_SIZE - 4 * sizeof(unsigned  int),
						&init, sizeof(unsigned  int));
		memcpy((byte*) tempData + PAGE_SIZE - 3*sizeof(unsigned  int),
						&offset, sizeof(unsigned  int));
		memcpy((byte*) tempData ,
							&rid.pageNum, sizeof(rid.pageNum));
		memcpy((byte*) tempData + sizeof(rid.pageNum),
							&rid.slotNum, sizeof(rid.slotNum));

		fileHandle.appendPage(tempData);

		

}


free(tempData);


    return 0;
}



RC RecordBasedFileManager::gettruepage(FileHandle &fileHandle, unsigned int &page, unsigned int &slot)
{
	RC rc = 0;
	void *data = malloc(PAGE_SIZE);
	while (true)
	{
		if (fileHandle.readPage(page-1, data) != 0)
		{
			rc = -1;
			break;
		}
	
 
		unsigned  int startpos;
		unsigned  int length;

		memcpy(&startpos, (byte*)data + PAGE_SIZE - slot * 2 * sizeof(unsigned  int), sizeof(unsigned  int));

		memcpy(&length, (byte*)data + PAGE_SIZE - slot * 2 * sizeof(unsigned  int) + sizeof(unsigned  int), sizeof(unsigned  int));


		// more tombstone
		if (length == PAGE_SIZE)
		{
			// move to the next
			memcpy(&page, (byte*)data+startpos, sizeof(unsigned int));
			memcpy(&slot, (byte*)data + startpos+ sizeof(unsigned int), sizeof(unsigned int));

		}
		else
		{
			break; // found
		}
	}

	free(data);
	return rc;
}





RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {


	void * temp_page = malloc(PAGE_SIZE);
	unsigned int page = rid.pageNum;
	unsigned int slot = rid.slotNum;
	 
	if (gettruepage(fileHandle, page, slot) != 0) return -1;
	fileHandle.readPage(page -1 , temp_page);

	unsigned  int availableIndex;
	unsigned  int length;
	 
	memcpy(&availableIndex, (byte*)temp_page + PAGE_SIZE - slot * 2 * sizeof(unsigned  int) ,sizeof(unsigned  int));

	memcpy(&length, (byte*)temp_page + PAGE_SIZE - slot * 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));
    
     if(length==0)
    	 return -1;
 


     memcpy(data, (byte*)temp_page+availableIndex+sizeof(rid.pageNum)+sizeof(rid.slotNum)+ recordDescriptor.size()*sizeof(unsigned  int),
		                     length-sizeof(rid.pageNum)-sizeof(rid.slotNum)-recordDescriptor.size()*sizeof(unsigned  int));


	  
	 

	free(temp_page);
	return 0;





}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {

	unsigned fieldNumber = recordDescriptor.size();

		unsigned nullLength = ceil((double) fieldNumber / 8);



		unsigned char * nullFieldsIndicator = new unsigned char[nullLength];
		memcpy(nullFieldsIndicator, data, nullLength);


		int * fieldPosition = new int[fieldNumber];

		unsigned positionOffset = nullLength;




		for (unsigned i = 0; i < fieldNumber; i++) {

			cout << recordDescriptor[i].name << ":\t";

			bool nullBit = nullFieldsIndicator[i / 8]
					& (1 << ( 7 - i % 8));

			if (!nullBit) {
				AttrType type = recordDescriptor[i].type;
				if (type == TypeInt) {
					int * dataValue = new int;
					memcpy(dataValue, (byte*) data + positionOffset, sizeof(int));

					cout << * dataValue << "\t";

					delete dataValue;
					positionOffset += sizeof(int);
				} else if (type == TypeReal) {
					float * dataValue = new float();
					memcpy(dataValue, (byte*) data + positionOffset, sizeof(float));

					cout << * dataValue << "\t";

					delete dataValue;
					positionOffset += sizeof(float);
				} else {
					int length = 0;
					memcpy(&length, (byte*) data + positionOffset, sizeof(int));
					positionOffset += sizeof(int);

					char * dataValue = new char[length + 1];
					memcpy(dataValue, (byte*) data + positionOffset,
							length * sizeof(char));
					dataValue[length] = '\0';

					cout << (char*) (dataValue) << "\t\t";

					delete[] dataValue;
					positionOffset += length;
				}
			} else {

				cout << "NULL" << "\t";


			}
			fieldPosition[i] = positionOffset;


		}

		cout << "" << endl;

		delete[] nullFieldsIndicator;
		return 0;
}

RC RecordBasedFileManager::deleteRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid)
{
void *page = malloc(PAGE_SIZE);
fileHandle.readPage(rid.pageNum -1 , page);

unsigned  int slotnum;
unsigned  int availableIndex;

memcpy(&slotnum, (byte*)page + PAGE_SIZE -  sizeof(unsigned  int) ,sizeof(unsigned  int));
memcpy(&availableIndex, (byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,sizeof(unsigned  int));



unsigned  int startpos;
unsigned  int length;

memcpy(&startpos, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) ,sizeof(unsigned  int));

memcpy(&length, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));

			if(length == PAGE_SIZE)
				{
					RID extra;
					memcpy(&extra.pageNum,(byte*)page+startpos,sizeof(unsigned int));
					memcpy(&extra.slotNum,(byte*)page+startpos+sizeof(unsigned int),sizeof(unsigned int));
					deleteRecord(fileHandle,recordDescriptor, extra);

					length = 2 * sizeof(int);
				}


		for(int i=2;i<=slotnum;i++)
		{
			int curslotoffset;
			memcpy(&curslotoffset, (byte*)page + PAGE_SIZE - i * 2 * sizeof(unsigned int), sizeof(unsigned int));

			if (curslotoffset>startpos)
			{
			
				int newpos = curslotoffset - length;
				memcpy((byte*)page + PAGE_SIZE - 2 * i * sizeof(unsigned int), &newpos, sizeof(int));
			}
		}
        int beginofdirectory=PAGE_SIZE-2*sizeof(unsigned int)*slotnum;



		int totallen=beginofdirectory-startpos-length;
		void *compact = malloc(totallen);
		memcpy(compact, (char *)page + startpos + length, totallen);
		memcpy((char *)page + startpos, compact, totallen);



		int zero = 0;
		 
		memcpy((byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned  int), &zero, sizeof(unsigned  int));

		// update freeSpace
		availableIndex -= length;
		memcpy((byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,&availableIndex, sizeof(unsigned  int));

		fileHandle.writePage(rid.pageNum-1, page);
		free(page);
		return 0;


}

int RecordBasedFileManager::DataToFormat(const vector<Attribute> &recordDescriptor,const void *data ,void* tempData){

	

	 

	unsigned int fieldNum = recordDescriptor.size();

	unsigned int nullFieldLength = ceil((double)fieldNum / 8);

	unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldLength);

	memcpy(nullsIndicator ,data,nullFieldLength);

	 unsigned  int * fieldPosition = new unsigned  int [fieldNum];

 

	unsigned  int offset=0;


	int pageNum = 0;
	int slotNum = 0;

	memcpy((byte*)tempData+offset ,&pageNum,sizeof(pageNum) );
	offset+=sizeof(pageNum);

	memcpy((byte*)tempData+offset, &slotNum,sizeof(slotNum));
	offset+=sizeof(slotNum);





	memcpy((byte*)tempData+offset+recordDescriptor.size() * sizeof(unsigned  int),data,nullFieldLength);
	  
	int fieldoffset = 2 * sizeof(int) ;

	offset += recordDescriptor.size() * sizeof(unsigned  int)+nullFieldLength;

	unsigned int dataOffset =nullFieldLength;
	for(int i =0; i < fieldNum;i++){
	    
		memcpy((byte*)tempData+ fieldoffset,&offset,sizeof(unsigned  int));
		fieldoffset += sizeof(unsigned int);
		bool nullBit = nullsIndicator[i / 8]
						& (1 << (8 - 1 - i % 8));
				if (!nullBit) { // not null
					AttrType type = recordDescriptor[i].type;
					if (type == TypeInt) {
						memcpy((byte*)tempData+offset ,(byte*)data+dataOffset ,sizeof(int));

						offset += sizeof(int);
						dataOffset += sizeof(int);

					} else if (type == TypeReal) {

						memcpy((byte*)tempData+offset ,(byte*)data+dataOffset ,sizeof(float));

						dataOffset += sizeof(float);
						offset +=sizeof(float);
					} else {
					 
						int length = 0;
						memcpy(&length, (byte*)data + dataOffset, sizeof(int));
						memcpy((byte*)tempData + offset, (byte*)data + dataOffset, sizeof(int) + length);
						offset += sizeof(int) + length;
						dataOffset += sizeof(int) + length;
					}
				}  
		}

 


	 
return offset;
}
RC RecordBasedFileManager::updateRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, const RID &rid)
{
	void *page = malloc(PAGE_SIZE);
    fileHandle.readPage(rid.pageNum -1 , page);

    unsigned  int slotnum;
	unsigned  int availableIndex;//pos of current data end

	memcpy(&slotnum, (byte*)page + PAGE_SIZE -  sizeof(unsigned  int) ,sizeof(unsigned  int));
	memcpy(&availableIndex, (byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,sizeof(unsigned  int));



	unsigned  int startpos;
	unsigned  int length;

	memcpy(&startpos, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) ,sizeof(unsigned  int));

	memcpy(&length, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));


	//ruguo jilu >1 page
	void *newrocord = malloc(PAGE_SIZE);
	int newlen = DataToFormat(recordDescriptor, data, newrocord);
    int afterlen=availableIndex-startpos-length;
    int afterrecord=startpos+length;


    int freespace=PAGE_SIZE-(slotnum ) * 2 * sizeof(unsigned  int)-availableIndex ;
    if(newlen<length+freespace)
      {//has enough space
                	// compact
                	///first we store after record information. then we update than we put back the orginal data

    	void *storetmp=malloc(afterlen);
        memcpy(storetmp,(char*)page+afterrecord,afterlen);
        memcpy((char*)page+startpos+newlen,storetmp,afterlen);
        free(storetmp);

        memcpy((char*)page+startpos,newrocord,newlen);
        memcpy((byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned  int),&newlen,sizeof(unsigned int));
        availableIndex=availableIndex-length+newlen;
        memcpy( (byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,&availableIndex,sizeof(unsigned  int));
		for (int i = 2; i <= slotnum; i++)
		{
			int curslotoffset;
			memcpy(&curslotoffset, (byte*)page + PAGE_SIZE - i * 2 * sizeof(unsigned int), sizeof(unsigned int));

			if (curslotoffset>startpos)
			{

				int newpos = curslotoffset - length + newlen;
				memcpy((byte*)page + PAGE_SIZE - 2 * i * sizeof(unsigned int), &newpos, sizeof(unsigned int));
			}
		}
      
        }
       else
         {
               RID newRid;

               buildinanewpage(fileHandle,recordDescriptor,newrocord,newRid,newlen);

               int beginofdirectory=PAGE_SIZE-2*sizeof(unsigned int)*slotnum;



              int totallen=beginofdirectory-startpos-length;
              void *compact = malloc(totallen);
              		memcpy(compact, (char *)page + startpos + length, totallen);
              		memcpy((char *)page + startpos+2*sizeof(int), compact,  totallen);
              		memcpy((char *)page + startpos, &newRid.pageNum, sizeof(unsigned int));
              		memcpy((char *)page + startpos+sizeof(int), &newRid.slotNum, sizeof(unsigned int));
             
               int pageSize = PAGE_SIZE;
               memcpy( (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned int), &pageSize, sizeof(unsigned int));
           }

				fileHandle.writePage(rid.pageNum-1, page); // update information of the page of the old record
				free(newrocord);
				free(page);
				return 0;

}
RC RecordBasedFileManager::buildinanewpage(FileHandle &fileHandle,const vector<Attribute> &recordDescriptor,const void* data,RID& rid,int offset)
{

	unsigned int totalPageNum = fileHandle.getNumberOfPages();
	unsigned  int availableIndex ;
	bool needAppend = true;
	unsigned int pageIndex = 1;
	unsigned  int slotNumber ;

	if(totalPageNum < 1){

		unsigned  int init = 0;
		rid.pageNum = 1;
		rid.slotNum = 2;
		 availableIndex = offset;

		 slotNumber = 2;

		memcpy((byte*) data + PAGE_SIZE - 2 * sizeof(unsigned  int),
					&availableIndex, sizeof(unsigned  int));




		memcpy((byte*) data + PAGE_SIZE - sizeof(unsigned  int),
					&slotNumber, sizeof(unsigned  int));

		memcpy((byte*) data + PAGE_SIZE - 4 * sizeof(unsigned  int),
						&init, sizeof(unsigned  int));

		memcpy((byte*) data + PAGE_SIZE - 3 * sizeof(unsigned  int),
						&offset, sizeof(unsigned  int));

		memcpy((byte*) data ,
							&rid.pageNum, sizeof(rid.pageNum));
		memcpy((byte*) data + sizeof(rid.pageNum),
							&rid.slotNum, sizeof(rid.slotNum));


		fileHandle.appendPage(data);


		needAppend = false;


	}else{


		void * tempPageData = malloc(PAGE_SIZE);


			while(pageIndex <=  totalPageNum){

				fileHandle.readPage(pageIndex -1, tempPageData);

				memcpy(&availableIndex,
							(byte*) tempPageData + PAGE_SIZE - 2 * sizeof(unsigned  int),
							sizeof(unsigned  int));
				memcpy(&slotNumber,(byte*) tempPageData + PAGE_SIZE - sizeof(unsigned  int),
							sizeof(unsigned  int));

				if (availableIndex * sizeof(byte)
							+ (slotNumber + 1) * 2 * sizeof(unsigned  int)
							+ offset < PAGE_SIZE) {

						needAppend = false;
					} else {


					}


				if(!needAppend){




					memcpy(&slotNumber,(byte*) tempPageData + PAGE_SIZE - sizeof(unsigned  int),
											sizeof(unsigned  int));

					slotNumber++;

					memcpy((byte*)tempPageData + PAGE_SIZE - sizeof(unsigned  int),&slotNumber,
											sizeof(unsigned  int));


					memcpy((byte *)data, &pageIndex , sizeof(pageIndex));
					memcpy((byte*)data+sizeof(pageIndex),&slotNumber,sizeof(slotNumber));

					memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned  int),&availableIndex , sizeof(unsigned  int));
					memcpy((byte*)tempPageData + PAGE_SIZE - slotNumber * 2 * sizeof(unsigned int) + sizeof(unsigned  int),&offset , sizeof(unsigned  int));
					memcpy((byte*)tempPageData+availableIndex ,data,offset);

					memcpy(&availableIndex,(byte*) tempPageData + PAGE_SIZE - 2 * sizeof(unsigned  int),
															sizeof(unsigned  int));


					availableIndex +=offset;

memcpy((byte*) tempPageData + PAGE_SIZE - 2* sizeof(unsigned  int),&availableIndex,
															sizeof(unsigned  int));



					fileHandle.writePage(pageIndex-1,tempPageData);

					free(tempPageData);


					  rid.pageNum = pageIndex;
					  rid.slotNum =slotNumber;

					break;




				}
				pageIndex++;

			}







	}




	if(needAppend){


		   unsigned  int init = 0;
			rid.pageNum = pageIndex ;
			rid.slotNum = 2;
			 availableIndex = offset;

			 slotNumber = 2;

			memcpy((byte*) data + PAGE_SIZE - 2 * sizeof(unsigned  int),
						&availableIndex, sizeof(unsigned  int));
			memcpy((byte*) data + PAGE_SIZE - sizeof(unsigned  int),
						&slotNumber, sizeof(unsigned  int));
			memcpy((byte*) data + PAGE_SIZE - 4 * sizeof(unsigned  int),
							&init, sizeof(unsigned  int));
			memcpy((byte*) data + PAGE_SIZE - 3*sizeof(unsigned  int),
							&offset, sizeof(unsigned  int));
			memcpy((byte*) data ,
								&rid.pageNum, sizeof(rid.pageNum));
			memcpy((byte*) data + sizeof(rid.pageNum),
								&rid.slotNum, sizeof(rid.slotNum));

			fileHandle.appendPage(data);



	}





	    return 0;


}


RC RecordBasedFileManager::readAttribute(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, const string attributeName, void *data)
{
	 
   
unsigned int slotpage=rid.pageNum;
unsigned int slotnum=rid.slotNum;

void *page=malloc(PAGE_SIZE);
if (gettruepage(fileHandle, slotpage, slotnum) != 0) return -1;
fileHandle.readPage(slotpage-1,page );

int dirpos = PAGE_SIZE - 2 * (slotnum ) * sizeof(unsigned  int);
int startpos ;
int length;

memcpy(&startpos,(byte*) page+dirpos,sizeof(unsigned int));
memcpy(&length,(byte*) page+dirpos+sizeof(unsigned int),sizeof(unsigned int));



unsigned int fieldNum = recordDescriptor.size();



unsigned int nullFieldLength = ceil((double)fieldNum / 8);

unsigned char *nullidorginal = (unsigned char *) malloc(nullFieldLength);

memcpy(nullidorginal ,(char*) page+ startpos+recordDescriptor.size() * sizeof(unsigned  int)+2*sizeof(unsigned int),nullFieldLength);

int offset = 0;
int dataOffset=1;
 memset(data,0,1);
 for(int i = 0; i < fieldNum; i++)
 	{

 		if(recordDescriptor[i].name == attributeName)
 		{
			memcpy(&offset,(char*) page + startpos + i * sizeof(int) + 2 * sizeof(int), sizeof(unsigned int));
			offset += startpos;
 			bool nullBit = nullidorginal[i / 8]& (1 << (8 - 1 - i % 8));
			if (nullBit)
			{
			((char*)data)[0] |= (1 << 7);
			
 							break;

 						}
 			AttrType type = recordDescriptor[i].type;
 			if (type == TypeInt) {

				       
 								memcpy((byte*)data+ dataOffset,(byte*)page+offset ,sizeof(int));

 								offset += sizeof(int);
 								dataOffset += sizeof(int);

 							} else if (type == TypeReal) {

 								memcpy((byte*)data+ dataOffset,(byte*)page+ offset,sizeof(float));

 								dataOffset += sizeof(float);
 								offset +=sizeof(float);
 							} else {
 								int length = 0;
 								memcpy(&length, (byte*) page + offset, sizeof(int));
 								memcpy((byte*)data+ dataOffset,(byte*)page+ offset, sizeof(int)+length);
 								offset +=sizeof(int)+length;
 								dataOffset +=sizeof(int)+length;

 							}
							break;

 		}
 	}



	free(nullidorginal);
	free(page);
	return 0;


}
RC RBFM_ScanIterator::getNextRecord(RID &rid, void *data){

	void *page = malloc(PAGE_SIZE);
	RC rc = 0;

	if(compOp != NO_OP)
	{
	        int tiaojian = 0;
			for(tiaojian = 0; tiaojian < recordDescriptor.size(); tiaojian++)
			{
				if(recordDescriptor[tiaojian].name == this->conditionAttribute)
					break;
			}

			if(tiaojian >= recordDescriptor.size())
			{
				free(page);
				return -1; 
			}

			bool findInAllPages = false;
			
			while(true)
			{
			
			 
				rc = fileHandle.readPage(scanid.pageNum-1, page);
				if(rc != 0)
				{
					free(page);
					return -1;
				}

			    unsigned  int slotnum;
				unsigned  int availableIndex;

				memcpy(&slotnum, (byte*)page + PAGE_SIZE -  sizeof(unsigned  int) ,sizeof(unsigned  int));
				memcpy(&availableIndex, (byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,sizeof(unsigned  int));


				bool findInThisPage=false;
				
				while(slotnum >scanid.slotNum)
				{
				

					// find record in a page, search record by record
				 int length;
				 int startpos;
				 memcpy(&startpos, (byte*)page + PAGE_SIZE - (1+scanid.slotNum) * 2 * sizeof(unsigned  int) ,sizeof(unsigned  int));

			      memcpy(&length, (byte*)page + PAGE_SIZE - (1 + scanid.slotNum) * 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));

					if(length == 0 || length == PAGE_SIZE)
					{
						scanid.slotNum++;
						continue;  
					}
	                    	 
				 
			 
					findInThisPage = CompV(page, startpos, tiaojian);
					 
					if (findInThisPage)
					{   
					
						break; // find it
					}
					scanid.slotNum++;
				}
				

				if(findInThisPage)
				{
					// do find a record in a page
					findInAllPages = true;
					break;
				}
				else
				{
					// prepare for next search(rid)
					scanid.pageNum++;
					scanid.slotNum = 1;
				}
			}
		
			
			if(findInAllPages)
			{
				 scanid.slotNum++;
				 getAttrs(data, page, scanid); 
			}
			else
			{
				rc = -1;
			}
	        rid = scanid; 
			free(page);
		   
	 
			return rc;
	}

	
			while(true)
			{
				if(fileHandle.getNumberOfPages()< scanid.pageNum)
				{
					free(page);
					return -1;
				}
				fileHandle.readPage(scanid.pageNum-1, page);
			    unsigned  int slotnum;
				unsigned  int availableIndex;

					memcpy(&slotnum, (byte*)page + PAGE_SIZE -  sizeof(unsigned  int) ,sizeof(unsigned  int));
					memcpy(&availableIndex, (byte*)page + PAGE_SIZE - 2* sizeof(unsigned  int) ,sizeof(unsigned  int));
				if(slotnum <= scanid.slotNum)
				{
					scanid.pageNum++;
					scanid.slotNum = 1;
					continue;
				}
				int length;
				memcpy(&length, (byte*)page + PAGE_SIZE - (scanid.slotNum +1)* 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));
			 
				if(length == 0 || length == PAGE_SIZE)
				{
					scanid.slotNum++;
					continue;
				}
				else
				{
					scanid.slotNum++;
					
					 getAttrs(data, page, scanid);
					break;
				}
			}

		
			rid = scanid;
	
			free(page);
			return rc;


}
RC  RecordBasedFileManager::scan(FileHandle &fileHandle,
    const vector<Attribute> &recordDescriptor,
    const string &conditionAttribute,
    const CompOp compOp,                  // comparision type such as "<" and "="
    const void *value,                    // used in the comparison
    const vector<string> &attributeNames, // a list of projected attributes
    RBFM_ScanIterator &rbfm_ScanIterator)
{
	rbfm_ScanIterator.fileHandle=fileHandle;
	rbfm_ScanIterator.recordDescriptor=recordDescriptor;
	rbfm_ScanIterator.conditionAttribute=conditionAttribute;
	rbfm_ScanIterator.compOp=compOp;
	rbfm_ScanIterator.attributeNames=attributeNames;
	rbfm_ScanIterator.value=value;

	return 0;


}

bool RBFM_ScanIterator::CompV(void * page, int startpos, int number)
{

	bool res = false;
	unsigned int fieldNum = recordDescriptor.size();
	unsigned int nullFieldLength = ceil((double)fieldNum / 8);
	unsigned char *nullidorginal = (unsigned char *)malloc(nullFieldLength);
	unsigned int recordplace;
	memcpy(nullidorginal, (char*)page + startpos + recordDescriptor.size() * sizeof(unsigned  int) + 2 * sizeof(unsigned int), nullFieldLength);
	memcpy(&recordplace, (char*)page + startpos + number * sizeof(int) + 2 * sizeof(int), sizeof(unsigned int));
	recordplace += startpos;
	bool nullBit = nullidorginal[number / 8] & (1 << (8 - 1 - number % 8));
	if (nullBit)
	{
		free(nullidorginal);
		return false;

	}
	if (recordDescriptor[number].type == TypeInt)
	{
		int condition = *((int *)value);
		int record;
		memcpy(&record, (char*)page + recordplace, sizeof(int));
		
		if ((compOp == EQ_OP && condition == record) ||
			(compOp == LT_OP && condition > record) ||
			(compOp == LE_OP && condition >= record) ||
			(compOp == GT_OP && condition < record) ||
			(compOp == GE_OP && condition <= record) ||
			(compOp == NE_OP && condition != record)
			)
		{
			res = true;
		}

	}
	else if(recordDescriptor[number].type == TypeReal)
	{
	
		int condition = *((float *)value);
		int record;
		memcpy(&record, (char*)page + recordplace, sizeof(float));

		if ((compOp == EQ_OP && condition == record) ||
			(compOp == LT_OP && condition > record) ||
			(compOp == LE_OP && condition >= record) ||
			(compOp == GT_OP && condition < record) ||
			(compOp == GE_OP && condition <= record) ||
			(compOp == NE_OP && condition != record)
			)
		{
			res = true;
		}

	}
	else
	{
		string condition;
		int varCharLength;
		memcpy(&varCharLength, (char*)value, sizeof(int));
	 
		for (int i = 0; i < varCharLength; i++)
		{
			condition += *((char *)value + 4 + i);
		}
		 
	    

		int varlen = 0;
		memcpy(&varlen, (char*)page + recordplace, sizeof(int));
		char* record = new char[varlen + 1];
         memcpy(record, (char*)page + recordplace + sizeof(int), varlen);
		 record[varlen] = '\0';

  


		if ((compOp == EQ_OP && condition == record) ||
			(compOp == LT_OP && condition > record) ||
			(compOp == LE_OP && condition >= record) ||
			(compOp == GT_OP && condition < record) ||
			(compOp == GE_OP && condition <= record) ||
			(compOp == NE_OP && condition != record))
		{
			
			res = true;
			
		}
		delete[] record;

	}
	free(nullidorginal);
	return res;
}

void RBFM_ScanIterator::getAttrs(void *data, void *page, RID rid)
{
	    unsigned  int startpos;
		unsigned  int length;
	    memcpy(&startpos, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int), sizeof(unsigned  int));
		memcpy(&length, (byte*)page + PAGE_SIZE - rid.slotNum * 2 * sizeof(unsigned  int) + sizeof(unsigned  int) ,sizeof(unsigned  int));

  //output
	int attlen = attributeNames.size();
	int nullFieldsIndicatorActualSize = ceil((double)attlen / 8.0);
	unsigned char *nullFieldsIndicator = (unsigned char *)malloc(nullFieldsIndicatorActualSize);
	memset(nullFieldsIndicator, 0, nullFieldsIndicatorActualSize);
	unsigned int offsetData = nullFieldsIndicatorActualSize;
//input

	unsigned int inputnullen = ceil((double) recordDescriptor.size() / 8);
	unsigned char *nullidorginal = (unsigned char *) malloc(inputnullen);
	memcpy(nullidorginal , (char*)page +startpos+2*sizeof(int)+sizeof(unsigned int)*recordDescriptor.size(),inputnullen);
	 
 
	for(int i = 0; i < attlen; i++)
	{
		for(int j = 0; j < recordDescriptor.size(); j++)
		{
			if(attributeNames[i] == recordDescriptor[j].name)
			{
				bool nullBit = nullidorginal[j/ 8]& (1 << (8 - 1 - j % 8));
				int offset;
				memcpy(&offset, (char*)page + startpos + j * sizeof(unsigned int)+2*sizeof(int), sizeof(unsigned int));
				offset += startpos;
				 
				
				if(recordDescriptor[j].type == TypeVarChar)
							{

								if(!nullBit)
								{
									int varlen=0;
									memcpy(&varlen, (char*)page +offset,sizeof(int));
								 	memcpy((char*)data + offsetData, &varlen, sizeof(int));
									offsetData += sizeof(int);
									memcpy((char*)data + offsetData, (char*)page + offset+sizeof(int),varlen);
									offsetData += varlen;
								}
								else
								{
									nullFieldsIndicator[i / 8] = nullFieldsIndicator[i / 8] | (1 << (7 - i % 8));
								}
							}
							else if (recordDescriptor[j].type == TypeInt)
							{
								if (!nullBit)
								{
								
							 
									// not null
								 	memcpy((char*)data + offsetData, (char*)page + offset,sizeof(int));
									 offsetData += sizeof(int);
								 
								}
								else
								{
									// null
									nullFieldsIndicator[i / 8] = nullFieldsIndicator[i / 8] | (1 << (7 - i % 8));
								 
								}

							}
							else
							{
								// int or real
								if(!nullBit)
								{
									// not null
								 	memcpy((char*)data + offsetData, (char*)page + offset, sizeof(float));
									offsetData += sizeof(float);
								}
								else
								{
									// null
									nullFieldsIndicator[i / 8] = nullFieldsIndicator[i / 8] | (1 << (7 - i % 8));
								}
							}



		    }


			}
		}
 
	memcpy((char*)data, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	
 
	free(nullFieldsIndicator);
	free(nullidorginal);
 


}

