#define __NO_VERSION__      
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/pagemap.h>
#include <linux/fs_struct.h>
#include <linux/module.h> // struct module
#include <linux/mm_types.h>
#include <linux/mmzone.h>
#include <linux/nsproxy.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <net/netns/mib.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <linux/snmp.h>
#include <net/sock.h>
#include <linux/net.h>
#include <net/ip_vs.h>
#include <net/net_namespace.h>
#include <linux/skbuff.h>   // Network data
#include <xen/balloon.h>    /* Xen balloon stats */
#include <asm/virtext.h>    /* cpu_has_vmx(), cpu_has svm() */
#if KVM_RUNNING
#include <linux/kvm_host.h>
#endif KVM_RUNNING

#define EKmemCacheSlabObject_VT_decl(X) void *X
#define EKmemCacheSlabFull_VT_decl(X) struct page *X
#define EKmemCacheNode_VT_decl(X) struct kmem_cache_node *X;int node = 0
#define EKmemCacheNode_VT_begin(X, Y, Z) (X) = (Y)[(Z)]
#define EKmemCacheNode_VT_advance(X, Y, Z) EKmemCacheNode_VT_begin(X, Y, Z)
#define KmemCache_VT_decl(X) struct kmem_cache *X
#define VirtualMemZone_VT_decl(X) struct zone *X
#define SysctlLowmemRR_VT_decl(X) int X
#define ELowmemReserve_VT_decl(X) int X
#define EIpVsStatsEstim_VT_decl(X) struct ip_vs_estimator *X
#define Process_VT_decl(X) struct task_struct *X
#define EProcessChild_VT_decl(X) struct task_struct *X
#define EThread_VT_decl(X) struct task_struct *X
#define EFile_VT_decl(X) struct file *X = NULL; int bit = 0
#define EFile_VT_begin(X, Y, Z) (X) = (Y)[(Z)]
#define EFile_VT_advance(X, Y, Z) EFile_VT_begin(X, Y, Z)
#define EGroup_VT_decl(X) int *X; int i = 0
#define EGroup_VT_begin(X, Y, Z) (X) = (int *)&(Y)[(Z)]
#define EGroup_VT_advance(X, Y, Z) EGroup_VT_begin(X, Y, Z)
#define NetNamespace_VT_decl(X) struct net *X
#define ERcvQueue_VT_decl(X) struct sk_buff *X;struct sk_buff *next
#if KERNEL_VERSION > 2.6.32
#define ENetDevice_VT_decl(X) struct net_device *X
#else
#define ENetDevice_VT_decl(X) struct net_device *X; struct net_device *aux
#endif
#define Superblock_VT_decl(X) struct super_block *X
#define KobjectSet_VT_decl(X) struct kobject *X
#define EKobjectSet_VT_decl(X) struct kobject *X
#define EKobjectList_VT_decl(X) struct kobject *X
#if KVM_RUNNING
#define EKVMArchPitChannelState_VT_decl(X) struct kvm_pit_channel_state *X; int i = 0

struct fdtable * get_fdtable(struct files_struct *file_struct) {
  if (file_struct != NULL)
    return file_struct->fdt;
  else
    return NULL;
};

int get_max_fds(struct files_struct *file_struct) {
  struct fdtable *fdt;
  if ((fdt = get_fdtable(file_struct)) != NULL)
    return fdt->max_fds;
  else
    return -1;
};


int get_uid_val(kuid_t kval) {
#if KERNEL_VERSION > 3.8.1
  return kval.val;
#else
  return kval;
#endif
}

int get_gid_val(kgid_t kval) {
#if KERNEL_VERSION > 3.8.1
  return kval.val;
#else
  return kval;
#endif
}

/*
struct rq {
        raw_spinlock_t lock;

        unsigned int nr_running;
        #define CPU_LOAD_IDX_MAX 5
        unsigned long cpu_load[CPU_LOAD_IDX_MAX];
        unsigned long last_load_update_tick;
#ifdef CONFIG_NO_HZ
        u64 nohz_stamp;
        unsigned long nohz_flags;
#endif
        int skip_clock_update;

        struct load_weight load;
        unsigned long nr_load_updates;
        u64 nr_switches;

        struct cfs_rq cfs;
        struct rt_rq rt;

#ifdef CONFIG_FAIR_GROUP_SCHED
        struct list_head leaf_cfs_rq_list;
#ifdef CONFIG_SMP
        unsigned long h_load_throttle;
#endif
#endif

#ifdef CONFIG_RT_GROUP_SCHED
        struct list_head leaf_rt_rq_list;
#endif

        unsigned long nr_uninterruptible;

        struct task_struct *curr, *idle, *stop;
        unsigned long next_balance;
        struct mm_struct *prev_mm;

        u64 clock;
        u64 clock_task;

        atomic_t nr_iowait;

#ifdef CONFIG_SMP
        struct root_domain *rd;
        struct sched_domain *sd;

        unsigned long cpu_power;

        unsigned char idle_balance;
        int post_schedule;
        int active_balance;
        int push_cpu;
        struct cpu_stop_work active_balance_work;
        int cpu;
        int online;

        struct list_head cfs_tasks;

        u64 rt_avg;
        u64 age_stamp;
        u64 idle_stamp;
        u64 avg_idle;
#endif

#ifdef CONFIG_IRQ_TIME_ACCOUNTING
        u64 prev_irq_time;
#endif
#ifdef CONFIG_PARAVIRT
        u64 prev_steal_time;
#endif
#ifdef CONFIG_PARAVIRT_TIME_ACCOUNTING
        u64 prev_steal_time_rq;
#endif

        unsigned long calc_load_update;
        long calc_load_active;

#ifdef CONFIG_SCHED_HRTICK
#ifdef CONFIG_SMP
        int hrtick_csd_pending;
        struct call_single_data hrtick_csd;
#endif
        struct hrtimer hrtick_timer;
#endif

#ifdef CONFIG_SCHEDSTATS
        struct sched_info rq_sched_info;
        unsigned long long rq_cpu_time;

        unsigned int yld_count;

        unsigned int sched_count;
        unsigned int sched_goidle;

        unsigned int ttwu_count;
        unsigned int ttwu_local;
#endif

#ifdef CONFIG_SMP
        struct llist_head wake_list;
#endif

        struct sched_avg avg;
};
*/

struct kvm_timer {
        struct hrtimer timer;
        s64 period;                             /* unit: ns */
        u32 timer_mode_mask;
        u64 tscdeadline;
        atomic_t pending;                       /* accumulated triggered timers */
        bool reinject;
        struct kvm_timer_ops *t_ops;
        struct kvm *kvm;
        struct kvm_vcpu *vcpu;
};

struct kvm_kpit_channel_state {
        u32 count; /* can be 65536 */
        u16 latched_count;
        u8 count_latched;
        u8 status_latched;
        u8 status;
        u8 read_state;
        u8 write_state;
        u8 write_latch;
        u8 rw_mode;
        u8 mode;
        u8 bcd; /* not supported */
        u8 gate; /* timer start */
        ktime_t count_load_time;
};

struct kvm_kpit_state {
        struct kvm_kpit_channel_state channels[3];
        u32 flags;
        struct kvm_timer pit_timer;
        bool is_periodic;
        u32    speaker_data_on;
        struct mutex lock;
        struct kvm_pit *pit;
        spinlock_t inject_lock;
        unsigned long irq_ack;
        struct kvm_irq_ack_notifier irq_ack_notifier;
};

struct kvm_io_device {
        const struct kvm_io_device_ops *ops;
};

struct kvm_pit {
        struct kvm_io_device dev;
        struct kvm_io_device speaker_dev;
        struct kvm *kvm;
        struct kvm_kpit_state pit_state;
        int irq_source_id;
        struct kvm_irq_mask_notifier mask_notifier;
        struct workqueue_struct *wq;
        struct work_struct expired;
};
#endif KVM_RUNNING

/*
 * This is taken from fs/proc/array.c .
 *
 * The task state array is a strange "bitmap" of
 * reasons to sleep. Thus "running" is zero, and
 * you can test for combinations of others with
 * simple bit tests.
 */
#if KERNEL_VERSION <= 3.8.1
 const char * const task_state_array[] = {
        "R (running)",          /*   0 */
        "S (sleeping)",         /*   1 */
        "D (disk sleep)",       /*   2 */
        "T (stopped)",          /*   4 */
        "t (tracing stop)",     /*   8 */
        "Z (zombie)",           /*  16 */
        "X (dead)",             /*  32 */
        "x (dead)",             /*  64 */
        "K (wakekill)",         /* 128 */
        "W (waking)",           /* 256 */
};

/* 
 * This is taken from include/linux/sched.h .
 *
 * for get_task_state() 
 */
// Defined in 3.6.10, not defined in 2.6.0
#define TASK_REPORT             (TASK_RUNNING | TASK_INTERRUPTIBLE | \
                                 TASK_UNINTERRUPTIBLE | __TASK_STOPPED | \
                                 __TASK_TRACED)
#define TASK_STATE_MAX          512


