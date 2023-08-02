# Fault Manager - Database Setup
The Fault Manager is written to communicate with a mysql database that can be located anywhere on the local network. The server and database must have the following things for the application to work:

- Server is located on the local network (has a local ip adress for the client computer)
- Server is configured to allow traffic on port 3306 (default mysql port number)
- Database is created using the following command:
	```
	CREATE DATABASE fmtooldb;
	```
- A user is created for each person and given access to the database using the following commands:
	```
	CREATE USER '[textronid]'@'%' IDENTIFIED BY '[password]';
	GRANT ALL ON fmtooldb.* to '[textronid]';
	```
- When prompted at application startup, the user enters the following info into the prompts
	- database hostname: local ip of the server with the database
	- user name: username of the user created for that person (probably just their textron id/email)
	- password: password of the user created for that person (can be all the same or different per person)

The first software release has to be added to the database manually in order to get the data in, but all subsequent ones can just be created in the tool.
To populate the database with the first release, the following tables need to be added with the following column names and types:

#### "[release_num]" - ex: "4_0_1_0"
	- id                             : int
	- fm_status                      : VARCHAR(200)
	- group_name                     : VARCHAR(200)
	- enum_name                      : VARCHAR(200)
	- fault_code                     : VARCHAR(200)
	- changee                        : VARCHAR(200)
	- fault_group                    : VARCHAR(200)
	- configuration                  : VARCHAR(200)
	- wca_severity                   : VARCHAR(200)
	- wca_message                    : VARCHAR(200)
	- wca_trigger                    : VARCHAR(200)
	- wca_clearing                   : VARCHAR(200)
	- failure_impact                 : VARCHAR(200)
	- failsafe_code                  : VARCHAR(200)
	- interlock_code                 : VARCHAR(200)
	- failsafe_summary               : VARCHAR(200)
	- onboard_lcs                    : VARCHAR(1000)
	- offboard_lcs                   : VARCHAR(1000)
	- lcs_lr                         : VARCHAR(1000)
	- corrective_maintenance_actions : VARCHAR(1000)
	- wca_notes                      : VARCHAR(200)
	- device                         : VARCHAR(200)
	- notes                          : VARCHAR(1000)
	- changelog                      : VARCHAR(1000)

#### "[release_num]_failsafes" - ex: "4_0_1_0_failsafes"
	- fm_status                      : VARCHAR(200)
	- failsafe_id                    : VARCHAR(200)
	- configuration                  : VARCHAR(200)
	- failure_name                   : VARCHAR(200)
	- operational_state              : VARCHAR(200)
	- triggers                       : VARCHAR(200)
	- triggering_faults              : VARCHAR(200)
	- failsafe_clearing              : VARCHAR(200)
	- indication_method              : VARCHAR(200)
	- failsafe_notes                 : VARCHAR(1000)
	- funtional_impact               : VARCHAR(1000)
	- rac                            : VARCHAR(200)
	- first_auto_response            : VARCHAR(1000)
	- second_auto_response           : VARCHAR(1000)
	- third_auto_response            : VARCHAR(1000)
	- failsafe_summary               : VARCHAR(1000)
	- applicable_safety_precept      : VARCHAR(200)
	- mitigated_rac                  : VARCHAR(200)
	- internal_notes                 : VARCHAR(1000)
	- changee                        : VARCHAR(200)
	- changelog                      : VARCHAR(1000)

#### "[release_num]_interlocks" - ex: "4_0_1_0_interlocks"
	- fm_status                      : VARCHAR(200)
	- interlock_id                   : VARCHAR(200)
	- configuration                  : VARCHAR(200)
	- components                     : VARCHAR(200)
	- interlock_description          : VARCHAR(1000)
	- associated_wcas                : VARCHAR(1000)
	- internal_notes                 : VARCHAR(1000)
	- changee                        : VARCHAR(200)
	- changelog                      : VARCHAR(1000)

#### "[release_num]_system_notes" - ex: "4_0_1_0_system_notes"
	- id                             : int
	- fm_status                      : VARCHAR(200)
	- configuration                  : VARCHAR(200)
	- operator                       : VARCHAR(200)
	- summary                        : VARCHAR(1000)
	- description                    : VARCHAR(1000)
	- workarounds                    : VARCHAR(1000)
	- external_notes                 : VARCHAR(1000)
	- internal_notes                 : VARCHAR(1000)
	- fault_groups                   : VARCHAR(200)
	- pcr_num                        : VARCHAR(200)
	- changee                        : VARCHAR(200)
	- changelog                      : VARCHAR(1000)

#### "releases"
	- id                             : int
	- version                        : VARCHAR(50)
	- status                         : VARCHAR(50)
	- parent                         : VARCHAR(200)

#### "faultconstants"
	- enum_name                      : VARCHAR(200)
	- class                          : VARCHAR(200)

**Note: The faultconstants table is used as the master list for the faults to be exported in the source code exports. The tool exports all faults that appear in this table to the source code files if they also have a status of "IN USE" or "ACTIVE" in the main faults table for that software release. This proccess and this table can be chagned later (any maybe deleted) but this is how it is currently built. The table is automatically updated on creating/editing faults in the tool.**

## Initial Data Import


All of the data for these tables will be added to the database before the first user uses the application. The procedure for import goes like the following:

1) Create the table in the database with the proper fields and data types
2) Find spreadsheet containing the data for the desired software release
3) Copy all desired information for the tables into a new excel spreadsheet, taking note of which database fields are in which columns
4) Save the excel spreadsheet as "Unicode Text (*.txt)"
5) Open saved data file in notepad++, right click the bottom right where it says "UTF-16 LE BOM" and select "Convert to UTF-8"
6) Save the file and close out of notepad++
7) To enable local file import, two things need to be set. First, in a mysql command line instance with the database selected, run the following command:
	```
	set global local_infile=on;
	```
8) With the global option set, the next thing to do is launch a new mysql command line instance with the local infile runtime option. In a new command prompt instance, run the following command (assuming mysql.exe is added to the path):
	```
	mysql --local-infile=1 -u root -p
	```
9) Once logged in, run an SQL command similar to the following to actually do the import, changing the "@colX" tags for how many columns there are and the name of the database fields for each spreadsheet column:
	```
	load data local infile '[Path of data file]' into table [release_num] fields terminated by '\t' lines terminated by '\r\n' (@col1,@col2,@col3) set fm_status=@col1, group_name=@col2, enum_name=@col3;
	```
10) Repeat all steps with all tables that need to be created