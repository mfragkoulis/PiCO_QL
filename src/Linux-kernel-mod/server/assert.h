#ifndef _ASSERT_H
#define _ASSERT_H

#include <linux/kernel.h>

#define assert(X) ((void)(unlikely(!(X)) ? printk(KERN_ERR "Assertion failed! %s,%s,%s,line=%d\n",\
    (#X),__FILE__,__FUNCTION__,__LINE__) : 0 ))

#endif