/*
 * This is taken from fs/proc/array.c .
 *
 */
static const char *get_task_state(struct task_struct *tsk)
{
        unsigned int state = (tsk->state & TASK_REPORT) | tsk->exit_state;
        const char * const *p = &task_state_array[0];

        BUILD_BUG_ON(1 + ilog2(TASK_STATE_MAX) != ARRAY_SIZE(task_state_array));

        while (state) {
                p++;
                state >>= 1;
        }
        return *p;
};

#else

static const char * const task_state_array[] = {
        "R (running)",          /*   0 */
        "S (sleeping)",         /*   1 */
        "D (disk sleep)",       /*   2 */
        "T (stopped)",          /*   4 */
        "t (tracing stop)",     /*   8 */
        "Z (zombie)",           /*  16 */
        "X (dead)",             /*  32 */
};

static inline const char *get_task_state(struct task_struct *tsk)
{
        unsigned int state = (tsk->state | tsk->exit_state) & TASK_REPORT;

        BUILD_BUG_ON(1 + ilog2(TASK_REPORT) != ARRAY_SIZE(task_state_array)-1);

        return task_state_array[fls(state)];
};

#endif

char rem_ip[23];
char local_ip[23];
char * convert_ip(long ip_host_fmt, char *ip) {
  long ip_ntw_fmt = htonl(ip_host_fmt);
  if (ip_host_fmt == 0) return (ip = "0");
  sprintf(ip, "%ld.%ld.%ld.%ld", (ip_ntw_fmt >> 24) & 255, (ip_ntw_fmt >> 16) & 255, (ip_ntw_fmt >> 8) & 255, ip_ntw_fmt & 255);
  return ip;
};

long get_tx_queue(struct sock *s) {
  struct tcp_sock *tp = tcp_sk(s);
  return (tp->write_seq - tp->snd_una);
};

long get_rx_queue(struct sock *s) {
  struct tcp_sock *tp = tcp_sk(s);
  int rx_queue;
  if (s->sk_state == TCP_LISTEN)
    rx_queue = s->sk_ack_backlog;
  else
  /*
   * because we dont lock socket, we might find a transient negative value
   */
    rx_queue = max_t(int, tp->rcv_nxt - tp->copied_seq, 0);
  return rx_queue;
};

char dpath[128];
char * checked_d_path(const struct path *path, char *buf, int buflen) {
  if (path == NULL) {
    buf = "INVALID_P";
    return buf;
  } else 
    return d_path(path, buf, buflen);
};

long get_file_offset(struct file *f) {
  long f_pos;
  f_pos = f->f_pos;
  return f_pos;
};

long get_page_offset(struct file *f) {
  long p_pos;
  p_pos = f->f_pos >> PAGE_CACHE_SHIFT;
  return p_pos;
};

long get_page_in_cache(struct file *f) {
  long page;
  page = (long)find_get_page(f->f_mapping, f->f_pos >> PAGE_CACHE_SHIFT);
  return page;
};

int get_pages_in_cache(struct address_space *a) {
  int pages;
  pages = a->nrpages;
  return pages;
};

unsigned get_pages_in_cache_contig(struct file *f, int current_offset) {
  pgoff_t index = 0;
  int malloc_pages = f->f_mapping->nrpages;  /* There will be at most so many */
  struct page **pages;                       /* contiguous as the total.      */
  unsigned int nr_pages;
  if (current_offset) {
    index = f->f_pos;
  }
  pages = (struct page **)kzalloc(malloc_pages * sizeof(struct page *), GFP_KERNEL);
  nr_pages = find_get_pages_contig(f->f_mapping, index, malloc_pages, pages);
  kfree(pages);
  return nr_pages;
};

unsigned get_pages_in_cache_tag(struct file *f, pgoff_t index, int tag) {
  int malloc_pages = f->f_mapping->nrpages;  /* There will be at most so many */
  struct page **pages;                       /* tagged as the total.          */
  unsigned int nr_pages;
  pages = (struct page **)kzalloc(malloc_pages * sizeof(struct page *), GFP_KERNEL);
  nr_pages = find_get_pages_tag(f->f_mapping, &index, tag, malloc_pages, pages);
  kfree(pages);
  return nr_pages;
};

/*
 * The slab lists for all objects.
 */
struct kmem_cache_node {
        spinlock_t list_lock;

#ifdef CONFIG_SLAB
        struct list_head slabs_partial; /* partial list first, better asm code */
        struct list_head slabs_full;
        struct list_head slabs_free;
        unsigned long free_objects;
        unsigned int free_limit;
        unsigned int colour_next;       /* Per-node cache coloring */
        struct array_cache *shared;     /* shared per node */
        struct array_cache **alien;     /* on other nodes */
        unsigned long next_reap;        /* updated without locking */
        int free_touched;               /* updated without locking */
#endif

#ifdef CONFIG_SLUB
        unsigned long nr_partial;
        struct list_head partial;
#ifdef CONFIG_SLUB_DEBUG
        atomic_long_t nr_slabs;
        atomic_long_t total_objects;
        struct list_head full;
#endif
#endif

};

#define for_each_object(__p, __s, __addr, __objects) \
        for (__p = (__addr); __p < (__addr) + (__objects) * (__s)->size;\
                        __p += (__s)->size)

long get_it(void *it) {
  return (long)it;
};

static char * const n_a = "N/A";

/* mm/page_alloc.c */
static char * const zone_names[] = {
#ifdef CONFIG_ZONE_DMA
         "DMA",
#endif
#ifdef CONFIG_ZONE_DMA32
         "DMA32",
#endif
         "Normal",
#ifdef CONFIG_HIGHMEM
         "HighMem",
#endif
         "Movable",
};

char * retrieve_zone_name(int zone) {
  switch (zone) {
#ifdef CONFIG_ZONE_DMA
  case ZONE_DMA:
    return zone_names[ZONE_DMA];
#endif
#ifdef CONFIG_ZONE_DMA32
  case ZONE_DMA32:
    return zone_names[ZONE_DMA32];
#endif
  case ZONE_NORMAL:
    return zone_names[ZONE_NORMAL];
#ifdef CONFIG_HIGHMEM
  case ZONE_HIGHMEM:
    return zone_names[ZONE_HIGHMEM];
#endif
  case ZONE_MOVABLE:
    return zone_names[ZONE_MOVABLE];
  }
  return n_a;
};

int retrieve_lowmem(int *lrr, int zone) {
  if (!lrr) return -1;
  switch (zone) {
#ifdef CONFIG_ZONE_DMA
  case ZONE_DMA:
    return lrr[ZONE_DMA];
#endif
#ifdef CONFIG_ZONE_DMA32
  case ZONE_DMA32:
    return lrr[ZONE_DMA32];
#endif
  case ZONE_NORMAL:
    return lrr[ZONE_NORMAL];
#ifdef CONFIG_HIGHMEM
  case ZONE_HIGHMEM:
    return lrr[ZONE_HIGHMEM];
#endif
  }
  return -1;
};

int retrieve_priority(struct zone *z) {
  if (!z) return -1;
  if (!strcmp(z->name, "DMA")) {
#ifdef CONFIG_ZONE_DMA
    return ZONE_DMA;
#endif
  } else if (!strcmp(z->name, "DMA32")) {
#ifdef CONFIG_ZONE_DMA32
    return ZONE_DMA32;
#endif
  } else if (!strcmp(z->name, "Normal")) {
    return ZONE_NORMAL;
  } else if (!strcmp(z->name, "Highmem")) {
#ifdef CONFIG_HIGHMEM
    return ZONE_HIGHMEM;
#endif
  } else if (!strcmp(z->name, "Movable")) {
    return ZONE_MOVABLE;
  }
  return -1;
};

long retrieve_protection(struct zone *z, int zone) {
  if (!z) return -1;
  switch (zone) {
#ifdef CONFIG_ZONE_DMA
  case ZONE_DMA:
    return z->lowmem_reserve[ZONE_DMA];
#endif
#ifdef CONFIG_ZONE_DMA32
  case ZONE_DMA32:
    return z->lowmem_reserve[ZONE_DMA32];
#endif
  case ZONE_NORMAL:
    return z->lowmem_reserve[ZONE_NORMAL];
#ifdef CONFIG_HIGHMEM
  case ZONE_HIGHMEM:
    return z->lowmem_reserve[ZONE_HIGHMEM];
#endif
  case ZONE_MOVABLE:
    return z->lowmem_reserve[ZONE_MOVABLE];
  }
  return -1;
};

/* mm/mmzone.c 
 * first_online_pgdat, next_online_pgdat, next_zone
 */
struct pglist_data *first_online_pgdat(void)
{
        return NODE_DATA(first_online_node);
};

struct pglist_data *next_online_pgdat(struct pglist_data *pgdat)
{
        int nid = next_online_node(pgdat->node_id);

        if (nid == MAX_NUMNODES)
                return NULL;
        return NODE_DATA(nid);
}

/*
 * next_zone - helper magic for for_each_zone()
 */
