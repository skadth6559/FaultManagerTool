# Fault Manager - Developer Guide

The Fault Manager is used to track 4 categories of data, which are faults, failsafes, interlocks, and system notes, for every software release as well as provide the user with abilities to modify this data for unlocked software releases.

Each major feature of the tool will be highlighted below and the backend process will be explained for each.

## Introduction to the Database
The Fault Manager relies on a MySQL database to store all of the fault, failsafe, interlock, and system note data for each software release. Each release is stored in the database with underscores replacing the dots in the name to conform to SQL syntax requirements (e.g. 4.0.1.0 would be stored as 4_0_1_0). There are 4 tables stored for each release in the database to represent the 4 different categories of data. The category that a specific table belongs to is denoted by the suffix and these suffixes are listed below. The fields associated with each table as well as more information on how to setup the database can be found in the **database_setup.md** file.
```
Faults: no suffix, only the version name (e.g. 4_0_1_0)
Failsafes: suffix is "_failsafes" (e.g. 4_0_1_0_failsafes)
Interlocks: suffix is "_interlocks" (e.g. 4_0_1_0_interlocks)
System Notes: suffix is "_system_notes" (e.g. 4_0_1_0_system_notes)
```
In addition to the sets of 4 tables for each software release, the database also contains the **releases** table which keeps track of all the release numbers currently being stored in the database as well as whether or not these tables are released. If a table is marked as released in the status field in this table, then options to edit this table through the tool will be disabled. The parent version for each version is also stored which is used to determine which previous version was used as the baseline for any given version.

The last table is the **faultconstants** table which stores enum names for faults as well as what class these enums belong to. This table is used primarily for the FaultConstants.h export option in the tool.

## Adding a new entry to Fault/Failsafe/Interlock/System Note tables
Clicking the new row button at the bottom of the main page presents a form specific to the category of data currently selected. When this form is filled in by the user and saved, the information is saved to the database through the process below.
    - Each form is a separate page in the QStackedWidget and each form contains the fields from the database as inputs that will be saved to the database when the save button is clicked.

Each category has two major functions in mainwindow.cpp related to saving a new entry which are startAdd\[Category\]() and saveNew\[Category\](). 
- The startAdd...() functions are intended to prompt the user for confirmation about saving the entry as well as an optional reason for why the entry is being added. This reason will get saved to the entry's changelog if provided, otherwise a default message of "Fault Created" is saved along with a timestamp.

- The saveNew...() functions are where the queries to actually save the entry are generated. These queries are written as QStrings in the form of a MySQL query and are executed using the execQuery() function in mymodel.cpp.
    - The query is given the tablename designated for the current version and category being viewed in the main table and the contents of the relevant fields are added to the query in order.

    - execQuery() calls execFetch(QSqlQuery *q, QString query, QString dbname) which is a function that connects to the database dbname and uses the QSqlQuery object q to execute the query.

After the entry is saved, the main page is displayed and the new entry can be found in the table.



# Locking and Unlocking Releases
The password to unlock a release is "editfaults".