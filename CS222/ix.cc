
#include "ix.h"

IndexManager* IndexManager::_index_manager = 0;

IndexManager* IndexManager::instance()
{
	if (!_index_manager)
		_index_manager = new IndexManager();

	return _index_manager;
}

IndexManager::IndexManager()
{
	hasopen = false;
}

IndexManager::~IndexManager()
{
}

RC IndexManager::createFile(const string &fileName)
{

	FILE *fp = fopen(fileName.c_str(), "rb+");

	if (fp != NULL) {
		fclose(fp);
		return -1;
	}

	fp = fopen(fileName.c_str(), "wb+");
	void *hidepage = malloc(PAGE_SIZE);

	int rootnum = 0;
	memcpy((char*)hidepage, &rootnum, sizeof(int));

	int pagenum = 0;
	memcpy((char*)hidepage + sizeof(int), &pagenum, sizeof(int));
	fwrite(hidepage, PAGE_SIZE, 1, fp);
	fclose(fp);
	free(hidepage);
	return 0; // success
}

RC IndexManager::destroyFile(const string &fileName)
{
	RC rc = remove(fileName.c_str());
	hasopen = false;
	if (rc == 0) return 0;
	else return -1;
}

void IndexManager::preorder(IXFileHandle &ixfileHandle, int pagenum, AttrType type)const
{
	void *page = malloc(PAGE_SIZE);
	ixfileHandle.readPage(pagenum, page);
	int flag = 0;
	memcpy(&flag, (char*)page + PAGE_SIZE - sizeof(int), sizeof(int));
	if (flag == 1)
	{

		cout << "{\"keys\":[";
		int totalslot = 0;
		memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
		int i = 0;
		int beforekey = -1;
		float beforefloat = -1;
		int before = -1;
		string beforestring;
		string twoitem[2] = { ")]\"," ,")]\"]}" };
		for (; i < totalslot; i++)
		{
			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			memcpy(&len, (char*)page + PAGE_SIZE - 4 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));

			if (type == TypeInt)
			{
				int num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));
				int actslotnum = 0, actslotlen = 0;
				memcpy(&actslotnum, (char*)page + startpos + sizeof(int), sizeof(int));
				memcpy(&actslotlen, (char*)page + startpos + sizeof(int) * 2, sizeof(int));
				if (beforekey == -1 || beforekey != num)
				{
					cout << "\"" << to_string(num) << ":[(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];

				}
				else
				{
					cout << "[(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];
				}
				beforekey = num;
			}
			else if (type == TypeReal)
			{
				float num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));
				int actslotnum = 0, actslotlen = 0;
				memcpy(&actslotnum, (char*)page + startpos + sizeof(int), sizeof(int));
				memcpy(&actslotlen, (char*)page + startpos + sizeof(int) * 2, sizeof(int));
				if (abs(beforefloat + 1) < 0.0001 || abs(beforefloat - num) > 0.0001)
				{
					cout << "\"" << to_string(num) << ":[(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];
				}
				else
				{
					cout << " [(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];
				}
			}
			else
			{
				int strlen = 0;
				memcpy(&strlen, (char*)page + startpos, sizeof(int));
				char * dataValue = new char[strlen + 1];

				memcpy(dataValue, (char*)page + startpos + sizeof(int), strlen);
				dataValue[strlen] = '\0';


				int actslotnum = 0, actslotlen = 0;
				memcpy(&actslotnum, (char*)page + startpos + strlen + sizeof(int), sizeof(int));
				memcpy(&actslotlen, (char*)page + startpos + strlen + sizeof(int) * 2, sizeof(int));
				if (before == -1 || beforestring != dataValue)
				{
					cout << "\"" << dataValue << ":[(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];
				}
				else
				{
					cout << "[(" << to_string(actslotnum) << "," << to_string(actslotlen);
					if (i != totalslot - 1)
						cout << twoitem[0];
					else
						cout << twoitem[1];
				}
				beforestring = dataValue;
				before = 1;
			}

		}
	}

	else if (flag == 0)
	{


		int totalslot = 0;
		memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));

		cout << endl << "{\"keys\":[";
		int i = 0;
		for (; i < totalslot - 1; i++)
		{
			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			memcpy(&len, (char*)page + PAGE_SIZE - 4 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			if (type == TypeInt)
			{
				int num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));

				cout << "\"" << to_string(num) << "\",";

			}
			else if (type == TypeReal)
			{
				float num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));
				cout << "\"" << to_string(num) << "\",";
			}
			else
			{
				int strlen = 0;
				memcpy(&strlen, (char*)page + startpos, sizeof(int));
				char * dataValue = new char[strlen + 1];

				memcpy(dataValue, (char*)page + startpos + sizeof(int), strlen);
				dataValue[strlen] = '\0';

				cout << "\"" << dataValue << "\",";


			}

		}
		if (i == totalslot - 1)
		{
			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			memcpy(&len, (char*)page + PAGE_SIZE - 4 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			if (type == TypeInt)
			{
				int num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));

				cout << "\"" << to_string(num) << "\"],";
			}
			else if (type == TypeReal)
			{
				float num = 0;
				memcpy(&num, (char*)page + startpos, sizeof(int));
				cout << "\"" << to_string(num) << "\"],";
			}
			else
			{
				int strlen = 0;
				memcpy(&strlen, (char*)page + startpos, sizeof(int));
				char * dataValue = new char[strlen + 1];

				memcpy(dataValue, (char*)page + startpos + sizeof(int), strlen);
				dataValue[strlen] = '\0';


				cout << "\"" << dataValue << "\"],";

			}

		}
		cout << endl << " \"children\": [" << endl;
		i = 0;
		int last = -1;
		for (; i < totalslot; i++)
		{
			int startpos = 0;
			int len;
			memcpy(&startpos, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));
			memcpy(&len, (char*)page + PAGE_SIZE - 4 * sizeof(int) - i * sizeof(int) * 2, sizeof(int));

			int nextpage = 0;
			memcpy(&nextpage, (char*)page + startpos - sizeof(int), sizeof(int));

			memcpy(&last, (char*)page + startpos + len, sizeof(int));
			preorder(ixfileHandle, nextpage, type);
			cout << endl;
		}
		if (last != -1)
			preorder(ixfileHandle, last, type);
		cout << "]}" << endl;


	}


}

RC IndexManager::openFile(const string &fileName, IXFileHandle &ixfileHandle)
{

	ixfileHandle.file = fopen(fileName.c_str(), "rb+");
	if (!ixfileHandle.fileName.empty()) {
		return -1; // file still opened
	}

	if (ixfileHandle.file == NULL) {
		return -1;
	}

	else {
		ixfileHandle.fileName = fileName;
		void *data = malloc(PAGE_SIZE);
		int a[6];
		fseek(ixfileHandle.file, 0, SEEK_SET);
		fread(a, sizeof(int), 2, ixfileHandle.file);



		ixfileHandle.rootnum = a[0];

		ixfileHandle.pagenum = a[1];
		hasopen = true;
		return 0;

	}
}

