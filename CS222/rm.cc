
#include "rm.h"

RelationManager* RelationManager::instance()
{
    static RelationManager _rm;
    return &_rm;
}

RelationManager::RelationManager()
{
	rbmf = new RecordBasedFileManager();
}

RelationManager::~RelationManager()
{
}
RC RelationManager::creatDataTable(int table_id, const string& file_name,void* data,const string &tablename)
{

int offset=0;

char nullIndicator = 0;

memcpy((byte*)data + offset, &nullIndicator, 1);
offset += 1;

memcpy((byte*)data+offset,&table_id,sizeof(int));
offset+=sizeof(int);
int l1=tablename.size();
memcpy((byte*)data+offset,&l1,sizeof(int));
offset+=sizeof(int);
memcpy((byte*)data+offset,tablename.c_str(),l1);
offset+=l1;
int l2=file_name.size();
memcpy((byte*)data+offset,&l2,sizeof(int));
offset+=sizeof(int);
memcpy((byte*)data+offset, file_name.c_str(),l2);
offset+=l2;

return 0;

}
void RelationManager::updateNextPoint(FileHandle &fh, int curid)
{
	char *data = (char*)malloc(PAGE_SIZE);
	int NP_id = 0;
	string NP_name = "nextID";
	int NP_type = TypeInt;
	int NP_length = 4;
	int NP_pos = curid;

	int offset = 0;
	char ch = 0;
	memcpy(data + offset, &ch, 1);
	offset += 1;


	memcpy(data + offset, &NP_id, sizeof(int));
	offset += sizeof(int);


	int length1 = NP_name.size();
	memcpy(data + offset, &length1, sizeof(int));
	offset += sizeof(int);
	memcpy(data + offset, NP_name.c_str(), length1);
	offset += length1;


	memcpy(data + offset, &NP_type, sizeof(int));
	offset += sizeof(int);


	memcpy(data + offset, &NP_length, sizeof(int));
	offset += sizeof(int);


	memcpy(data + offset, &NP_pos, sizeof(int));
	offset += sizeof(int);

	vector<Attribute> RD;
	getAttributes("Columns", RD);
	RID tmprid;
	tmprid.pageNum = 1;
	tmprid.slotNum = 2;
	rbmf->updateRecord(fh, RD, data, tmprid);
	free(data);
}
void RelationManager::nextPoint(FileHandle &fh,int curid)
{
	char *data = (char*)malloc(PAGE_SIZE);
	int NP_id = 0;
	string NP_name = "nextID";
	int NP_type = TypeInt;
	int NP_length = 4;
	int NP_pos = curid;

	int offset = 0;
	char ch = 0;
	memcpy(data + offset, &ch, 1);
	offset += 1;


	memcpy(data+offset, &NP_id, sizeof(int));
	offset += sizeof(int);


	int length1 = NP_name.size();
	memcpy(data + offset, &length1, sizeof(int));
	offset += sizeof(int);
	memcpy(data + offset, NP_name.c_str(), length1);
	offset += length1;


	memcpy(data + offset, &NP_type, sizeof(int));
	offset += sizeof(int);


	memcpy(data + offset, &NP_length, sizeof(int));
	offset += sizeof(int);


	memcpy(data + offset, &NP_pos, sizeof(int));
	offset += sizeof(int);

	vector<Attribute> RD;
	getAttributes("Columns", RD);
	RID tmprid;
	rbmf->insertRecord(fh, RD, data, tmprid);
	free(data);
}
RC RelationManager::createCatalog()
{

   FileHandle r1,r2;
   rbmf->createFile("Tables");
   rbmf->createFile("Columns");
   rbmf->openFile("Tables",r1);
   rbmf->openFile("Columns",r2);
    
   vector<Attribute> rd1(3);
   vector<Attribute> rd2(5);
  

 

   rd1[0].length=4;
   rd1[0].name="table-id";
   rd1[0].type=TypeInt;

   rd1[1].length=50;
   rd1[1].name="table-name";
   rd1[1].type=TypeVarChar;

   rd1[2].length=50;
   rd1[2].name="file-name";
   rd1[2].type=TypeVarChar;


   rd2[0].length=4;
   rd2[0].name="table-id";
   rd2[0].type=TypeInt;

   rd2[1].length=50;
   rd2[1].name="column-name";
   rd2[1].type=TypeVarChar;

   rd2[2].length=4;
   rd2[2].name="column-type";
   rd2[2].type=TypeInt;

    rd2[3].length=4;
    rd2[3].name="column-length";
    rd2[3].type=TypeInt;



    rd2[4].length=4;
    rd2[4].name="column-position";
    rd2[4].type=TypeInt;
    void *data1=malloc(PAGE_SIZE);
    void *data2=malloc(PAGE_SIZE);
    creatDataTable(1,"Tables",data1,"Tables");
    RID rid;
    RC rc = rbmf->insertRecord(r1, rd1, data1, rid);
	memset(data1, 0, sizeof(char)*PAGE_SIZE);
    creatDataTable(2,"Columns",data1,"Columns");
    rc = rbmf->insertRecord(r1, rd2, data1, rid);
	
	//next pointer
	 
	nextPoint(r2,3);
    creatDataColumn(r2,1,rd1,data2);
	memset(data2, 0, sizeof(char)*PAGE_SIZE);
    creatDataColumn(r2,2,rd2,data2);

        free(data1);
    	free(data2);

    	rbmf->closeFile(r1);

       rbmf->closeFile(r2);

    	return 0;

}
RC RelationManager::creatDataColumn(FileHandle f,const int table_id, vector<Attribute> rid,void *data)
{
	RID tmpid;
	vector<Attribute> RD;
	getAttributes("Columns", RD);
	for(int i=0;i<rid.size();i++)
	{
	int offset=0;
	
	
	char ch = 0;
	memcpy((char*)data + offset, &ch, sizeof(char));
	offset += sizeof(char);
	
	
	memcpy((char*)data+offset,&table_id,sizeof(int));
	offset+=sizeof(int);
	
	string column_name=rid[i].name;
	int length1 = column_name.size();
	memcpy((char*)data + offset, &length1, sizeof(int));
	offset += sizeof(int);
	memcpy((char*)data + offset, column_name.c_str() ,length1);
	
	int coltype=rid[i].type;
	offset += length1;
	memcpy((char*)data + offset, &coltype, sizeof(int));
	offset += sizeof(int);
		
	int collen=rid[i].length;
	memcpy((char*)data + offset, &collen, sizeof(int));
	offset += sizeof(int);
	
	int colpos=i+1;
	memcpy((char*)data + offset, &colpos, sizeof(int));
	offset += sizeof(int);
			
				 
	rbmf->insertRecord(f, RD, data, tmpid);

	}
	return 0;
}
RC RelationManager::deleteCatalog()
{


     RC rc= rbmf->destroyFile("Tables");
	  errno = 0;
	  if (rc != 0)
		  return -1;
	  rc = rbmf->destroyFile("Columns");
	  errno = 0;
	  if (rc != 0)
		  return -1;


	return 0;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{


         rbmf->createFile(tableName);

	     FileHandle fh1,fh2;

	     rbmf->openFile("Tables", fh1);

	     rbmf->openFile("Columns", fh2);



	    int curid;
	    void* data = malloc(PAGE_SIZE);

		RID rid; rid.pageNum = 1; rid.slotNum = 2;
	    readAttribute("Columns", rid, "column-position", data);
	
	    memcpy(&curid, (char*)data+1 , sizeof(int));

	    // Insert tuples to Tables and Columns
	    void* data1 = malloc(PAGE_SIZE);
	    creatDataTable(curid, tableName, data1, tableName);
		RID rid1;
		vector<Attribute> RD;
		getAttributes("Tables", RD);
		rbmf->insertRecord(fh1, RD, data1, rid);
	 
	   void* data2 = malloc(PAGE_SIZE);
	   RID  rid2;
	   creatDataColumn(fh2, curid,attrs,data);
	   
	   updateNextPoint(fh2, ++curid);



	    free(data);
	    free(data1);
	    free(data2);
	    rbmf->closeFile(fh1);

	    rbmf->closeFile(fh2);
	    return 0;

}

RC RelationManager::deleteTable(const string &tableName)
{
	    RC rc;
		
		if (tableName=="Tables"||tableName=="Columns") return -1;
	   
		
		rbmf->destroyFile(tableName);


		vector<string> idnum;
		RID rid;

		
		RM_ScanIterator scanIterator;


 

		idnum.push_back("table-id");
		int nameLength = tableName.size();
		void *value = malloc(4+ nameLength);
		 
		memcpy((char *)value, &nameLength, 4);
		memcpy((char *)value + 4, tableName.c_str(), nameLength);
		scan("Tables", "table-name", EQ_OP, value, idnum, scanIterator);
	 


		void *data = malloc(PAGE_SIZE);
		scanIterator.getNextTuple(rid, data);

        int tableid;
        memcpy(&tableid,(char*)data+1,sizeof(int));

		scanIterator.close();
		free(data);


		 FileHandle fh;
		 rbmf->openFile("Tables", fh);

		    vector<Attribute> RD;
		    getAttributes("Tables", RD);
		   rbmf->deleteRecord(fh, RD, rid);

		   rbmf->closeFile(fh);


		// Collect rids from "Columns" using tid
		vector<RID> rids;
		vector<string> rd_col;
			rd_col.push_back("column-name");
		rc = scan("Columns", "table-id", EQ_OP, &tableid, rd_col, scanIterator);

		void *data1 = malloc(PAGE_SIZE);
		while (( scanIterator.getNextTuple(rid, data1)) != RM_EOF) {
			rids.push_back(rid);
			 

		}

		free(data1);
		scanIterator.close();
 
		vector<Attribute> RD1;
		rbmf->openFile("Columns", fh);
		getAttributes("Columns", RD1);
		for (int i = 0; i < rids.size(); i++) {

			    RC rc = rbmf->deleteRecord(fh, RD1, rids[i]);


		}
		free(value);
   rbmf->closeFile(fh);
		return 0;
}
void RelationManager::getTableAtt(vector<Attribute> &attrs)
{
	    Attribute at;
	    at.name = "table-id";
	    at.type = TypeInt;
	    at.length = (AttrLength)4;
	    attrs.push_back(at);

	    at.name = "table-name";
	    at.type = TypeVarChar;
	    at.length = (AttrLength)50;
		attrs.push_back(at);

		at.name = "file-name";
		at.type = TypeVarChar;
		at.length = (AttrLength)50;
		attrs.push_back(at);
}
void RelationManager::getColumnsAtt(vector<Attribute> &attrs)
{    
	
Attribute at;
at.length=4;
at.name="table-id";
at.type=TypeInt;
attrs.push_back(at);
at.length=50;
at.name="column-name";
at.type=TypeVarChar;
attrs.push_back(at);
at.length=4;
at.name="column-type";
at.type=TypeInt;
attrs.push_back(at);
at.length=4;
at.name="column-length";
at.type=TypeInt;
attrs.push_back(at);


at.length=4;
at.name="column-position";
at.type=TypeInt;
attrs.push_back(at);
}
RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
	if(tableName=="Tables"){
		getTableAtt(attrs);
          return 0;
	}
	else if(tableName=="Columns"){
		getColumnsAtt(attrs);
        return 0;
	}
	else{
		RM_ScanIterator rmscan;
		int length = tableName.size();
		
		vector<string> RD_id;
		RD_id.push_back("table-id");

	 
		int nameLength = tableName.size();
		void *value = malloc(4 + nameLength);

		memcpy((char *)value, &nameLength, 4);
		memcpy((char *)value + 4, tableName.c_str(), nameLength);
		
        scan("Tables", "table-name", EQ_OP, value, RD_id, rmscan);
        
		RID rid;

		void* data = malloc(PAGE_SIZE);

		rmscan.getNextTuple(rid, data);

		int tid;
		memcpy(&tid, (char*)data+1, sizeof(int));
		rmscan.close();
	

		// Read actual attrs from "Columns" using tid
		vector<string> rd_col;
		rd_col.push_back("column-name");
		rd_col.push_back("column-type");
		rd_col.push_back("column-length");

		scan("Columns", "table-id", EQ_OP, &tid, rd_col, rmscan);


		while ((rmscan.getNextTuple(rid, data)) != -1) {

			    Attribute attr;
				int len = 0,offset=1;

				memcpy(&len, (char*)data+offset , sizeof(int));
				offset += sizeof(int);
				char *str = new char[len+1];
				memcpy(str,(char*)data+offset,len);
				str[len] = '\0';
				string tempstring(str);
				delete[]str;
				attr.name = tempstring;
				offset+=len;
			 
				memcpy(&attr.type, (char*)data + offset, sizeof(int));
			 
				offset += sizeof(int);
				int attrlen = 0;
				memcpy(&attrlen, (char*)data + offset, sizeof(int));
				attr.length = attrlen;
				attrs.push_back(attr);



		}
		free(value);
		free(data);
		rmscan.close();

	    return 0;
	}


    return -1;
}

RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{

	if (tableName == "Tables" || tableName == "Columns") { //assume that the user do not create a table named Tables or Columns
		return -1;
	}
	FileHandle fh;
	RC rc=rbmf->openFile(tableName, fh);
	if (rc != 0)
		return -1;
	 vector<Attribute> RD;
		   getAttributes(tableName, RD);
		    rbmf->insertRecord(fh, RD, data, rid);


     rbmf->closeFile(fh);
	  insertIndex(tableName, data, rid);
	return 0;
}

RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
	if (tableName == "Tables" || tableName == "Columns") { //assume that the user do not create a table named Tables or Columns
		return -1;
	}
		FileHandle fh;
		 rbmf->openFile(tableName, fh);

		 void* original = malloc(PAGE_SIZE);
	 
	 
		  readTuple(tableName, rid, original);
		 
		 deleteIndex(tableName, original, rid);
		 free(original);

		 vector<Attribute> RD;
		   getAttributes(tableName, RD);
		  rbmf->deleteRecord(fh, RD, rid);

	      rbmf->closeFile(fh);

	    return 0;
}

RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
	if (tableName == "Tables" || tableName == "Columns") { //assume that the user do not create a table named Tables or Columns
		return -1;
	}


	void* original = malloc(PAGE_SIZE);
	 
 

	readTuple(tableName, rid, original);
	 
	deleteIndex(tableName, original, rid);
	 
	free(original);

	 
	insertIndex(tableName, data, rid);


