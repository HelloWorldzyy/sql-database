
#ifndef _rm_h_
#define _rm_h_

#include <string>
#include <vector>

#include "rbfm.h"
#include"ix.h"
using namespace std;

# define RM_EOF (-1)  // end of a scan operator

// RM_ScanIterator is an iteratr to go through tuples
class RM_ScanIterator {
public:
  RM_ScanIterator() {};
  ~RM_ScanIterator() {};

  // "data" follows the same format as RelationManager::insertTuple()
  RC getNextTuple(RID &rid, void *data) ;
  RC close();
   RBFM_ScanIterator Sacniterator;
};

// RM_IndexScanIterator is an iterator to go through index entries
class RM_IndexScanIterator {
public:
	RM_IndexScanIterator() {};  	// Constructor
	~RM_IndexScanIterator() {}; 	// Destructor

									// "key" follows the same format as in IndexManager::insertEntry()
	RC getNextEntry(RID &rid, void *key) {
		RC rc = ix_ScanIterator.getNextEntry(rid, key);
		return rc;
	};  	// Get next matching entry
	RC close() { ix_ScanIterator.close(); 
	return 0;
	};             			// Terminate index scan
	IX_ScanIterator ix_ScanIterator;
};


// Relation Manager
class RelationManager
{
public:
  RecordBasedFileManager *rbmf;
  IndexManager  *ixmf;
  static RelationManager* instance();

  RC createCatalog();

  RC deleteCatalog();

  RC createTable(const string &tableName, const vector<Attribute> &attrs);
  void getColumnsAtt(vector<Attribute> &attrs);
  void getTableAtt(vector<Attribute> &attrs);
  RC deleteTable(const string &tableName);
  RC creatDataColumn(FileHandle f,const int table_id, vector<Attribute> rid,void *data);
  RC getAttributes(const string &tableName, vector<Attribute> &attrs);
  RC creatDataTable(int table_id, const string& file_name,void* data,const string &tablename);
  void updateNextPoint(FileHandle & fh, int curid);
  void nextPoint(FileHandle &fh,int curid);
  RC insertTuple(const string &tableName, const void *data, RID &rid);

  RC deleteTuple(const string &tableName, const RID &rid);

  RC updateTuple(const string &tableName, const void *data, const RID &rid);

  RC readTuple(const string &tableName, const RID &rid, void *data);

  // Print a tuple that is passed to this utility method.
  // The format is the same as printRecord().
  RC printTuple(const vector<Attribute> &attrs, const void *data);

  RC readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data);

  // Scan returns an iterator to allow the caller to go through the results one by one.
  // Do not store entire results in the scan iterator.
  RC scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  // comparison type such as "<" and "="
      const void *value,                    // used in the comparison
      const vector<string> &attributeNames, // a list of projected attributes
      RM_ScanIterator &rm_ScanIterator);

  RC createIndex(const string &tableName, const string &attributeName);

  RC destroyIndex(const string &tableName, const string &attributeName);

  // indexScan returns an iterator to allow the caller to go through qualified entries in index
  RC indexScan(const string &tableName,
	  const string &attributeName,
	  const void *lowKey,
	  const void *highKey,
	  bool lowKeyInclusive,
	  bool highKeyInclusive,
	  RM_IndexScanIterator &rm_IndexScanIterator);
   RC  insertIndex(string tablename, const void * data ,const RID &rid);
   RC  deleteIndex(string tablename, const void * data, const RID &rid);
   bool hasIndexfile(string filename);
// Extra credit work (10 points)
public:
  RC addAttribute(const string &tableName, const Attribute &attr);

  RC dropAttribute(const string &tableName, const string &attributeName);


protected:
  RelationManager();
  ~RelationManager();

};

#endif