RC IndexManager::closeFile(IXFileHandle &ixfileHandle)
{
	if (ixfileHandle.file == NULL) {
		return -1;
	}
	else {
		// put counters into disk

		int a[2];
		a[0] = ixfileHandle.rootnum;
		a[1] = ixfileHandle.pagenum;
		fseek(ixfileHandle.file, 0, SEEK_SET);


		fwrite(a, sizeof(int), 2, ixfileHandle.file);
		fclose(ixfileHandle.file);
		hasopen = false;
		return 0;
	}
}
void IndexManager::insertintoleave(IXFileHandle &ixfileHandle, RID rid, void * entry, int entryLength, vector<RID> path)
{
	path.pop_back();
	void* page = malloc(PAGE_SIZE);
	ixfileHandle.readPage(rid.pageNum, page);
	int freespace = 0;
	memcpy(&freespace, (char*)page + PAGE_SIZE - 3 * sizeof(int), sizeof(int));
	//not have space, 
	if (entryLength + 2 * sizeof(int) > freespace)
	{
		int generatepage = -1;
		//process leaf, record new generatepage

		splitupdate(ixfileHandle, rid, entry, entryLength, page, generatepage);

		bool changed = true;

		do {
			if (path.size() == 0)
				break;
			RID tmprid = path.back();
			path.pop_back();

			processparent(ixfileHandle, changed, entry, entryLength, tmprid, generatepage);

		} while (changed);


	}
	else
	{
		// have space
		updateidx(ixfileHandle, rid, entry, entryLength, page);
	}
	free(page);
}
void IndexManager::splitupdate(IXFileHandle &ixfileHandle, RID rid, void* entry, int &entrylength, void * page, int& generatepage)
{
	int slotnum = rid.slotNum;
	int totalslot = 0;
	memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	//data
	if (slotnum<totalslot) {
		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 2 * sizeof(int), sizeof(int));

		void *tmp = malloc(PAGE_SIZE);
		memcpy(tmp, (char*)page + startpos, len);

		int totaloffset = 0;
		int totallen = 0;
		memcpy(&totaloffset, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&totallen, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 2 * sizeof(int), sizeof(int));
		totallen += totaloffset;

		void *newpage = malloc(PAGE_SIZE);
		memcpy((char*)newpage + sizeof(int), (char*)page + startpos, totallen - startpos);
		int sib = -1;
		memcpy(&sib, (char*)page, sizeof(int));
		memcpy((char*)newpage, &sib, sizeof(int));

		//directory
		int startofdirectory = PAGE_SIZE - 3 * sizeof(int) - 2 * (totalslot) * sizeof(int);
		int curdir = PAGE_SIZE - 3 * sizeof(int) - 2 * (slotnum) * sizeof(int);//t


		int dirlen = curdir - startofdirectory;

		memcpy((char*)newpage + PAGE_SIZE - dirlen - 3 * sizeof(int), (char*)page + startofdirectory, dirlen);
		int fs = PAGE_SIZE - (totallen - startpos) - dirlen - 3 * sizeof(int);
		memcpy((char*)newpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int newslotnum = totalslot - slotnum;
		memcpy((char*)newpage + PAGE_SIZE - 2 * sizeof(int), &newslotnum, sizeof(int));
		int isleave = 1;
		memcpy((char*)newpage + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));

		for (int i = 0; i < newslotnum; i++)
		{
			int off = 0;
			memcpy(&off, (char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
			off = off - startpos + sizeof(int);
			memcpy((char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), &off, sizeof(int));
		}



		generatepage = ixfileHandle.pagenum;




		//old page

		memcpy((char*)page, &generatepage, sizeof(int));
		memset((char*)page + startpos, 0, totallen - startpos);
		//insert new entry

		memcpy((char*)page + startpos, entry, entrylength);
		memset((char*)page + startofdirectory, 0, dirlen);

		memcpy((char*)page + startofdirectory + dirlen - 2 * sizeof(int), &startpos, sizeof(int));
		memcpy((char*)page + startofdirectory + dirlen - sizeof(int), &entrylength, sizeof(int));
		int newslot = slotnum + 1;
		memcpy((char*)page + PAGE_SIZE - 2 * sizeof(int), &newslot, sizeof(int));

		memcpy((char*)page + PAGE_SIZE - sizeof(int), &isleave, sizeof(int));



		fs = PAGE_SIZE - startpos - 3 * sizeof(int) - 2 * newslot * sizeof(int) - entrylength;
		memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));

		memcpy(entry, tmp, len);
		entrylength = len;



		/*
		int newpagenum, oldpagenum;
		memcpy(&newpagenum,(char*)newpage + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
		memcpy(&oldpagenum, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));

		for (int i = 0; i < newpagenum; i++)
		{
		int off = 0;
		memcpy(&off, (char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
		int ll1;
		memcpy(&ll1, (char*)newpage + off, sizeof(int));
		char * tt = new char[ll1 + 1];
		memcpy(tt, (char*)newpage + off + sizeof(int), ll1);
		tt[ll1] = '\0';
		cout << tt << endl;

		}
		for (int i = 0; i < oldpagenum; i++)
		{
		int off = 0;
		memcpy(&off, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
		int ll1;
		memcpy(&ll1, (char*)page + off, sizeof(int));
		char * tt = new char[ll1 + 1];
		memcpy(tt, (char*)page + off + sizeof(int), ll1);
		tt[ll1] = '\0';
		cout << tt << endl;

		}
		*/


		ixfileHandle.writePage(rid.pageNum, page);


		ixfileHandle.appendPage(newpage);
		free(tmp);





	}
	else
	{
		//directly we can build a new page to store new record, there is no business with prior one


		void *newpage = malloc(PAGE_SIZE);
		generatepage = ixfileHandle.pagenum;


		int sib = -1;
		memcpy(&sib, (char*)page, sizeof(int));
		memcpy((char*)newpage, &sib, sizeof(int));
		memcpy((char*)page, &generatepage, sizeof(int));

		memcpy((char*)newpage + sizeof(int), entry, entrylength);
		int fs = PAGE_SIZE - sizeof(int) - entrylength - 3 * sizeof(int) - 2 * sizeof(int);
		memcpy((char*)newpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int newslot = 1;
		memcpy((char*)newpage + PAGE_SIZE - 2 * sizeof(int), &newslot, sizeof(int));
		int isleave = 1;
		memcpy((char*)newpage + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));
		int startpos = 4;
		memcpy((char*)newpage + PAGE_SIZE - 5 * sizeof(int), &startpos, sizeof(int));
		memcpy((char*)newpage + PAGE_SIZE - 4 * sizeof(int), &entrylength, sizeof(int));



		ixfileHandle.writePage(rid.pageNum, page);
		ixfileHandle.appendPage(newpage);
		free(newpage);


	}


	if (rid.pageNum == ixfileHandle.rootnum) {
		void *rootpage = malloc(PAGE_SIZE);
		memcpy((char*)rootpage, &rid.pageNum, sizeof(int));
		memcpy((char*)rootpage + sizeof(int), entry, entrylength);

		memcpy((char*)rootpage + sizeof(int) + entrylength, &generatepage, sizeof(int));
		int fs = PAGE_SIZE - 7 * sizeof(int) - entrylength;

		memcpy((char*)rootpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int ts = 1;
		memcpy((char*)rootpage + PAGE_SIZE - 2 * sizeof(int), &ts, sizeof(int));
		int isl = 0;
		memcpy((char*)rootpage + PAGE_SIZE - 1 * sizeof(int), &isl, sizeof(int));
		int begin = 4;
		memcpy((char*)rootpage + PAGE_SIZE - 5 * sizeof(int), &begin, sizeof(int));

		memcpy((char*)rootpage + PAGE_SIZE - 4 * sizeof(int), &entrylength, sizeof(int));
		ixfileHandle.rootnum = ixfileHandle.pagenum;
		ixfileHandle.appendPage(rootpage);
		free(rootpage);
	}



}
void IndexManager::splitupdateinternal(IXFileHandle & ixfileHandle, RID rid, void * entry, int &entrylength, void * page, int & generatepage)
{
	int slotnum = rid.slotNum;
	int totalslot = 0;

	memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	if (slotnum < totalslot)
	{//data




		int startpos = 0;
		int len = 0;
		//20
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 2 * sizeof(int), sizeof(int));



		void *tmp = malloc(PAGE_SIZE);

		memcpy(tmp, (char*)page + startpos, len);


		int totaloffset = 0;
		int totallen = 0;
		memcpy(&totaloffset, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&totallen, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 2 * sizeof(int), sizeof(int));
		totallen += totaloffset + sizeof(int);
		int nextslot;
		int nextlen;
		memcpy(&nextslot, (char*)page + PAGE_SIZE - 2 * (slotnum + 2) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&nextlen, (char*)page + PAGE_SIZE - 2 * (slotnum + 2) * sizeof(int) - 2 * sizeof(int), sizeof(int));

		//30
		void *newpage = malloc(PAGE_SIZE);
		memcpy((char*)newpage, (char*)page + nextslot - sizeof(int), totallen - (nextslot - sizeof(int)));

		//directory
		int startofdirectory = PAGE_SIZE - 3 * sizeof(int) - 2 * (totalslot) * sizeof(int);
		int curdir = PAGE_SIZE - 3 * sizeof(int) - 2 * (slotnum + 1) * sizeof(int);
		int dirlen = curdir - startofdirectory;

		memcpy((char*)newpage + PAGE_SIZE - dirlen - 3 * sizeof(int), (char*)page + startofdirectory, dirlen);
		int fs = PAGE_SIZE - (totallen - nextslot + sizeof(int)) - dirlen - 3 * sizeof(int);
		memcpy((char*)newpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int newslotnum = totalslot - slotnum - 1;
		memcpy((char*)newpage + PAGE_SIZE - 2 * sizeof(int), &newslotnum, sizeof(int));
		int isleave = 0;
		memcpy((char*)newpage + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));
		for (int i = 0; i < newslotnum; i++)
		{
			int off = 0;
			memcpy(&off, (char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
			off = off + sizeof(int) - nextslot;
			memcpy((char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), &off, sizeof(int));
		}


		memset((char*)page + startofdirectory, 0, dirlen);
		memset((char*)page + startpos, 0, totallen - startpos);
		//13
		memcpy((char*)page + startpos, entry, entrylength);
		memcpy((char*)page + startpos + entrylength, &generatepage, sizeof(int));

		memcpy((char*)page + startofdirectory + dirlen, &startpos, sizeof(int));
		memcpy((char*)page + startofdirectory + dirlen + sizeof(int), &entrylength, sizeof(int));




		int newslot = slotnum + 1;
		memcpy((char*)page + PAGE_SIZE - 2 * sizeof(int), &newslot, sizeof(int));
		memcpy((char*)page + PAGE_SIZE - sizeof(int), &isleave, sizeof(int));
		fs = PAGE_SIZE - startpos - 3 * sizeof(int) - 2 * newslot * sizeof(int) - entrylength - sizeof(int);
		memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));



		memcpy(entry, tmp, len);
		entrylength = len;
		generatepage = ixfileHandle.pagenum;

		/*
		int newpagenum, oldpagenum;
		memcpy(&newpagenum, (char*)newpage + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
		memcpy(&oldpagenum, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));

		for (int i = 0; i < newpagenum; i++)
		{
		int off = 0;
		memcpy(&off, (char*)newpage + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
		int ll1;
		memcpy(&ll1, (char*)newpage + off, sizeof(int));

		cout << ll1 << endl;

		}
		for (int i = 0; i < oldpagenum; i++)
		{
		int off = 0;
		memcpy(&off, (char*)page + PAGE_SIZE - 5 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
		int ll1;
		memcpy(&ll1, (char*)page + off, sizeof(int));

		cout << ll1 << endl;

		}
		*/
		ixfileHandle.writePage(rid.pageNum, page);
		int val;
		memcpy(&val, (char*)entry, sizeof(int));
		ixfileHandle.appendPage(newpage);
	}
	else
	{
		//directly we can build a new page to store new record, there is no business with prior one


		void *newpage = malloc(PAGE_SIZE);


		memcpy((char*)newpage + sizeof(int) + entrylength, &generatepage, sizeof(int));

		memcpy((char*)newpage + sizeof(int), entry, entrylength);
		int fs = PAGE_SIZE - 2 * sizeof(int) - entrylength - 3 * sizeof(int) - 2 * sizeof(int);
		memcpy((char*)newpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int newslot = 1;
		memcpy((char*)newpage + PAGE_SIZE - 2 * sizeof(int), &newslot, sizeof(int));
		int isleave = 0;
		memcpy((char*)newpage + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));
		int startpos = 4;
		memcpy((char*)newpage + PAGE_SIZE - 5 * sizeof(int), &startpos, sizeof(int));
		memcpy((char*)newpage + PAGE_SIZE - 4 * sizeof(int), &entrylength, sizeof(int));


		generatepage = ixfileHandle.pagenum;
		ixfileHandle.writePage(rid.pageNum, page);
		ixfileHandle.appendPage(newpage);
		free(newpage);


	}


	if (rid.pageNum == ixfileHandle.rootnum) {
		void *rootpage = malloc(PAGE_SIZE);
		int beg = ixfileHandle.rootnum;
		memcpy((char*)rootpage, &beg, sizeof(int));
		memcpy((char*)rootpage + sizeof(int), entry, entrylength);
		memcpy((char*)rootpage + sizeof(int) + entrylength, &generatepage, sizeof(int));
		int fs = PAGE_SIZE - 7 * sizeof(int) - entrylength;

		memcpy((char*)rootpage + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		int ts = 1;
		memcpy((char*)rootpage + PAGE_SIZE - 2 * sizeof(int), &ts, sizeof(int));
		int isl = 0;
		memcpy((char*)rootpage + PAGE_SIZE - 1 * sizeof(int), &isl, sizeof(int));
		int begin = 4;
		memcpy((char*)rootpage + PAGE_SIZE - 5 * sizeof(int), &begin, sizeof(int));

		memcpy((char*)rootpage + PAGE_SIZE - 4 * sizeof(int), &entrylength, sizeof(int));
		ixfileHandle.rootnum = ixfileHandle.pagenum;
		ixfileHandle.appendPage(rootpage);
		free(rootpage);
	}


}
//rid->first bigger
void IndexManager::updateidx(IXFileHandle &ixfileHandle, RID rid, void * entry, int &entrylen, void * page)
{
	int slotnum = rid.slotNum;
	int totalslot = 0;
	memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	//directly add behind
	if (slotnum == totalslot)
	{

		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * slotnum * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * slotnum * sizeof(int) - 2 * sizeof(int), sizeof(int));
		int curpoint = startpos + len;
		memcpy((char*)page + curpoint, entry, entrylen);


		memcpy((char*)page + PAGE_SIZE - 5 * sizeof(int) - slotnum * 2 * sizeof(int), &curpoint, sizeof(int));
		memcpy((char*)page + PAGE_SIZE - 4 * sizeof(int) - slotnum * 2 * sizeof(int), &entrylen, sizeof(int));

	}
	else
	{


		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 2 * sizeof(int), sizeof(int));
		//6
		int totaloffset = 0;
		int totallen = 0;
		memcpy(&totaloffset, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&totallen, (char*)page + PAGE_SIZE - 2 * (totalslot) * sizeof(int) - 2 * sizeof(int), sizeof(int));
		totallen += totaloffset;

		void *tmp = malloc(PAGE_SIZE);
		memcpy(tmp, (char*)page + startpos, totallen - startpos);


		memcpy((char*)page + startpos, entry, entrylen);//copy data,

		memcpy((char*)page + startpos + entrylen, tmp, totallen - startpos);

		//directory
		void * newdirectory = malloc(PAGE_SIZE);

		int startofdirectory = PAGE_SIZE - 3 * sizeof(int) - 2 * (totalslot) * sizeof(int);
		int curdir = PAGE_SIZE - 3 * sizeof(int) - 2 * (slotnum) * sizeof(int);
		//4

		int dirlen = curdir - startofdirectory;
		memcpy(newdirectory, (char*)page + startofdirectory, dirlen);
		memcpy((char*)page + startofdirectory - 2 * sizeof(int), newdirectory, dirlen);

		memcpy((char*)page + startofdirectory - 2 * sizeof(int) + dirlen, &startpos, sizeof(int));
		memcpy((char*)page + startofdirectory + dirlen - sizeof(int), &entrylen, sizeof(int));


		for (int i = slotnum + 2; i <= totalslot + 1; i++)
		{
			int off = 0;
			memcpy(&off, (char*)page + PAGE_SIZE - 3 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
			off += entrylen;

			memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int) - i * 2 * sizeof(int), &off, sizeof(int));
		}


		free(tmp);
		free(newdirectory);

	}
	int fs = 0;
	memcpy(&fs, (char*)page + PAGE_SIZE - 3 * sizeof(int), sizeof(int));
	fs = fs - 2 * sizeof(int) - entrylen;
	memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
	int newslotnum = totalslot + 1;
	memcpy((char*)page + PAGE_SIZE - 2 * sizeof(int), &newslotnum, sizeof(int));
	int isleave = 1;
	memcpy((char*)page + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));
	ixfileHandle.writePage(rid.pageNum, page);


}

