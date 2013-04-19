#include <linux/version.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/mm_types.h>
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
#define __NO_VERSION__      
#define EIpVsStatsEstim_VT_decl(X) struct ip_vs_estimator *X
#define Process_VT_decl(X) struct task_struct *X
#define EProcessChild_VT_decl(X) struct task_struct *X
#define EThread_VT_decl(X) struct task_struct *X
#define EFile_VT_decl(X) struct file *X; int bit = 0
#define EFile_VT_begin(X, Y, Z) (X) = (Y)[(Z)]
#define EFile_VT_advance(X, Y, Z) EFile_VT_begin(X,Y,Z)
#define EGroup_VT_decl(X) int *X; int i = 0
#define EGroup_VT_begin(X, Y, Z) (X) = (int *)&(Y)[(Z)]
#define EGroup_VT_advance(X, Y, Z) EGroup_VT_begin(X, Y, Z)
$

USE C LOCK rcu_read_lock() UNLOCK rcu_read_unlock()
$


CREATE STRUCT VIEW VirtualMemRegion_SV (
       vm_usage INT FROM vm_usage
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
       cached_hole_size BIGINT FROM cached_hole_size,
       free_area_cache BIGINT FROM free_area_cache,
       pgd BIGINT FROM pgd->pgd,
       users BIGINT FROM mm_users.counter,
       counter INT FROM mm_count.counter,
       map_count INT FROM map_count,
       hiwater_rss BIGINT FROM hiwater_rss,
       hiwater_vm BIGINT FROM hiwater_vm,
       total_vm BIGINT FROM total_vm,
       locked_vm BIGINT FROM locked_vm,
       pinned_vm BIGINT FROM pinned_vm,
       shared_vm BIGINT FROM shared_vm,
       exec_vm BIGINT FROM exec_vm,
       stack_vm BIGINT FROM stack_vm,
       reserved_vm BIGINT FROM reserved_vm,
       def_flags BIGINT FROM def_flags,
       nr_ptes BIGINT FROM nr_ptes,
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
//       FOREIGN KEY(rss_stat_id) FROM rss_stat REFERENCES ,
       faultstamp INT FROM faultstamp,
       token_priority INT FROM token_priority,
       last_interval INT FROM last_interval,
       flags BIGINT FROM flags,
//     FOREIGN KEY(process_owner_id) FROM owner REFERENCES EProcess_VT POINTER, CONFIG_MM_OWNER 
//     FOREIGN KEY(exe_file_id) FROM exe_file REFERENCES FILE POINTER,  
       num_exe_file_vmas BIGINT FROM num_exe_file_vmas,
       locked INT FROM mmap_sem.count
)
$

CREATE VIRTUAL TABLE EVirtualMem_VT
USING STRUCT VIEW VirtualMem_SV
WITH REGISTERED C TYPE struct mm_struct$

CREATE STRUCT VIEW TcpStat_SV (
       RtoAlgorithm BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_RTOALGORITHM),
       RtoMin BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_RTOMIN),
       RtoMax BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_RTOMAX),
       MaxConn BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_MAXCONN),
       ActiveOpens BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_ACTIVEOPENS),
       PassiveOpens BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_PASSIVEOPENS),
       AttemptFails BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_ATTEMPTFAILS),
       EstabResets BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_ESTABRESETS),
       CurrEstab BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_CURRESTAB),
       InSegs BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_INSEGS),
       OutSegs BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_OUTSEGS),
       RetransSegs BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_RETRANSSEGS),
       InErrs BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_INERRS),
       OutRsts BIGINT FROM snmp_fold_field((void __percpu **) this.mibs, TCP_MIB_OUTRSTS)
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
       type INT FROM type,
       flags BIGINT FROM flags,
       FOREIGN KEY(file_id) FROM file REFERENCES EFile_VT POINTER,
       FOREIGN KEY(sock_id) FROM sk REFERENCES ESock_VT POINTER
)
$

CREATE VIRTUAL TABLE ESocket_VT
USING STRUCT VIEW Socket_SV
WITH REGISTERED C TYPE struct socket$      

