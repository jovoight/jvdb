# JVDB: The Ultimate Employee Database
This is my first major project in the C language. Since I use databases all the time both at work and in my personal projects, I wanted to gain a deeper understanding of how they actually work by building my own.
## High-Level Overview
For the purposes of this project, the database will be a simple .db file (similar to a SQLite database). My program supports basic CRUD functionality for this file. Further optimizations and additional features will be added later down the line.
## Basic Functionality
By running the binary "./jvdb", the contents of the DB file (employee structs) will be read into memory. Data will be validated to ensure the file is secure, it is actually the correct information, and nothing is missing. At this point, the program will check for additional flags to see which feature(s) is/are currently being requested.
## Features (CLI Flags)
Specify DB file (-f "\<file_name\>)
Create new DB file if does not exist (-n)
Add a new employee (-a \<name,address,hours\>)
List all current employees (-l)
Remove an employee by name (-r \<name\>)
Update an employee's hours by name (-u \<name,hours\>)
Help (list all flags and usage) (-h)
