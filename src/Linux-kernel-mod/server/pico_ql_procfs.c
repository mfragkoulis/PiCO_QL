/*  pico_ql_procfs.c -  create a "file" in /proc, which allows both input and *  output.
 */


//#include <config/modversions.h>   
#include <linux/init.h>
#include <linux/module.h>   
#include <linux/version.h>

MODULE_AUTHOR("Marios Fragkoulis");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A relational interface to selected kernel data structures.");

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
#include <linux/slab.h> /* struct kmem_cache, kmalloc_caches */
#include <linux/mmzone.h> /* sysctl_lowmem_reserve_ratio */
#include <linux/nsproxy.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <net/net_namespace.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <net/sock.h> /* struct sock */
#include <linux/skbuff.h>
#include <linux/net.h>
#include <xen/balloon.h>

#include <unistd.h>
#include <stdlib.h> /* for calculating memory footprint */
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

#define EFile_VT_decl(X) struct file *X; int bit = 0
#define EFile_VT_begin(X, Y, Z) (X) = (Y)[(Z)]
#define EFile_VT_advance(X, Y, Z) EFile_VT_begin(X,Y,Z)


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
  if (!root_query_result_set) {
    printf("[picoQL] [place_result_set()] Error: out of memory (root_query_result_set).");
    query_result_set_partitions = -ENOMEM;
  } else {
    for (current_partition = 0; current_partition < query_result_set_partitions; current_partition++) {
      if (!root_query_result_set[current_partition]) {
        printf("[picoQL] [place_result_set()] Error: out of memory (query_result_set).");
        query_result_set_partitions = -ENOMEM;
      }
    }
    current_partition = 0;
    if (query_result_set_partitions != -ENOMEM)
      query_result_set = root_query_result_set[0];
  }
  printf("PiCO QL's peak memory footprint for this query is %lu.\n", memMaxFootprint);
  clear_temp_structs();
  PICO_QL_RS_AVAILABLE;
  return 0;
}


/* The module's functions ********************** */
/* Function to read picoQL virtual file from user space:
 * cat /proc/picoQL
 */
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

  if (query_result_set_partitions == -ENOMEM) {
    len = -ENOMEM;
    goto exit;
  }

  if (current_partition < query_result_set_partitions) {
    len = sprintf(page, "%s", query_result_set);
#ifdef PICO_QL_DEBUG
      printf("Consumed query partition %i of total %i. PICO_QL_RS_ACTIVE is %i, PICO_QL_READY is %i.\n", 
             current_partition, query_result_set_partitions, PICO_QL_RS_ACTIVE, PICO_QL_READY);
#endif
    current_partition++;
    /* The last seat (current_partition == q_r_s_p) is always empty. */
    query_result_set = root_query_result_set[current_partition];
    if (current_partition == query_result_set_partitions) {
exit:
      for(current_partition = 0; current_partition < query_result_set_partitions; current_partition++)
        sqlite3_free(root_query_result_set[current_partition]);
      sqlite3_free(root_query_result_set);
      query_result_set = NULL;
      query_result_set_partitions = 0;
      current_partition = 0;
      PICO_QL_RS_NAVAILABLE;
      PICO_QL_AVAILABLE;
    }
  }
  // Perhaps check malloc/free sync. (SMP?)
  return len;  /* Return the number of bytes "read" */
}


/* This function receives input from the user when the
 * user writes to the /proc file:
 * echo "SELECT...;" > /proc/picoQL
 */
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

  /* Zero memory footprint counter and max. */
  memMaxFootprint = 0;
  memFootprint = 0;

  PICO_QL_BUSY;

  getnstimeofday(&picoQL_ts_start);
  getrawmonotonic(&picoQL_ts_start);

/* j for debugging, execute the query multiple times. */
  while (j < 1 && (rc = prep_exec(db, QUERY_BUFFER)) == SQLITE_DONE){
    j++;
  }

  switch (rc) {
  case SQLITE_OK:
  case SQLITE_DONE:
    return len;
    break;
  case SQLITE_NOMEM:
    return -ENOMEM;
    break;
  default:
    /* Map the error code you see (include/uapi/asm-generic/errno-base.h)
     * to the SQLite error code that rc carries. 
     */
    return rc;
    break;
  }
}

