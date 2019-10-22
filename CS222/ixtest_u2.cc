#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ix_test_util.h"

IndexManager *indexManager;

int testCase_u2(const string &indexFileName, const Attribute &attribute)
{
	// insert multiple entries (int) and print one node
    cout << endl << "***** In IX Test Unit Case 2 *****" << endl;

    RID rid;
    int x;
    unsigned numOfTuples = 100;
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
        rid.pageNum = i;
        rid.slotNum = i;
        x = i;

        rc = indexManager->insertEntry(ixfileHandle, attribute, &x, rid);
        assert(rc == success && "indexManager::insertEntry() should not fail.");
    }


    void *pageData = malloc(PAGE_SIZE);
    void *tempData = malloc(PAGE_SIZE);
    short int nodeType;
    PageInfo pageInfo;
    RecordOffset rOffset;
    RID tRid;
    int y;
    PageNum pageId = 0;
//    PageNum pageId = ixfileHandle.rootNodePage;
    ixfileHandle.readPage(pageId, pageData);
    memcpy(&pageInfo, (char*)pageData+PAGE_SIZE-sizeof(short int)-sizeof(PageInfo), sizeof(PageInfo));
    memcpy(&nodeType, (char*)pageData+PAGE_SIZE-sizeof(short int), sizeof(short int));
    memcpy(tempData, pageData, sizeof(PageNum));
    cout << endl;
    cout << "page id, node type: " << pageId << ", " << nodeType;
    memcpy(&pageId, pageData, sizeof(PageNum));
    cout << "; left most pageid: " << pageId << endl;
    for(int i = 0; i < pageInfo.numOfSlots; i++) {
    		memcpy(&rOffset, (char*)pageData+PAGE_SIZE-sizeof(short int)-sizeof(PageInfo)-(i+1)*sizeof(RecordOffset), sizeof(RecordOffset));
    		memcpy(tempData, (char*)pageData+rOffset.offset, rOffset.length);
    		memcpy(&y, tempData, sizeof(int));
    		memcpy(&tRid, (char*)tempData+sizeof(int), sizeof(RID));
    		if (nodeType == 0) {
    			memcpy(&pageId, (char*)tempData+sizeof(int)+sizeof(RID), sizeof(PageNum));
    			cout << "entry[" << y << ", (" << tRid.pageNum << "," << tRid.slotNum << "), " <<  pageId << "]" << endl;
    		} else {
    			cout << "entry[" << *((int*)tempData) << ", (" << ((RID*)tempData)->pageNum << "," << ((RID*)tempData)->slotNum << ")]" << endl;
    		}
    }
    free(pageData);
    free(tempData);

    indexManager->printBtree(ixfileHandle, attribute);

    rc = indexManager->closeFile(ixfileHandle);
    assert(rc == success && "indexManager::closeFile() should not fail.");

    free(entry);
    return 0;
}

int main(){
    indexManager = IndexManager::instance();
    const string indexFileName = "ix_unit_testFile2";
    Attribute attr;
    attr.length = 4;
    attr.name = "Int";
    attr.type = TypeInt;

    remove("ix_unit_testFile2");

    int rcmain = testCase_u2(indexFileName, attr);

    if (rcmain == success) {
        cout << "***** IX Test Unit Case 2 finished. The result will be examined. *****" << endl;
        return success;
    } else {
        cout << "***** [FAIL] IX Test Unit Case 2 failed. *****" << endl;
        return fail;
    }

}