CREATE STRUCT VIEW Sock_SV (
       timestamp_last_rcv BIGINT FROM sk_stamp.tv64,
       drops INT FROM atomic_read(&this.sk_drops),
       errors INT FROM sk_err,
       errors_soft INT FROM sk_err_soft,
       transmit_bytes_committed INT FROM sk_wmem_alloc.counter,
       persistent_queue_size INT FROM sk_wmem_queued,
       pending_writes INT FROM sk_write_pending,
       snd_buf_size INT FROM sk_sndbuf,
       rcv_buf_size INT FROM sk_rcvbuf,
       FOREIGN KEY(socket_id) FROM sk_socket REFERENCES ESocket_VT POINTER
//       FOREIGN KEY(peer_process_id) FROM get_pid_task(this->sk_peer_pid, PIDTYPE_PID) REFERENCES EProcess_VT POINTER
)
$

CREATE VIRTUAL TABLE ESock_VT
USING STRUCT VIEW Sock_SV
WITH REGISTERED C TYPE struct sock$

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
USING LOOP list_for_each_entry_rcu(iter, &base->list, list)$

CREATE STRUCT VIEW NetnsIpvs_SV (
       FOREIGN KEY(ipvs_stats_estim_id) FROM tot_stats.est REFERENCES EIpVsStatsEstim_VT
)
$

CREATE VIRTUAL TABLE ENetnsIpvs_VT
USING STRUCT VIEW NetnsIpvs_SV
WITH REGISTERED C TYPE struct netns_ipvs$

CREATE STRUCT VIEW NetNamespace_SV (
       count_to_free INT FROM passive.counter,
       count_to_shut INT FROM count.counter,
//       use_count INT FROM use_count.counter,
       FOREIGN KEY(rtnl_sock_id) FROM rtnl REFERENCES ESock_VT POINTER,
       FOREIGN KEY(genl_sock_id) FROM genl_sock REFERENCES ESock_VT POINTER,
       FOREIGN KEY(nfnl_sock_id) FROM nfnl REFERENCES ESock_VT POINTER,
       FOREIGN KEY(nfnl_stash_sock_id) FROM nfnl_stash REFERENCES ESock_VT POINTER,
       FOREIGN KEY(netns_mib_id) FROM mib REFERENCES ENetMib_VT,
       FOREIGN KEY(netns_ipvs) FROM ipvs REFERENCES ENetnsIpvs_VT POINTER
)
$

CREATE VIRTUAL TABLE ENetNamespace_VT
USING STRUCT VIEW NetNamespace_SV
WITH REGISTERED C TYPE struct net$

CREATE STRUCT VIEW Nsproxy_SV (
       count INT FROM count.counter,
//      FOREIGN KEY(uts_ns_id) FROM uts_ns REFERENCES EUtsNs POINTER,
//       FOREIGN KEY(ipc_ns_id) FROM ipc_ns REFERENCES EIpcNs POINTER,
//       FOREIGN KEY(mount_ns_id) FROM mnt_ns REFERENCES EMountNs POINTER,
//       FOREIGN KEY(pid_ns_id) FROM pid_ns REFERENCES EPidNs POINTER,
       FOREIGN KEY(net_ns_id) FROM net_ns REFERENCES ENetNamespace_VT POINTER,
)
$

CREATE VIRTUAL TABLE Nsproxy_VT
USING STRUCT VIEW Nsproxy_SV
WITH REGISTERED C NAME init_task_nsproxy
WITH REGISTERED C TYPE struct nsproxy
$

CREATE STRUCT VIEW IO_SV (
       bytes_read BIGINT FROM rchar, //CONFIG_TASK_XACCT
       bytes_written BIGINT FROM wchar, //CONFIG_TASK_XACCT
       syscalls_read BIGINT FROM syscr, //CONFIG_TASK_XACCT
       syscalls_write BIGINT FROM syscw, //CONFIG_TASK_XACCT
       bytes_read_store BIGINT FROM read_bytes, //CONFIG_TASK_IO_ACCOUNTING
       bytes_write_store BIGINT FROM write_bytes, //CONFIG_TASK_IO_ACCOUNTING
       cancelled_write_bytes BIGINT FROM cancelled_write_bytes //CONFIG_TASK_IO_ACCOUNTING
)
$