struct zone *next_zone(struct zone *zone)
{
        pg_data_t *pgdat = zone->zone_pgdat;

        if (zone < pgdat->node_zones + MAX_NR_ZONES - 1)
                zone++;
        else {
                pgdat = next_online_pgdat(pgdat);
                if (pgdat)
                        zone = pgdat->node_zones;
                else
                        zone = NULL;
        }
        return zone;
};


long is_socket(struct file *f) {
  if (S_ISSOCK(f->f_path.dentry->d_inode->i_mode))
    return (long)f->private_data;
  return 0;
};

int check_vmx(void *dummy) {
  (void)dummy;
  return cpu_has_vmx();
};

char *check_svm(void *dummy, char *msg) {
  (void)dummy;
  if (cpu_has_svm((const char **)&msg))
    sprintf(msg, "1");
  return msg;
};

#if KVM_RUNNING
int is_kvm_file(struct file *f) {
  if ((f) && (f->f_cred)) {
    if ((get_uid_val(f->f_cred->uid) == 107) && 
       (get_gid_val(f->f_cred->gid) == 107) && 
       (!strcmp(f->f_path.dentry->d_name.name, "kvm-vm")))
      return 1;
  }
  return 0;
};

long check_kvm(struct file *f) {
  if ((f) && (f->f_cred)) {
    if ((get_uid_val(f->f_cred->uid) == 107) && 
        (get_gid_val(f->f_cred->gid) == 107) && 
        (!strcmp(f->f_path.dentry->d_name.name, "kvm-vm")))
      return (long)f->private_data;
  }
  return 0;
};

int is_kvm_vcpu_file(struct file *f) {
  if ((f) && (f->f_cred)) {
    if ((get_uid_val(f->f_cred->uid) == 107) && 
       (get_gid_val(f->f_cred->gid) == 107) &&
       (!strcmp(f->f_path.dentry->d_name.name, "kvm-vcpu")))
      return 1;
  }
  return 0;
};

long check_kvm_vcpu(struct file *f) {
  if ((f) && (f->f_cred)) {
    if ((get_uid_val(f->f_cred->uid) == 107) && 
       (get_gid_val(f->f_cred->gid) == 107) &&
       (!strcmp(f->f_path.dentry->d_name.name, "kvm-vcpu")))
      return (long)f->private_data;
  }
  return 0;
};

int get_cpl(struct kvm_vcpu *vcpu) {
  return kvm_x86_ops->get_cpl(vcpu);
};

int check_cpl(struct kvm_vcpu *vcpu) {
  return (kvm_x86_ops->get_cpl(vcpu) == 0);
};


/* global; hope this is not a problem
 * wrt frame size.
 */
struct kvm_vcpu_events vcpu_events;
/* Clone of the function sharing the same name in 
 * arch/x86/kvm/x86.c as implemented in 2.6.33.
 * As opposed to the 3.2.37 implementation
 * member initialization is not complete.
 * Also includes non exported process_nmi() implemented in
 * the same source file (3.2.37).
 * We are emulating an ioctl for KVM_GET_VCPU_EVENTS in fact.
 */
long kvm_vcpu_ioctl_x86_get_vcpu_events(struct kvm_vcpu *vcpu) {
  /* process_nmi() inline */
  unsigned limit = 2;

  /*
   * x86 is limited to one NMI running, and one NMI pending after it.
   * If an NMI is already in progress, limit further NMIs to just one.
   * Otherwise, allow two (and we'll inject the first one immediately).
   */
  if (kvm_x86_ops->get_nmi_mask(vcpu) || vcpu->arch.nmi_injected)
    limit = 1;

  vcpu->arch.nmi_pending += atomic_xchg(&vcpu->arch.nmi_queued, 0);
  vcpu->arch.nmi_pending = min(vcpu->arch.nmi_pending, limit);
  kvm_make_request(KVM_REQ_EVENT, vcpu);

  /* End of process_nmi(vcpu); */

  vcpu_events.exception.injected = vcpu->arch.exception.pending;
  vcpu_events.exception.nr = vcpu->arch.exception.nr;
  vcpu_events.exception.has_error_code = vcpu->arch.exception.has_error_code;
  vcpu_events.exception.error_code = vcpu->arch.exception.error_code;

  vcpu_events.interrupt.injected = vcpu->arch.interrupt.pending;
  vcpu_events.interrupt.nr = vcpu->arch.interrupt.nr;
  vcpu_events.interrupt.soft = vcpu->arch.interrupt.soft;

  vcpu_events.nmi.injected = vcpu->arch.nmi_injected;
  vcpu_events.nmi.pending = vcpu->arch.nmi_pending;
  vcpu_events.nmi.masked = kvm_x86_ops->get_nmi_mask(vcpu);

#if KERNEL_VERSION <= 3.8.1
  vcpu_events.sipi_vector = vcpu->arch.sipi_vector;
#endif

  vcpu_events.flags = (KVM_VCPUEVENT_VALID_NMI_PENDING
                         | KVM_VCPUEVENT_VALID_SIPI_VECTOR);

  return (long)&vcpu_events;
};

/* global; hope this is not a problem
 * wrt frame size.
 */
struct kvm_pit_state pit_state;
/* Emulating an ioctl for KVM_GET_PIT in fact.
 * Kernel code in arch/x86/kvm/x86.c -
 * kvm_vm_ioctl_get_pit(); it's implementation 
 * is the same in 2.6.33 and 3.2.37 .
 */
long mem_copy_pit_state(struct kvm_kpit_state *kpit_state) {
  memcpy(&pit_state, kpit_state, sizeof(struct kvm_pit_state));
  return (long)&pit_state;
};
#endif KVM_RUNNING

unsigned long flags;
$

// 2.6.32.38 - atomic_t
CREATE STRUCT VIEW VirtualMemRegion_SV (
#if KERNEL_VERSION > 2.6.32
       vm_usage INT FROM vm_usage
#else
       vm_usage INT FROM vm_usage.counter
#endif
)
$

CREATE VIRTUAL TABLE EVirtualMemRegion_VT
USING STRUCT VIEW VirtualMemRegion_SV
WITH REGISTERED C TYPE struct vm_region
$

CREATE STRUCT VIEW VirtualMemArea_SV (
       vm_start BIGINT FROM vm_start,
       vm_end BIGINT FROM vm_end,
//       FOREIGN KEY(vm_id) FROM vm_mm REFERENCES EVirtualMem_VT POINTER,
       FOREIGN KEY(vm_file_id) FROM vm_file REFERENCES EFile_VT POINTER,
       flags BIGINT FROM vm_flags,
//       FOREIGN KEY (vm_region_id) FROM vm_region REFERENCES EVirtualMemRegion_VT POINTER
//       vm_page_prot pgprot_t FROM vm_page_prot
)
$

CREATE VIRTUAL TABLE EVirtualMemArea_VT
USING STRUCT VIEW VirtualMemArea_SV
WITH REGISTERED C TYPE struct vm_area_struct
$

CREATE STRUCT VIEW VirtualMem_SV (
       FOREIGN KEY(mmap_id) FROM mmap REFERENCES EVirtualMemArea_VT POINTER,
       FOREIGN KEY(mmap_cache_id) FROM mmap_cache REFERENCES EVirtualMemArea_VT POINTER,
       mmap_base BIGINT FROM mmap_base,
       task_size UNSIGNED BIG INT FROM task_size,
#if KERNEL_VERSION <= 3.8.1
       cached_hole_size BIGINT FROM cached_hole_size,
       free_area_cache BIGINT FROM free_area_cache,
#endif
       pgd BIGINT FROM pgd->pgd,
       users BIGINT FROM mm_users.counter,
       counter INT FROM mm_count.counter,
       map_count INT FROM map_count,
       hiwater_rss BIGINT FROM hiwater_rss,
       hiwater_vm BIGINT FROM hiwater_vm,
       total_vm BIGINT FROM total_vm,
       locked_vm BIGINT FROM locked_vm,
#if KERNEL_VERSION > 2.6.32
       pinned_vm BIGINT FROM pinned_vm,
#endif
       shared_vm BIGINT FROM shared_vm,
       exec_vm BIGINT FROM exec_vm,
       stack_vm BIGINT FROM stack_vm,
//       reserved_vm BIGINT FROM reserved_vm,
       def_flags BIGINT FROM def_flags,
#if KERNEL_VERSION <= 3.8.1
       nr_ptes BIGINT FROM nr_ptes,
#else
       nr_ptes BIGINT FROM atomic_long_read(&tuple_iter->nr_ptes),
#endif
       start_code BIGINT FROM start_code,
       end_code BIGINT FROM end_code,
       start_data BIGINT FROM start_data,
       end_data BIGINT FROM end_data,
       start_brk BIGINT FROM start_brk,
       brk BIGINT FROM brk,
       start_stack BIGINT FROM start_stack,
       arg_start BIGINT FROM arg_start,
       arg_end BIGINT FROM arg_end,
       env_start BIGINT FROM env_start,
       env_end BIGINT FROM env_end,
/*       FOREIGN KEY(rss_stat_id) FROM rss_stat REFERENCES ,
         faultstamp INT FROM faultstamp,
         token_priority INT FROM token_priority,
         last_interval INT FROM last_interval,
*/
       flags BIGINT FROM flags,
/*     FOREIGN KEY(process_owner_id) FROM owner REFERENCES EProcess_VT POINTER, CONFIG_MM_OWNER 
       FOREIGN KEY(exe_file_id) FROM exe_file REFERENCES FILE POINTER,  
       num_exe_file_vmas BIGINT FROM num_exe_file_vmas, */
       locked INT FROM mmap_sem.count
)
$

