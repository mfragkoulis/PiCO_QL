#include <unistd.h>
#include <valgrind.h>


int rmdir(const char *path){return 0;};

/* Returns an absolute file name representing 
 * the current working directory, storing it 
 * in the character array buffer that you provide.
 */
//#define getcwd(X,Y) 1
char *getcwd(char *buf, size_t size){return NULL;};

/* Check access permissions of file or path name */
//#define access(X,Y) ((X = 0) == 0)
int access(const char *path, int mode){return 0;};

/* Change file permissions. */
int fchmod(int fildes, mode_t mode){return 0;}

/* Close an open file descriptor */
//#define close(X) (((X = 0) == 0) ? X : -1)
void close(int fd){return VG_(close)(fd);};

/* Determine information about a file based on its 
 * file descriptor
 */
//#define fstat(X,Y) 0
int fstat(int fd, struct stat *buf){return 0;};

/* Remove file from file system */
//#define unlink(X) (((X = NULL) == NULL) ? 0 : -1)
//VG:int unlink(int fd){return 0;};

/* Read data into a buffer */
//#define read(X,Y,Z) 0
/* VG */
int read(int fd, void *buf, size_t size){return VG_(read)(fd, buf, size);};

/* VG */
int pread(int fd, void *buf, size_t size, off_t offset){return 0;};

/* Determine information about a file based 
 * on its file path. 
 */
//#define stat(X,Y) 0
int stat(const char *path, struct stat *buf){return 0;};

/* Write data out of a buffer */
//#define write(X,Y,Z) 0
int write(int fd, const void *buf, size_t size){return VG_(write)(fd, buf, size);};

/* VG */
int pwrite(int fd, const void *buf, size_t size, off_t offset){return 0;};

/* VG */
int unlink(const char *name) {return VG_(unlink)(name);};
/* Truncates the file indicated by the open 
 * file descriptor file_descriptor to the 
 * indicated length.
 */
//#define ftruncate(X,Y) 0
int ftruncate(int fd, off_t length){return 0;};

/* VG */
int fsync(int fd){return 0;}

/* VG: /usr/include/sys/fcntl.h in __APPLE__ */
int fcntl(int n, int m, ...){return 0;};

/* VG */
int fstatfs(int fd, struct statfs *buf){return 0;};

/* VG */
int statfs(const char *fs, struct statfs *buf){return 0;};

/* VG */
int mkdir(const char * pathname , mode_t mode){return 0;};

/* VG */
mode_t umask(mode_t mask){return (mode_t)0;};

#ifdef __APPLE__

/* VG: for __APPLE__ */
int fsctl(const char *path, unsigned long request, void *data,
             unsigned long options){return 0;};

/* VG */
int flock(int fd, int operation){return 0;};


/* VG */
int futimes(int fd, const struct timeval tv[2]){return 0;};

/* VG */
int sysctlbyname(const char *name, void *oldp, size_t *oldlenp, void *newp, size_t newlen){return 0;};

#endif

#ifdef __linux__
typedef long off64_t;

int open64(const char *path, int flags, ...);
int fstat64(int fd, struct stat *buf);
int stat64(const char *path, struct stat *buf);
int posix_fallocate64(int fd, off64_t offset, off64_t len);

int fstat64(int fd, struct stat *buf){return 0;};
int stat64(const char *path, struct stat *buf){return 0;};
int posix_fallocate64(int fd, off64_t offset, off64_t len){return 0;};
int open64(const char *path, int flags, ...) {
  va_list ap;
  mode_t mode;

  va_start(ap, flags);
  mode = va_arg(ap, int);
  va_end(ap);

  SysRes sr = VG_(open)(path, flags, mode);
  return (int)sr_Res(sr);
}
#endif

int open(const char *path, int flags, ...) {
  va_list ap;
  mode_t mode;

  va_start(ap, flags);
  mode = va_arg(ap, int);
  va_end(ap);

  SysRes sr = VG_(open)(path, flags, mode);
  return (int)sr_Res(sr);
}