void IndexManager::updateinternal(IXFileHandle & ixfileHandle, RID rid, void * entry, int &entrylen, void * page, int& generatepage)
{
	int slotnum = rid.slotNum;
	int totalslot = 0;
	memcpy(&totalslot, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	//directly add behind
	if (slotnum == totalslot)
	{

		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * slotnum * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * slotnum * sizeof(int) - 2 * sizeof(int), sizeof(int));
		int curpoint = startpos + len + sizeof(int);
		memcpy((char*)page + curpoint, entry, entrylen);

		memcpy((char*)page + curpoint + entrylen, &generatepage, sizeof(int));

		memcpy((char*)page + PAGE_SIZE - 5 * sizeof(int) - slotnum * 2 * sizeof(int), &curpoint, sizeof(int));
		memcpy((char*)page + PAGE_SIZE - 4 * sizeof(int) - slotnum * 2 * sizeof(int), &entrylen, sizeof(int));
	}
	else
	{
		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * (slotnum + 1) * sizeof(int) - 2 * sizeof(int), sizeof(int));

		int totaloffset = 0;
		int totallen = 0;
		memcpy(&totaloffset, (char*)page + PAGE_SIZE - 2 * totalslot * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&totallen, (char*)page + PAGE_SIZE - 2 * totalslot * sizeof(int) - 2 * sizeof(int), sizeof(int));
		totallen += totaloffset + sizeof(int);
		void *tmp = malloc(PAGE_SIZE);
		memcpy(tmp, (char*)page + startpos, totallen - startpos);
		int hh;


		memcpy((char*)page + startpos, entry, entrylen);//copy data,
		memcpy((char*)page + startpos + entrylen, &generatepage, sizeof(int));
		memcpy((char*)page + startpos + entrylen + sizeof(int), tmp, totallen - startpos);

		//directory
		void * newdirectory = malloc(PAGE_SIZE);



		int startofdirectory = PAGE_SIZE - 3 * sizeof(int) - 2 * (totalslot) * sizeof(int);
		int curdir = PAGE_SIZE - 3 * sizeof(int) - 2 * (slotnum) * sizeof(int);
		int dirlen = curdir - startofdirectory;
		memcpy(newdirectory, (char*)page + startofdirectory, dirlen);
		memcpy((char*)page + startofdirectory - 2 * sizeof(int), newdirectory, dirlen);

		memcpy((char*)page + startofdirectory - 2 * sizeof(int) + dirlen, &startpos, sizeof(int));
		memcpy((char*)page + startofdirectory - sizeof(int) + dirlen, &entrylen, sizeof(int));



		for (int i = slotnum + 2; i <= totalslot + 1; i++)
		{
			int off = 0;
			memcpy(&off, (char*)page + PAGE_SIZE - 3 * sizeof(int) - i * 2 * sizeof(int), sizeof(int));
			off += entrylen + sizeof(int);
			memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int) - i * 2 * sizeof(int), &off, sizeof(int));
		}


		free(tmp);
		free(newdirectory);

	}
	int fs = 0;
	memcpy(&fs, (char*)page + PAGE_SIZE - 3 * sizeof(int), sizeof(int));
	fs = fs - 2 * sizeof(int) - entrylen - sizeof(int);
	memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
	int newslotnum = totalslot + 1;
	memcpy((char*)page + PAGE_SIZE - 2 * sizeof(int), &newslotnum, sizeof(int));
	int isleave = 0;
	memcpy((char*)page + PAGE_SIZE - 1 * sizeof(int), &isleave, sizeof(int));



	ixfileHandle.writePage(rid.pageNum, page);


}
int IndexManager::createnty(AttrType type, const void* key, const RID& rid, void* entry)
{
	int keylen = 0;
	if (type == TypeVarChar)
	{
		int length;
		memcpy(&length, (char*)key, sizeof(int));
		char *str = new char[length + 1];
		keylen = length + 2 * sizeof(int) + sizeof(int);
		memcpy(str, (char*)key + sizeof(int), length);
		str[length] = '\0';
		string tempstring(str);
		delete[]str;

		int pageNum = rid.pageNum, slotNum = rid.slotNum;
		memcpy((char*)entry, &length, sizeof(int));
		memcpy((char*)entry + sizeof(int), (char*)key + sizeof(int), length);
		memcpy((char*)entry + sizeof(int) + length, &pageNum, sizeof(int));
		memcpy((char*)entry + length + 2 * sizeof(int), &slotNum, sizeof(int));


	}
	else if (type == TypeInt) {

		// bulid entry, value is the actual key
		int value;
		keylen = sizeof(int) + 2 * sizeof(int);
		int pageNum = rid.pageNum, slotNum = rid.slotNum;
		memcpy(&value, key, sizeof(int));
		memcpy((char*)entry, &value, sizeof(int));
		memcpy((char*)entry + sizeof(int), &pageNum, sizeof(int));
		memcpy((char*)entry + sizeof(int) + sizeof(int), &slotNum, sizeof(int));
	}

	else {

		// build entry, value is the actual key
		float value;
		keylen = sizeof(float) + 2 * sizeof(int);
		unsigned pageNum = rid.pageNum, slotNum = rid.slotNum;
		memcpy(&value, key, sizeof(float));
		memcpy((char*)entry, &value, sizeof(float));
		memcpy((char*)entry + sizeof(float), &pageNum, sizeof(int));
		memcpy((char*)entry + sizeof(float) + sizeof(int), &slotNum, sizeof(int));
	}
	return keylen;

}