CREATE VIRTUAL TABLE EVirtualMem_VT
USING STRUCT VIEW VirtualMem_SV
WITH REGISTERED C TYPE struct mm_struct$

CREATE STRUCT VIEW KmemCacheSlab_SV (
	objects INT FROM objects,
	inuse INT FROM inuse,
	active INT FROM active
)$

CREATE VIRTUAL TABLE EKmemCacheSlabFull_VT
USING STRUCT VIEW KmemCacheSlab_SV
WITH REGISTERED C TYPE struct kmem_cache_node:struct page *
USING LOOP list_for_each_entry(tuple_iter, &base->full, lru)$

CREATE STRUCT VIEW KMemCacheSlabObject_SV (
	addr BIGINT FROM get_it(tuple_iter)
)$

CREATE VIRTUAL TABLE EKmemCacheSlabObject_VT
USING STRUCT VIEW KMemCacheSlabObject_SV
WITH REGISTERED C TYPE struct page:void *
USING LOOP for_each_object(tuple_iter, base->slab_cache, page_address(base), base->objects)$

CREATE STRUCT VIEW KmemCacheNode_SV (
	nr_slabs BIGINT FROM atomic_long_read(&tuple_iter->nr_slabs),
	total_objects BIGINT FROM atomic_long_read(&tuple_iter->total_objects),
	nr_partial BIGINT FROM nr_partial,
	FOREIGN KEY(full_slabs_id) FROM tuple_iter REFERENCES EKmemCacheSlabFull_VT POINTER,
	FOREIGN KEY(partial_slabs_id) FROM tuple_iter REFERENCES EKmemCacheSlabPartial_VT
)$

CREATE VIRTUAL TABLE EKmemCacheNode_VT
USING STRUCT VIEW KmemCacheNode_SV
WITH REGISTERED C TYPE struct kmem_cache:struct kmem_cache_node *
USING LOOP for (EKmemCacheNode_VT_begin(tuple_iter, base->node, (node = first_online_node)); node < MAX_NUMNODES; EKmemCacheNode_VT_advance(tuple_iter, base->node, (node = next_online_node(node))))$

CREATE STRUCT VIEW KmemCache_SV (
	name TEXT FROM name,
	FOREIGN KEY(nodes_id) FROM tuple_iter REFERENCES EKmemCacheNode_VT POINTER,
	cache_cpu_id BIGINT FROM this_cpu_ptr(tuple_iter->cpu_slab)->tid,
	FOREIGN KEY(slab_alloc_id) FROM this_cpu_ptr(tuple_iter->cpu_slab)->page REFERENCES EKmemCacheSlabObject_VT POINTER
)$

CREATE VIRTUAL TABLE KmemCache_VT
USING STRUCT VIEW KmemCache_SV
WITH REGISTERED C NAME kmem_caches
WITH REGISTERED C TYPE struct kmem_cache
USING LOOP list_for_each_entry(tuple_iter, &base->list, list)$

CREATE STRUCT VIEW VirtualMemZone_SV (
	name TEXT FROM name,
	priority INT FROM retrieve_priority(tuple_iter),
	watermark_min BIGINT FROM watermark[WMARK_MIN],
	watermark_low BIGINT FROM watermark[WMARK_LOW],
	watermark_high BIGINT FROM watermark[WMARK_HIGH],
	free_pages BIGINT FROM atomic_long_read(&tuple_iter->vm_stat[NR_FREE_PAGES]
),
	spanned_pages BIGINT FROM spanned_pages,
	managed_pages BIGINT FROM managed_pages,
	FOREIGN KEY(lowmem_zone_protection_id) FROM tuple_iter REFERENCES ELowmemReserve_VT POINTER
)$

CREATE VIRTUAL TABLE VirtualMemZone_VT
USING STRUCT VIEW VirtualMemZone_SV
WITH REGISTERED C NAME mem_zones
WITH REGISTERED C TYPE struct zone
USING LOOP for_each_zone(tuple_iter)$

CREATE STRUCT VIEW LowmemReserve_SV (
        page_type TEXT FROM retrieve_zone_name(tuple_iter),
	page_type_priority INT FROM tuple_iter,
	lowmem_reserve INT FROM {retrieve_protection(base, tuple_iter)}
)$

CREATE VIRTUAL TABLE ELowmemReserve_VT
USING STRUCT VIEW LowmemReserve_SV
WITH REGISTERED C TYPE struct zone *:int
USING LOOP for (tuple_iter = 0; tuple_iter < MAX_NR_ZONES; tuple_iter++)$

CREATE STRUCT VIEW SysctlLowmemRR_SV (
        page_type TEXT FROM retrieve_zone_name(tuple_iter),
	page_type_priority INT FROM tuple_iter,
	lowmem_reserve INT FROM {retrieve_lowmem(pqlPub_sysctl_lowmem, tuple_iter)}
)$

CREATE VIRTUAL TABLE SysctlLowmemRR_VT
USING STRUCT VIEW SysctlLowmemRR_SV
WITH REGISTERED C NAME sysctl_lowmem
WITH REGISTERED C TYPE int *:int
USING LOOP for (tuple_iter = 0; tuple_iter < MAX_NR_ZONES-1; tuple_iter++)$

CREATE STRUCT VIEW NetDevice_SV (
	name TEXT FROM name,
	ifalias TEXT FROM ifalias,
	ifindex INT FROM ifindex,
	iflink INT FROM iflink,
	mem_end BIGINT FROM mem_end,
	mem_start BIGINT FROM mem_start,
	address BIGINT FROM base_addr,
	irq_no INT FROM irq,
	state BIGINT FROM state,
	active_features BIGINT FROM features,
#if KERNEL_VERSION > 2.6.32
	user_changeable_features BIGINT FROM hw_features,
	user_requested_features BIGINT FROM wanted_features,
#endif
	vlan_inheritable_features BIGINT FROM vlan_features,
	rx_packets BIGINT FROM stats.rx_packets,
	tx_packets BIGINT FROM stats.tx_packets,
	rx_bytes BIGINT FROM stats.rx_bytes,
	tx_bytes BIGINT FROM stats.tx_bytes,
	rx_errors BIGINT FROM stats.rx_errors,
	tx_errors BIGINT FROM stats.tx_errors,
	rx_dropped BIGINT FROM stats.rx_dropped,
	tx_dropped BIGINT FROM stats.tx_dropped,
	multicast BIGINT FROM stats.multicast,
	collisions BIGINT FROM stats.collisions,
	rx_length_errors BIGINT FROM stats.rx_length_errors,
	rx_over_errors BIGINT FROM stats.rx_over_errors,
	rx_crc_errors BIGINT FROM stats.rx_crc_errors,
	rx_frame_errors BIGINT FROM stats.rx_frame_errors,
	rx_fifo_errors BIGINT FROM stats.rx_fifo_errors,
	rx_missed_errors BIGINT FROM stats.rx_missed_errors,
	tx_aborted_errors BIGINT FROM stats.tx_aborted_errors,
	tx_carrier_errors BIGINT FROM stats.tx_carrier_errors,
	tx_fifo_errors BIGINT FROM stats.tx_fifo_errors,
	tx_heartbeat_errors BIGINT FROM stats.tx_heartbeat_errors,
	tx_window_errors BIGINT FROM stats.tx_window_errors,
	rx_compressed BIGINT FROM stats.rx_compressed,
	tx_compressed BIGINT FROM stats.tx_compressed,
	FOREIGN KEY(queues_kset_id) FROM queues_kset REFERENCES EKobjectSet_VT POINTER
)
$

CREATE VIRTUAL TABLE ENetDevice_VT
USING STRUCT VIEW NetDevice_SV
WITH REGISTERED C TYPE struct net:struct net_device *
#if KERNEL_VERSION > 2.6.32
USING LOOP for_each_netdev_rcu(base, tuple_iter)
#else
USING LOOP for_each_netdev_safe(base, tuple_iter, aux)
#endif
$

// 2.6.32.38 (void **) instead of (void *__percpu *)
CREATE STRUCT VIEW TcpStat_SV (
#if KERNEL_VERSION > 2.6.32
       RtoAlgorithm BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_RTOALGORITHM)},
       RtoMin BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_RTOMIN)},
       RtoMax BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_RTOMAX)},
       MaxConn BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_MAXCONN)},
       ActiveOpens BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_ACTIVEOPENS)},
       PassiveOpens BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_PASSIVEOPENS)},
       AttemptFails BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_ATTEMPTFAILS)},
       EstabResets BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_ESTABRESETS)},
       CurrEstab BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_CURRESTAB)},
       InSegs BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_INSEGS)},
       OutSegs BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_OUTSEGS)},
       RetransSegs BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_RETRANSSEGS)},
       InErrs BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_INERRS)},
       OutRsts BIGINT FROM {snmp_fold_field((void __percpu **) tuple_iter.mibs, TCP_MIB_OUTRSTS)}
