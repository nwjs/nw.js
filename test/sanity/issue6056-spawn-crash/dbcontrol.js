//	Program:  5DBat -  by BlatSoft
//	04/2017	version: 0.1.0
//  dbcontrol.js
//  This file used to control all database functions
//  - Called from end of start.html/running.html

"use strict";

var dblite = require('dblite');

var dbPath = 'dogs.db';

// **********************  Define SQLite bin path  **************************
//dblite.bin = "node_modules/sqlite3/sqlite3"; // if not specified then default install will be used if exists (/usr/local/bin/sqlite3)
dblite.bin = "C:\\sqlite\\badnamesqlite3.exe"; // if not specified then default install will be used if exists (/usr/local/bin/sqlite3)
//dblite.bin = "data/bin/sqlite3"; // if not specified then default install will be used if exists (/usr/bin/sqlite3)


//######################################################################################################
//#     DB Connect (dblite)
//######################################################################################################
/**
 * Connect Database
 * @param dbPath
 *
 */
//var db = function connectDb(dbPath) {
    var db = dblite(dbPath);  // creates if no exist