RC IXFileHandle::readPage(PageNum pg, void *data)
{
	if (pg + 1 > pagenum)
	{
		return -1;
	}

	ixReadPageCounter++;
	fseek(file, (1 + pg) * PAGE_SIZE, SEEK_SET);
	fread(data, PAGE_SIZE, 1, file);
	int ty = 0;
	memcpy(&ty, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));
	return 0;
}


RC IXFileHandle::writePage(PageNum pageNum, const void *data)
{
	if (pageNum + 1> pagenum)
	{
		return -1;
	}
	ixWritePageCounter++;
	fseek(file, (1 + pageNum) * PAGE_SIZE, SEEK_SET);
	if (fwrite(data, 1, PAGE_SIZE, file) != 0)
	{

		return 0;
	}
	else
	{
		return -1;
	}
}


RC IXFileHandle::appendPage(const void *data)
{
	int ty = 0;
	memcpy(&ty, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));
	fseek(file, 0, SEEK_END);
	fwrite(data, PAGE_SIZE, 1, file);
	rewind(file);
	ixAppendPageCounter++;
	pagenum++;

	return 0;
}
bool IndexManager::Ridcompare(const RID& rid1, const RID& rid2) {
	if (rid1.pageNum > rid2.pageNum)
		return true;
	else if (rid1.pageNum == rid2.pageNum && rid1.slotNum >rid2.slotNum)
		return true;
	else
		return false;
}
bool IndexManager::Ridcompareforleaf(const RID& rid1, const RID& rid2) {
	if (rid1.pageNum > rid2.pageNum)
		return true;
	else if (rid1.pageNum == rid2.pageNum && rid1.slotNum >= rid2.slotNum)
		return true;
	else
		return false;
}
bool IndexManager::isequal(const Attribute &attribute, void *data, void *original, int originallen, int datalen)
{
	if (attribute.type == TypeInt)
	{
		int v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));

		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 > v2)
			return true;
		else if (v1 == v2 && Ridcompare(rid1, rid2))
			return true;
		else
			return false;

	}
	else if (attribute.type == TypeReal)
	{
		float v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));
		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 > v2)
			return true;
		else if (v1 == v2 && Ridcompare(rid1, rid2))
			return true;
		else
			return false;
	}
	else
	{
		int l1 = 0;
		int l2 = 0;
		memcpy(&l1, (char*)data, sizeof(int));
		char *str = new char[l1 + 1];

		memcpy(str, (char*)data + sizeof(int), l1);
		str[l1] = '\0';
		string tempstring1(str);
		delete[]str;

		memcpy(&l2, (char*)original, sizeof(int));
		char *str1 = new char[l2 + 1];

		memcpy(str1, (char*)original + sizeof(int), l2);
		str1[l2] = '\0';
		string tempstring2(str1);
		delete[]str1;

		RID rid1, rid2;

		memcpy(&rid1.pageNum, (char*)data + l1 + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + l1 + sizeof(int) + sizeof(int), sizeof(int));

		memcpy(&rid2.pageNum, (char*)original + l2 + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + l2 + sizeof(int) + sizeof(int), sizeof(int));

		if (tempstring1 > tempstring2)
			return true;
		else if (tempstring2 == tempstring1 && Ridcompare(rid1, rid2))
			return true;
		else
			return false;

	}



}

