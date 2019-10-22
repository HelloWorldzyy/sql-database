#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ix_test_util.h"

IndexManager *indexManager;
RC printOneNode(const Attribute &attribute, PageNum pageId, IXFileHandle &ixfileHandle);

int testCase_u3(const string &indexFileName, const Attribute &attribute)
{
    // insert & delete multiple entries with varchar key
    cout << endl << "***** In IX Test Unit Case 3 *****" << endl;

    RID rid;
    char ch;
    int count;
    unsigned numOfTuples = 400;
    void *entry = malloc(PAGE_SIZE);
    IXFileHandle ixfileHandle;

    // create index files
    RC rc = indexManager->createFile(indexFileName);
    assert(rc == success && "indexManager::createFile() should not fail.");

    // open the index files
    rc = indexManager->openFile(indexFileName, ixfileHandle);
    assert(rc == success && "indexManager::openFile() should not fail.");

    // insert entry
    for(unsigned i = 0; i < numOfTuples; i++) {
        //generate varchar key with variable length
    		rid.pageNum = i;
        rid.slotNum = i;
        ch = 'a' + (i%26);
        count = i % 20 + 1;
		memcpy(entry, &count, sizeof(int));
        for(int j = 0; j < count; j++) {
        		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
        }
        //insert
        rc = indexManager->insertEntry(ixfileHandle, attribute, entry, rid);
        assert(rc == success && "indexManager::insertEntry() should not fail.");
    }
//    indexManager->printBtree(ixfileHandle, attribute);
    cout << "result of insert: ";
    printOneNode(attribute, 0, ixfileHandle);


    cout << endl;
    //delete 4 entries from pageid = 0
	//delete the first one
    rid.pageNum = 0;
	rid.slotNum = 0;
    ch = 'a';
    count = 1;
	memcpy(entry, &count, sizeof(int));
    for(int j = 0; j < count; j++) {
    		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
    }
    rc = indexManager->deleteEntry(ixfileHandle, attribute, entry, rid);
    assert(rc == success && "indexManager::deleteEntry() should not fail.");
    //delete the second one
	rid.pageNum = 260;
	rid.slotNum = 260;
    ch = 'a';
    count = 1;
	memcpy(entry, &count, sizeof(int));
    for(int j = 0; j < count; j++) {
    		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
    }
    rc = indexManager->deleteEntry(ixfileHandle, attribute, entry, rid);
    assert(rc == success && "indexManager::deleteEntry() should not fail.");
    //delete mid one
	rid.pageNum = 1;
	rid.slotNum = 1;
    ch = 'b';
    count = 2;
	memcpy(entry, &count, sizeof(int));
    for(int j = 0; j < count; j++) {
    		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
    }
    rc = indexManager->deleteEntry(ixfileHandle, attribute, entry, rid);
    assert(rc == success && "indexManager::deleteEntry() should not fail.");
    //delete the last one
	rid.pageNum = 292;
	rid.slotNum = 292;
    ch = 'g';
    count = 13;
	memcpy(entry, &count, sizeof(int));
    for(int j = 0; j < count; j++) {
    		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
    }
    rc = indexManager->deleteEntry(ixfileHandle, attribute, entry, rid);
    assert(rc == success && "indexManager::deleteEntry() should not fail.");
    //delete a non-existing one
    rid.pageNum = 0;
	rid.slotNum = 0;
    ch = 'a';
    count = 3;
	memcpy(entry, &count, sizeof(int));
    for(int j = 0; j < count; j++) {
    		memcpy((char*)entry+sizeof(int)+j, &ch, sizeof(char));
    }
    rc = indexManager->deleteEntry(ixfileHandle, attribute, entry, rid);
    assert(rc == -1 && "indexManager::deleteEntry() should not fail.");


    cout << "result of delete: ";
    printOneNode(attribute, 0, ixfileHandle);



    rc = indexManager->closeFile(ixfileHandle);
    assert(rc == success && "indexManager::closeFile() should not fail.");
    free(entry);
    return 0;
}

int main(){
    indexManager = IndexManager::instance();
    const string indexFileName = "ix_unit_testFile3";
    Attribute attr;
    attr.length = 30;
    attr.name = "VarChar";
    attr.type = TypeVarChar;

    remove("ix_unit_testFile3");

    int rcmain = testCase_u3(indexFileName, attr);

    if (rcmain == success) {
        cout << "***** IX Test Unit Case 3 finished. The result will be examined. *****" << endl;
        return success;
    } else {
        cout << "***** [FAIL] IX Test Unit Case 3 failed. *****" << endl;
        return fail;
    }

}

RC printOneNode(const Attribute &attribute, PageNum pageId, IXFileHandle &ixfileHandle) {
    //print one page
    void *pageData = malloc(PAGE_SIZE);
    void *tempData = malloc(PAGE_SIZE);
    void *str = malloc(PAGE_SIZE);
    short int nodeType;
    int l;
    PageInfo pageInfo;
    RecordOffset rOffset;
    RID tRid;
    ixfileHandle.readPage(pageId, pageData);
    memcpy(&pageInfo, (char*)pageData+PAGE_SIZE-sizeof(short int)-sizeof(PageInfo), sizeof(PageInfo));
    memcpy(&nodeType, (char*)pageData+PAGE_SIZE-sizeof(short int), sizeof(short int));
    cout << endl;
    cout << "page id = " << pageId << ", node type = "   << nodeType << ", pageInfo.numOfSlots = " << pageInfo.numOfSlots << endl ;
    memcpy(&pageId, pageData, sizeof(PageNum));
    cout << "left most pageid: " << pageId << endl;
    for(int i = 0; i < pageInfo.numOfSlots; i++) {
    		memcpy(&rOffset, (char*)pageData+PAGE_SIZE-sizeof(short int)-sizeof(PageInfo)-(i+1)*sizeof(RecordOffset), sizeof(RecordOffset));
    		memcpy(tempData, (char*)pageData+rOffset.offset, rOffset.length);
    		if (nodeType == 0) {
        		l = rOffset.length-sizeof(RID)-sizeof(PageNum);
    			memcpy(&tRid, (char*)tempData+rOffset.length-sizeof(RID)-sizeof(PageNum), sizeof(RID));
    			memcpy(str, tempData, l);
//        		memcpy(str, tempData, sizeof(char));
        		((char*)str)[l] = '\0';
    			memcpy(&pageId, (char*)tempData+rOffset.length-sizeof(PageNum), sizeof(PageNum));
    			cout << "entry[" << (char*)str << ", (" << tRid.pageNum << "," << tRid.slotNum << "), " <<  pageId << "]" << endl;
    		} else {
    			l = rOffset.length-sizeof(RID);
    			memcpy(&tRid, (char*)tempData+rOffset.length-sizeof(RID), sizeof(RID));
    			memcpy(str, tempData, l);
        		((char*)str)[l] = '\0';
//        		memcpy(str, tempData, sizeof(char));
    			cout << "entry[" << (char*)str << ", (" << tRid.pageNum << "," << tRid.pageNum << ")]" << endl;
    		}
    }
    free(tempData);
    free(pageData);
    free(str);
    return 0;
}
