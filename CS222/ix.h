#ifndef _ix_h_
#define _ix_h_

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "rbfm.h"
# define IX_EOF (-1)  // end of the index scan

class IX_ScanIterator;
class IXFileHandle;

class IndexManager {

public:
	static IndexManager* instance();
	bool hasopen;
	// Create an index file.
	RC createFile(const string &fileName);

	// Delete an index file.
	RC destroyFile(const string &fileName);

	// Open an index and return an ixfileHandle.
	RC openFile(const string &fileName, IXFileHandle &ixfileHandle);

	// Close an ixfileHandle for an index.
	RC closeFile(IXFileHandle &ixfileHandle);
	void insertintoleave(IXFileHandle &ixfileHandle, RID rid, void *entry, int entryLength, vector<RID> path);
	void splitupdate(IXFileHandle &ixfileHandle, RID rid, void* entry, int &entrylength, void* page, int &generatepage);
	void splitupdateinternal(IXFileHandle &ixfileHandle, RID rid, void* entry, int &entrylength, void* page, int & generatepage);
	void updateidx(IXFileHandle &ixfileHandle, RID rid, void * entry, int &entrylen, void *page);
	void updateinternal(IXFileHandle &ixfileHandle, RID rid, void * entry, int &entrylen, void *page, int& pagenum);
	RC searchidx(const Attribute &attribute, void * data, RID & rids, IXFileHandle & ixfileHandle, int & pageNum, void * Tentry, int & entrylen);
	bool Ridcompare(const RID & rid1, const RID & rid2);
	bool Ridcompareforleaf(const RID & rid1, const RID & rid2);
	bool isequal(const Attribute &attribute, void *data, void *original, int originallen, int datalen);
	bool isequalforleaf(const Attribute & attribute, void * data, void * original, int originallen, int datalen);
	bool totalequal(const Attribute &attribute, void *data, void *original, int originallen, int datalen);
	void processparent(IXFileHandle &ixfileHandle, bool &changed, void *entry, int entryLength, RID prid, int& generatepage);
	// Insert an entry into the given index that is indicated by the given ixfileHandle.
	RC insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);
	int createnty(AttrType type, const void* key, const RID& rid, void* entry);
	void preorder(IXFileHandle &ixfileHandle, int pagenum, AttrType type) const;
	int  printBtreeHelper(unsigned currentNum, unsigned level, IXFileHandle &ixfileHandle, const Attribute &attribute) const;
	// Delete an entry from the given index that is indicated by the given ixfileHandle.
	RC deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);
	int searchfordelte(const Attribute &attribute, void * data, RID & rids, IXFileHandle & ixfileHandle, int & pageNum, void * Tentry, int & entrylen);
	// Initialize and IX_ScanIterator to support a range search
	bool checkequal(const Attribute & attribute, int pageNum, void* Tentry, RID& rids, int entrylen, IXFileHandle ixfileHandle);
	RC scan(IXFileHandle &ixfileHandle,
		const Attribute &attribute,
		const void *lowKey,
		const void *highKey,
		bool lowKeyInclusive,
		bool highKeyInclusive,
		IX_ScanIterator &ix_ScanIterator);
	void  printBtree(IXFileHandle &ixfileHandle, const Attribute &attribute) const;
protected:
	IndexManager();
	~IndexManager();

private:
	static IndexManager *_index_manager;
};


class IX_ScanIterator {
public:
	IXFileHandle *ixfileHandle;
	Attribute attribute;
	const void      *lowKey;
	const void      *highKey;
	bool			lowKeyInclusive;
	bool        	highKeyInclusive;
	string lowstr;
	string highstr;
	bool firsttime;
	int curpage;
	RID scanid;
	RID Nextrid;
	// Constructor
	IX_ScanIterator();

	// Destructor
	~IX_ScanIterator();
	int findstartleaf(int pagenum);
	// Get next matching entry
	RC getNextEntry(RID &rid, void *key);
	void CompV(void* data, int startpos, int length, int&cmplow, int& cmpbig);
	// Terminate index scan
	RC close();
};



class IXFileHandle {
public:

	// variables to keep counter for each operation
	unsigned ixReadPageCounter;
	unsigned ixWritePageCounter;
	unsigned ixAppendPageCounter;
	int pagenum;
	int rootnum;
	FILE * file;
	string fileName;
	// Constructor
	IXFileHandle();
	RC readPage(PageNum pageNum, void *data);
	RC writePage(PageNum pageNum, const void *data);

	RC  appendPage(const void *data);
	// Destructor
	~IXFileHandle();

	// Put the current counter values of associated PF FileHandles into variables
	RC collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount);

};

#endif