FileHandle fh;
 rbmf->openFile(tableName, fh);
 vector<Attribute> RD;
 getAttributes(tableName, RD);
 RC rc = rbmf->updateRecord(fh, RD, data, rid);

 rbmf->closeFile(fh);

return 0;

}

RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
	    RC rc;
	    FileHandle fh;
	    rc = rbmf->openFile(tableName, fh);
	    if (rc != 0) return -1;


	    vector<Attribute> RD;
	    getAttributes(tableName, RD);
	    rc = rbmf->readRecord(fh, RD, rid, data);
	    if (rc != 0) {
	    	rc = rbmf->closeFile(fh);
	    	return -1;
	    }
		rc = rbmf->closeFile(fh);
		if (rc != 0) return -1;
		return 0;
}

RC RelationManager::printTuple(const vector<Attribute> &attrs, const void *data)
{

		RC rc= rbmf->printRecord(attrs, data);
		if (rc != 0) return -1;
		return 0;
}

RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{

	    FileHandle fh;
	    rbmf->openFile(tableName, fh);

	    vector<Attribute> RD;
	    getAttributes(tableName, RD);
		 
		rbmf->readAttribute(fh, RD, rid, attributeName, data);

		 rbmf->closeFile(fh);

		return 0;
}

RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  
      const void *value,                    
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{

    FileHandle fh;
    rbmf->openFile(tableName, fh);

     vector<Attribute> RD;
     getAttributes(tableName, RD);
     rbmf->scan(fh, RD, conditionAttribute, compOp, value, attributeNames, rm_ScanIterator.Sacniterator);

	return 0;
}
RC RelationManager::createIndex(const string & tableName, const string & attributeName)
{
	string indexname = tableName + attributeName + "_index";
	ixmf->createFile(indexname);



	int curid;
	void* data = malloc(PAGE_SIZE);

	RID rid; rid.pageNum = 1; rid.slotNum = 2;
	readAttribute("Columns", rid, "column-position", data);

	memcpy(&curid, (char*)data + 1, sizeof(int));

	FileHandle fh1, fh2;

	rbmf->openFile("Tables", fh1);

	rbmf->openFile("Columns", fh2);


 

	// Insert tuples to Tables and Columns
	void* data1 = malloc(PAGE_SIZE);
	creatDataTable(curid, indexname, data1, indexname);
	RID rid1;
	vector<Attribute> RD;
	getAttributes("Tables", RD);
	rbmf->insertRecord(fh1, RD, data1, rid);

	void* data2 = malloc(PAGE_SIZE);


	updateNextPoint(fh2, ++curid);



	free(data);
	free(data1);
	free(data2);
	rbmf->closeFile(fh1);
	rbmf->closeFile(fh2);

	vector<Attribute> attrs;
	getAttributes(tableName, attrs);

	 
	 
 
	int i;
	for (i = 0; i < attrs.size(); i++) {
		 
		if (attrs[i].name == attributeName)
			break;
	}
	IXFileHandle ixfh;
	RM_ScanIterator rm_scaniter;
	ixmf->openFile(indexname, ixfh);
	vector<string> attrNames;
	attrNames.push_back(attributeName);
	scan(tableName, attributeName, NO_OP, NULL, attrNames, rm_scaniter);
	void *data3 = malloc(PAGE_SIZE);
	void *data4 = malloc(PAGE_SIZE);
	while (rm_scaniter.getNextTuple(rid, data3) != RM_EOF) {
		memcpy(data4, (char*)data3 + 1, PAGE_SIZE - 1);
		ixmf->insertEntry(ixfh, attrs[i], data4, rid);
	 
	}
	 rm_scaniter.close();
	 

	 ixmf->closeFile(ixfh);
	 free(data3);
	 free(data4);
	 return 0;
}
RC RelationManager::destroyIndex(const string & tableName, const string & attributeName)
{
	string indexname = tableName + attributeName + "_index";
	ixmf->destroyFile(indexname);
	FileHandle fh;
	RM_ScanIterator rm_scaniter;
	rbmf->openFile("Tables", fh);
	void * name = malloc(PAGE_SIZE);
	int len = indexname.size();
	memcpy((char*)name, &len, sizeof(int));
	memcpy((char*)name + sizeof(int), &indexname, len);
	vector<string> RD;
	RD.push_back("table-id");
	scan("Tables", "table-name", EQ_OP, name,RD ,rm_scaniter);


	RID rid;
	
	void* data = malloc(PAGE_SIZE);

	RC rc = rm_scaniter.getNextTuple(rid, data);
	if (rc != 0) return -15;
	rm_scaniter.close();

	vector<Attribute> arri;
	getAttributes("Tables", arri);
	rbmf->deleteRecord(fh, arri, rid);
 
	 
	rc = rbmf->closeFile(fh);
 
	free(name);
	free(data);

	return rc;
}
RC RelationManager::indexScan(const string & tableName, const string & attributeName, const void * lowKey, const void * highKey, bool lowKeyInclusive, bool highKeyInclusive, RM_IndexScanIterator & rm_IndexScanIterator)
{
	string indexname = tableName + attributeName + "_index";
	vector<Attribute> attrs;
	 
	 getAttributes(tableName, attrs);
	int i;
	for (i = 0; i < attrs.size(); i++) {

		if (attrs[i].name == attributeName)
			break;
	}
	IXFileHandle ixfh;
	 ixmf->openFile(indexname, ixfh);
	 
	 RC rc=ixmf->scan(ixfh, attrs[i], lowKey, highKey, lowKeyInclusive, highKeyInclusive, rm_IndexScanIterator.ix_ScanIterator);
	 
	 
	 
	 return rc;

 
}
RC RelationManager::insertIndex(string tablename, const void * data, const RID & rid)
{
	vector<Attribute> attrs;
	getAttributes(tablename, attrs);
	void* key = malloc(PAGE_SIZE);
	unsigned int nullFieldLength = ceil((double)attrs.size() / 8);

	unsigned char *nullsIndicator = (unsigned char *)malloc(nullFieldLength);
	unsigned int dataOffset = nullFieldLength;
	memcpy(nullsIndicator, data, nullFieldLength);
	
	for (int i = 0; i < attrs.size(); i++)
	{
		string indexfilename = tablename + attrs[i].name + "_index";
		bool nullBit = nullsIndicator[i / 8]
			& (1 << (8 - 1 - i % 8));
		if (!nullBit) { // not null
			AttrType type = attrs[i].type;
			if (type == TypeInt) {
				memcpy((byte*)key , (byte*)data + dataOffset, sizeof(int));

				
				dataOffset += sizeof(int);

			}
			else if (type == TypeReal) {

				memcpy((byte*)key , (byte*)data + dataOffset, sizeof(float));

				dataOffset += sizeof(float);
				
			}
			else {
				int length = 0;
				memcpy(&length, (byte*)data + dataOffset, sizeof(int));
				memcpy((byte*)key , (byte*)data + dataOffset, sizeof(int) + length);
				
				dataOffset += sizeof(int) + length;

			}
		}
		if (hasIndexfile(indexfilename))
		{
			IXFileHandle fh;
			ixmf->openFile(indexfilename, fh);
		 
			ixmf->insertEntry(fh, attrs[i], key, rid);
		 
			ixmf->closeFile(fh);
			 


		}


	}
	free(nullsIndicator);
	free(key);
	return 0;
}
RC RelationManager::deleteIndex(string tablename, const void * data,const RID & rid)
{

	vector<Attribute> attrs;
	getAttributes(tablename, attrs);
	void* key = malloc(PAGE_SIZE);
	unsigned int nullFieldLength = ceil((double)attrs.size() / 8);

	unsigned char *nullsIndicator = (unsigned char *)malloc(nullFieldLength);
	unsigned int dataOffset = nullFieldLength;
	memcpy(nullsIndicator, data, nullFieldLength);

	for (int i = 0; i < attrs.size(); i++)
	{
		string indexfilename = tablename + attrs[i].name + "_index";
		bool nullBit = nullsIndicator[i / 8]
			& (1 << (8 - 1 - i % 8));
		if (!nullBit) { // not null
			AttrType type = attrs[i].type;
			if (type == TypeInt) {
				memcpy((byte*)key, (byte*)data + dataOffset, sizeof(int));


				dataOffset += sizeof(int);

			}
			else if (type == TypeReal) {

				memcpy((byte*)key, (byte*)data + dataOffset, sizeof(float));

				dataOffset += sizeof(float);

			}
			else {
				int length = 0;
				memcpy(&length, (byte*)data + dataOffset, sizeof(int));
				memcpy((byte*)key, (byte*)data + dataOffset, sizeof(int) + length);

				dataOffset += sizeof(int) + length;

			}
		}
		if (hasIndexfile(indexfilename))
		{
			IXFileHandle fh;
			ixmf->openFile(indexfilename, fh);

			ixmf->deleteEntry(fh, attrs[i], key, rid);

			ixmf->closeFile(fh);



		}


	}
	free(key);
	free(nullsIndicator);
	return 0;
}
bool RelationManager::hasIndexfile(string filename)
{
 

	FILE* fp = fopen(filename.c_str(), "rb+");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
	}
	return true;
}
RC RM_ScanIterator::getNextTuple(RID &rid, void *data) {
	RC rc = Sacniterator.getNextRecord(rid, data);
	return rc;
}


RC RM_ScanIterator::close() {
	RC rc = Sacniterator.close();
	return rc;
}
// Extra credit work
RC RelationManager::dropAttribute(const string &tableName, const string &attributeName)
{
    return -1;
}

// Extra credit work
RC RelationManager::addAttribute(const string &tableName, const Attribute &attr)
{
    return -1;
}


