/*  pico_ql_procfs.c -  create a "file" in /proc, which allows both input and *  output.
 */


//#include <config/modversions.h>   
#include <linux/module.h>   
#include <linux/version.h>

/* Necessary because we use proc fs */
#include <linux/proc_fs.h>

/* For using time */
#include <linux/time.h>

/* In 2.2.3 /usr/include/linux/version.h includes a 
 * macro for this, but 2.0.35 doesn't - so I add it 
 * here if necessary. */
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) ((a)*65536+(b)*256+(c))
#endif


#include <asm/uaccess.h>  /* for get_user and put_user */
#include <linux/sched.h>
#include <linux/nsproxy.h>
#include <linux/fs.h>
#include <net/net_namespace.h>
#include <linux/spinlock.h>
#include <unistd.h>
#include <stdio.h>
#include <sqlite3.h>
#include "pico_ql_search.h"
#include "pico_ql_vt.h"
#include "pico_ql_exec.h"

struct timespec picoQL_ts_start;

static int picoQL_processing = 0;
static int picoQL_res_avail = 0;
#define PICO_QL_BUSY picoQL_processing = 1
#define PICO_QL_AVAILABLE picoQL_processing = 0
#define PICO_QL_READY picoQL_processing == 0
#define PICO_QL_RS_AVAILABLE picoQL_res_avail = 1
#define PICO_QL_RS_NAVAILABLE picoQL_res_avail = 0
#define PICO_QL_RS_ACTIVE picoQL_res_avail == 1

sqlite3 *db;
sqlite3_module *mod;
static char QUERY_BUFFER[PICO_QL_RESULT_SET_SIZE];
static char **root_query_result_set;
static int query_result_set_partitions = 0;
static char *query_result_set;
static int current_partition = 0;

int place_result_set(const char **root_result_set, int *argc_slots) {
#ifdef PICO_QL_DEBUG
  printf("In place_result_set, result_set partitioned in %i pieces.", *argc_slots);
#endif
  root_query_result_set = (char **)root_result_set;
  query_result_set_partitions = *argc_slots;
  query_result_set = root_query_result_set[0];
  clear_temp_structs();
  PICO_QL_RS_AVAILABLE;
  return 0;
}


/* The module's functions ********************** */
/* Function to read picoQL virtual file from user space */
ssize_t picoQL_read(         struct file *f,
                             char *page,
                             size_t page_len,
                             loff_t *offset)
{
  int len = 0;
  
#ifdef PICO_QL_DEBUG
  printf("/proc/picoQL read initiated. File offset is %i, page_len is %i, PICO_QL_RS_ACTIVE %i, PICO_QL_READY %i.\n", (int)*offset, (int)page_len, PICO_QL_RS_ACTIVE, PICO_QL_READY);
#endif

  /* We return 0 to indicate end of file, that we
   * have no more information. Otherwise, 
   * processes will
   * continue to read from us in an endless loop. 
   * Also we return -EBUSY if a call for reading 
   * a result set is submitted while the engine is 
   * processing.
   */
  if (!PICO_QL_RS_ACTIVE) {
    if (PICO_QL_READY)
      return 0;
    else
      return -EBUSY;
  }

  if (current_partition < query_result_set_partitions) {
    len = sprintf(page, "%s", query_result_set);
    current_partition++;
    query_result_set = root_query_result_set[current_partition];
    if (current_partition == query_result_set_partitions) {
      for(current_partition = 0; current_partition < query_result_set_partitions; current_partition++)
        sqlite3_free(root_query_result_set[current_partition]);
      sqlite3_free(root_query_result_set);
      query_result_set = NULL;
      query_result_set_partitions = 0;
      current_partition = 0;
      PICO_QL_RS_NAVAILABLE;
      PICO_QL_AVAILABLE;
#ifdef PICO_QL_DEBUG
      printf("Consumed query partition %i. Another %i to go. PICO_QL_RS_ACTIVE is %i, PICO_QL_READY is %i.\n", current_partition, query_result_set_partitions, PICO_QL_RS_ACTIVE, PICO_QL_READY);
#endif
    }
  }
  // Perhaps check malloc/free sync. (SMP?)
  return len;  /* Return the number of bytes "read" */
}


/* This function receives input from the user when the
 * user writes to the /proc file. */
ssize_t picoQL_write(
                              struct file *file,
                              const char __user *buf,
                              size_t len,
                              loff_t *offset)
{
  int j = 0, rc;

#ifdef PICO_QL_DEBUG
  printf("/proc/picoQL write %s of length %li.\n", buf, len);
#endif

  if (!PICO_QL_READY)
    return -EAGAIN;

  if (len > PICO_QL_RESULT_SET_SIZE) {
    printk(KERN_ERR "Query bigger than page size.\n");
    return -ENOSPC;
  }

  if (copy_from_user(QUERY_BUFFER, buf, len)) {
    return -EFAULT;
  }

  PICO_QL_BUSY;

  getnstimeofday(&picoQL_ts_start);
  getrawmonotonic(&picoQL_ts_start);

/* j for debugging, execute the query multiple times. */
  while (j < 1 && (rc = prep_exec(db, QUERY_BUFFER)) == SQLITE_DONE){
    j++;
  }
  return len;
}