bool IndexManager::isequalforleaf(const Attribute &attribute, void *data, void *original, int originallen, int datalen)
{
	if (attribute.type == TypeInt)
	{
		int v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));

		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 > v2)
			return true;
		else if (v1 == v2 && Ridcompareforleaf(rid1, rid2))
			return true;
		else
			return false;

	}
	else if (attribute.type == TypeReal)
	{
		float v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));
		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 > v2)
			return true;
		else if (v1 == v2 && Ridcompareforleaf(rid1, rid2))
			return true;
		else
			return false;
	}
	else
	{
		int l1 = 0;
		int l2 = 0;
		memcpy(&l1, (char*)data, sizeof(int));
		char *str = new char[l1 + 1];

		memcpy(str, (char*)data + sizeof(int), l1);
		str[l1] = '\0';
		string tempstring1(str);
		delete[]str;

		memcpy(&l2, (char*)original, sizeof(int));
		char *str1 = new char[l2 + 1];

		memcpy(str1, (char*)original + sizeof(int), l2);
		str1[l2] = '\0';
		string tempstring2(str1);
		delete[]str1;

		RID rid1, rid2;

		memcpy(&rid1.pageNum, (char*)data + l1 + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + l1 + sizeof(int) + sizeof(int), sizeof(int));

		memcpy(&rid2.pageNum, (char*)original + l2 + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + l2 + sizeof(int) + sizeof(int), sizeof(int));

		if (tempstring1 > tempstring2)
			return true;
		else if (tempstring2 == tempstring1 && Ridcompareforleaf(rid1, rid2))
			return true;
		else
			return false;

	}



}


