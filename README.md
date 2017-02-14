
``
**Clone the repository**
git clone git@github.com:PayasR/redbase.git 

**Install the dependencies**
sudo apt-get install flex bison g++ g++-multilib git cmake make 

**Build the code**
cd redbase
mkdir build
cd build
cmake ..
make

**Test**
./dbcreate Test
./redbase Test

**DDL Commands**
create table data(name c20, id i);
drop table data;

**DML Commands**
insert into data values ("abc", 1);
select * from data;
````

Recommended familiarity with:
CMake, Make, Valgrind, Grep, Vim, Flex, Bison, 