/* Structures to register as the /proc file, with
 * pointers to all the relevant functions. ********** */

/* Directory entry */
static struct proc_dir_entry *PicoQL_Proc_File;
  
/* Temp solution for testing. */
static int mysysctl_lowmem_reserve_ratio[MAX_NR_ZONES-1] = {256, 256, 32};

/* Module initialization and cleanup ******************* */

int init_sqlite3(void) {
  int output, re;
  struct task_struct *iter;
  struct file *iterf;
  int bit = 0;
  struct super_block *sb = NULL;

  struct kset *ks;
  int i;
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
  if (!mod)
    return -ENOMEM;
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
  pico_ql_register(mysysctl_lowmem_reserve_ratio, "sysctl_lowmem");
  for (i = KMALLOC_SHIFT_LOW; i <= KMALLOC_SHIFT_HIGH; i++) {
    if (kmalloc_caches[i]) {
      //printf("kmalloc_caches[%d] is: %lx", i, (long)kmalloc_caches);
      pico_ql_register(kmalloc_caches[i], "kmem_caches");
      break;
    }
  }
  pico_ql_register(&init_task.nsproxy, "namespace_proxy");
  pico_ql_register(&net_namespace_list, "network_namespaces");
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,4)
  pico_ql_register(hypervisor_kobj, "sysfs_hypervisor_kobject");
#endif
  ks = init_net.loopback_dev->queues_kset;
  pico_ql_register(ks, "net_queues_kset");
  pico_ql_register(&pci_bus_type, "pci_bus");
  rcu_read_lock();
  list_for_each_entry_rcu(iter, &init_task.tasks, tasks) {
    if (iter && iter->files && iter->files->fdt) {
    for (EFile_VT_begin(iterf, iter->files->fdt->fd, (bit = find_first_bit((unsigned long *)iter->files->fdt->open_fds, iter->files->fdt->max_fds))); 
         bit < iter->files->fdt->max_fds; 
         EFile_VT_advance(iterf, iter->files->fdt->fd, (bit = find_next_bit((unsigned long *)iter->files->fdt->open_fds, iter->files->fdt->max_fds, bit + 1)))) {
      if (iterf != NULL) {
        sb = iterf->f_path.dentry->d_inode->i_sb;
        break;
      }
    }
  }
 }
  rcu_read_unlock();
  if (sb == NULL) return -ECANCELED;
  pico_ql_register(sb, "superblock");
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,4)
  pico_ql_register(&balloon_stats, "xen_balloon_stats");
#endif
  pico_ql_register((first_online_pgdat())->node_zones, "mem_zones");
  output = pico_ql_serve(db);
  if (output != SQLITE_DONE) {
    printk(KERN_ERR "Serve failed with error code %i.\n", output);
    return -ECANCELED;
  } else {
    start_serving();
    printf("picoQL service initiated.\n");
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
  if (re) return -ECANCELED;
  /* substitute 0664 with 0660 to provide access only to the owner
   * and the owner's group. */
//  PicoQL_Proc_File = create_proc_entry("picoQL", 0664, NULL);
  PicoQL_Proc_File = proc_create("picoQL", 0664, NULL, &picoQL_fops);
  if (!PicoQL_Proc_File) {
    remove_proc_entry("picoQL", NULL);
    return -ECANCELED;
  }
//  PicoQL_Proc_File->proc_iops = &picoQL_iops;
//  PicoQL_Proc_File->proc_fops = &picoQL_fops;
#ifdef PICO_QL_DEBUG
  printf("Created proc entry %s.\n", "picoQL");
#endif
  return re;
}

/* Cleanup - unregister our file from /proc */
void cleanup_module()
{
  deinit_selectors();
  sqlite3_close(db);
  sqlite3_free(mod);
  remove_proc_entry("picoQL", NULL);
  printf("Removed picoQL module.\n");
}