/* Structures to register as the /proc file, with
 * pointers to all the relevant functions. ********** */

/* Directory entry */
static struct proc_dir_entry *PicoQL_Proc_File;

/* Module initialization and cleanup ******************* */

int init_sqlite3(void) {
  int output, re;
  re = sqlite3_open(":memory:", &db);

  if (re) {
    printk(KERN_ERR "can't open database\n");
    sqlite3_close(db);
    return -ECANCELED;
  }
#ifdef PICO_QL_DEBUG
  printf("SQLite in-memory database %lx opened successfully.\n", (long)db);
#endif
  mod = (sqlite3_module *)sqlite3_malloc(sizeof(sqlite3_module));
  fill_module(mod);
  output = sqlite3_create_module(db, "PicoQL", mod, NULL);
  if (output == 1) {
    printk(KERN_ERR "Error while registering module.");
    return -ECANCELED;
  }
#ifdef PICO_QL_DEBUG
  else if (output == 0)
    printf("Module registered successfully.");
#endif
  pico_ql_register(&init_task, "processes");
  pico_ql_register(&init_task.nsproxy, "namespace_proxy");
  pico_ql_register(&net_namespace_list, "network_namespaces");
  pico_ql_register(&super_blocks, "superblock_list");
  output = pico_ql_serve(db);
  if (output != SQLITE_DONE) {
    printk(KERN_ERR "Serve failed with error code %i.\n", output);
    return -ECANCELED;
  } else {
    start_serving();
#ifdef PICO_QL_DEBUG
    printf("Serve succeeded.\n");
#endif
  }
  return 0;
}

static int picoQL_copen = 0;
#define PICO_QL_OPEN picoQL_copen == 1
int picoQL_output = 10;
int picoQL_metadata = 0;

int picoQL_open(struct inode *i, struct file *f) {
  (void)i;
  (void)f;
#ifdef PICO_QL_DEBUG
  printf("Opening /proc/picoQL. Was open? %i\n", PICO_QL_OPEN); 
#endif
  if (PICO_QL_OPEN) 
    return -EBUSY;
  picoQL_copen = 1;
  return 0;
}

long picoQL_ioctl(struct file *f,
                 unsigned int picoQL_code, 
                 unsigned long picoQL_parameter) {

  (void)f;
  (void)picoQL_parameter;
#ifdef PICO_QL_DEBUG
  printf("ioctl being configured with code %i.\n", picoQL_code);
#endif
  switch(picoQL_code) {
  case 100:
    picoQL_metadata = 0;
    break;
  case 111:
    picoQL_metadata = 1;
    break;
  case 10:
    picoQL_output = 10;
    break;
  case 11:
    picoQL_output = 11;
    break;
  }
  return 0;
}

int picoQL_release(struct inode *i, struct file *f) {
  (void)i;
  (void)f;
#ifdef PICO_QL_DEBUG
  printf("Releasing /proc/picoQL. Was open? %i\n", PICO_QL_OPEN); 
#endif
  if (PICO_QL_OPEN)
    picoQL_copen = 0;
  else
    return -EPERM;
  return 0;
}

int picoQL_permission (struct inode *i, int mask) {
#ifdef PICO_QL_DEBUG
  printf("Requesting permission to /proc/picoQL for operation %i.\n", mask);
#endif
  if ((mask & MAY_READ) || (mask & MAY_WRITE))
    return 0;
  printf("Access to /proc/picoQL denied.\n");
  return -EACCES;
}

struct inode_operations picoQL_iops = {
  .permission = picoQL_permission,
};

struct file_operations picoQL_fops = {
  .open = picoQL_open,
  .unlocked_ioctl = picoQL_ioctl,
  .read = picoQL_read,
  .write = picoQL_write,
  .release = picoQL_release,
};

/* Initialize the module - register the proc file */
int init_module()
{
  int re = init_sqlite3();
  PicoQL_Proc_File = create_proc_entry("picoQL", 0644, NULL);
  if (!PicoQL_Proc_File) {
    remove_proc_entry("picoQL", NULL);
    return -ECANCELED;
  }
  PicoQL_Proc_File->proc_iops = &picoQL_iops;
  PicoQL_Proc_File->proc_fops = &picoQL_fops;
#ifdef PICO_QL_DEBUG
  printf("Created proc entry %s.\n", PicoQL_Proc_File->name);
#endif
  return re;
}

/* Cleanup - unregister our file from /proc */
void cleanup_module()
{
  deinit_selectors();
  sqlite3_close(db);
  sqlite3_free(mod);
  remove_proc_entry(PicoQL_Proc_File->name, NULL);
#ifdef PICO_QL_DEBUG
  printf("Cleaned up after picoQL module.\n");
#endif
}

MODULE_LICENSE("GPL");