#else
       RtoAlgorithm BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_RTOALGORITHM)},
       RtoMin BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_RTOMIN)},
       RtoMax BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_RTOMAX)},
       MaxConn BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_MAXCONN)},
       ActiveOpens BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_ACTIVEOPENS)},
       PassiveOpens BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_PASSIVEOPENS)},
       AttemptFails BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_ATTEMPTFAILS)},
       EstabResets BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_ESTABRESETS)},
       CurrEstab BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_CURRESTAB)},
       InSegs BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_INSEGS)},
       OutSegs BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_OUTSEGS)},
       RetransSegs BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_RETRANSSEGS)},
       InErrs BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_INERRS)},
       OutRsts BIGINT FROM {snmp_fold_field((void **) tuple_iter.mibs, TCP_MIB_OUTRSTS)}
#endif
)
$

CREATE VIRTUAL TABLE ETcpStat_VT
USING STRUCT VIEW TcpStat_SV
WITH REGISTERED C TYPE struct tcp_mib$

CREATE STRUCT VIEW NetMib_SV (
       FOREIGN KEY(tcp_stats_id) FROM tcp_statistics REFERENCES ETcpStat_VT
)
$

CREATE VIRTUAL TABLE ENetMib_VT
USING STRUCT VIEW NetMib_SV
WITH REGISTERED C TYPE struct netns_mib$

CREATE STRUCT VIEW Socket_SV (
       socket_state INT FROM state,
       socket_type INT FROM type,
       flags BIGINT FROM flags,
       FOREIGN KEY(file_id) FROM file REFERENCES EFile_VT POINTER,
       FOREIGN KEY(sock_id) FROM sk REFERENCES ESock_VT POINTER
)
$

CREATE VIRTUAL TABLE ESocket_VT
USING STRUCT VIEW Socket_SV
WITH REGISTERED C TYPE struct socket$      


CREATE STRUCT VIEW Sock_SV (
       proto_name TEXT FROM __sk_common.skc_prot->name,
       rem_ip TEXT FROM {convert_ip(tuple_iter->__sk_common.skc_daddr, rem_ip)},
       rem_port BIGINT FROM inet_sk(tuple_iter)->inet_dport,
       local_ip TEXT FROM {convert_ip(tuple_iter->__sk_common.skc_rcv_saddr, local_ip)},
       local_port BIGINT FROM inet_sk(tuple_iter)->inet_num,
       tx_queue BIGINT FROM get_tx_queue(tuple_iter),
       rx_queue BIGINT FROM get_rx_queue(tuple_iter),
       is_icsk INT FROM inet_sk(tuple_iter)->is_icsk,
       timestamp_last_rcv BIGINT FROM sk_stamp.tv64,
       drops INT FROM atomic_read(&tuple_iter.sk_drops),
       errors INT FROM sk_err,
       errors_soft INT FROM sk_err_soft,
       transmit_bytes_committed INT FROM sk_wmem_alloc.counter,
       persistent_queue_size INT FROM sk_wmem_queued,
       pending_writes INT FROM sk_write_pending,
       snd_buf_size INT FROM sk_sndbuf,
       rcv_buf_size INT FROM sk_rcvbuf,
       FOREIGN KEY(socket_id) FROM sk_socket REFERENCES ESocket_VT POINTER,
//       FOREIGN KEY(peer_process_id) FROM get_pid_task(tuple_iter->sk_peer_pid, PIDTYPE_PID) REFERENCES EProcess_VT POINTER
       FOREIGN KEY(receive_queue_id) FROM tuple_iter REFERENCES ERcvQueue_VT POINTER,
//       FOREIGN KEY(receive_queue_id) FROM tuple_iter REFERENCES EWriteQueue_VT POINTER
)
$

CREATE VIRTUAL TABLE ESock_VT
USING STRUCT VIEW Sock_SV
WITH REGISTERED C TYPE struct sock
$

CREATE STRUCT VIEW SkBuff_SV (
	len INT FROM len
)$

CREATE VIRTUAL TABLE ERcvQueue_VT
USING STRUCT VIEW SkBuff_SV
WITH REGISTERED C TYPE struct sock:struct sk_buff *
USING LOOP skb_queue_walk_safe(&base->sk_receive_queue, tuple_iter, next)
$

#if KERNEL_VERSION > 2.6.32
CREATE STRUCT VIEW IpVsStatsEstim_SV (
       last_inbytes BIGINT FROM last_inbytes,
       last_outbytes BIGINT FROM last_outbytes,
       last_conns INT FROM last_conns,
       last_inpkts INT FROM last_inpkts,
       last_outpkts INT FROM last_outpkts,
       cps INT FROM cps,
       inpps INT FROM inpps,
       outpps INT FROM outpps,
       inbps INT FROM inbps,
       outbps INT FROM outbps
)
$

CREATE VIRTUAL TABLE EIpVsStatsEstim_VT
USING STRUCT VIEW IpVsStatsEstim_SV
WITH REGISTERED C TYPE struct ip_vs_estimator *
USING LOOP list_for_each_entry_rcu(tuple_iter, &base->list, list)
$

// 2.6.32.38 - incomplete type
CREATE STRUCT VIEW NetnsIpvs_SV (
       FOREIGN KEY(ipvs_stats_estim_id) FROM tot_stats.est REFERENCES EIpVsStatsEstim_VT
)
$

CREATE VIRTUAL TABLE ENetnsIpvs_VT
USING STRUCT VIEW NetnsIpvs_SV
WITH REGISTERED C TYPE struct netns_ipvs$

CREATE STRUCT VIEW NetnsIpv4_SV (
       FOREIGN KEY(fib_sock_id) FROM fibnl REFERENCES ESock_VT POINTER
)
$

CREATE VIRTUAL TABLE ENetnsIpv4_VT
USING STRUCT VIEW NetnsIpv4_SV
WITH REGISTERED C TYPE struct netns_ipv4$
#endif

CREATE STRUCT VIEW NetNamespace_SV (
#if KERNEL_VERSION > 2.6.32
       count_to_free INT FROM passive.counter,
       FOREIGN KEY(nfnl_stash_sock_id) FROM nfnl_stash REFERENCES ESock_VT POINTER,
       FOREIGN KEY(netns_ipvs) FROM ipvs REFERENCES ENetnsIpvs_VT POINTER,
       FOREIGN KEY(netns_ipv4) FROM ipv4 REFERENCES ENetnsIpv4_VT,
#endif
       count_to_shut INT FROM count.counter,
//       use_count INT FROM use_count.counter,
       FOREIGN KEY(rtnl_sock_id) FROM rtnl REFERENCES ESock_VT POINTER,
       FOREIGN KEY(genl_sock_id) FROM genl_sock REFERENCES ESock_VT POINTER,
       FOREIGN KEY(dev_list_id) FROM tuple_iter REFERENCES ENetDevice_VT POINTER,
       FOREIGN KEY(netns_mib_id) FROM mib REFERENCES ENetMib_VT
)
$

CREATE VIRTUAL TABLE NetNamespace_VT
USING STRUCT VIEW NetNamespace_SV
WITH REGISTERED C NAME network_namespaces
WITH REGISTERED C TYPE struct list_head *:struct net *
USING LOOP list_for_each_entry_rcu(tuple_iter, base, list)
$

CREATE VIRTUAL TABLE ENetNamespace_VT
USING STRUCT VIEW NetNamespace_SV
WITH REGISTERED C TYPE struct net$

CREATE STRUCT VIEW Nsproxy_SV (
       count INT FROM count.counter,
/*      FOREIGN KEY(uts_ns_id) FROM uts_ns REFERENCES EUtsNs POINTER,
        FOREIGN KEY(ipc_ns_id) FROM ipc_ns REFERENCES EIpcNs POINTER,
        FOREIGN KEY(mount_ns_id) FROM mnt_ns REFERENCES EMountNs POINTER,
        FOREIGN KEY(pid_ns_id) FROM pid_ns REFERENCES EPidNs POINTER,
*/
       FOREIGN KEY(net_ns_id) FROM net_ns REFERENCES ENetNamespace_VT POINTER,
)
$

CREATE VIRTUAL TABLE Nsproxy_VT
USING STRUCT VIEW Nsproxy_SV
WITH REGISTERED C NAME namespace_proxy
WITH REGISTERED C TYPE struct nsproxy
$