CREATE VIRTUAL TABLE EIO_VT
USING STRUCT VIEW IO_SV
WITH REGISTERED C TYPE struct task_io_accounting
$

CREATE STRUCT VIEW ProcessSignal (
       signal_counter INT FROM sigcnt.counter,
       live INT FROM live.counter,
       nr_threads INT FROM nr_threads,
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
       prev_utime BIGINT FROM prev_utime, 
//CONFIG_VIRT_CPU_ACCOUNTING
       prev_stime BIGINT FROM prev_stime, 
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


CREATE STRUCT VIEW File_SV (
       inode_name TEXT FROM f_path.dentry->d_iname,
       inode_mode INT FROM f_path.dentry->d_inode->i_mode,
       inode_bytes INT FROM f_path.dentry->d_inode->i_bytes,
       inode_size INT FROM f_path.dentry->d_inode->i_size,
       count BIGINT FROM f_count.counter,
       flags INT FROM f_flags,
       path_dentry BIGINT FROM (long)this.f_dentry,
       path_mount BIGINT FROM (long)this.f_vfsmnt,
       fowner_uid INT FROM f_owner.uid,
       fowner_euid INT FROM f_owner.euid,
       fcred_gid INT FROM f_cred->gid,
       fcred_egid INT FROM f_cred->egid,
       fmode INT FROM f_mode,
       FOREIGN KEY(socket_id) FROM private_data REFERENCES ESocket_VT POINTER
)
$

CREATE VIRTUAL TABLE EFile_VT
USING STRUCT VIEW File_SV
WITH REGISTERED C TYPE struct fdtable:struct file*
USING LOOP for (EFile_VT_begin(iter, base->fd, (bit = find_first_bit((unsigned long *)base->open_fds, base->max_fds))); bit < base->max_fds; EFile_VT_advance(iter, base->fd, (bit = find_next_bit((unsigned long *)base->open_fds, base->max_fds, bit + 1))))
$

CREATE STRUCT VIEW FilesStruct_SV (
       count INT FROM count.counter,
       FOREIGN KEY(fdtablefile_id) FROM fdt REFERENCES EFile_VT POINTER
)
$

CREATE VIRTUAL TABLE EFilesStruct_VT
USING STRUCT VIEW FilesStruct_SV
WITH REGISTERED C TYPE struct files_struct
$

CREATE STRUCT VIEW Group_SV (
       gid INT FROM self
)
$

CREATE VIRTUAL TABLE EGroup_VT
USING STRUCT VIEW Group_SV
WITH REGISTERED C TYPE struct group_info*:int
USING LOOP for (EGroup_VT_begin(iter, base->small_block, i); i < base->ngroups; EGroup_VT_advance(iter, base->small_block, ++i))
$

CREATE STRUCT VIEW Process_SV (
       name TEXT FROM comm,
       pid INT FROM pid,
       tgid INT FROM tgid,
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
       FOREIGN KEY(group_set_id) FROM real_cred->group_info REFERENCES EGroup_VT POINTER,
       state BIGINT FROM state,
       usage INT FROM usage.counter,
       flags INT FROM flags,
       ptrace INT FROM ptrace,
       on_cpu INT FROM on_cpu, // CONFIG_SMP
       on_rq INT FROM on_rq,
       rt_priority INT FROM rt_priority,
       prio INT FROM prio,
       static_prio INT FROM static_prio,
       normal_prio INT FROM normal_prio,
       exit_state INT FROM exit_state,
       exit_code INT FROM exit_code,
       exit_signal INT FROM exit_signal,
       pdeath_signal INT FROM pdeath_signal,
       jobctl INT FROM jobctl,
       personality INT FROM personality,
       FOREIGN KEY(children_id) FROM self REFERENCES EProcessChild_VT, // why does not need POINTER
       FOREIGN KEY(sibling_id) FROM self REFERENCES EProcessChild_VT,
       FOREIGN KEY(parent_id) FROM parent REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(real_parent_id) FROM real_parent REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(thread_group_leader_id) FROM group_leader REFERENCES EProcess_VT POINTER,
       FOREIGN KEY(thread_group_id) FROM self REFERENCES EThread_VT POINTER,
//       did_exec INT FROM did_exec:1,
//       fpu_counter CHAR FROM fpu_counter, CHAR support
       policy INT FROM policy,
       run_on_cpu BIGINT FROM sched_info.pcount,
       run_delay BIGINT FROM sched_info.run_delay,
       last_arrival BIGINT FROM sched_info.last_arrival,
       last_queued BIGINT FROM sched_info.last_queued,
       rss_stat_events INT FROM rss_stat.events,
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
//       FOREIGN KEY(fs_struct_id) FROM fs REFERENCES EFs POINTER,
       FOREIGN KEY(files_struct_id) FROM files REFERENCES EFilesStruct_VT POINTER,
       fs_count BIGINT FROM files->count.counter,
       fs_fd_max_fds INT FROM files->fdt->max_fds,
       FOREIGN KEY(fs_fd_file_id) FROM files->fdt REFERENCES EFile_VT POINTER,
       FOREIGN KEY(io_id) FROM ioac REFERENCES EIO_VT,
       FOREIGN KEY(vm_id) FROM mm REFERENCES EVirtualMem_VT POINTER,
//       FOREIGN KEY(nsproxy_id) FROM nsproxy REFERENCES ENsproxy_VT POINTER,
       FOREIGN KEY(active_virtual_memory_id) FROM active_mm REFERENCES EVirtualMem_VT POINTER,
       FOREIGN KEY(process_signal_id) FROM signal REFERENCES EProcessSignal POINTER,
       min_flt BIGINT FROM min_flt,
       maj_flt BIGINT FROM maj_flt,
       sas_ss_sp BIGINT FROM sas_ss_sp,
       sas_ss_size INT FROM sas_ss_size,
       loginuid INT FROM (uid_t)this.loginuid, // CONFIG_AUDITSYSCALL
       sessionid INT FROM sessionid, // CONFIG_AUDITSYSCALL
//       self_exec_id INT FROM (u32)this.self_exec_id,
       parent_exec_id INT FROM (u32)this.parent_exec_id,
       acct_rss_mem1 BIGINT FROM (u64)this.acct_rss_mem1, // defined(CONFIG_TASK_XACCT)
       acct_vm_mem1 BIGINT FROM (u64)this.acct_vm_mem1, // defined(CONFIG_TASK_XACCT)
       cpuset_mem_spread_rotor INT FROM cpuset_mem_spread_rotor, // CONFIG_CPUSETS
       cpuset_slab_spread_rotor INT FROM cpuset_slab_spread_rotor, //CONFIG_CPUSETS
       nr_dirtied INT FROM nr_dirtied,
       nr_dirtied_pause INT FROM nr_dirtied_pause,
       timer_slack_ns BIGINT FROM timer_slack_ns,
       default_timer_slack_ns BIGINT FROM default_timer_slack_ns,
       trace BIGINT FROM trace, // CONFIG_TRACING
       trace_recursion BIGINT FROM trace_recursion, // CONFIG_TRACING
       nr_pages BIGINT FROM memcg_batch.nr_pages, // CONFIG_CGROUP_MEM_RES_CTLR
       memsw_nr_pages BIGINT FROM memcg_batch.memsw_nr_pages // CONFIG_CGROUP_MEM_RES_CTLR
)
$

CREATE VIRTUAL TABLE Process_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C NAME processes
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(iter, &base->tasks, tasks)
$

CREATE VIRTUAL TABLE EProcess_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct
$

CREATE VIRTUAL TABLE EProcessChild_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(iter, &base->children, children)
$

CREATE VIRTUAL TABLE EThread_VT
USING STRUCT VIEW Process_SV
WITH REGISTERED C TYPE struct task_struct *
USING LOOP list_for_each_entry_rcu(iter, &base->thread_group, thread_group)
$

