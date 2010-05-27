#include <stdio.h>
#include <string.h>
#include "stl_to_sql.h"



int main() {
  int failure=0, count=0;
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classH USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classF USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE,classH_id references classH)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classE USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classB USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE,classE_id references classE,classF_id references classF)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classG USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classD USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE,classG_id references classG)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classC USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE classA USING mod(INTEGER PRIMARY KEY AUTOINCREMENT,att1 INT,att2 TEXT,att3 DOUBLE,classC_id references classC,classD_id references classD)");
    if (!failure) count++;
  }
  if (!failure) {
    failure=register_table("foo.db","CREATE VIRTUAL TABLE test USING mod(nick_name TEXT,att1 INT,att2 TEXT,att3 DOUBLE,classA_id references classA,classB_id references classB)");
    if (!failure) count++;
  }
  if (failure) printf("\n\nERROR STATE. CANCELLING COMPLETED OPERATIONS...\n\n");
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classH");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classF");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classE");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classB");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classG");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classD");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classC");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE classA");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
  if (count>0) {
    failure=register_table("foo.db","DROP TABLE test");
    count--;
    if (failure) printf("\n\nFAILURE TO DROP TABLE\n\n");
  }
}