CREATE STRUCT VIEW IO_SV (
       read_bytes_syscall BIGINT FROM rchar, //CONFIG_TASK_XACCT
       write_bytes_syscall BIGINT FROM wchar, //CONFIG_TASK_XACCT
       syscalls_read BIGINT FROM syscr, //CONFIG_TASK_XACCT
       syscalls_write BIGINT FROM syscw, //CONFIG_TASK_XACCT
       read_bytes_store BIGINT FROM read_bytes, //CONFIG_TASK_IO_ACCOUNTING
       write_bytes_store BIGINT FROM write_bytes, //CONFIG_TASK_IO_ACCOUNTING
       cancelled_write_bytes BIGINT FROM cancelled_write_bytes //CONFIG_TASK_IO_ACCOUNTING
)
$

CREATE VIRTUAL TABLE EIO_VT
USING STRUCT VIEW IO_SV
WITH REGISTERED C TYPE struct task_io_accounting
$

CREATE STRUCT VIEW ProcessSignal (
#if KERNEL_VERSION > 2.6.32
       signal_counter INT FROM sigcnt.counter,
       nr_threads INT FROM nr_threads,
#endif
       live INT FROM live.counter,
       group_exit_code INT FROM group_exit_code,
       notify_count INT FROM notify_count,
       group_stop_count INT FROM group_stop_count,
       session_leader INT FROM leader,
       utime BIGINT FROM utime,
       stime BIGINT FROM stime,
       gtime BIGINT FROM gtime,
       cgtime BIGINT FROM cgtime,
       cutime BIGINT FROM cutime,
       cstime BIGINT FROM cstime,
//CONFIG_VIRT_CPU_ACCOUNTING
//       prev_utime BIGINT FROM prev_utime, 
//CONFIG_VIRT_CPU_ACCOUNTING
//       prev_stime BIGINT FROM prev_stime, 
       nvcsw BIGINT FROM nvcsw,
       nivcsw BIGINT FROM nivcsw,
       cnvcsw BIGINT FROM cnvcsw,
       cnivcsw BIGINT FROM cnivcsw,
       min_flt BIGINT FROM min_flt,
       maj_flt BIGINT FROM maj_flt,
       cmin_flt BIGINT FROM cmin_flt,
       cmaj_flt BIGINT FROM cmaj_flt,
       maxrss BIGINT FROM maxrss,
       cmaxrss BIGINT FROM cmaxrss,
       FOREIGN KEY(io_id) FROM ioac REFERENCES EIO_VT,
       sum_sched_runtime BIGINT FROM sum_sched_runtime
)
$

CREATE VIRTUAL TABLE EProcessSignal
USING STRUCT VIEW ProcessSignal
WITH REGISTERED C TYPE struct signal_struct
$

CREATE STRUCT VIEW Bus_SV (
	name TEXT FROM name,
#if KERNEL_VERSION > 3.2.0
	dev_name TEXT FROM dev_name,
#endif
)
$

CREATE VIRTUAL TABLE Bus_VT
USING STRUCT VIEW Bus_SV
WITH REGISTERED C NAME pci_bus
WITH REGISTERED C TYPE struct bus_type
$

CREATE STRUCT VIEW Device_SV (
	name TEXT FROM init_name,
#if KERNEL_VERSION > 3.2.0
	id INT FROM id,
#endif
	dev BIGINT FROM devt
)
$

CREATE VIRTUAL TABLE EDevice_VT
USING STRUCT VIEW Device_SV
WITH REGISTERED C TYPE struct device
$


CREATE STRUCT VIEW Kobject_SV (
	name TEXT FROM name,
	container_ref INT FROM atomic_read(&tuple_iter->kref.refcount),
	FOREIGN KEY(kobject_list_id) FROM entry REFERENCES EKobjectList_VT,
	FOREIGN KEY(kobject_parent_id) FROM parent REFERENCES EKobject_VT POINTER,
	FOREIGN KEY(kset_id) FROM kset REFERENCES EKobjectSet_VT POINTER,
	state_initialized INT FROM state_initialized,
	state_in_sysfs INT FROM state_in_sysfs,
	state_add_uevent_sent INT FROM state_add_uevent_sent,
	state_remove_uevent_sent INT FROM state_remove_uevent_sent,
	uevent_suppress INT FROM uevent_suppress
)
$

CREATE VIRTUAL TABLE EKobject_VT
USING STRUCT VIEW Kobject_SV
WITH REGISTERED C TYPE struct kobject
$
	
CREATE VIRTUAL TABLE EKobjectList_VT
USING STRUCT VIEW Kobject_SV
WITH REGISTERED C TYPE struct list_head:struct kobject *
USING LOOP list_for_each_entry_rcu(tuple_iter, base, entry)
$

#if KERNEL_VERSION < 3.14.4
CREATE VIRTUAL TABLE Kobject_VT
USING STRUCT VIEW Kobject_SV
WITH REGISTERED C NAME sysfs_hypervisor_kobject
WITH REGISTERED C TYPE struct kobject
$
#endif
	
CREATE VIRTUAL TABLE KobjectSet_VT
USING STRUCT VIEW Kobject_SV
WITH REGISTERED C NAME net_queues_kset
WITH REGISTERED C TYPE struct kset:struct kobject *
USING LOOP list_for_each_entry(tuple_iter, &base->list, entry)
$

CREATE VIRTUAL TABLE EKobjectSet_VT
USING STRUCT VIEW Kobject_SV
WITH REGISTERED C TYPE struct kset:struct kobject *
USING LOOP list_for_each_entry(tuple_iter, &base->list, entry)
$

CREATE STRUCT VIEW Inode_SV (
       ino BIGINT FROM i_ino,
       i_mode INT FROM i_mode,
       bytes INT FROM i_bytes,
       i_size INT FROM i_size,
       blocks BIGINT FROM i_blocks
)
$

CREATE VIRTUAL TABLE EInode_VT
USING STRUCT VIEW Inode_SV
WITH REGISTERED C TYPE struct inode *
$

CREATE STRUCT VIEW Superblock_SV (
	name TEXT FROM s_id,
	subtype TEXT FROM s_subtype,
	//fs_name TEXT FROM s_type->name,
        //dentry_root_name TEXT FROM s_root->d_name.name,
	blocksize BIGINT FROM s_blocksize,
	max_file_size INT FROM s_maxbytes,
	count INT FROM s_count,
	flags BIGINT FROM s_flags,
	magic BIGINT FROM s_magic
	//fs_flags INT FROM s_type->fs_flags
)
$

CREATE VIRTUAL TABLE Superblock_VT
USING STRUCT VIEW Superblock_SV
WITH REGISTERED C NAME superblock
WITH REGISTERED C TYPE struct super_block *
USING LOOP list_for_each_entry_rcu(tuple_iter, &base->s_list, s_list)
$

CREATE VIRTUAL TABLE ESuperblock_VT
USING STRUCT VIEW Superblock_SV
WITH REGISTERED C TYPE struct super_block *
$

CREATE STRUCT VIEW File_SV (
       inode_name TEXT FROM f_path.dentry->d_name.name,
       inode_dname TEXT FROM {checked_d_path(&tuple_iter->f_path, dpath, 128)},
       inode_no BIGINT FROM f_path.dentry->d_inode->i_ino,
       inode_mode INT FROM f_path.dentry->d_inode->i_mode,
       inode_bytes INT FROM f_path.dentry->d_inode->i_bytes,
       inode_size_bytes BIGINT FROM i_size_read(tuple_iter->f_mapping->host),
       inode_size_pages BIGINT FROM ((i_size_read(tuple_iter->f_mapping->host) + PAGE_CACHE_SIZE -1) >> PAGE_CACHE_SHIFT),
       file_offset BIGINT FROM get_file_offset(tuple_iter),
       page_offset BIGINT FROM get_page_offset(tuple_iter),
       page_in_cache BIGINT FROM get_page_in_cache(tuple_iter),
       pages_in_cache_contig_start INT FROM {get_pages_in_cache_contig(tuple_iter, 0)},
       pages_in_cache_contig_current INT FROM {get_pages_in_cache_contig(tuple_iter, 1)},
       pages_in_cache_tag_dirty INT FROM {get_pages_in_cache_tag(tuple_iter, 0, PAGECACHE_TAG_DIRTY)},
       pages_in_cache_tag_writeback INT FROM {get_pages_in_cache_tag(tuple_iter, 0, PAGECACHE_TAG_WRITEBACK)},
       pages_in_cache_tag_towrite INT FROM {get_pages_in_cache_tag(tuple_iter, 0, PAGECACHE_TAG_TOWRITE)},
       pages_in_cache INT FROM get_pages_in_cache(tuple_iter->f_mapping),
       count BIGINT FROM f_count.counter,
       flags INT FROM f_flags,
       path_dentry BIGINT FROM (long)tuple_iter.f_dentry,
//     path_mount BIGINT FROM (long)tuple_iter.f_vfsmnt,
       path_mount BIGINT FROM (long)tuple_iter.f_path.mnt,
#if KERNEL_VERSION <= 3.8.1
       fowner_uid INT FROM f_owner.uid,
       fowner_euid INT FROM f_owner.euid,
       fcred_uid INT FROM f_cred->uid,
       fcred_euid INT FROM f_cred->euid,
       fcred_gid INT FROM f_cred->gid,
       fcred_egid INT FROM f_cred->egid,
#else
       fowner_uid INT FROM get_uid_val(tuple_iter->f_owner.uid),
       fowner_euid INT FROM get_uid_val(tuple_iter->f_owner.euid),
       fcred_uid INT FROM get_uid_val(tuple_iter->f_cred->uid),
       fcred_euid INT FROM get_uid_val(tuple_iter->f_cred->euid),
       fcred_gid INT FROM get_gid_val(tuple_iter->f_cred->gid),
       fcred_egid INT FROM get_gid_val(tuple_iter->f_cred->egid),
#endif
       fmode INT FROM f_mode,
       fra_pages INT FROM f_ra.size,
       fra_mmap_miss INT FROM f_ra.mmap_miss,
       is_socket INT FROM S_ISSOCK(tuple_iter->f_path.dentry->d_inode->i_mode),
#if KVM_RUNNING
       is_kvm_file INT FROM is_kvm_file(tuple_iter),
       FOREIGN KEY(kvm_id) FROM check_kvm(tuple_iter) REFERENCES EKVM_VT POINTER,
       is_kvm_vcpu_file INT FROM is_kvm_vcpu_file(tuple_iter),
       FOREIGN KEY(kvm_vcpu_id) FROM check_kvm_vcpu(tuple_iter) REFERENCES EKVMVCPU_VT POINTER,
#endif KVM_RUNNING
       special_interface BIGINT FROM (long)tuple_iter->private_data,
       FOREIGN KEY(socket_id) FROM is_socket(tuple_iter) REFERENCES ESocket_VT POINTER,
       FOREIGN KEY(sb_id) FROM f_path.dentry->d_inode->i_sb REFERENCES ESuperblock_VT POINTER
)
$

