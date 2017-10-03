+++
date = "2017-10-02T18:20:18-07:00"
title = "Project Milestone 1: Getting started"
sidemenu = "true"
draft = false
+++

#### Table of Contents
1. [Overview](#overview)
2. [Life of an SQL Query](#life)
3. [Spatial data handling](#spatial)
4. [Introduction to Redbase](#redbase)
5. [Assigned Task](#assignment)

#### Overview <a name="overview"></a>
A vast majority of databases in use these days are relational, and popular implementations like Oracle, Microsoft SQL Server, MySQL and PostgreSQL dominate the database software market. A critical component of a database system is its index, a structure that allows efficient retrieval and join operations on the underlying tables. Over the quarter we shall be using [Redbase](https://github.com/PayasR/redbase-spatial), an instructional relational database written in C++ that is otherwise complete, but does not implement an index. Our objective would be to implement an [R-tree](http://www.cs.ucr.edu/~ravi/CS236Papers/R-tree.pdf) index. 

This first assignment is designed to introduce you to the internals of relational databases, get you familiar with spatial data and help you set up the Linux environment for Redbase. The instructions for submission can be found at the end of this document.

#### Life of an SQL query<a name="life"></a>
This section is a slightly modified excerpt from Section 1, [Architecture of a Database System](http://db.cs.berkeley.edu/papers/fntdb07-architecture.pdf)
 
{{% fluid_img class="pure-u-1-1" src="/img/archDB.PNG" %}}
<!-- _Figure 1.1: Architecture of a typical RDBMS_ -->

At heart, a typical RDBMS has five main components, as illustrated in Figure 1.1. Consider a simple but typical database interaction at an airport, in which a gate agent clicks on a form to request the passenger list for a flight. This button click results in a single-query transaction that works roughly as follows:

1. The client software on the personal computer at the airport gate (the “client”) calls an API that in turn communicates over a network to establish a connection with the Client Communications Manager of a DBMS (top of Figure 1.1). In some cases, this connection is established between the client and the database server directly, e.g., via the ODBC or JDBC connectivity protocol. This arrangement is termed a “two-tier” or “client-server” system. In other cases, the client may communicate with a “middle-tier server” (a web server, transaction processing monitor, or the like), which in turn uses a protocol to proxy the communication between the client and the DBMS. This is usually called a “three-tier” system. In many web based scenarios there is yet another “application server” tier between the web server and the DBMS, resulting in four tiers. Given these various options, a typical DBMS needs to be compatible with many different connectivity protocols used by various client drivers and middleware systems. At base, however, the responsibility of the DBMS’ client communications manager in all these protocols is roughly the same: to establish and remember the connection state for the caller (be it a client or a middleware server), to respond to SQL commands from the caller, and to return both data and control messages (result codes, errors, etc.) as appropriate. In our simple example, the communications manager would establish the security credentials of the client, set up state to remember the details of the new connection and the current SQL command across calls, and forward the client’s first request deeper into the DBMS to be processed.
2. Upon receiving the client’s first SQL command, the DBMS must assign a “thread of computation” to the command. It must also make sure that the thread’s data and control outputs are connected via the communications manager to the client. These tasks are the job of the DBMS Process Manager (left side of Figure 1.1). The most important decision that the DBMS needs to make at this stage in the query regards admission control: whether the system should begin processing the query immediately, or defer execution until a time when enough system resources are available to devote to this query.
3. Once admitted and allocated as a thread of control, the gate agent’s query can begin to execute. It does so by invoking the code in the Relational Query Processor (center, Figure 1.1). This set of modules checks that the user is authorized to run the query, and compiles the user’s SQL query text into an internal query plan. Once compiled, the resulting query plan is handled via the plan executor. The plan executor consists of a suite of “operators” (relational algorithm implementations) for executing any query. Typical operators implement relational query processing tasks including joins, selection, projection, aggregation, sorting and so on, as well as calls to request data records from lower layers of the system. In our example query, a small subset of these operators — as assembled by the query optimization process — is invoked to satisfy the gate agent’s query. 
4. At the base of the gate agent’s query plan, one or more operators exist to request data from the database. These operators make calls to fetch data from the DBMS’ Transactional Storage Manager (Figure 1.1, bottom), which manages all data access (read) and manipulation (create, update, delete) calls. The storage system includes algorithms and data structures for organizing and accessing data on disk (“access methods”), including basic structures like tables and indexes. It also includes a buffer management module that decides when and what data to transfer between disk and memory buffers. Returning to our example, in the course of accessing data in the access methods, the gate agent’s query must invoke the transaction management code to ensure the “ACID” properties of transactions. Before accessing data, locks are acquired from a lock manager to ensure correct execution in the face of other concurrent queries. If the gate agent’s query involved updates to the database, it would interact with the log manager to ensure that the transaction was durable if committed, and fully undone if aborted.
5. At this point in the example query’s life, it has begun to access data records, and is ready to use them to compute results for the client. This is done by “unwinding the stack” of activities we described up to this point. The access methods return control to the query executor’s operators, which orchestrate the computation of result tuples from database data; as result tuples are generated, they are placed in a buffer for the client communications manager, which ships the results back to the caller. For large result sets, the client typically will make additional calls to fetch more data incrementally from the query, resulting in multiple iterations through the communications manager, query executor, and storage manager. In our simple example, at the end of the query the transaction is completed and the connection closed; this results in the transaction manager cleaning up state for the transaction, the process manager freeing any control structures for the query, and the communications manager cleaning up communication state for the connection.
6. The right-hand side of Figure 1.1 depicts several shared components and utilities that are vital to the operation of a full-function DBMS. The catalog and memory managers are invoked as utilities during any transaction, including our example query. The catalog is used by the query processor during authentication, parsing, and query optimization. The memory manager is used throughout the DBMS whenever memory needs to be dynamically allocated or deallocated. The remaining modules listed in the rightmost box of Figure 1.1 are utilities that run independently of any query, keeping the database well-tuned and reliable. 

#### Spatial data<a name="spatial"></a>
Spatial data is information about the locations and shapes of geographic features and the relationships between them, usually stored as coordinates and topology (ESRI, 2017). In other words, it is data that is connected to a ‘location’ on earth, usually represented as latitude and longitude coordinates. Such data is generated by sensors, GPS devices, smartphones and is at the center of GIS systems. Since spatial data is multidimensional (the queries involve both latitude and longitude, in some cases even time), managing it requires special techniques and indexes, some of which will be covered in the papers we read in the course. The simplest spatial indexes are R-trees, Quadtrees and kD-trees. Specialized databases called geodatabases implement these techniques and are used by GIS implementations like ESRI’s ArcGIS and the open source QGIS and GRASS. Popular databases like MySQL, PostgreSQL, Oracle, SQL Server all support spatial data either natively or through extensions. 

{{% fluid_img src="http://resources.arcgis.com/en/communities/analysis/GUID-143B3B54-7960-4958-85DE-524E3D0BB68F-web.png" %}}
_Figure 1.2: A heat map using spatial data_

{{% fluid_img src="http://axismaps.github.io/thematic-cartography/images/classed_choropleth.jpg" %}}
_Figure 1.3: A chloropleth map using spatial data_


#### Spatial datatypes and SQL extensions for spatial data
Since spatial data is inherently multidimensional, the Open Geospatial Consortium (OGC) defines standards that define the types of spatial objects that can be created and stored. Simple spatial datatypes include:

* Points: Simple points in a 2D plane
* Lines: Infinite lines or finite line segments
* Polylines: Also called ‘paths’ by certain implementations, used to represent irregular continuous lines like roads and rivers
* Polygons: Used to represent areas

{{% fluid_img src="https://docs.microsoft.com/en-us/sql/relational-databases/spatial/media/geom-hierarchy.gif" %}} 
_Figure 1.4: Spatial datatypes supported by SQL Server_

Different spatial DBMS implement different subsets of the spatial datatypes of the original OGC specification. Spatial DBMS also extend the regular SQL to support spatial datatypes and relations between the objects. We shall look at a simple example of PostGIS and see how these extensions work. Please note that PostGIS ships as an extension to regular PostgreSQL and needs to be installed separately. 

#### Spatial SQL example: Neighborhoods in NYC
New York has a rich history of neighborhood names and extent. Neighborhoods are social constructs that do not follow lines laid down by the government. For example, the Brooklyn neighborhoods of Carroll Gardens, Red Hook, and Cobble Hill were once collectively known as “South Brooklyn.” And now, depending on which real estate agent you talk to, the same four blocks in the-neighborhood-formerly-known-as-Red-Hook can be referred to as Columbia Heights, Carroll Gardens West, or Red Hook!

Let’s say the dataset has the following schema:

| Column name | Description |
| ----------- | ----------- |
| **name** | Name of the neighborhood |
| **boroname**  | Name of the New York borough. Manhattan, The Bronx, Brooklyn, Staten Island, Queens |
| **geom**	    | Polygon boundary of the neighborhood |

{{% fluid_img src="http://workshops.boundlessgeo.com/postgis-intro/_images/nyc_neighborhoods.png" %}} 
_Figure 1.5: The neighbourhoods of NYC_

* Creating a spatial table
    ```
    CREATE TABLE nyc_neighbourhoods (name varchar, boroname varchar, geom geometry);
    ```

* Inserting data
    ```
    INSERT INTO nyc_neighbourhoods VALUES ('Carroll Hills', ’Brooklyn’, 'POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))');
    ```  
Since the datasets are usually large, SQL IMPORT statements are used to populate the tables. 

* Intersect Query
    ```
    SELECT name, boroname
    FROM nyc_neighborhoods
    WHERE ST_Intersects(geom, ST_GeomFromText('POINT(583571 4506714)',26918));
    ```
The above query can be read as: Return the name of a neighborhood and the borough that this point (583571, 4506714) intersects with.

The `ST_GeomFromText()` is a function used to create a point object from the text specified in the query. In addition to intersection, the OGC specification also lists several other spatial relationships: Contains, Crosses, Disjoint, Distance, DistanceWithin, Equals, Intersects, Touches, Within. 

**A detailed tutorial for spatial data handling in PostGIS can be found [here](workshops.boundlessgeo.com/postgis-intro/).**

#### Introduction to Redbase<a name="redbase"></a>

```
                    +----------------------------------------+
                    |            Query Language              | 
                    +----------------------------------------+
                    |           System Management            |
                    +-------------------+--------------------+
                    |     Indexes       |  Record Management |
                    +-------------------+--------------------+
                    |              Paged File                |
                    +----------------------------------------+
```

Redbase is a stripped down relational DBMS, that implements the [Stanford Redbase interfaces](https://web.stanford.edu/class/cs346/2015/redbase.html). The system is kept as simple as possible and does not include the Process Manager, the Client Components Manager and the Shared Utilities of Figure 1.1. It is assumed that the queries are input directly to the database command line and no separate clients are available. The system is organized into five major components:
 
* **Paged File** - The PF component provides facilities for higher-level client components to perform file I/O in terms of pages. In the PF component, methods are provided to create, destroy, open, and close paged files, to scan through the pages of a given file, to read a specific page of a given file, to add and delete pages of a given file, and to obtain and release pages for scratch use. It also implements the buffer pool for use by the other components. The interface for this layer is composed of 3 classes: `PF_Manager`, `PF_FileHandle` and `PF_PageHandle`. 
    * The `PF_Manager` class provides the functions for creating, deleting, opening and closing paged files
    ```
    class PF_Manager
    {
    public:
        PF_Manager    ();                           // Constructor
        ~PF_Manager   ();                           // Destructor
        RC CreateFile    (const char *fileName);       // Create a new file
        RC DestroyFile   (const char *fileName);       // Destroy a file
        RC OpenFile      (const char *fileName, PF_FileHandle &fileHandle);  
                                                    // Open a file
        RC CloseFile     (PF_FileHandle &fileHandle);  // Close a file
        RC AllocateBlock (char *&buffer);              // Allocate a new scratch page in buffer
        RC DisposeBlock  (char *buffer);               // Dispose of a scratch page
    };
    ```

    * The `PF_FileHandler` contains functions that are used to access pages of an open file
    ```
    class PF_FileHandle {
    public:
        PF_FileHandle  ();                                  // Default constructor
        ~PF_FileHandle ();                                  // Destructor
        PF_FileHandle  (const PF_FileHandle &fileHandle);   // Copy constructor
        PF_FileHandle& operator= (const PF_FileHandle &fileHandle);
                                                            // Overload =
        RC GetFirstPage   (PF_PageHandle &pageHandle) const;   // Get the first page
        RC GetLastPage    (PF_PageHandle &pageHandle) const;   // Get the last page

        RC GetNextPage    (PageNum current, PF_PageHandle &pageHandle) const; 
                                                            // Get the next page
        RC GetPrevPage    (PageNum current, PF_PageHandle &pageHandle) const;
                                                            // Get the previous page
        RC GetThisPage    (PageNum pageNum, PF_PageHandle &pageHandle) const;  
                                                            // Get a specific page
        RC AllocatePage   (PF_PageHandle &pageHandle);         // Allocate a new page
        RC DisposePage    (PageNum pageNum);                   // Dispose of a page 
        RC MarkDirty      (PageNum pageNum) const;             // Mark a page as dirty
        RC UnpinPage      (PageNum pageNum) const;             // Unpin a page
        RC ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
                                                            //   to disk
    };
    ```
    * The `PF_PageHandler` class contains functions that access the contents of a single page stored in the file. 
    ```
    class PF_PageHandle {
    public:
        PF_PageHandle  ();                          // Default constructor
        ~PF_PageHandle ();                          // Destructor
        PF_PageHandle  (const PF_PageHandle &pageHandle); 
                                                    // Copy constructor
        PF_PageHandle& operator= (const PF_PageHandle &pageHandle);
                                                    // Overload =
        RC GetData        (char *&pData) const;        // Set pData to point to
                                                    //   the page contents
        RC GetPageNum     (PageNum &pageNum) const;    // Return the page number
    };
    ```
* **Record Management** – The RM component is built on top of the PF Layer and provides classes and methods for managing files of unordered records. The interface of this layer is similar to the PF Layer, and contains `RM_Manager`, `RM_FileHandle`, `RM_Record` classes. In addition, the RM component also provides an `RM_FileScan` class, that allows the above layers to scan over the RM_Record objects stored in the file. 
* **Indexing** - The IX component provides classes and methods for managing persistent indexes over unordered data records stored in paged files. The indexes ultimately will be used to speed up processing of relational selections, joins, and condition-based update and delete operations. Like the data records themselves, the indexes are stored in paged files. The API for this component is specified here. We shall be implementing this layer.
* **System Management** - The SM compoment provides the following functions: 
    * Unix command line utilities - for creating and destroying RedBase databases, invoking the system.
    * Data definition language (DDL) commands - for creating and dropping relations, creating and dropping indexes.
    * System utilities - for bulk loading, help, printing relations and setting parameters.
    * Metadata management - for maintaining system catalogs containing table names and schema.
* **Query Language** - The QL component implements the language RQL (for “RedBase Query Language”). RQL’s data retrieval command is a restricted version of the SQL Select statement. RQL’s data modification commands are restricted versions of SQL’s Insert, Delete, and Update statements. The QL component uses classes and methods from the IX, RM, and SM components. More details can be found here.

#### Assigned task<a name="assignment"></a>
The first assignment is simple. You need to download Redbase, build it and run simple RQL queries. You will need a linux-like environment to be able to complete the following steps. Windows/Mac users can download [VirtualBox](https://www.virtualbox.org/) and run any linux distribution on a VM. We recommend using either [Fedora](http://fedoraproject.org/) or [Ubuntu](https://www.ubuntu.com/desktop). Windows 10 users can also have a look at the WSL (Windows Subsystem for Linux) to compile the given code (Note that you will require a Windows Insider Build installed on your system). 

**Install the dependencies**
```
sudo apt-get install flex bison g++ g++-multilib git cmake make 
```

**Clone repository**
```
git clone git@github.com:PayasR/redbase-spatial.git 
```

**Build the project**
```
cd redbase-spatial
mkdir build
cd build
cmake ..
make -j4
```

**Test**
```
./dbcreate Test
./redbase Test
```

**DDL commands**
```
create table data (name c20, id i);
drop table data;
```

**DML commands**
```
insert into data values ("abc", 1);
select * from data;
```

**Deliverables**

1. Write a short two page report (.PDF) on the architecture of the Redbase system, the interfaces of the five layers and the functions of the following classes: `PF_Manager`, `PF_FileHandle`, `PF_PageHandle`, `RM_Manager`, `RM_FileHandle`, `RM_FileScan`, `RM_Record`, `RID`, `IX_Manager`, `IX_IndexHandle`, `IX_IndexScan`, `SM_Manager`, `QL_Manager`. You may use the [Redbase Specifications](https://web.stanford.edu/class/cs346/2015/redbase.html), but write the description in your own words. Do _not_ copy text verbatim. **(7 points)**

2. Using [RQL](https://web.stanford.edu/class/cs346/2015/redbase-ql.html#rql), create a simple table of your choice, insert 5 data entries and show outputs of 3 different SELECT statements. The SELECT queries must use the WHERE clause. Attach screenshots of the queries and the outputs after the two-page writeup. **(3 points)**

**Due date**: 11<sup>th</sup> October 2017, 12:00 pm