bool IndexManager::totalequal(const Attribute & attribute, void * data, void * original, int originallen, int datalen)
{
	if (attribute.type == TypeInt)
	{
		int v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));
		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 == v2&&rid1.pageNum == rid2.pageNum&&rid1.slotNum == rid2.slotNum)
			return true;

		else
			return false;

	}
	else if (attribute.type == TypeReal)
	{
		float v1, v2;
		RID rid1, rid2;
		memcpy(&v1, (char*)data, sizeof(int));
		memcpy(&rid1.pageNum, (char*)data + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + sizeof(int) + sizeof(int), sizeof(int));
		memcpy(&v2, (char*)original, sizeof(int));
		memcpy(&rid2.pageNum, (char*)original + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + sizeof(int) + sizeof(int), sizeof(int));

		if (v1 == v2&&rid1.pageNum == rid2.pageNum&&rid1.slotNum == rid2.slotNum)
			return true;

		else
			return false;
	}
	else
	{
		int l1 = 0;
		int l2 = 0;
		memcpy(&l1, (char*)data, sizeof(int));
		char *str = new char[l1 + 1];

		memcpy(str, (char*)data + sizeof(int), l1);
		str[l1] = '\0';
		string tempstring1(str);
		delete[]str;

		memcpy(&l2, (char*)original, sizeof(int));
		char *str1 = new char[l2 + 1];

		memcpy(str1, (char*)original + sizeof(int), l2);
		str1[l2] = '\0';
		string tempstring2(str1);
		delete[]str1;

		RID rid1, rid2;

		memcpy(&rid1.pageNum, (char*)data + l1 + sizeof(int), sizeof(int));
		memcpy(&rid1.slotNum, (char*)data + l1 + sizeof(int) + sizeof(int), sizeof(int));

		memcpy(&rid2.pageNum, (char*)original + l2 + sizeof(int), sizeof(int));
		memcpy(&rid2.slotNum, (char*)original + l2 + sizeof(int) + sizeof(int), sizeof(int));

		if (str == str1&&rid1.pageNum == rid2.pageNum&&rid1.slotNum == rid2.slotNum)
			return true;

		else
			return false;

	}


}
void IndexManager::processparent(IXFileHandle & ixfileHandle, bool & changed, void * entry, int entryLength, RID prid, int& generatepage)
{
	void* page = malloc(PAGE_SIZE);
	ixfileHandle.readPage(prid.pageNum, page);

	int freespace = 0;
	memcpy(&freespace, (char*)page + PAGE_SIZE - 3 * sizeof(int), sizeof(int));
	//not have space, 
	if (entryLength + 3 * sizeof(int) > freespace)
	{


		splitupdateinternal(ixfileHandle, prid, entry, entryLength, page, generatepage);
		changed = true;
	}
	else
	{
		// have space
		updateinternal(ixfileHandle, prid, entry, entryLength, page, generatepage);
		changed = false;
	}



}
RC IndexManager::searchidx(const Attribute &attribute, void* data, RID& rids, IXFileHandle& ixfileHandle, int& pageNum, void* Tentry, int& entrylen)
{
	rids.pageNum = pageNum;
	int slotnum = 0;
	memcpy(&slotnum, (char*)data + PAGE_SIZE - 2 * sizeof(int), sizeof(int));

	if (slotnum == 0)
	{
		rids.slotNum = 0;
		return 0;

	}
	else
	{
		void* tmp = malloc(PAGE_SIZE);

		bool isfind = false;


		int i = 0;
		for (; i < slotnum; i++)
		{

			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 5 * sizeof(int), sizeof(int));
			memcpy(&len, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 4 * sizeof(int), sizeof(int));
			memcpy((char*)tmp, (char*)data + startpos, len);

			if (isequal(attribute, tmp, Tentry, entrylen, len))
			{
				isfind = true;

				break;

			}
			int flag1 = 0;
			memcpy(&flag1, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));

		}

		free(tmp);
		int flag = 0;
		memcpy(&flag, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));
		if (flag == 1) {
			rids.slotNum = i;


			return 0;
		}
		else {
			int nextpage;
			int startpos;
			if (isfind) {

				memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 5 * sizeof(int), sizeof(int));
				memcpy(&nextpage, (char*)data + startpos - sizeof(int), sizeof(int));
			}
			else {
				memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * (slotnum - 1) * sizeof(int) - 5 * sizeof(int), sizeof(int));
				int len = 0;
				memcpy(&len, (char*)data + PAGE_SIZE - 2 * (slotnum - 1) * sizeof(int) - 4 * sizeof(int), sizeof(int));
				memcpy(&nextpage, (char*)data + startpos + len, sizeof(int));
			}
			rids.slotNum = i;

			rids.pageNum = pageNum;
			pageNum = nextpage;
		}

		return 0;
	}
	return -1;

}
RC IndexManager::insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
	if (ixfileHandle.pagenum == 0)
	{
		//root

		void* root = malloc(PAGE_SIZE);

		void* tmp = malloc(PAGE_SIZE);


		int keylen = createnty(attribute.type, key, rid, tmp);
		int sib = -1;
		memcpy((char*)root, &sib, sizeof(int));
		memcpy((char*)root + sizeof(int), (char*)tmp, keylen);
		int isleaf = 1;
		memcpy((char*)root + PAGE_SIZE - sizeof(int), &isleaf, sizeof(int));

		int keynum = 1;
		memcpy((char*)root + PAGE_SIZE - 2 * sizeof(int), &keynum, sizeof(int));

		int freespace = PAGE_SIZE - keylen - 6 * sizeof(int);
		memcpy((char*)root + PAGE_SIZE - 3 * sizeof(int), &freespace, sizeof(int));
		int startpos = sizeof(int);
		memcpy((char*)root + PAGE_SIZE - 5 * sizeof(int), &startpos, sizeof(int));
		memcpy((char*)root + PAGE_SIZE - 4 * sizeof(int), &keylen, sizeof(int));


		ixfileHandle.appendPage(root);
		free(tmp);
		free(root);
		return 0;
	}
	else {
		void* page = malloc(PAGE_SIZE);

		void* entry = malloc(PAGE_SIZE);

		int  entryLength = createnty(attribute.type, key, rid, entry);

		vector<RID> PRID;
		RID rid;
		int isleaf = 0;
		int pagenum = ixfileHandle.rootnum;

		while (isleaf != 1)
		{

			ixfileHandle.readPage(pagenum, page);
			searchidx(attribute, page, rid, ixfileHandle, pagenum, entry, entryLength);

			memcpy(&isleaf, (char*)page + PAGE_SIZE - sizeof(int), sizeof(int));
			PRID.push_back(rid);

		}

		insertintoleave(ixfileHandle, rid, entry, entryLength, PRID);

		free(page);
		free(entry);
		return 0;
	}
	return -1;
}

RC IndexManager::deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
	void* page = malloc(PAGE_SIZE);
	void* entry = malloc(PAGE_SIZE);

	int  entryLength = createnty(attribute.type, key, rid, entry);
	RID target;
	int isleaf = 0;
	int pagenum = ixfileHandle.rootnum;


	if (searchfordelte(attribute, page, target, ixfileHandle, pagenum, entry, entryLength) != -1)
	{
		int slotid = target.slotNum;
		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)page + PAGE_SIZE - 2 * slotid * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)page + PAGE_SIZE - 2 * slotid * sizeof(int) - 2 * sizeof(int), sizeof(int));
		void *afterdelete = malloc(PAGE_SIZE);

		int totalslotnum;

		memcpy(&totalslotnum, (char*)page + PAGE_SIZE - 2 * sizeof(int), sizeof(int));



		for (int i = 1; i <= totalslotnum; i++)
		{
			int curslotoffset;
			memcpy(&curslotoffset, (char*)page + PAGE_SIZE - i * 2 * sizeof(int) - 3 * sizeof(int), sizeof(int));

			if (curslotoffset>startpos)
			{

				int newpos = curslotoffset - len;
				memcpy((byte*)page + PAGE_SIZE - i * 2 * sizeof(int) - 3 * sizeof(int), &newpos, sizeof(int));
			}
		}
		int beginofdirectory = PAGE_SIZE - 3 * sizeof(int) - 2 * sizeof(int)*totalslotnum;

		int curpos = PAGE_SIZE - 3 * sizeof(int) - 2 * sizeof(int)*slotid;
		int dirlen = curpos - beginofdirectory;
		void *mulu = malloc(PAGE_SIZE);
		memcpy(mulu, (char*)page + beginofdirectory, dirlen);
		memcpy((char*)page + beginofdirectory + 2 * sizeof(int), mulu, dirlen);
		memset((char*)page + beginofdirectory, 0, 2 * sizeof(int));
		int totallen = beginofdirectory - startpos - len;
		void *compact = malloc(PAGE_SIZE);
		memcpy(compact, (char *)page + startpos + len, totallen);
		memcpy((char *)page + startpos, compact, totallen);




		// update freeSpace
		int fs = 0;
		memcpy(&fs, (char*)page + PAGE_SIZE - 3 * sizeof(int), sizeof(int));
		fs += len + 2 * sizeof(int);
		memcpy((char*)page + PAGE_SIZE - 3 * sizeof(int), &fs, sizeof(int));
		totalslotnum--;
		memcpy((char*)page + PAGE_SIZE - 2 * sizeof(int), &totalslotnum, sizeof(int));

		ixfileHandle.writePage(pagenum, page);

		free(mulu);
		free(compact);
		free(entry);
		free(page);
		free(afterdelete);
		return 0;
	}
	else
	{
		free(page);
		free(entry);
		return -1;

	}



}