CREATE VIRTUAL TABLE EFile_VT
USING STRUCT VIEW File_SV
WITH REGISTERED C TYPE struct fdtable:struct file*
USING LOOP for (EFile_VT_begin(tuple_iter, base->fd, (bit = find_first_bit((unsigned long *)base->open_fds, base->max_fds))); bit < base->max_fds; EFile_VT_advance(tuple_iter, base->fd, (bit = find_next_bit((unsigned long *)base->open_fds, base->max_fds, bit + 1))))
$

CREATE STRUCT VIEW FilesStruct_SV (
       count INT FROM count.counter,
       FOREIGN KEY(fdtablefile_id) FROM get_fdtable(tuple_iter) REFERENCES EFile_VT POINTER
)
$

CREATE VIRTUAL TABLE EFilesStruct_VT
USING STRUCT VIEW FilesStruct_SV
WITH REGISTERED C TYPE struct files_struct
$

CREATE STRUCT VIEW Group_SV (
       gid INT FROM tuple_iter
)
$

CREATE VIRTUAL TABLE EGroup_VT
USING STRUCT VIEW Group_SV
WITH REGISTERED C TYPE struct group_info*:int*
USING LOOP for (EGroup_VT_begin(tuple_iter, base->small_block, i); i < base->ngroups; EGroup_VT_advance(tuple_iter, base->small_block, ++i))
$

CREATE STRUCT VIEW Process_SV (
       name TEXT FROM comm,
       pid INT FROM pid,
       tgid INT FROM tgid,
#if KERNEL_VERSION <= 3.8.1
       cred_uid INT FROM cred->uid,
       cred_gid INT FROM cred->gid,
       cred_euid INT FROM cred->euid,
       cred_egid INT FROM cred->egid,
       cred_fsuid INT FROM cred->fsuid,
       cred_fsgid INT FROM cred->fsgid,
       ecred_uid INT FROM real_cred->uid,
       ecred_gid INT FROM real_cred->gid,
       ecred_euid INT FROM real_cred->euid,
       ecred_egid INT FROM real_cred->egid,
       ecred_fsuid INT FROM real_cred->fsuid,
       ecred_fsgid INT FROM real_cred->fsgid,
#else
       cred_uid INT FROM get_uid_val(tuple_iter->cred->uid),
       cred_gid INT FROM get_gid_val(tuple_iter->cred->gid),
       cred_euid INT FROM get_uid_val(tuple_iter->cred->euid),
       cred_egid INT FROM get_gid_val(tuple_iter->cred->egid),
       cred_fsuid INT FROM get_uid_val(tuple_iter->cred->fsuid),
       cred_fsgid INT FROM get_gid_val(tuple_iter->cred->fsgid),
       ecred_uid INT FROM get_uid_val(tuple_iter->real_cred->uid),
       ecred_gid INT FROM get_gid_val(tuple_iter->real_cred->gid),
       ecred_euid INT FROM get_uid_val(tuple_iter->real_cred->euid),
       ecred_egid INT FROM get_gid_val(tuple_iter->real_cred->egid),
       ecred_fsuid INT FROM get_uid_val(tuple_iter->real_cred->fsuid),
       ecred_fsgid INT FROM get_gid_val(tuple_iter->real_cred->fsgid),
#endif
       FOREIGN KEY(group_set_id) FROM real_cred->group_info REFERENCES EGroup_VT POINTER,
       state_family BIGINT FROM state,
       state TEXT FROM get_task_state(tuple_iter),
       usage INT FROM usage.counter,
       flags INT FROM flags,
       ptrace INT FROM ptrace,
#if KERNEL_VERSION > 2.6.32
       on_cpu INT FROM on_cpu, // CONFIG_SMP
       on_rq INT FROM on_rq,
       jobctl INT FROM jobctl,
       rss_stat_events INT FROM rss_stat.events,
       cpuset_slab_spread_rotor INT FROM cpuset_slab_spread_rotor, //CONFIG_CPUSETS
       nr_dirtied INT FROM nr_dirtied,
       nr_dirtied_pause INT FROM nr_dirtied_pause,
       nr_pages BIGINT FROM memcg_batch.nr_pages, // CONFIG_CGROUP_MEM_RES_CTLR
       memsw_nr_pages BIGINT FROM memcg_batch.memsw_nr_pages, // CONFIG_CGROUP_MEM_RES_CTLR
#endif
       rt_priority INT FROM rt_priority,
       prio INT FROM prio,
       static_prio INT FROM static_prio,
       normal_prio INT FROM normal_prio,
       exit_state INT FROM exit_state,
       exit_code INT FROM exit_code,
       exit_signal INT FROM exit_signal,
       pdeath_signal INT FROM pdeath_signal,
       personality INT FROM personality,
       FOREIGN KEY(children_id) FROM tuple_iter REFERENCES EProcessChild_VT, // why does not need POINTER
       FOREIGN KEY(sibling_id) FROM tuple_iter REFERENCES EProcessChild_VT,
       FOREIGN KEY(parent_id) FROM parent REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(real_parent_id) FROM real_parent REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(thread_group_leader_id) FROM group_leader REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(thread_group_id) FROM tuple_iter REFERENCES EThread_VT POINTER,
//       did_exec INT FROM did_exec:1,
//       fpu_counter CHAR FROM fpu_counter, CHAR support
       policy INT FROM policy,
       run_on_cpu BIGINT FROM sched_info.pcount,
       run_delay BIGINT FROM sched_info.run_delay,
       last_arrival BIGINT FROM sched_info.last_arrival,
       last_queued BIGINT FROM sched_info.last_queued,
//       FOREIGN KEY(rss_stat_counter_id) FROM rss_stat.count REFERENCES CounterInt POINTER, // require array support
       utime BIGINT FROM utime,
       stime BIGINT FROM stime,
       utimescaled BIGINT FROM utimescaled,
       stimescaled BIGINT FROM stimescaled,
       start_time INT FROM start_time.tv_sec,
       real_start_time INT FROM real_start_time.tv_sec,
       nvcsw BIGINT FROM nvcsw,
       nivcsw BIGINT FROM nivcsw,
       link_count INT FROM link_count,
       total_link_count INT FROM total_link_count,
       root_path_name TEXT FROM fs->root.dentry->d_name.name,
       pwd_path_name TEXT FROM fs->pwd.dentry->d_name.name,
//       FOREIGN KEY(fs_struct_id) FROM fs REFERENCES EFs POINTER,
       FOREIGN KEY(files_struct_id) FROM files REFERENCES EFilesStruct_VT POINTER,
       fs_count BIGINT FROM files->count.counter,
       fs_fd_max_fds INT FROM get_max_fds(tuple_iter->files),
       FOREIGN KEY(fs_fd_file_id) FROM get_fdtable(tuple_iter->files) REFERENCES EFile_VT POINTER,
       FOREIGN KEY(io_id) FROM ioac REFERENCES EIO_VT,
       FOREIGN KEY(vm_id) FROM mm REFERENCES EVirtualMem_VT POINTER,
//       FOREIGN KEY(nsproxy_id) FROM nsproxy REFERENCES ENsproxy_VT POINTER,
       FOREIGN KEY(netns_ipv4_id) FROM nsproxy->net_ns->ipv4 REFERENCES ENetnsIpv4_VT,
       FOREIGN KEY(active_virtual_memory_id) FROM active_mm REFERENCES EVirtualMem_VT POINTER,
       FOREIGN KEY(process_signal_id) FROM signal REFERENCES EProcessSignal POINTER,
       min_flt BIGINT FROM min_flt,
       maj_flt BIGINT FROM maj_flt,
       sas_ss_sp BIGINT FROM sas_ss_sp,
       sas_ss_size INT FROM sas_ss_size,
#if KERNEL_VERSION <= 3.8.1
       loginuid INT FROM (uid_t)tuple_iter.loginuid, // CONFIG_AUDITSYSCALL
#else
       loginuid INT FROM get_uid_val(tuple_iter.loginuid), // CONFIG_AUDITSYSCALL
#endif
       sessionid INT FROM sessionid, // CONFIG_AUDITSYSCALL
//       self_exec_id INT FROM (u32)tuple_iter.self_exec_id,
       parent_exec_id INT FROM (u32)tuple_iter.parent_exec_id,
       acct_rss_mem1 BIGINT FROM (u64)tuple_iter.acct_rss_mem1, // defined(CONFIG_TASK_XACCT)
       acct_vm_mem1 BIGINT FROM (u64)tuple_iter.acct_vm_mem1, // defined(CONFIG_TASK_XACCT)
       cpuset_mem_spread_rotor INT FROM cpuset_mem_spread_rotor, // CONFIG_CPUSETS
       timer_slack_ns BIGINT FROM timer_slack_ns,
       default_timer_slack_ns BIGINT FROM default_timer_slack_ns,
       trace BIGINT FROM trace, // CONFIG_TRACING
       trace_recursion BIGINT FROM trace_recursion, // CONFIG_TRACING
)
$

