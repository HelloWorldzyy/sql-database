# sql-database
a c++ program for sql database including file system, relation manager, query engine
The target for this project is build a sql database, and can process some easy query language like select, delete ,search ,update and insert.



This project has 3 steps, 



The first step: I first implemented a file manager to manager records. It contains two parts, one is file system. It stores all records in disk, I need to design the record format(basically it contains null field, item field,  and actual data), and how to get a record quickly in one page. Another is index system. I implemented a B+ tree by myself, which can handle insert and delete. Also keep balance, with can help us quickly find one item based on index(ex. age)   



Second step: I implemented a relation manager, it is used to manage my file manager. It can create table for index and file. Also it can scan items in table, and process tuples of records.


Third step: I implemented a query engine, I implemented two methods for query one is index based loop. Another is block nested-loop join. They are built above the relation manager. They just use the scan function from relation manger to do some task , like join two tables. Or find items meet some requirement. 