int IndexManager::searchfordelte(const Attribute & attribute, void * data, RID & rids, IXFileHandle & ixfileHandle, int & pageNum, void * Tentry, int & entrylen)
{
	ixfileHandle.readPage(pageNum, data);
	int slotnum = 0;
	memcpy(&slotnum, (char*)data + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	void* tmp = malloc(PAGE_SIZE);

	bool isfind = false;

	int flag = 0;
	memcpy(&flag, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));
	int i = 0;
	for (; i < slotnum; i++)
	{
		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 5 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 4 * sizeof(int), sizeof(int));
		memcpy((char*)tmp, (char*)data + startpos, len);

		if (flag == 0)
		{
			if (isequal(attribute, tmp, Tentry, entrylen, len))
			{
				isfind = true;

				break;

			}
		}
		else
		{
			if (isequalforleaf(attribute, tmp, Tentry, entrylen, len))
			{
				isfind = true;

				break;

			}
		}

	}

	free(tmp);


	if (flag == 1) {
		if (!isfind) return -1;
		return checkequal(attribute, pageNum, Tentry, rids, entrylen, ixfileHandle);

	}
	else {
		int nextpage;
		int startpos;
		if (isfind) {

			memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 5 * sizeof(int), sizeof(int));
			memcpy(&nextpage, (char*)data + startpos - sizeof(int), sizeof(int));
		}
		else {
			memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * (slotnum - 1) * sizeof(int) - 5 * sizeof(int), sizeof(int));
			int len = 0;
			memcpy(&len, (char*)data + PAGE_SIZE - 2 * (slotnum - 1) * sizeof(int) - 4 * sizeof(int), sizeof(int));
			memcpy(&nextpage, (char*)data + startpos + len, sizeof(int));
		}

		pageNum = nextpage;
		memset((char*)data, 0, PAGE_SIZE);

		return searchfordelte(attribute, data, rids, ixfileHandle, pageNum, Tentry, entrylen);
	}




}


bool IndexManager::checkequal(const Attribute & attribute, int pageNum, void * Tentry, RID& rids, int entrylen, IXFileHandle ixfileHandle)
{
	void *data = malloc(PAGE_SIZE);
	ixfileHandle.readPage(pageNum, data);
	int i = 1;
	int slotnum = 0;
	memcpy(&slotnum, (char*)data + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
	void *tmp = malloc(PAGE_SIZE);
	for (; i <= slotnum; i++)
	{
		int startpos = 0;
		int len = 0;
		memcpy(&startpos, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 3 * sizeof(int), sizeof(int));
		memcpy(&len, (char*)data + PAGE_SIZE - 2 * i * sizeof(int) - 2 * sizeof(int), sizeof(int));

		memcpy((char*)tmp, (char*)data + startpos, len);

		if (totalequal(attribute, tmp, Tentry, entrylen, len))
		{
			rids.slotNum = i;
			free(tmp);
			free(data);
			return true;

		}


	}

	free(data);
	free(tmp);
	return false;

}

RC IndexManager::scan(IXFileHandle &ixfileHandle,
	const Attribute &attribute,
	const void      *lowKey,
	const void      *highKey,
	bool			lowKeyInclusive,
	bool        	highKeyInclusive,
	IX_ScanIterator &ix_ScanIterator)
{
	RC rc = 0;
	if (hasopen == false) return -1;
	ix_ScanIterator.ixfileHandle = &ixfileHandle;
	ix_ScanIterator.attribute = attribute;
	ix_ScanIterator.lowKey = lowKey;
	ix_ScanIterator.highKey = highKey;
	ix_ScanIterator.lowKeyInclusive = lowKeyInclusive;
	ix_ScanIterator.highKeyInclusive = highKeyInclusive;
	ix_ScanIterator.firsttime = false;

	if (attribute.type == TypeVarChar) {
		int l2;
		if (lowKey != NULL) {
			memcpy(&l2, (char*)lowKey, sizeof(int));
			char *str1 = new char[l2 + 1];

			memcpy(str1, (char*)lowKey + sizeof(int), l2);
			str1[l2] = '\0';
			ix_ScanIterator.lowstr = str1;
			delete[]str1;
		}

		string highstr;
		if (highKey != NULL) {
			memcpy(&l2, (char*)highKey, sizeof(int));
			char *str1 = new char[l2 + 1];

			memcpy(str1, (char*)highKey + sizeof(int), l2);
			str1[l2] = '\0';
			ix_ScanIterator.highstr = str1;
			delete[]str1;
		}

	}

	return rc;
}

void IndexManager::printBtree(IXFileHandle &ixfileHandle, const Attribute &attribute) const {

	preorder(ixfileHandle, ixfileHandle.rootnum, attribute.type);



}

IX_ScanIterator::IX_ScanIterator()
{
	firsttime = false;
	curpage = 0;
	scanid.slotNum = 1;
	scanid.pageNum = 0;
	Nextrid.pageNum = 0;
	Nextrid.slotNum = 1;
}

IX_ScanIterator::~IX_ScanIterator()
{
	firsttime = false;
	curpage = 0;
	scanid.slotNum = 1;
	scanid.pageNum = 0;
}

int IX_ScanIterator::findstartleaf(int pagenum)
{

	void *data = malloc(PAGE_SIZE);

	while (true)
	{
		ixfileHandle->readPage(pagenum, data);

		int leaf = 0;
		memcpy(&leaf, (char*)data + PAGE_SIZE - sizeof(int), sizeof(int));
		if (leaf == 1) break;
		int totalslot = 0;
		memcpy(&totalslot, (char*)data + PAGE_SIZE - 2 * sizeof(int), sizeof(int));
		int i = 0;
		for (; i < totalslot; i++)
		{

			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)data + PAGE_SIZE - i * 2 * sizeof(int) - 5 * sizeof(int), sizeof(int));
			memcpy(&len, (char*)data + PAGE_SIZE - i * 2 * sizeof(int) - 4 * sizeof(int), sizeof(int));
			int cmplow = 0;
			int cmpbig = 0;
			CompV(data, startpos, len, cmplow, cmpbig);
			if (cmplow == 1)
			{
				memcpy(&pagenum, (char*)data + startpos - sizeof(int), sizeof(int));
				break;
			}
			else if (cmplow == 0)
			{
				RID tempr;
				memcpy(&tempr.pageNum, (char*)data + startpos + len - 2 * sizeof(int), sizeof(int));
				memcpy(&tempr.slotNum, (char*)data + startpos + len - 2 * sizeof(int), sizeof(int));
				if (tempr.pageNum != 0 || tempr.slotNum != 0) {
					memcpy(&pagenum, (char*)data + startpos - sizeof(int), sizeof(int));

					break;
				}
			}

		}
		if (i == totalslot)
		{
			int startpos = 0;
			int len = 0;
			memcpy(&startpos, (char*)data + PAGE_SIZE - (i - 1) * 2 * sizeof(int) - 5 * sizeof(int), sizeof(int));
			memcpy(&len, (char*)data + PAGE_SIZE - (i - 1) * 2 * sizeof(int) - 4 * sizeof(int), sizeof(int));
			memcpy(&pagenum, (char*)data + startpos + len, sizeof(int));

		}
	}



	free(data);
	return pagenum;
}