CREATE VIRTUAL TABLE Process_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C NAME processes
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(tuple_iter, &base->tasks, tasks)
$

CREATE VIRTUAL TABLE EProcess_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct
$

CREATE VIRTUAL TABLE EProcessChild_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(tuple_iter, &base->children, children)
$

CREATE VIRTUAL TABLE EThread_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(tuple_iter, &base->thread_group, thread_group)
$

#if KVM_RUNNING
CREATE STRUCT VIEW KVMVCPUEvents_SV (
        exception_injected INT FROM exception.injected,
        exception_nr INT FROM exception.nr,
        exception_has_error_code INT FROM exception.has_error_code,
        exception_pad INT FROM exception.pad,
        exception_error_code INT FROM exception.error_code,
        interrupt_injected INT FROM interrupt.injected,
        interrupt_nr INT FROM interrupt.nr,
        interrupt_soft INT FROM interrupt.soft,
#if KERNEL_VERSION > 2.6.33
        interrupt_shadow INT FROM interrupt.shadow,
#else
        interrupt_shadow INT FROM interrupt.pad,
#endif
        nmi_injected INT FROM nmi.injected,
        nmi_pending INT FROM nmi.pending,
//        nmi_masked INT FROM kvm_x86_ops->get_nmi_mask(tuple_iter),  // NULL check fails
        nmi_masked INT FROM nmi.masked,
        nmi_pad INT FROM nmi.pad,
#if KERNEL_VERSION <= 3.8.1
	sipi_vector INT FROM sipi_vector,
#endif
        flags INT FROM flags
)
$

CREATE VIRTUAL TABLE EKVMVCPUEvents_VT
USING STRUCT VIEW KVMVCPUEvents_SV
WITH REGISTERED C TYPE struct kvm_vcpu_events
$

CREATE STRUCT VIEW KVMVCPU_SV (
        cpu_has_vmx INT FROM check_vmx(tuple_iter),
        cpu_has_svm TEXT FROM {char *msg = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE/4); 
                               check_svm(tuple_iter, msg);
                               sqlite3_free(msg);},
        cpu INT FROM cpu,
        vcpu_id INT FROM vcpu_id,
	mode INT FROM mode,
        current_privilege_level INT FROM get_cpl(tuple_iter),
        hypercalls_allowed INT FROM check_cpl(tuple_iter),
        requests BIGINT FROM requests,
// vcpu_stat
	FOREIGN KEY(vcpu_events_id) FROM kvm_vcpu_ioctl_x86_get_vcpu_events(tuple_iter) REFERENCES EKVMVCPUEvents_VT POINTER
)
$

CREATE VIRTUAL TABLE EKVMVCPU_VT
USING STRUCT VIEW KVMVCPU_SV
WITH REGISTERED C TYPE struct kvm_vcpu *
$

CREATE STRUCT VIEW KVMArchPitChannelState_SV (
        count INT FROM count, // can be 65536
        latched_count INT FROM latched_count,
        count_latched INT FROM count_latched,
        status_latched INT FROM status_latched,
        status INT FROM status,
        read_state INT FROM read_state,
        write_state INT FROM write_state,
        write_latch INT FROM write_latch,
        rw_mode INT FROM rw_mode,
        mode INT FROM mode,
        bcd INT FROM bcd,
        gate INT FROM gate,
        count_load_time BIGINT FROM count_load_time
)
$

CREATE VIRTUAL TABLE EKVMArchPitChannelState_VT
USING STRUCT VIEW KVMArchPitChannelState_SV
WITH REGISTERED C TYPE struct kvm_pit_state:struct kvm_pit_channel_state *
USING LOOP for(tuple_iter = &base->channels[i]; i < 3; tuple_iter = &base->channels[++i])
$

CREATE STRUCT VIEW KVM_SV (
	bsp_vcpu_id INT FROM bsp_vcpu_id,
	online_vcpus INT FROM atomic_read(&tuple_iter->online_vcpus),
        last_boosted_vcpu INT FROM last_boosted_vcpu,
	users INT FROM atomic_read(&tuple_iter->users_count),
        tlbs_dirty BIGINT FROM tlbs_dirty,
// kvm_vm_stat
// mm_list
// vcpus
	FOREIGN KEY(pit_state_id) FROM mem_copy_pit_state(tuple_iter->arch.vpit->pit_state) REFERENCES EKVMArchPitChannelState_VT
)
$

CREATE VIRTUAL TABLE EKVM_VT
USING STRUCT VIEW KVM_SV
WITH REGISTERED C TYPE struct kvm *
$

#endif KVM_RUNNING


#if KERNEL_VERSION >= 3.2.0 < 3.14.4
CREATE STRUCT VIEW XenStats_SV (
        cpu_has_vmx INT FROM check_vmx(tuple_iter),
        cpu_has_svm TEXT FROM {char *msg = (char *)sqlite3_malloc(sizeof(char) * PAGE_SIZE/4); 
                               check_svm(tuple_iter, msg);
                               sqlite3_free(msg);},
        cur_pages BIGINT FROM current_pages,
        target_pages BIGINT FROM target_pages,
        balloon_low BIGINT FROM balloon_low,
        balloon_high BIGINT FROM balloon_high,
        sched_delay BIGINT FROM schedule_delay,
        max_schedule_delay BIGINT FROM max_schedule_delay,
        retry_count BIGINT FROM retry_count,
        max_retry_count BIGINT FROM max_retry_count,
//        hotplug_pages BIGINT FROM hotplug_pages, // #ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
//        balloon_hotplug BIGINT FROM balloon_hotplug // #ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
)
$

CREATE VIRTUAL TABLE XenStats_VT
USING STRUCT VIEW XenStats_SV
WITH REGISTERED C NAME xen_balloon_stats
WITH REGISTERED C TYPE struct balloon_stats
$

#endif

#if KVM_RUNNING
CREATE VIEW KVM_View AS
       SELECT P.name AS kvm_process_name, 
	      F.inode_name AS kvm_inode_name, 
              users AS kvm_users, 
              online_vcpus AS kvm_online_vcpus,
//              stats_id AS kvm_stats_id, 
//              online_vcpus_id AS kvm_online_vcpus_id,
              tlbs_dirty AS kvm_tlbs_dirty, 
              pit_state_id AS kvm_pit_state_id
       FROM Process_VT as P
       JOIN EFile_VT as F
       ON F.base = P.fs_fd_file_id
       JOIN EKVM_VT AS KVM
       ON KVM.base = F.kvm_id
       WHERE F.is_kvm_file;
$

CREATE VIEW KVM_VCPU_View AS
       SELECT P.name AS vcpu_process_name, 
              F.inode_name AS vcpu_inode_name, 
              cpu_has_vmx, cpu_has_svm,
              cpu, vcpu_id, mode AS vcpu_mode,
              current_privilege_level,
              hypercalls_allowed, 
              requests AS vcpu_requests, 
//              stats_id AS vcpu_stats_id,
              vcpu_events_id
       FROM Process_VT as P
       JOIN EFile_VT as F
       ON F.base = P.fs_fd_file_id
       JOIN EKVMVCPU_VT AS KVM_VCPU
       ON KVM_VCPU.base = F.kvm_vcpu_id
       WHERE F.is_kvm_vcpu_file;
$
#endif KVM_RUNNING
