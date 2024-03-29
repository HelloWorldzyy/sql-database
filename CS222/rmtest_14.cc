#include "rm_test_util.h"

RC TEST_RM_14(const string &tableName)
{
    // Functions Tested:
    // 1. System Catalog Implementation - Tables table
	cout << endl << "***** In RM Test Case 14 *****" << endl;

    // Get Catalog Attributes
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    // There should be at least three attributes: table-id, table-name, file-name
    if (attrs.size() < 3) {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed *****" << endl;
        return -1;
    } else if (attrs[0].name != "table-id" || attrs[1].name != "table-name" || attrs[2].name != "file-name") {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed *****" << endl;
        return -1;
    }

    RID rid;
    void *returnedData = malloc(1000);

    // Set up the iterator
    RM_ScanIterator rmsi;
    vector<string> projected_attrs;
    for (int i = 0; i < attrs.size(); i++){
      projected_attrs.push_back(attrs[i].name);
    }

    rc = rm->scan(tableName, "", NO_OP, NULL, projected_attrs, rmsi);
    assert(rc == success && "RelationManager::scan() should not fail.");

    int count = 0;
    while(rmsi.getNextTuple(rid, returnedData) != RM_EOF)
    {
    	// We will manually check the returned tuples to see whether your implementation is correct or not.
        rm->printTuple(attrs, returnedData);
        count++;
    }
    rmsi.close();

    // There should be at least two rows - one for Tables and one for Columns
    if (count < 2) {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed" << endl;
        free(returnedData);
        return -1;
    }

    // Deleting the catalog should fail.
    rc = rm->deleteTable(tableName);
    assert(rc != success && "RelationManager::deleteTable() on the system catalog table should fail.");

    free(returnedData);
    cout << "***** Test Case 14 Finished. The result will be examined. *****"<<endl;
    return 0;
}

RC TEST_RM_15(const string &tableName)
{
	// Functions Tested:
	// 1. System Catalog Implementation - Columns table
	cout << endl << "***** In RM Test Case 15 *****" << endl;

	// Get Catalog Attributes
	vector<Attribute> attrs;
	RC rc = rm->getAttributes(tableName, attrs);
	assert(rc == success && "RelationManager::getAttributes() should not fail.");

	// There should be at least five attributes: table-id, column-name, column-type, column-length, column-position
	if (attrs.size() < 5) {
		cout << "Your system catalog schema is not correct." << endl;
		cout << "***** [FAIL] Test Case 15 failed *****" << endl;
		return -1;
	}
	else if (attrs[0].name != "table-id" || attrs[1].name != "column-name" ||
		attrs[2].name != "column-type" || attrs[3].name != "column-length" || attrs[4].name != "column-position") {
		cout << "Your system catalog schema is not correct." << endl;
		cout << "***** [FAIL] Test Case 15 failed *****" << endl;
		return -1;
	}

	RID rid;
	void *returnedData = malloc(200);

	// Set up the iterator
	RM_ScanIterator rmsi;
	vector<string> projected_attrs;
	for (int i = 0; i < attrs.size(); i++) {
		projected_attrs.push_back(attrs[i].name);
	}

	rc = rm->scan(tableName, "", NO_OP, NULL, projected_attrs, rmsi);
	assert(rc == success && "RelationManager::scan() should not fail.");

	int count = 0;
	while (rmsi.getNextTuple(rid, returnedData) != RM_EOF)
	{
		// We will manually check the returned tuples to see whether your implementation is correct or not.
		rm->printTuple(attrs, returnedData);
		count++;
	}
	rmsi.close();

	// There should be at least ten rows - two for Tables and eight for Columns
	if (count < 10) {
		cout << "Your system catalog schema is not correct." << endl;
		cout << "***** [FAIL] Test Case 15 failed" << endl;
		free(returnedData);
		return -1;
	}

	// Deleting the catalog should fail.
	rc = rm->deleteTable(tableName);
	assert(rc != success && "RelationManager::deleteTable() on the system catalog table should fail.");

	free(returnedData);
	cout << "***** Test Case 15 Finished. The result will be examined. *****" << endl;
	return 0;
}

int main()
{
    // NOTE: your Tables table must be called "Tables"
    string catalog_table_name = "Tables";

    // Test Catalog Information
    RC rcmain = TEST_RM_14(catalog_table_name);

    return rcmain;
}