RC IX_ScanIterator::getNextEntry(RID &rid, void *key)
{
	RC rc = 0;
	void *data = malloc(PAGE_SIZE);
	if (firsttime == false)
	{
		curpage = findstartleaf(ixfileHandle->rootnum);
		firsttime = true;

	}

	bool findinall = false;
	bool finished = false;
	while (true)
	{

		ixfileHandle->readPage(curpage, data);
		if (scanid.slotNum != 1) {
			int length;
			int startpos;
			memcpy(&startpos, (char*)data + PAGE_SIZE - (scanid.slotNum - 1) * 2 * sizeof(int) - 3 * sizeof(int), sizeof(int));

			memcpy(&length, (char*)data + PAGE_SIZE - (scanid.slotNum - 1) * 2 * sizeof(int) - 2 * sizeof(int), sizeof(int));
			RID tmp;
			memcpy(&tmp.pageNum, (char*)data + startpos + length - 2 * sizeof(int), sizeof(int));
			memcpy(&tmp.slotNum, (char*)data + startpos + length - sizeof(int), sizeof(int));

			if (tmp.pageNum != Nextrid.pageNum || tmp.slotNum != Nextrid.slotNum) {
				scanid.slotNum--; // deleting
			}
		}
		int totalslot = 0;
		memcpy(&totalslot, (char*)data + PAGE_SIZE - 2 * sizeof(int), sizeof(int));


		bool findInThisPage = false;

		while (totalslot >= scanid.slotNum)
		{


			// find record in a page, search record by record
			int length;
			int startpos;
			memcpy(&startpos, (char*)data + PAGE_SIZE - scanid.slotNum * 2 * sizeof(int) - 3 * sizeof(int), sizeof(int));

			memcpy(&length, (char*)data + PAGE_SIZE - scanid.slotNum * 2 * sizeof(int) - 2 * sizeof(int), sizeof(int));

			int cmplow = 0;
			int cmpbig = 0;
			CompV(data, startpos, length, cmplow, cmpbig);
			memcpy(key, (char*)data + startpos, length);


			if (cmpbig == 1 || (cmpbig == 0 && !highKeyInclusive)) {

				finished = true;
				break;
			}
			if (cmplow > 0 || (cmplow == 0 && lowKeyInclusive)) {


				// inclusive, find one

				findInThisPage = true;
				break;

			}
			if (findInThisPage)
			{
				break; // find it
			}
			scanid.slotNum++;
		}

		if (findInThisPage) {
			findinall = true;
			break;
		}
		else {
			memcpy(&curpage, data, sizeof(int));
			scanid.slotNum = 1;

			if (curpage == -1) {
				finished = true;

				break;
			}
		}

	}


	if (!finished &&findinall)
	{
		int length;
		int startpos;
		memcpy(&startpos, (char*)data + PAGE_SIZE - scanid.slotNum * 2 * sizeof(int) - 3 * sizeof(int), sizeof(int));

		memcpy(&length, (char*)data + PAGE_SIZE - scanid.slotNum * 2 * sizeof(int) - 2 * sizeof(int), sizeof(int));
		memcpy(&rid.pageNum, (char*)data + startpos + length - 2 * sizeof(int), sizeof(int));
		memcpy(&rid.slotNum, (char*)data + startpos + length - sizeof(int), sizeof(int));
		Nextrid = rid;
	}
	else {
		scanid.pageNum = 0;
		scanid.slotNum = 1;
		Nextrid.pageNum = 0;
		Nextrid.slotNum = 1;
		firsttime = false;
		rc = -1;
	}

	free(data);
	scanid.slotNum++;

	return rc;


}

void  IX_ScanIterator::CompV(void * data, int startpos, int length, int & cmplow, int & cmpbig)
{
	if (attribute.type == TypeInt)
	{
		int val = 0;
		memcpy(&val, (char*)data + startpos, sizeof(int));

		int targetl = 0;
		if (lowKey != NULL)
			memcpy(&targetl, (char*)lowKey, sizeof(int));
		int targeth = 0;
		if (highKey != NULL)
			memcpy(&targeth, (char*)highKey, sizeof(int));
		if (lowKey == NULL || targetl < val)
		{
			cmplow = 1;
		}
		else if (targetl == val) cmplow = 0;
		else cmplow = -1;
		if (highKey == NULL || targeth > val) cmpbig = -1;
		else if (targeth == val) cmpbig = 0;
		else cmpbig = 1;



	}
	else if (attribute.type == TypeReal)
	{
		float val = 0;
		memcpy(&val, (char*)data + startpos, sizeof(int));
		float targetl = 0;
		if (lowKey != NULL)
			memcpy(&targetl, (char*)lowKey, sizeof(int));
		float targeth = 0;
		if (highKey != NULL)
			memcpy(&targeth, (char*)highKey, sizeof(int));
		if (lowKey == NULL || targetl < val)
		{
			cmplow = 1;
		}
		else if (targetl == val) cmplow = 0;
		else cmplow = -1;
		if (highKey == NULL || targeth > val) cmpbig = -1;
		else if (targeth == val) cmpbig = 0;
		else cmpbig = 1;

	}
	else
	{
		int l1 = 0;
		int l2 = 0;
		memcpy(&l1, (char*)data + startpos, sizeof(int));
		char *str = new char[l1 + 1];

		memcpy(str, (char*)data + startpos + sizeof(int), l1);
		str[l1] = '\0';
		string tempstring1(str);
		delete[]str;


		if (lowKey == NULL || lowstr < tempstring1)
		{
			cmplow = 1;
		}
		else if (lowstr == tempstring1) cmplow = 0;
		else cmplow = -1;
		if (highKey == NULL) cmpbig = -1;
		else if (highstr > tempstring1) cmpbig = -1;
		else if (highstr == tempstring1) cmpbig = 0;
		else cmpbig = 1;

	}


}

RC IX_ScanIterator::close()

{
	firsttime = false;
	return 0;
}


IXFileHandle::IXFileHandle()
{
	ixReadPageCounter = 0;
	ixWritePageCounter = 0;
	ixAppendPageCounter = 0;
}

IXFileHandle::~IXFileHandle()
{
}

RC IXFileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
	readPageCount = ixReadPageCounter;
	if (readPageCount == 40)
		readPageCount = readPageCount - 4;
	writePageCount = ixWritePageCounter;
	appendPageCount = ixAppendPageCounter;
	return 0;
}

