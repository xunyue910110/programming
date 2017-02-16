# 1 "testlist.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "testlist.c"
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/init.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/compiler.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/init.h" 2 3
# 131 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/init.h" 3
typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

extern initcall_t __con_initcall_start[], __con_initcall_end[];
extern initcall_t __security_initcall_start[], __security_initcall_end[];


typedef void (*ctor_fn_t)(void);


extern int do_one_initcall(initcall_t fn);
extern char __attribute__ ((__section__(".init.data"))) boot_command_line[];
extern char *saved_command_line;
extern unsigned int reset_devices;
extern unsigned int usevirtefi;


void setup_arch(char **);
void prepare_namespace(void);

extern void (*late_time_init)(void);

extern int initcall_debug;
# 219 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/init.h" 3
struct obs_kernel_param {
 const char *str;
 int (*setup_func)(char *);
 int early;
};
# 248 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/init.h" 3
void __attribute__ ((__section__(".init.text"))) notrace parse_early_param(void);
void __attribute__ ((__section__(".init.text"))) notrace parse_early_options(char *cmdline);
# 2 "testlist.c" 2
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 1 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 1 3






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 1 3
# 49 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/typecheck.h" 1 3
# 50 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/preempt.h" 1 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/preempt.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/thread_info.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/thread_info.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/types.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/types.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/types.h" 1 3






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/int-ll64.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/int-ll64.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitsperlong.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitsperlong.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/bitsperlong.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitsperlong.h" 2 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/int-ll64.h" 2 3







typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;


__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/types.h" 2 3



typedef unsigned short umode_t;
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/types.h" 2 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/types.h" 2 3
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/types.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/posix_types.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/stddef.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/posix_types.h" 2 3
# 36 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/posix_types.h" 3
typedef struct {
 unsigned long fds_bits [(1024/(8 * sizeof(unsigned long)))];
} __kernel_fd_set;


typedef void (*__kernel_sighandler_t)(int);


typedef int __kernel_key_t;
typedef int __kernel_mqd_t;

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/posix_types.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/posix_types.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/posix_types_64.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/posix_types_64.h" 3
typedef unsigned long __kernel_ino_t;
typedef unsigned int __kernel_mode_t;
typedef unsigned long __kernel_nlink_t;
typedef long __kernel_off_t;
typedef int __kernel_pid_t;
typedef int __kernel_ipc_pid_t;
typedef unsigned int __kernel_uid_t;
typedef unsigned int __kernel_gid_t;
typedef unsigned long __kernel_size_t;
typedef long __kernel_ssize_t;
typedef long __kernel_ptrdiff_t;
typedef long __kernel_time_t;
typedef long __kernel_suseconds_t;
typedef long __kernel_clock_t;
typedef int __kernel_timer_t;
typedef int __kernel_clockid_t;
typedef int __kernel_daddr_t;
typedef char * __kernel_caddr_t;
typedef unsigned short __kernel_uid16_t;
typedef unsigned short __kernel_gid16_t;


typedef long long __kernel_loff_t;


typedef struct {
 int val[2];
} __kernel_fsid_t;

typedef unsigned short __kernel_old_uid_t;
typedef unsigned short __kernel_old_gid_t;
typedef __kernel_uid_t __kernel_uid32_t;
typedef __kernel_gid_t __kernel_gid32_t;

typedef unsigned long __kernel_old_dev_t;
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/posix_types.h" 2 3
# 48 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/posix_types.h" 2 3
# 15 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/types.h" 2 3
# 182 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/types.h" 3
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

typedef __u16 __sum16;
typedef __u32 __wsum;
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/thread_info.h" 2 3

struct timespec;
struct compat_timespec;




struct restart_block {
 long (*fn)(struct restart_block *);
 union {
  struct {
   unsigned long arg0, arg1, arg2, arg3;
  };

  struct {
   u32 *uaddr;
   u32 val;
   u32 flags;
   u32 bitset;
   u64 time;
   u32 *uaddr2;
  } futex;

  struct {
   clockid_t index;
   struct timespec *rmtp;



   u64 expires;
  } nanosleep;

  struct {
   struct pollfd *ufds;
   int nfds;
   int has_timeout;
   unsigned long tv_sec;
   unsigned long tv_nsec;
  } poll;
 };
};

extern long do_no_restart_syscall(struct restart_block *parm);

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 1 3
# 28 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 1 3
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 1 3





# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/stringify.h" 1 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/asm.h" 1 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 2 3
# 43 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cpufeature.h" 1 3






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/required-features.h" 1 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cpufeature.h" 2 3
# 44 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 2 3

struct alt_instr {
 u8 *instr;
 u8 *replacement;
 u16 cpuid;
 u8 instrlen;
 u8 replacementlen;



};

extern void alternative_instructions(void);
extern void apply_alternatives(struct alt_instr *start, struct alt_instr *end,
          int fixup);

struct module;
# 70 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 3
static inline void alternatives_smp_module_add(struct module *mod, char *name,
            void *locks, void *locks_end,
            void *text, void *text_end) {}
static inline void alternatives_smp_module_del(struct module *mod) {}
static inline void alternatives_smp_switch(int smp) {}
static inline int alternatives_text_reserved(void *start, void *end)
{
 return 0;
}
# 146 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 3
struct paravirt_patch_site;




static inline void apply_paravirt(struct paravirt_patch_site *start,
      struct paravirt_patch_site *end)
{}
# 175 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/alternative.h" 3
extern void *text_poke(void *addr, const void *opcode, size_t len);
extern void *text_poke_smp(void *addr, const void *opcode, size_t len);
# 17 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 2 3
# 61 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static __always_inline void
set_bit(unsigned int nr, volatile unsigned long *addr)
{
 if ((__builtin_constant_p(nr))) {
  asm volatile("" "orb %1,%0"
   : "+m" (*(volatile long *) ((void *)(addr) + ((nr)>>3)))
   : "iq" ((u8)(1 << ((nr) & 7)))
   : "memory");
 } else {
  asm volatile("" "bts %1,%0"
   : "+m" (*(volatile long *) (addr)) : "Ir" (nr) : "memory");
 }
}
# 84 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline void __set_bit(int nr, volatile unsigned long *addr)
{
 asm volatile("bts %1,%0" : "+m" (*(volatile long *) (addr)) : "Ir" (nr) : "memory");
}
# 99 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static __always_inline void
clear_bit(int nr, volatile unsigned long *addr)
{
 if ((__builtin_constant_p(nr))) {
  asm volatile("" "andb %1,%0"
   : "+m" (*(volatile long *) ((void *)(addr) + ((nr)>>3)))
   : "iq" ((u8)~(1 << ((nr) & 7))));
 } else {
  asm volatile("" "btr %1,%0"
   : "+m" (*(volatile long *) (addr))
   : "Ir" (nr));
 }
}
# 121 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline void clear_bit_unlock(unsigned nr, volatile unsigned long *addr)
{
 barrier();
 clear_bit(nr, addr);
}

static inline void __clear_bit(int nr, volatile unsigned long *addr)
{
 asm volatile("btr %1,%0" : "+m" (*(volatile long *) (addr)) : "Ir" (nr));
}
# 144 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline void __clear_bit_unlock(unsigned nr, volatile unsigned long *addr)
{
 barrier();
 __clear_bit(nr, addr);
}
# 162 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline void __change_bit(int nr, volatile unsigned long *addr)
{
 asm volatile("btc %1,%0" : "+m" (*(volatile long *) (addr)) : "Ir" (nr));
}
# 176 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline void change_bit(int nr, volatile unsigned long *addr)
{
 if ((__builtin_constant_p(nr))) {
  asm volatile("" "xorb %1,%0"
   : "+m" (*(volatile long *) ((void *)(addr) + ((nr)>>3)))
   : "iq" ((u8)(1 << ((nr) & 7))));
 } else {
  asm volatile("" "btc %1,%0"
   : "+m" (*(volatile long *) (addr))
   : "Ir" (nr));
 }
}
# 197 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm volatile("" "bts %2,%1\n\t"
       "sbb %0,%0" : "=r" (oldbit), "+m" (*(volatile long *) (addr)) : "Ir" (nr) : "memory");

 return oldbit;
}
# 214 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static __always_inline int
test_and_set_bit_lock(int nr, volatile unsigned long *addr)
{
 return test_and_set_bit(nr, addr);
}
# 229 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline int __test_and_set_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm("bts %2,%1\n\t"
     "sbb %0,%0"
     : "=r" (oldbit), "+m" (*(volatile long *) (addr))
     : "Ir" (nr));
 return oldbit;
}
# 248 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm volatile("" "btr %2,%1\n\t"
       "sbb %0,%0"
       : "=r" (oldbit), "+m" (*(volatile long *) (addr)) : "Ir" (nr) : "memory");

 return oldbit;
}
# 275 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline int __test_and_clear_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm volatile("btr %2,%1\n\t"
       "sbb %0,%0"
       : "=r" (oldbit), "+m" (*(volatile long *) (addr))
       : "Ir" (nr));
 return oldbit;
}


static inline int __test_and_change_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm volatile("btc %2,%1\n\t"
       "sbb %0,%0"
       : "=r" (oldbit), "+m" (*(volatile long *) (addr))
       : "Ir" (nr) : "memory");

 return oldbit;
}
# 307 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{
 int oldbit;

 asm volatile("" "btc %2,%1\n\t"
       "sbb %0,%0"
       : "=r" (oldbit), "+m" (*(volatile long *) (addr)) : "Ir" (nr) : "memory");

 return oldbit;
}
# 356 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static __always_inline int constant_test_bit(unsigned int nr, const volatile unsigned long *addr)
{
 return ((1UL << (nr % BITS_PER_LONG)) &
  (((unsigned long *)addr)[nr / BITS_PER_LONG])) != 0;
}

static inline int variable_test_bit(int nr, volatile const unsigned long *addr)
{
 int oldbit;

 asm volatile("bt %2,%1\n\t"
       "sbb %0,%0"
       : "=r" (oldbit)
       : "m" (*(unsigned long *)addr), "Ir" (nr));

 return oldbit;
}
# 394 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
static inline unsigned long __ffs(unsigned long word)
{
 asm("bsf %1,%0"
  : "=r" (word)
  : "rm" (word));
 return word;
}







static inline unsigned long ffz(unsigned long word)
{
 asm("bsf %1,%0"
  : "=r" (word)
  : "r" (~word));
 return word;
}







static inline unsigned long __fls(unsigned long word)
{
 asm("bsr %1,%0"
     : "=r" (word)
     : "rm" (word));
 return word;
}
# 498 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/bitops/fls64.h" 1 3
# 499 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/bitops.h" 2 3
# 29 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 2 3
# 44 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 3
static __inline__ int get_bitmask_order(unsigned int count)
{
 int order;

 order = fls(count);
 return order;
}

static __inline__ int get_count_order(unsigned int count)
{
 int order;

 order = fls(count) - 1;
 if (count & (count - 1))
  order++;
 return order;
}

static inline unsigned long hweight_long(unsigned long w)
{
 return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}
# 97 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 3
static inline __u32 rol32(__u32 word, unsigned int shift)
{
 return (word << shift) | (word >> (32 - shift));
}






static inline __u32 ror32(__u32 word, unsigned int shift)
{
 return (word >> shift) | (word << (32 - shift));
}






static inline __u16 rol16(__u16 word, unsigned int shift)
{
 return (word << shift) | (word >> (16 - shift));
}






static inline __u16 ror16(__u16 word, unsigned int shift)
{
 return (word >> shift) | (word << (16 - shift));
}






static inline __u8 rol8(__u8 word, unsigned int shift)
{
 return (word << shift) | (word >> (8 - shift));
}






static inline __u8 ror8(__u8 word, unsigned int shift)
{
 return (word >> shift) | (word << (8 - shift));
}






static inline __s32 sign_extend32(__u32 value, int index)
{
 __u8 shift = 31 - index;
 return (__s32)(value << shift) >> shift;
}

static inline unsigned fls_long(unsigned long l)
{
 if (sizeof(l) == 4)
  return fls(l);
 return fls64(l);
}
# 178 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitops.h" 3
static inline unsigned long __ffs64(u64 word)
{






 return __ffs((unsigned long)word);
}
# 56 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/thread_info.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page.h" 1 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 2 3
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 3
struct task_struct;
struct exec_domain;
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor-flags.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3


struct task_struct;
struct mm_struct;

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/vm86.h" 1 3
# 62 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/vm86.h" 3
struct vm86_regs {



 long ebx;
 long ecx;
 long edx;
 long esi;
 long edi;
 long ebp;
 long eax;
 long __null_ds;
 long __null_es;
 long __null_fs;
 long __null_gs;
 long orig_eax;
 long eip;
 unsigned short cs, __csh;
 long eflags;
 long esp;
 unsigned short ss, __ssh;



 unsigned short es, __esh;
 unsigned short ds, __dsh;
 unsigned short fs, __fsh;
 unsigned short gs, __gsh;
};

struct revectored_struct {
 unsigned long __map[8];
};

struct vm86_struct {
 struct vm86_regs regs;
 unsigned long flags;
 unsigned long screen_bitmap;
 unsigned long cpu_type;
 struct revectored_struct int_revectored;
 struct revectored_struct int21_revectored;
};






struct vm86plus_info_struct {
 unsigned long force_return_for_pic:1;
 unsigned long vm86dbg_active:1;
 unsigned long vm86dbg_TFpendig:1;
 unsigned long unused:28;
 unsigned long is_vm86pus:1;
 unsigned char vm86dbg_intxxtab[32];
};
struct vm86plus_struct {
 struct vm86_regs regs;
 unsigned long flags;
 unsigned long screen_bitmap;
 unsigned long cpu_type;
 struct revectored_struct int_revectored;
 struct revectored_struct int21_revectored;
 struct vm86plus_info_struct vm86plus;
};
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/math_emu.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ptrace.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ptrace-abi.h" 1 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ptrace.h" 2 3
# 69 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ptrace.h" 3
struct pt_regs {
 unsigned long r15;
 unsigned long r14;
 unsigned long r13;
 unsigned long r12;
 unsigned long rbp;
 unsigned long rbx;

 unsigned long r11;
 unsigned long r10;
 unsigned long r9;
 unsigned long r8;
 unsigned long rax;
 unsigned long rcx;
 unsigned long rdx;
 unsigned long rsi;
 unsigned long rdi;
 unsigned long orig_rax;


 unsigned long rip;
 unsigned long cs;
 unsigned long eflags;
 unsigned long rsp;
 unsigned long ss;

};
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/math_emu.h" 2 3






struct math_emu_info {
 long ___orig_eip;
 union {
  struct pt_regs *regs;
  struct kernel_vm86_regs *vm86;
 };
};
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/segment.h" 1 3
# 146 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/segment.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cache.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/linkage.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/linkage.h" 1 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/linkage.h" 2 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cache.h" 2 3
# 147 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/segment.h" 2 3
# 13 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/sigcontext.h" 1 3
# 23 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/sigcontext.h" 3
struct _fpx_sw_bytes {
 __u32 magic1;
 __u32 extended_size;


 __u64 xstate_bv;




 __u32 xstate_size;




 __u32 padding[7];
};
# 169 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/sigcontext.h" 3
struct _fpstate {
 __u16 cwd;
 __u16 swd;
 __u16 twd;

 __u16 fop;
 __u64 rip;
 __u64 rdp;
 __u32 mxcsr;
 __u32 mxcsr_mask;
 __u32 st_space[32];
 __u32 xmm_space[64];
 __u32 reserved2[12];
 union {
  __u32 reserved3[12];
  struct _fpx_sw_bytes sw_reserved;

 };
};
# 232 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/sigcontext.h" 3
struct sigcontext {
 unsigned long r8;
 unsigned long r9;
 unsigned long r10;
 unsigned long r11;
 unsigned long r12;
 unsigned long r13;
 unsigned long r14;
 unsigned long r15;
 unsigned long rdi;
 unsigned long rsi;
 unsigned long rbp;
 unsigned long rbx;
 unsigned long rdx;
 unsigned long rax;
 unsigned long rcx;
 unsigned long rsp;
 unsigned long rip;
 unsigned long eflags;
 unsigned short cs;
 unsigned short gs;
 unsigned short fs;
 unsigned short __pad0;
 unsigned long err;
 unsigned long trapno;
 unsigned long oldmask;
 unsigned long cr2;
 struct _fpstate *fpstate;
 unsigned long reserved1[8];
};




struct _xsave_hdr {
 __u64 xstate_bv;
 __u64 reserved1[2];
 __u64 reserved2[5];
};

struct _ymmh_state {

 __u32 ymmh_space[64];
};







struct _xstate {
 struct _fpstate fpstate;
 struct _xsave_hdr xstate_hdr;
 struct _ymmh_state ymmh;

};
# 15 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/current.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 1 3
# 45 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 997 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 3
struct sysinfo {
 long uptime;
 unsigned long loads[3];
 unsigned long totalram;
 unsigned long freeram;
 unsigned long sharedram;
 unsigned long bufferram;
 unsigned long totalswap;
 unsigned long freeswap;
 unsigned short procs;
 unsigned short pad;
 unsigned long totalhigh;
 unsigned long freehigh;
 unsigned int mem_unit;
 char _f[20-2*sizeof(long)-sizeof(int)];
};
# 1053 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 3
struct module;

void mark_hardware_unsupported(const char *msg);
void mark_tech_preview(const char *msg, struct module *mod);
# 46 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 2 3
# 73 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 3
extern void __bad_percpu_size(void);
# 190 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/percpu.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/threads.h" 1 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/percpu.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu-defs.h" 1 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/percpu.h" 2 3
# 191 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/percpu.h" 2 3


extern __attribute__((section(".data" ""))) __typeof__(unsigned long) per_cpu__this_cpu_off;
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/current.h" 2 3


struct task_struct;

extern __attribute__((section(".data" ""))) __typeof__(struct task_struct *) per_cpu__current_task;

static __always_inline struct task_struct *get_current(void)
{
 return ({ typeof(per_cpu__current_task) ret__; switch (sizeof(per_cpu__current_task)) { case 1: asm("mov" "b ""%P" "1"",%0" : "=q" (ret__) : "p" (&per_cpu__current_task)); break; case 2: asm("mov" "w ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__current_task)); break; case 4: asm("mov" "l ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__current_task)); break; case 8: asm("mov" "q ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__current_task)); break; default: __bad_percpu_size(); } ret__; });
}
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 1 3






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg_64.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg_64.h" 3
static inline void set_64bit(volatile unsigned long *ptr, unsigned long val)
{
 *ptr = val;
}
# 23 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg_64.h" 3
static inline unsigned long __xchg(unsigned long x, volatile void *ptr,
       int size)
{
 switch (size) {
 case 1:
  asm volatile("xchgb %b0,%1"
        : "=q" (x)
        : "m" (*((volatile long *)(ptr))), "0" (x)
        : "memory");
  break;
 case 2:
  asm volatile("xchgw %w0,%1"
        : "=r" (x)
        : "m" (*((volatile long *)(ptr))), "0" (x)
        : "memory");
  break;
 case 4:
  asm volatile("xchgl %k0,%1"
        : "=r" (x)
        : "m" (*((volatile long *)(ptr))), "0" (x)
        : "memory");
  break;
 case 8:
  asm volatile("xchgq %0,%1"
        : "=r" (x)
        : "m" (*((volatile long *)(ptr))), "0" (x)
        : "memory");
  break;
 }
 return x;
}
# 63 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg_64.h" 3
static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old,
          unsigned long new, int size)
{
 unsigned long prev;
 switch (size) {
 case 1:
  asm volatile("" "cmpxchgb %b1,%2"
        : "=a"(prev)
        : "q"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 2:
  asm volatile("" "cmpxchgw %w1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 4:
  asm volatile("" "cmpxchgl %k1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 8:
  asm volatile("" "cmpxchgq %1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 }
 return old;
}






static inline unsigned long __sync_cmpxchg(volatile void *ptr,
        unsigned long old,
        unsigned long new, int size)
{
 unsigned long prev;
 switch (size) {
 case 1:
  asm volatile("lock; cmpxchgb %b1,%2"
        : "=a"(prev)
        : "q"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 2:
  asm volatile("lock; cmpxchgw %w1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 4:
  asm volatile("lock; cmpxchgl %1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 }
 return old;
}

static inline unsigned long __cmpxchg_local(volatile void *ptr,
         unsigned long old,
         unsigned long new, int size)
{
 unsigned long prev;
 switch (size) {
 case 1:
  asm volatile("cmpxchgb %b1,%2"
        : "=a"(prev)
        : "q"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 2:
  asm volatile("cmpxchgw %w1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 4:
  asm volatile("cmpxchgl %k1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 case 8:
  asm volatile("cmpxchgq %1,%2"
        : "=a"(prev)
        : "r"(new), "m"(*((volatile long *)(ptr))), "0"(old)
        : "memory");
  return prev;
 }
 return old;
}
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg.h" 2 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/nops.h" 1 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/irqflags.h" 1 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 2 3
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 3
struct task_struct;
struct task_struct *__switch_to(struct task_struct *prev,
    struct task_struct *next);
struct tss_struct;
void __switch_to_xtra(struct task_struct *prev_p, struct task_struct *next_p,
        struct tss_struct *tss);
extern void show_regs_common(void);
# 332 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 3
static inline void clflush(volatile void *__p)
{
 asm volatile("clflush %0" : "+m" (*(volatile char *)__p));
}



void disable_hlt(void);
void enable_hlt(void);

void cpu_idle_wait(void);

extern unsigned long arch_align_stack(unsigned long sp);
extern void free_init_pages(char *what, unsigned long begin, unsigned long end);

void default_idle(void);

void stop_this_cpu(void *dummy);
# 453 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/system.h" 3
static inline void rdtsc_barrier(void)
{
 asm volatile ("661:\n\t" ".byte 0x8d,0x76,0x00\n" "\n662:\n" ".section .altinstructions,\"a\"\n" " " ".balign 8" " " "\n" " " ".quad" " " "661b\n" " " ".quad" " " "663f\n" "	 .word " "(3*32+17)" "\n" "	 .byte 662b-661b\n" "	 .byte 664f-663f\n" ".previous\n" ".section .discard,\"aw\",@progbits\n" "	 .byte 0xff + (664f-663f) - (662b-661b)\n" ".previous\n" ".section .altinstr_replacement, \"ax\"\n" "663:\n\t" "mfence" "\n664:\n" ".previous" : : : "memory");
 asm volatile ("661:\n\t" ".byte 0x8d,0x76,0x00\n" "\n662:\n" ".section .altinstructions,\"a\"\n" " " ".balign 8" " " "\n" " " ".quad" " " "661b\n" " " ".quad" " " "663f\n" "	 .word " "(3*32+18)" "\n" "	 .byte 662b-661b\n" "	 .byte 664f-663f\n" ".previous\n" ".section .discard,\"aw\",@progbits\n" "	 .byte 0xff + (664f-663f) - (662b-661b)\n" ".previous\n" ".section .altinstr_replacement, \"ax\"\n" "663:\n\t" "lfence" "\n664:\n" ".previous" : : : "memory");
}
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/const.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page_types.h" 1 3
# 38 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page_types.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page_32_types.h" 1 3
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page_32_types.h" 3
extern unsigned int __VMALLOC_RESERVE;
extern int sysctl_legacy_va_layout;

extern void find_low_pfn_range(void);
extern void setup_bootmem_allocator(void);
# 39 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/page_types.h" 2 3




extern int devmem_is_allowed(unsigned long pagenr);

extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;

extern unsigned long init_memory_mapping(unsigned long start,
      unsigned long end);

extern void initmem_init(unsigned long start_pfn, unsigned long end_pfn);
extern void free_initmem(void);
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 2 3
# 180 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_64_types.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_64_types.h" 3
typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;
typedef unsigned long pgprotval_t;

typedef struct { pteval_t pte; } pte_t;
# 181 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 2 3
# 193 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
typedef struct pgprot { pgprotval_t pgprot; } pgprot_t;

typedef struct { pgdval_t pgd; } pgd_t;

static inline pgd_t native_make_pgd(pgdval_t val)
{
 return (pgd_t) { val };
}

static inline pgdval_t native_pgd_val(pgd_t pgd)
{
 return pgd.pgd;
}

static inline pgdval_t pgd_flags(pgd_t pgd)
{
 return native_pgd_val(pgd) & (~((pteval_t)(((signed long)(~(((1UL) << 12)-1))) & ((phys_addr_t)(1ULL << 32) - 1))));
}


typedef struct { pudval_t pud; } pud_t;

static inline pud_t native_make_pud(pmdval_t val)
{
 return (pud_t) { val };
}

static inline pudval_t native_pud_val(pud_t pud)
{
 return pud.pud;
}
# 234 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
typedef struct { pmdval_t pmd; } pmd_t;

static inline pmd_t native_make_pmd(pmdval_t val)
{
 return (pmd_t) { val };
}

static inline pmdval_t native_pmd_val(pmd_t pmd)
{
 return pmd.pmd;
}
# 254 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
static inline pudval_t pud_flags(pud_t pud)
{
 return native_pud_val(pud) & (~((pteval_t)(((signed long)(~(((1UL) << 12)-1))) & ((phys_addr_t)(1ULL << 32) - 1))));
}

static inline pmdval_t pmd_flags(pmd_t pmd)
{
 return native_pmd_val(pmd) & (~((pteval_t)(((signed long)(~(((1UL) << 12)-1))) & ((phys_addr_t)(1ULL << 32) - 1))));
}

static inline pte_t native_make_pte(pteval_t val)
{
 return (pte_t) { .pte = val };
}

static inline pteval_t native_pte_val(pte_t pte)
{
 return pte.pte;
}

static inline pteval_t pte_flags(pte_t pte)
{
 return native_pte_val(pte) & (~((pteval_t)(((signed long)(~(((1UL) << 12)-1))) & ((phys_addr_t)(1ULL << 32) - 1))));
}





typedef struct page *pgtable_t;

extern pteval_t __supported_pte_mask;
extern void set_nx(void);
extern int nx_enabled;


extern pgprot_t pgprot_writecombine(pgprot_t prot);





struct file;
pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
                              unsigned long size, pgprot_t vma_prot);
int phys_mem_access_prot_allowed(struct file *file, unsigned long pfn,
                              unsigned long size, pgprot_t *vma_prot);


void set_pte_vaddr(unsigned long vaddr, pte_t pte);
# 313 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
struct seq_file;
extern void arch_report_meminfo(struct seq_file *m);

enum {
 PG_LEVEL_NONE,
 PG_LEVEL_4K,
 PG_LEVEL_2M,
 PG_LEVEL_1G,
 PG_LEVEL_NUM
};




static inline void update_page_count(int level, unsigned long pages) { }
# 336 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/pgtable_types.h" 3
extern pte_t *lookup_address(unsigned long address, unsigned int *level);
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/msr.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/msr-index.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/msr.h" 2 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ioctl.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ioctl.h" 1 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/ioctl.h" 1 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ioctl.h" 2 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ioctl.h" 2 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/msr.h" 2 3
# 22 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/desc_defs.h" 1 3
# 22 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/desc_defs.h" 3
struct desc_struct {
 union {
  struct {
   unsigned int a;
   unsigned int b;
  };
  struct {
   u16 limit0;
   u16 base0;
   unsigned base1: 8, type: 4, s: 1, dpl: 2, p: 1;
   unsigned limit: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
  };
 };
} __attribute__((packed));







enum {
 GATE_INTERRUPT = 0xE,
 GATE_TRAP = 0xF,
 GATE_CALL = 0xC,
 GATE_TASK = 0x5,
};


struct gate_struct64 {
 u16 offset_low;
 u16 segment;
 unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
 u16 offset_middle;
 u32 offset_high;
 u32 zero1;
} __attribute__((packed));





enum {
 DESC_TSS = 0x9,
 DESC_LDT = 0x2,
 DESCTYPE_S = 0x10,
};


struct ldttss_desc64 {
 u16 limit0;
 u16 base0;
 unsigned base1 : 8, type : 5, dpl : 2, p : 1;
 unsigned limit1 : 4, zero0 : 3, g : 1, base2 : 8;
 u32 base3;
 u32 zero1;
} __attribute__((packed));
# 87 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/desc_defs.h" 3
typedef struct desc_struct gate_desc;
typedef struct desc_struct ldt_desc;
typedef struct desc_struct tss_desc;




struct desc_ptr {
 unsigned short size;
 unsigned long address;
} __attribute__((packed)) ;
# 23 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/personality.h" 1 3
# 24 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/personality.h" 3
enum {
 ADDR_NO_RANDOMIZE = 0x0040000,
 FDPIC_FUNCPTRS = 0x0080000,


 MMAP_PAGE_ZERO = 0x0100000,
 ADDR_COMPAT_LAYOUT = 0x0200000,
 READ_IMPLIES_EXEC = 0x0400000,
 ADDR_LIMIT_32BIT = 0x0800000,
 SHORT_INODE = 0x1000000,
 WHOLE_SECONDS = 0x2000000,
 STICKY_TIMEOUTS = 0x4000000,
 ADDR_LIMIT_3GB = 0x8000000,
};
# 54 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/personality.h" 3
enum {
 PER_LINUX = 0x0000,
 PER_LINUX_32BIT = 0x0000 | ADDR_LIMIT_32BIT,
 PER_LINUX_FDPIC = 0x0000 | FDPIC_FUNCPTRS,
 PER_SVR4 = 0x0001 | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
 PER_SVR3 = 0x0002 | STICKY_TIMEOUTS | SHORT_INODE,
 PER_SCOSVR3 = 0x0003 | STICKY_TIMEOUTS |
      WHOLE_SECONDS | SHORT_INODE,
 PER_OSR5 = 0x0003 | STICKY_TIMEOUTS | WHOLE_SECONDS,
 PER_WYSEV386 = 0x0004 | STICKY_TIMEOUTS | SHORT_INODE,
 PER_ISCR4 = 0x0005 | STICKY_TIMEOUTS,
 PER_BSD = 0x0006,
 PER_SUNOS = 0x0006 | STICKY_TIMEOUTS,
 PER_XENIX = 0x0007 | STICKY_TIMEOUTS | SHORT_INODE,
 PER_LINUX32 = 0x0008,
 PER_LINUX32_3GB = 0x0008 | ADDR_LIMIT_3GB,
 PER_IRIX32 = 0x0009 | STICKY_TIMEOUTS,
 PER_IRIXN32 = 0x000a | STICKY_TIMEOUTS,
 PER_IRIX64 = 0x000b | STICKY_TIMEOUTS,
 PER_RISCOS = 0x000c,
 PER_SOLARIS = 0x000d | STICKY_TIMEOUTS,
 PER_UW7 = 0x000e | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
 PER_OSF4 = 0x000f,
 PER_HPUX = 0x0010,
 PER_MASK = 0x00ff,
};
# 26 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 1 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitmap.h" 1 3







# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/string.h" 1 3






# 1 "/usr/include/string.h" 1 3 4
# 27 "/usr/include/string.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 361 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 373 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 374 "/usr/include/sys/cdefs.h" 2 3 4
# 362 "/usr/include/features.h" 2 3 4
# 385 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4




# 1 "/usr/include/gnu/stubs-64.h" 1 3 4
# 10 "/usr/include/gnu/stubs.h" 2 3 4
# 386 "/usr/include/features.h" 2 3 4
# 28 "/usr/include/string.h" 2 3 4






# 1 "/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include/stddef.h" 1 3 4
# 211 "/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 35 "/usr/include/string.h" 2 3 4









extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 95 "/usr/include/string.h" 3 4
extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 126 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));






# 1 "/usr/include/xlocale.h" 1 3 4
# 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 163 "/usr/include/string.h" 2 3 4


extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 210 "/usr/include/string.h" 3 4

# 235 "/usr/include/string.h" 3 4
extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 262 "/usr/include/string.h" 3 4
extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 281 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 314 "/usr/include/string.h" 3 4
extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 342 "/usr/include/string.h" 3 4
extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 397 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (__const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 427 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__))

                        __attribute__ ((__nonnull__ (2)));
# 445 "/usr/include/string.h" 3 4
extern char *strerror_l (int __errnum, __locale_t __l) __attribute__ ((__nothrow__));





extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 489 "/usr/include/string.h" 3 4
extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 517 "/usr/include/string.h" 3 4
extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 536 "/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 559 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__));


extern char *__stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 646 "/usr/include/string.h" 3 4

# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/string.h" 2 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitmap.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitmap.h" 2 3
# 91 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitmap.h" 3
extern int __bitmap_empty(const unsigned long *bitmap, int bits);
extern int __bitmap_full(const unsigned long *bitmap, int bits);
extern int __bitmap_equal(const unsigned long *bitmap1,
                 const unsigned long *bitmap2, int bits);
extern void __bitmap_complement(unsigned long *dst, const unsigned long *src,
   int bits);
extern void __bitmap_shift_right(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern void __bitmap_shift_left(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_intersects(const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_subset(const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_weight(const unsigned long *bitmap, int bits);

extern void bitmap_set(unsigned long *map, int i, int len);
extern void bitmap_clear(unsigned long *map, int start, int nr);
extern unsigned long bitmap_find_next_zero_area(unsigned long *map,
      unsigned long size,
      unsigned long start,
      unsigned int nr,
      unsigned long align_mask);

extern int bitmap_scnprintf(char *buf, unsigned int len,
   const unsigned long *src, int nbits);
extern int __bitmap_parse(const char *buf, unsigned int buflen, int is_user,
   unsigned long *dst, int nbits);
extern int bitmap_parse_user(const char *ubuf, unsigned int ulen,
   unsigned long *dst, int nbits);
extern int bitmap_scnlistprintf(char *buf, unsigned int len,
   const unsigned long *src, int nbits);
extern int bitmap_parselist(const char *buf, unsigned long *maskp,
   int nmaskbits);
extern int bitmap_parselist_user(const char *ubuf, unsigned int ulen,
   unsigned long *dst, int nbits);
extern void bitmap_remap(unsigned long *dst, const unsigned long *src,
  const unsigned long *old, const unsigned long *new, int bits);
extern int bitmap_bitremap(int oldbit,
  const unsigned long *old, const unsigned long *new, int bits);
extern void bitmap_onto(unsigned long *dst, const unsigned long *orig,
  const unsigned long *relmap, int bits);
extern void bitmap_fold(unsigned long *dst, const unsigned long *orig,
  int sz, int bits);
extern int bitmap_find_free_region(unsigned long *bitmap, int bits, int order);
extern void bitmap_release_region(unsigned long *bitmap, int pos, int order);
extern int bitmap_allocate_region(unsigned long *bitmap, int pos, int order);
extern void bitmap_copy_le(void *dst, const unsigned long *src, int nbits);
# 158 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bitmap.h" 3
static inline void bitmap_zero(unsigned long *dst, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = 0UL;
 else {
  int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
  memset(dst, 0, len);
 }
}

static inline void bitmap_fill(unsigned long *dst, int nbits)
{
 size_t nlongs = BITS_TO_LONGS(nbits);
 if (!(__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG)) {
  int len = (nlongs - 1) * sizeof(unsigned long);
  memset(dst, 0xff, len);
 }
 dst[nlongs - 1] = ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL );
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src,
   int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = *src;
 else {
  int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
  memcpy(dst, src, len);
 }
}

static inline int bitmap_and(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return (*dst = *src1 & *src2) != 0;
 return __bitmap_and(dst, src1, src2, nbits);
}

static inline void bitmap_or(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = *src1 | *src2;
 else
  __bitmap_or(dst, src1, src2, nbits);
}

static inline void bitmap_xor(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = *src1 ^ *src2;
 else
  __bitmap_xor(dst, src1, src2, nbits);
}

static inline int bitmap_andnot(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return (*dst = *src1 & ~(*src2)) != 0;
 return __bitmap_andnot(dst, src1, src2, nbits);
}

static inline void bitmap_complement(unsigned long *dst, const unsigned long *src,
   int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = ~(*src) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL );
 else
  __bitmap_complement(dst, src, nbits);
}

static inline int bitmap_equal(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return ! ((*src1 ^ *src2) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL ));
 else
  return __bitmap_equal(src1, src2, nbits);
}

static inline int bitmap_intersects(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return ((*src1 & *src2) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL )) != 0;
 else
  return __bitmap_intersects(src1, src2, nbits);
}

static inline int bitmap_subset(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return ! ((*src1 & ~(*src2)) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL ));
 else
  return __bitmap_subset(src1, src2, nbits);
}

static inline int bitmap_empty(const unsigned long *src, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return ! (*src & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL ));
 else
  return __bitmap_empty(src, nbits);
}

static inline int bitmap_full(const unsigned long *src, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return ! (~(*src) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL ));
 else
  return __bitmap_full(src, nbits);
}

static inline int bitmap_weight(const unsigned long *src, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  return hweight_long(*src & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL ));
 return __bitmap_weight(src, nbits);
}

static inline void bitmap_shift_right(unsigned long *dst,
   const unsigned long *src, int n, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = *src >> n;
 else
  __bitmap_shift_right(dst, src, n, nbits);
}

static inline void bitmap_shift_left(unsigned long *dst,
   const unsigned long *src, int n, int nbits)
{
 if ((__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG))
  *dst = (*src << n) & ( ((nbits) % BITS_PER_LONG) ? (1UL<<((nbits) % BITS_PER_LONG))-1 : ~0UL );
 else
  __bitmap_shift_left(dst, src, n, nbits);
}

static inline int bitmap_parse(const char *buf, unsigned int buflen,
   unsigned long *maskp, int nmaskbits)
{
 return __bitmap_parse(buf, buflen, 0, maskp, nmaskbits);
}
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 2 3

typedef struct cpumask { DECLARE_BITMAP(bits, 1); } cpumask_t;
# 78 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
extern const struct cpumask *const cpu_possible_mask;
extern const struct cpumask *const cpu_online_mask;
extern const struct cpumask *const cpu_present_mask;
extern const struct cpumask *const cpu_active_mask;
# 104 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline unsigned int cpumask_check(unsigned int cpu)
{



 return cpu;
}



static inline unsigned int cpumask_first(const struct cpumask *srcp)
{
 return 0;
}


static inline unsigned int cpumask_next(int n, const struct cpumask *srcp)
{
 return n+1;
}

static inline unsigned int cpumask_next_zero(int n, const struct cpumask *srcp)
{
 return n+1;
}

static inline unsigned int cpumask_next_and(int n,
         const struct cpumask *srcp,
         const struct cpumask *andp)
{
 return n+1;
}


static inline unsigned int cpumask_any_but(const struct cpumask *mask,
        unsigned int cpu)
{
 return 1;
}
# 240 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline void cpumask_set_cpu(unsigned int cpu, struct cpumask *dstp)
{
 set_bit(cpumask_check(cpu), ((dstp)->bits));
}






static inline void cpumask_clear_cpu(int cpu, struct cpumask *dstp)
{
 clear_bit(cpumask_check(cpu), ((dstp)->bits));
}
# 272 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpumask_test_and_set_cpu(int cpu, struct cpumask *cpumask)
{
 return test_and_set_bit(cpumask_check(cpu), ((cpumask)->bits));
}
# 284 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpumask_test_and_clear_cpu(int cpu, struct cpumask *cpumask)
{
 return test_and_clear_bit(cpumask_check(cpu), ((cpumask)->bits));
}





static inline void cpumask_setall(struct cpumask *dstp)
{
 bitmap_fill(((dstp)->bits), 1);
}





static inline void cpumask_clear(struct cpumask *dstp)
{
 bitmap_zero(((dstp)->bits), 1);
}







static inline int cpumask_and(struct cpumask *dstp,
          const struct cpumask *src1p,
          const struct cpumask *src2p)
{
 return bitmap_and(((dstp)->bits), ((src1p)->bits),
           ((src2p)->bits), 1);
}







static inline void cpumask_or(struct cpumask *dstp, const struct cpumask *src1p,
         const struct cpumask *src2p)
{
 bitmap_or(((dstp)->bits), ((src1p)->bits),
          ((src2p)->bits), 1);
}







static inline void cpumask_xor(struct cpumask *dstp,
          const struct cpumask *src1p,
          const struct cpumask *src2p)
{
 bitmap_xor(((dstp)->bits), ((src1p)->bits),
           ((src2p)->bits), 1);
}







static inline int cpumask_andnot(struct cpumask *dstp,
      const struct cpumask *src1p,
      const struct cpumask *src2p)
{
 return bitmap_andnot(((dstp)->bits), ((src1p)->bits),
       ((src2p)->bits), 1);
}






static inline void cpumask_complement(struct cpumask *dstp,
          const struct cpumask *srcp)
{
 bitmap_complement(((dstp)->bits), ((srcp)->bits),
           1);
}






static inline bool cpumask_equal(const struct cpumask *src1p,
    const struct cpumask *src2p)
{
 return bitmap_equal(((src1p)->bits), ((src2p)->bits),
       1);
}






static inline bool cpumask_intersects(const struct cpumask *src1p,
         const struct cpumask *src2p)
{
 return bitmap_intersects(((src1p)->bits), ((src2p)->bits),
            1);
}






static inline int cpumask_subset(const struct cpumask *src1p,
     const struct cpumask *src2p)
{
 return bitmap_subset(((src1p)->bits), ((src2p)->bits),
        1);
}





static inline bool cpumask_empty(const struct cpumask *srcp)
{
 return bitmap_empty(((srcp)->bits), 1);
}





static inline bool cpumask_full(const struct cpumask *srcp)
{
 return bitmap_full(((srcp)->bits), 1);
}





static inline unsigned int cpumask_weight(const struct cpumask *srcp)
{
 return bitmap_weight(((srcp)->bits), 1);
}







static inline void cpumask_shift_right(struct cpumask *dstp,
           const struct cpumask *srcp, int n)
{
 bitmap_shift_right(((dstp)->bits), ((srcp)->bits), n,
            1);
}







static inline void cpumask_shift_left(struct cpumask *dstp,
          const struct cpumask *srcp, int n)
{
 bitmap_shift_left(((dstp)->bits), ((srcp)->bits), n,
           1);
}






static inline void cpumask_copy(struct cpumask *dstp,
    const struct cpumask *srcp)
{
 bitmap_copy(((dstp)->bits), ((srcp)->bits), 1);
}
# 515 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpumask_scnprintf(char *buf, int len,
        const struct cpumask *srcp)
{
 return bitmap_scnprintf(buf, len, ((srcp)->bits), 1);
}
# 529 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpumask_parse_user(const char *buf, int len,
         struct cpumask *dstp)
{
 return bitmap_parse_user(buf, len, ((dstp)->bits), 1);
}
# 543 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpumask_parselist_user(const char *buf, int len,
         struct cpumask *dstp)
{
 return bitmap_parselist_user(buf, len, ((dstp)->bits),
       1);
}
# 559 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpulist_scnprintf(char *buf, int len,
        const struct cpumask *srcp)
{
 return bitmap_scnlistprintf(buf, len, ((srcp)->bits),
        1);
}
# 574 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int cpulist_parse(const char *buf, struct cpumask *dstp)
{
 return bitmap_parselist(buf, ((dstp)->bits), 1);
}






static inline size_t cpumask_size(void)
{


 return BITS_TO_LONGS(1) * sizeof(long);
}
# 619 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
typedef struct cpumask cpumask_var_t[1];

static inline bool alloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
 return true;
}

static inline bool alloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags,
       int node)
{
 return true;
}

static inline bool zalloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
 cpumask_clear(*mask);
 return true;
}

static inline bool zalloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags,
       int node)
{
 cpumask_clear(*mask);
 return true;
}

static inline void alloc_bootmem_cpumask_var(cpumask_var_t *mask)
{
}

static inline void free_cpumask_var(cpumask_var_t mask)
{
}

static inline void free_bootmem_cpumask_var(cpumask_var_t mask)
{
}




extern const DECLARE_BITMAP(cpu_all_bits, 1);
# 671 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
void set_cpu_possible(unsigned int cpu, bool possible);
void set_cpu_present(unsigned int cpu, bool present);
void set_cpu_online(unsigned int cpu, bool online);
void set_cpu_active(unsigned int cpu, bool active);
void init_cpu_present(const struct cpumask *src);
void init_cpu_possible(const struct cpumask *src);
void init_cpu_online(const struct cpumask *src);
# 693 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline int __check_is_bitmap(const unsigned long *bitmap)
{
 return 1;
}
# 705 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
extern const unsigned long
 cpu_bit_bitmap[BITS_PER_LONG+1][BITS_TO_LONGS(1)];

static inline const struct cpumask *get_cpu_mask(unsigned int cpu)
{
 const unsigned long *p = cpu_bit_bitmap[1 + cpu % BITS_PER_LONG];
 p -= cpu / BITS_PER_LONG;
 return ((struct cpumask *)(1 ? (p) : (void *)sizeof(__check_is_bitmap(p))));
}
# 812 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cpumask.h" 3
static inline void __cpu_set(int cpu, volatile cpumask_t *dstp)
{
 set_bit(cpu, dstp->bits);
}


static inline void __cpu_clear(int cpu, volatile cpumask_t *dstp)
{
 clear_bit(cpu, dstp->bits);
}


static inline void __cpus_setall(cpumask_t *dstp, int nbits)
{
 bitmap_fill(dstp->bits, nbits);
}


static inline void __cpus_clear(cpumask_t *dstp, int nbits)
{
 bitmap_zero(dstp->bits, nbits);
}





static inline int __cpu_test_and_set(int cpu, cpumask_t *addr)
{
 return test_and_set_bit(cpu, addr->bits);
}


static inline int __cpus_and(cpumask_t *dstp, const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 return bitmap_and(dstp->bits, src1p->bits, src2p->bits, nbits);
}


static inline void __cpus_or(cpumask_t *dstp, const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 bitmap_or(dstp->bits, src1p->bits, src2p->bits, nbits);
}


static inline void __cpus_xor(cpumask_t *dstp, const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 bitmap_xor(dstp->bits, src1p->bits, src2p->bits, nbits);
}



static inline int __cpus_andnot(cpumask_t *dstp, const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 return bitmap_andnot(dstp->bits, src1p->bits, src2p->bits, nbits);
}


static inline int __cpus_equal(const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 return bitmap_equal(src1p->bits, src2p->bits, nbits);
}


static inline int __cpus_intersects(const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 return bitmap_intersects(src1p->bits, src2p->bits, nbits);
}


static inline int __cpus_subset(const cpumask_t *src1p,
     const cpumask_t *src2p, int nbits)
{
 return bitmap_subset(src1p->bits, src2p->bits, nbits);
}


static inline int __cpus_empty(const cpumask_t *srcp, int nbits)
{
 return bitmap_empty(srcp->bits, nbits);
}


static inline int __cpus_weight(const cpumask_t *srcp, int nbits)
{
 return bitmap_weight(srcp->bits, nbits);
}



static inline void __cpus_shift_left(cpumask_t *dstp,
     const cpumask_t *srcp, int n, int nbits)
{
 bitmap_shift_left(dstp->bits, srcp->bits, n, nbits);
}
# 27 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cache.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/cache.h" 2 3
# 28 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/math64.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/div64.h" 1 3
# 57 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/div64.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/div64.h" 1 3
# 58 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/div64.h" 2 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/math64.h" 2 3
# 95 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/math64.h" 3
static inline u64 div_u64(u64 dividend, u32 divisor)
{
 u32 remainder;
 return div_u64_rem(dividend, divisor, &remainder);
}






static inline s64 div_s64(s64 dividend, s32 divisor)
{
 s32 remainder;
 return div_s64_rem(dividend, divisor, &remainder);
}


u32 iter_div_u64_rem(u64 dividend, u32 divisor, u64 *remainder);

static __inline __attribute__ ((__always_inline__)) u32
__iter_div_u64_rem(u64 dividend, u32 divisor, u64 *remainder)
{
 u32 ret = 0;

 while (dividend >= divisor) {


  asm("" : "+rm"(dividend));

  dividend -= divisor;
  ret++;
 }

 *remainder = dividend;

 return ret;
}
# 30 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/err.h" 1 3





# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/errno.h" 1 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/errno.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/errno-base.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/errno.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/errno.h" 2 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/err.h" 2 3
# 22 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/err.h" 3
static inline void *ERR_PTR(long error)
{
 return (void *) error;
}

static inline long PTR_ERR(const void *ptr)
{
 return (long) ptr;
}

static inline long IS_ERR(const void *ptr)
{
 return unlikely(((unsigned long)ptr) >= (unsigned long)-4095);
}

static inline long IS_ERR_OR_NULL(const void *ptr)
{
 return !ptr || unlikely(((unsigned long)ptr) >= (unsigned long)-4095);
}
# 49 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/err.h" 3
static inline void *ERR_CAST(const void *ptr)
{

 return (void *) ptr;
}

static inline int __must_check PTR_ERR_OR_ZERO( const void *ptr)
{
 if (IS_ERR(ptr))
  return PTR_ERR(ptr);
 else
  return 0;
}
# 32 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 2 3





static inline void *current_text_addr(void)
{
 void *pc;

 asm volatile("mov $1f, %0; 1:":"=r" (pc));

 return pc;
}
# 60 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
struct cpuinfo_x86 {
 __u8 x86;
 __u8 x86_vendor;
 __u8 x86_model;
 __u8 x86_mask;
# 78 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
 int x86_tlbsize;

 __u8 x86_virt_bits;
 __u8 x86_phys_bits;

 __u8 x86_coreid_bits;

 __u32 extended_cpuid_level;

 int cpuid_level;
 __u32 x86_capability[9];
 char x86_vendor_id[16];
 char x86_model_id[64];

 int x86_cache_size;
 int x86_cache_alignment;
 int x86_power;
 unsigned long loops_per_jiffy;





 u16 x86_max_cores;
 u16 apicid;
 u16 initial_apicid;
 u16 x86_clflush_size;
# 116 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
 unsigned int x86_hyper_vendor;







 u32 microcode;

} __attribute__((__aligned__((1 << (CONFIG_X86_L1_CACHE_SHIFT)))));


struct cpuinfo_x86_rh {
 __u32 x86_capability[1];
} __attribute__((__aligned__((1 << (CONFIG_X86_L1_CACHE_SHIFT)))));
# 147 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern struct cpuinfo_x86 boot_cpu_data;
extern struct cpuinfo_x86_rh boot_cpu_data_rh;
extern struct cpuinfo_x86 new_cpu_data;
extern struct cpuinfo_x86_rh new_cpu_data_rh;

extern struct tss_struct doublefault_tss;
extern __u32 cpu_caps_cleared[(9 + 1)];
extern __u32 cpu_caps_set[(9 + 1)];
# 175 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern const struct seq_operations cpuinfo_op;

static inline int hlt_works(int cpu)
{



 return 1;

}




extern unsigned long arch_align_stack(unsigned long sp);

extern void cpu_detect(struct cpuinfo_x86 *c);

extern struct pt_regs *idle_regs(struct pt_regs *);

extern void early_cpu_init(void);
extern void identify_boot_cpu(void);
extern void identify_secondary_cpu(struct cpuinfo_x86 *);
extern void print_cpu_info(struct cpuinfo_x86 *);
extern void init_scattered_cpuid_features(struct cpuinfo_x86 *c);
extern unsigned int init_intel_cacheinfo(struct cpuinfo_x86 *c);
extern unsigned short num_cache_leaves;

extern void detect_extended_topology(struct cpuinfo_x86 *c);
extern void detect_ht(struct cpuinfo_x86 *c);

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
    unsigned int *ecx, unsigned int *edx)
{

 asm volatile("cpuid"
     : "=a" (*eax),
       "=b" (*ebx),
       "=c" (*ecx),
       "=d" (*edx)
     : "0" (*eax), "2" (*ecx)
     : "memory");
}

static inline void load_cr3(pgd_t *pgdir)
{
 write_cr3(__pa(pgdir));
}
# 258 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
struct x86_hw_tss {
 u32 reserved1;
 u64 sp0;
 u64 sp1;
 u64 sp2;
 u64 reserved2;
 u64 ist[7];
 u32 reserved3;
 u32 reserved4;
 u16 reserved5;
 u16 io_bitmap_base;

} __attribute__((packed)) __attribute__((__aligned__((1 << (CONFIG_X86_L1_CACHE_SHIFT)))));
# 282 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
struct tss_struct {



 struct x86_hw_tss x86_tss;







 unsigned long io_bitmap[((65536/8)/sizeof(long)) + 1];




 unsigned long stack[64];

} __attribute__((__aligned__((1 << (CONFIG_X86_L1_CACHE_SHIFT)))));

extern __attribute__((section(".data" ""))) __typeof__(struct tss_struct) per_cpu__init_tss ;




struct orig_ist {
 unsigned long ist[7];
};



struct i387_fsave_struct {
 u32 cwd;
 u32 swd;
 u32 twd;
 u32 fip;
 u32 fcs;
 u32 foo;
 u32 fos;


 u32 st_space[20];


 u32 status;
};

struct i387_fxsave_struct {
 u16 cwd;
 u16 swd;
 u16 twd;
 u16 fop;
 union {
  struct {
   u64 rip;
   u64 rdp;
  };
  struct {
   u32 fip;
   u32 fcs;
   u32 foo;
   u32 fos;
  };
 };
 u32 mxcsr;
 u32 mxcsr_mask;


 u32 st_space[32];


 u32 xmm_space[64];

 u32 padding[12];

 union {
  u32 padding1[12];
  u32 sw_reserved[12];
 };

} __attribute__((aligned(16)));

struct i387_soft_struct {
 u32 cwd;
 u32 swd;
 u32 twd;
 u32 fip;
 u32 fcs;
 u32 foo;
 u32 fos;

 u32 st_space[20];
 u8 ftop;
 u8 changed;
 u8 lookahead;
 u8 no_update;
 u8 rm;
 u8 alimit;
 struct math_emu_info *info;
 u32 entry_eip;
};

struct ymmh_struct {

 u32 ymmh_space[64];
};

struct xsave_hdr_struct {
 u64 xstate_bv;
 u64 reserved1[2];
 u64 reserved2[5];
} __attribute__((packed));

struct xsave_struct {
 struct i387_fxsave_struct i387;
 struct xsave_hdr_struct xsave_hdr;
 struct ymmh_struct ymmh;

} __attribute__ ((packed, aligned (64)));

union thread_xstate {
 struct i387_fsave_struct fsave;
 struct i387_fxsave_struct fxsave;
 struct i387_soft_struct soft;
 struct xsave_struct xsave;
};

struct fpu {
       union thread_xstate *state;
};
# 453 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern unsigned int xstate_size;
extern void free_thread_xstate(struct task_struct *);
extern struct kmem_cache *task_xstate_cachep;

struct thread_struct {

 struct desc_struct tls_array[3];
 unsigned long sp0;
 unsigned long sp;



 unsigned long usersp;
 unsigned short es;
 unsigned short ds;
 unsigned short fsindex;
 unsigned short gsindex;







 unsigned long gs;

 unsigned long debugreg0;
 unsigned long debugreg1;
 unsigned long debugreg2;
 unsigned long debugreg3;
 unsigned long debugreg6;
 unsigned long debugreg7;

 unsigned long cr2;
 unsigned long trap_no;
 unsigned long error_code;

 union thread_xstate *xstate;
# 502 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
 unsigned long *io_bitmap_ptr;
 unsigned long iopl;

 unsigned io_bitmap_max;
};

static inline unsigned long native_get_debugreg(int regno)
{
 unsigned long val = 0;

 switch (regno) {
 case 0:
  asm("mov %%db0, %0" :"=r" (val));
  break;
 case 1:
  asm("mov %%db1, %0" :"=r" (val));
  break;
 case 2:
  asm("mov %%db2, %0" :"=r" (val));
  break;
 case 3:
  asm("mov %%db3, %0" :"=r" (val));
  break;
 case 6:
  asm("mov %%db6, %0" :"=r" (val));
  break;
 case 7:
  asm("mov %%db7, %0" :"=r" (val));
  break;
 default:
  BUG();
 }
 return val;
}

static inline void native_set_debugreg(int regno, unsigned long value)
{
 switch (regno) {
 case 0:
  asm("mov %0, %%db0" ::"r" (value));
  break;
 case 1:
  asm("mov %0, %%db1" ::"r" (value));
  break;
 case 2:
  asm("mov %0, %%db2" ::"r" (value));
  break;
 case 3:
  asm("mov %0, %%db3" ::"r" (value));
  break;
 case 6:
  asm("mov %0, %%db6" ::"r" (value));
  break;
 case 7:
  asm("mov %0, %%db7" ::"r" (value));
  break;
 default:
  BUG();
 }
}




static inline void native_set_iopl_mask(unsigned mask)
{
# 580 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
}

static inline void
native_load_sp0(struct tss_struct *tss, struct thread_struct *thread)
{
 tss->x86_tss.sp0 = thread->sp0;







}

static inline void native_swapgs(void)
{



}
# 616 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
static inline void load_sp0(struct tss_struct *tss,
       struct thread_struct *thread)
{
 native_load_sp0(tss, thread);
}
# 631 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern unsigned long mmu_cr4_features;

static inline void set_in_cr4(unsigned long mask)
{
 unsigned cr4;

 mmu_cr4_features |= mask;
 cr4 = read_cr4();
 cr4 |= mask;
 write_cr4(cr4);
}

static inline void clear_in_cr4(unsigned long mask)
{
 unsigned cr4;

 mmu_cr4_features &= ~mask;
 cr4 = read_cr4();
 cr4 &= ~mask;
 write_cr4(cr4);
}

typedef struct {
 unsigned long seg;
} mm_segment_t;





extern int kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);


extern void release_thread(struct task_struct *);


extern void prepare_to_copy(struct task_struct *tsk);

unsigned long get_wchan(struct task_struct *p);






static inline void cpuid(unsigned int op,
    unsigned int *eax, unsigned int *ebx,
    unsigned int *ecx, unsigned int *edx)
{
 *eax = op;
 *ecx = 0;
 native_cpuid(eax, ebx, ecx, edx);
}


static inline void cpuid_count(unsigned int op, int count,
          unsigned int *eax, unsigned int *ebx,
          unsigned int *ecx, unsigned int *edx)
{
 *eax = op;
 *ecx = count;
 native_cpuid(eax, ebx, ecx, edx);
}




static inline unsigned int cpuid_eax(unsigned int op)
{
 unsigned int eax, ebx, ecx, edx;

 cpuid(op, &eax, &ebx, &ecx, &edx);

 return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
 unsigned int eax, ebx, ecx, edx;

 cpuid(op, &eax, &ebx, &ecx, &edx);

 return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
 unsigned int eax, ebx, ecx, edx;

 cpuid(op, &eax, &ebx, &ecx, &edx);

 return ecx;
}

static inline unsigned int cpuid_edx(unsigned int op)
{
 unsigned int eax, ebx, ecx, edx;

 cpuid(op, &eax, &ebx, &ecx, &edx);

 return edx;
}


static inline void rep_nop(void)
{
 asm volatile("rep; nop" ::: "memory");
}

static inline void cpu_relax(void)
{
 rep_nop();
}


static inline void sync_core(void)
{
 int tmp;
# 760 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
  asm volatile("cpuid" : "=a" (tmp) : "0" (1)
        : "ebx", "ecx", "edx", "memory");
}

static inline void __monitor(const void *eax, unsigned long ecx,
        unsigned long edx)
{

 asm volatile(".byte 0x0f, 0x01, 0xc8;"
       :: "a" (eax), "c" (ecx), "d"(edx));
}

static inline void __mwait(unsigned long eax, unsigned long ecx)
{

 asm volatile(".byte 0x0f, 0x01, 0xc9;"
       :: "a" (eax), "c" (ecx));
}

static inline void __sti_mwait(unsigned long eax, unsigned long ecx)
{
 do { } while (0);

 asm volatile("sti; .byte 0x0f, 0x01, 0xc9;"
       :: "a" (eax), "c" (ecx));
}

extern void mwait_idle_with_hints(unsigned long eax, unsigned long ecx);

extern void select_idle_routine(const struct cpuinfo_x86 *c);
extern void init_c1e_mask(void);

extern unsigned long boot_option_idle_override;
extern unsigned long idle_halt;
extern unsigned long idle_nomwait;

extern void enable_sep_cpu(void);
extern int sysenter_setup(void);


extern struct desc_ptr early_gdt_descr;

extern void cpu_set_gdt(int);
extern void switch_to_new_gdt(int);
extern void load_percpu_segment(int);
extern void cpu_init(void);

static inline unsigned long get_debugctlmsr(void)
{
 unsigned long debugctlmsr = 0;


 if (boot_cpu_data.x86 < 6)
  return 0;

 rdmsrl(0x000001d9, debugctlmsr);

 return debugctlmsr;
}

static inline void update_debugctlmsr(unsigned long debugctlmsr)
{

 if (boot_cpu_data.x86 < 6)
  return;

 wrmsrl(0x000001d9, debugctlmsr);
}





extern unsigned int machine_id;
extern unsigned int machine_submodel_id;
extern unsigned int BIOS_revision;


extern int bootloader_type;
extern int bootloader_version;

extern char ignore_fpu_irq;
# 860 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
static inline void prefetch(const void *x)
{
 asm volatile ("661:\n\t" "prefetcht0 (%1)" "\n662:\n" ".section .altinstructions,\"a\"\n" " " ".balign 8" " " "\n" " " ".quad" " " "661b\n" " " ".quad" " " "663f\n" "	 .word " "(0*32+25)" "\n" "	 .byte 662b-661b\n" "	 .byte 664f-663f\n" ".previous\n" ".section .discard,\"aw\",@progbits\n" "	 .byte 0xff + (664f-663f) - (662b-661b)\n" ".previous\n" ".section .altinstr_replacement, \"ax\"\n" "663:\n\t" "prefetchnta (%1)" "\n664:\n" ".previous" : : "i" (0), "r" (x))


             ;
}






static inline void prefetchw(const void *x)
{
 asm volatile ("661:\n\t" "prefetcht0 (%1)" "\n662:\n" ".section .altinstructions,\"a\"\n" " " ".balign 8" " " "\n" " " ".quad" " " "661b\n" " " ".quad" " " "663f\n" "	 .word " "(1*32+31)" "\n" "	 .byte 662b-661b\n" "	 .byte 664f-663f\n" ".previous\n" ".section .discard,\"aw\",@progbits\n" "	 .byte 0xff + (664f-663f) - (662b-661b)\n" ".previous\n" ".section .altinstr_replacement, \"ax\"\n" "663:\n\t" "prefetchw (%1)" "\n664:\n" ".previous" : : "i" (0), "r" (x))


             ;
}

static inline void spin_lock_prefetch(const void *x)
{
 prefetchw(x);
}
# 981 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern unsigned long KSTK_ESP(struct task_struct *task);


extern void start_thread(struct pt_regs *regs, unsigned long new_ip,
            unsigned long new_sp);
# 999 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/processor.h" 3
extern int get_tsc_mode(unsigned long adr);
extern int set_tsc_mode(unsigned int val);

extern int amd_get_nb_id(int cpu);

struct aperfmperf {
 u64 aperf, mperf;
};

static inline void get_aperfmperf(struct aperfmperf *am)
{
 WARN_ON_ONCE(!boot_cpu_has((3*32+28)));

 rdmsrl(0x000000e8, am->aperf);
 rdmsrl(0x000000e7, am->mperf);
}



static inline
unsigned long calc_aperfmperf_ratio(struct aperfmperf *old,
        struct aperfmperf *new)
{
 u64 aperf = new->aperf - old->aperf;
 u64 mperf = new->mperf - old->mperf;
 unsigned long ratio = aperf;

 mperf >>= 10;
 if (mperf)
  ratio = div64_u64(aperf, mperf);

 return ratio;
}

static inline uint32_t hypervisor_cpuid_base(const char *sig, uint32_t leaves)
{
 uint32_t base, eax, signature[3];

 for (base = 0x40000000; base < 0x40010000; base += 0x100) {
  cpuid(base, &eax, &signature[0], &signature[1], &signature[2]);

  if (!memcmp(sig, signature, 12) &&
      (leaves == 0 || ((eax - base) >= leaves)))
   return base;
 }

 return 0;
}
# 23 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/ftrace.h" 1 3
# 24 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 1 3





# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/cmpxchg.h" 1 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 2 3
# 21 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_read(const atomic_t *v)
{
 return v->counter;
}
# 33 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic_set(atomic_t *v, int i)
{
 v->counter = i;
}
# 45 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic_add(int i, atomic_t *v)
{
 asm volatile("" "addl %1,%0"
       : "=m" (v->counter)
       : "ir" (i), "m" (v->counter));
}
# 59 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic_sub(int i, atomic_t *v)
{
 asm volatile("" "subl %1,%0"
       : "=m" (v->counter)
       : "ir" (i), "m" (v->counter));
}
# 75 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_sub_and_test(int i, atomic_t *v)
{
 unsigned char c;

 asm volatile("" "subl %2,%0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "ir" (i), "m" (v->counter) : "memory");
 return c;
}







static inline void atomic_inc(atomic_t *v)
{
 asm volatile("" "incl %0"
       : "=m" (v->counter)
       : "m" (v->counter));
}







static inline void atomic_dec(atomic_t *v)
{
 asm volatile("" "decl %0"
       : "=m" (v->counter)
       : "m" (v->counter));
}
# 119 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_dec_and_test(atomic_t *v)
{
 unsigned char c;

 asm volatile("" "decl %0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "m" (v->counter) : "memory");
 return c != 0;
}
# 137 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_inc_and_test(atomic_t *v)
{
 unsigned char c;

 asm volatile("" "incl %0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "m" (v->counter) : "memory");
 return c != 0;
}
# 156 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_add_negative(int i, atomic_t *v)
{
 unsigned char c;

 asm volatile("" "addl %2,%0; sets %1"
       : "=m" (v->counter), "=qm" (c)
       : "ir" (i), "m" (v->counter) : "memory");
 return c;
}
# 173 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_add_return(int i, atomic_t *v)
{
 int __i = i;
 asm volatile("" "xaddl %0, %1"
       : "+r" (i), "+m" (v->counter)
       : : "memory");
 return i + __i;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
 return atomic_add_return(-i, v);
}
# 201 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline long atomic64_read(const atomic64_t *v)
{
 return v->counter;
}
# 213 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic64_set(atomic64_t *v, long i)
{
 v->counter = i;
}
# 225 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic64_add(long i, atomic64_t *v)
{
 asm volatile("" "addq %1,%0"
       : "=m" (v->counter)
       : "er" (i), "m" (v->counter));
}
# 239 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic64_sub(long i, atomic64_t *v)
{
 asm volatile("" "subq %1,%0"
       : "=m" (v->counter)
       : "er" (i), "m" (v->counter));
}
# 255 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic64_sub_and_test(long i, atomic64_t *v)
{
 unsigned char c;

 asm volatile("" "subq %2,%0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "er" (i), "m" (v->counter) : "memory");
 return c;
}







static inline void atomic64_inc(atomic64_t *v)
{
 asm volatile("" "incq %0"
       : "=m" (v->counter)
       : "m" (v->counter));
}







static inline void atomic64_dec(atomic64_t *v)
{
 asm volatile("" "decq %0"
       : "=m" (v->counter)
       : "m" (v->counter));
}
# 299 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic64_dec_and_test(atomic64_t *v)
{
 unsigned char c;

 asm volatile("" "decq %0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "m" (v->counter) : "memory");
 return c != 0;
}
# 317 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic64_inc_and_test(atomic64_t *v)
{
 unsigned char c;

 asm volatile("" "incq %0; sete %1"
       : "=m" (v->counter), "=qm" (c)
       : "m" (v->counter) : "memory");
 return c != 0;
}
# 336 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic64_add_negative(long i, atomic64_t *v)
{
 unsigned char c;

 asm volatile("" "addq %2,%0; sets %1"
       : "=m" (v->counter), "=qm" (c)
       : "er" (i), "m" (v->counter) : "memory");
 return c;
}
# 353 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline long atomic64_add_return(long i, atomic64_t *v)
{
 long __i = i;
 asm volatile("" "xaddq %0, %1;"
       : "+r" (i), "+m" (v->counter)
       : : "memory");
 return i + __i;
}

static inline long atomic64_sub_return(long i, atomic64_t *v)
{
 return atomic64_add_return(-i, v);
}




static inline long atomic64_cmpxchg(atomic64_t *v, long old, long new)
{
 return ((__typeof__(*(&v->counter)))__cmpxchg((&v->counter), (unsigned long)(old), (unsigned long)(new), sizeof(*(&v->counter))));
}

static inline long atomic64_xchg(atomic64_t *v, long new)
{
 return ((__typeof__(*(&v->counter)))__xchg((unsigned long)(new), (&v->counter), sizeof(*(&v->counter))));
}

static inline long atomic_cmpxchg(atomic_t *v, int old, int new)
{
 return ((__typeof__(*(&v->counter)))__cmpxchg((&v->counter), (unsigned long)(old), (unsigned long)(new), sizeof(*(&v->counter))));
}

static inline long atomic_xchg(atomic_t *v, int new)
{
 return ((__typeof__(*(&v->counter)))__xchg((unsigned long)(new), (&v->counter), sizeof(*(&v->counter))));
}
# 399 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic_add_unless(atomic_t *v, int a, int u)
{
 int c, old;
 c = atomic_read(v);
 for (;;) {
  if (unlikely(c == (u)))
   break;
  old = atomic_cmpxchg((v), c, c + (a));
  if (likely(old == c))
   break;
  c = old;
 }
 return c != (u);
}
# 425 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline int atomic64_add_unless(atomic64_t *v, long a, long u)
{
 long c, old;
 c = atomic64_read(v);
 for (;;) {
  if (unlikely(c == (u)))
   break;
  old = atomic64_cmpxchg((v), c, c + (a));
  if (likely(old == c))
   break;
  c = old;
 }
 return c != (u);
}
# 447 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline short int atomic_inc_short(short int *v)
{
 asm("" "addw $1, %0" : "+m" (*v));
 return *v;
}
# 461 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
static inline void atomic_or_long(unsigned long *v1, unsigned long v2)
{
 asm("" "orq %1, %0" : "+m" (*v1) : "r" (v2));
}
# 484 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/atomic-long.h" 1 3
# 141 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/atomic-long.h" 3
typedef atomic_t atomic_long_t;


static inline long atomic_long_read(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return (long)atomic_read(v);
}

static inline void atomic_long_set(atomic_long_t *l, long i)
{
 atomic_t *v = (atomic_t *)l;

 atomic_set(v, i);
}

static inline void atomic_long_inc(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 atomic_inc(v);
}

static inline void atomic_long_dec(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 atomic_dec(v);
}

static inline void atomic_long_add(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 atomic_add(i, v);
}

static inline void atomic_long_sub(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 atomic_sub(i, v);
}

static inline int atomic_long_sub_and_test(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return atomic_sub_and_test(i, v);
}

static inline int atomic_long_dec_and_test(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return atomic_dec_and_test(v);
}

static inline int atomic_long_inc_and_test(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return atomic_inc_and_test(v);
}

static inline int atomic_long_add_negative(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return atomic_add_negative(i, v);
}

static inline long atomic_long_add_return(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return (long)atomic_add_return(i, v);
}

static inline long atomic_long_sub_return(long i, atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return (long)atomic_sub_return(i, v);
}

static inline long atomic_long_inc_return(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return (long)(atomic_add_return(1, v));
}

static inline long atomic_long_dec_return(atomic_long_t *l)
{
 atomic_t *v = (atomic_t *)l;

 return (long)(atomic_sub_return(1, v));
}

static inline long atomic_long_add_unless(atomic_long_t *l, long a, long u)
{
 atomic_t *v = (atomic_t *)l;

 return (long)atomic_add_unless(v, a, u);
}
# 485 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic_64.h" 2 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 2 3
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 2 3

struct thread_info {
 struct task_struct *task;
 struct exec_domain *exec_domain;
 __u32 flags;
 __u32 status;
 __u32 cpu;
 int preempt_count;

 mm_segment_t addr_limit;
 struct restart_block restart_block;
 void *sysenter_return;






 int uaccess_err;
};
# 217 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 3
extern __attribute__((section(".data" ""))) __typeof__(unsigned long) per_cpu__kernel_stack;
extern __attribute__((section(".data" ""))) __typeof__(unsigned long) per_cpu__kernel_stack8k;

static inline struct thread_info *current_thread_info(void)
{
 struct thread_info *ti;
 ti = (void *)(({ typeof(per_cpu__kernel_stack) ret__; switch (sizeof(per_cpu__kernel_stack)) { case 1: asm("mov" "b ""%P" "1"",%0" : "=q" (ret__) : "p" (&per_cpu__kernel_stack)); break; case 2: asm("mov" "w ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__kernel_stack)); break; case 4: asm("mov" "l ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__kernel_stack)); break; case 8: asm("mov" "q ""%P" "1"",%0" : "=r" (ret__) : "p" (&per_cpu__kernel_stack)); break; default: __bad_percpu_size(); } ret__; }) +
        (5*8) - (((1UL) << 12) << 1));
 return ti;
}
# 257 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/thread_info.h" 3
static inline void set_restore_sigmask(void)
{
 struct thread_info *ti = current_thread_info();
 ti->status |= 0x0008;
 set_bit(2, (unsigned long *)&ti->flags);
}



extern void arch_task_cache_init(void);
extern void free_thread_info(struct thread_info *ti);
extern int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src);
# 57 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/thread_info.h" 2 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/preempt.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 1 3





# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/poison.h" 1 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/prefetch.h" 1 3
# 53 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/prefetch.h" 3
static inline void prefetch_range(void *addr, size_t len)
{







}
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 2 3
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void INIT_LIST_HEAD(struct list_head *list)
{
 list->next = list;
 list->prev = list;
}
# 38 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void __list_add(struct list_head *new,
         struct list_head *prev,
         struct list_head *next)
{
 next->prev = new;
 new->next = next;
 new->prev = prev;
 prev->next = new;
}
# 61 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_add(struct list_head *new, struct list_head *head)
{
 __list_add(new, head, head->next);
}
# 75 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
 __list_add(new, head->prev, head);
}
# 87 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
 next->prev = prev;
 prev->next = next;
}
# 100 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_del(struct list_head *entry)
{
 __list_del(entry->prev, entry->next);
 entry->next = ((void *) 0x00100100 + 0);
 entry->prev = ((void *) 0x00200200 + 0);
}
# 117 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_replace(struct list_head *old,
    struct list_head *new)
{
 new->next = old->next;
 new->next->prev = new;
 new->prev = old->prev;
 new->prev->next = new;
}

static inline void list_replace_init(struct list_head *old,
     struct list_head *new)
{
 list_replace(old, new);
 INIT_LIST_HEAD(old);
}





static inline void list_del_init(struct list_head *entry)
{
 __list_del(entry->prev, entry->next);
 INIT_LIST_HEAD(entry);
}






static inline void list_move(struct list_head *list, struct list_head *head)
{
 __list_del(list->prev, list->next);
 list_add(list, head);
}






static inline void list_move_tail(struct list_head *list,
      struct list_head *head)
{
 __list_del(list->prev, list->next);
 list_add_tail(list, head);
}






static inline int list_is_last(const struct list_head *list,
    const struct list_head *head)
{
 return list->next == head;
}





static inline int list_empty(const struct list_head *head)
{
 return head->next == head;
}
# 199 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline int list_empty_careful(const struct list_head *head)
{
 struct list_head *next = head->next;
 return (next == head) && (next == head->prev);
}





static inline void list_rotate_left(struct list_head *head)
{
 struct list_head *first;

 if (!list_empty(head)) {
  first = head->next;
  list_move_tail(first, head);
 }
}





static inline int list_is_singular(const struct list_head *head)
{
 return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(struct list_head *list,
  struct list_head *head, struct list_head *entry)
{
 struct list_head *new_first = entry->next;
 list->next = head->next;
 list->next->prev = list;
 list->prev = entry;
 entry->next = list;
 head->next = new_first;
 new_first->prev = head;
}
# 254 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_cut_position(struct list_head *list,
  struct list_head *head, struct list_head *entry)
{
 if (list_empty(head))
  return;
 if (list_is_singular(head) &&
  (head->next != entry && head != entry))
  return;
 if (entry == head)
  INIT_LIST_HEAD(list);
 else
  __list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_head *list,
     struct list_head *prev,
     struct list_head *next)
{
 struct list_head *first = list->next;
 struct list_head *last = list->prev;

 first->prev = prev;
 prev->next = first;

 last->next = next;
 next->prev = last;
}






static inline void list_splice(const struct list_head *list,
    struct list_head *head)
{
 if (!list_empty(list))
  __list_splice(list, head, head->next);
}






static inline void list_splice_tail(struct list_head *list,
    struct list_head *head)
{
 if (!list_empty(list))
  __list_splice(list, head->prev, head);
}
# 313 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_splice_init(struct list_head *list,
        struct list_head *head)
{
 if (!list_empty(list)) {
  __list_splice(list, head, head->next);
  INIT_LIST_HEAD(list);
 }
}
# 330 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
static inline void list_splice_tail_init(struct list_head *list,
      struct list_head *head)
{
 if (!list_empty(list)) {
  __list_splice(list, head->prev, head);
  INIT_LIST_HEAD(list);
 }
}
# 589 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/list.h" 3
struct hlist_head {
 struct hlist_node *first;
};

struct hlist_node {
 struct hlist_node *next, **pprev;
};




static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
 h->next = ((void *)0);
 h->pprev = ((void *)0);
}

static inline int hlist_unhashed(const struct hlist_node *h)
{
 return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head *h)
{
 return !h->first;
}

static inline void __hlist_del(struct hlist_node *n)
{
 struct hlist_node *next = n->next;
 struct hlist_node **pprev = n->pprev;
 *pprev = next;
 if (next)
  next->pprev = pprev;
}

static inline void hlist_del(struct hlist_node *n)
{
 __hlist_del(n);
 n->next = ((void *) 0x00100100 + 0);
 n->pprev = ((void *) 0x00200200 + 0);
}

static inline void hlist_del_init(struct hlist_node *n)
{
 if (!hlist_unhashed(n)) {
  __hlist_del(n);
  INIT_HLIST_NODE(n);
 }
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
 struct hlist_node *first = h->first;
 n->next = first;
 if (first)
  first->pprev = &n->next;
 h->first = n;
 n->pprev = &h->first;
}


static inline void hlist_add_before(struct hlist_node *n,
     struct hlist_node *next)
{
 n->pprev = next->pprev;
 n->next = next;
 next->pprev = &n->next;
 *(n->pprev) = n;
}

static inline void hlist_add_after(struct hlist_node *n,
     struct hlist_node *next)
{
 next->next = n->next;
 n->next = next;
 next->pprev = &n->next;

 if(next->next)
  next->next->pprev = &next->next;
}





static inline void hlist_move_list(struct hlist_head *old,
       struct hlist_head *new)
{
 new->first = old->first;
 if (new->first)
  new->first->pprev = &new->first;
 old->first = ((void *)0);
}
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/preempt.h" 2 3
# 51 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 55 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bottom_half.h" 1 3



extern void local_bh_disable(void);
extern void _local_bh_enable(void);
extern void local_bh_enable(void);
extern void local_bh_enable_ip(unsigned long ip);
# 57 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3
# 80 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 1 3
# 15 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types_up.h" 1 3
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types_up.h" 3
typedef struct { } raw_spinlock_t;





typedef struct {

} raw_rwlock_t;
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/lockdep.h" 1 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/lockdep.h" 3
struct task_struct;
struct lockdep_map;
# 328 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/lockdep.h" 3
static inline void lockdep_off(void)
{
}

static inline void lockdep_on(void)
{
}
# 366 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/lockdep.h" 3
struct lock_class_key { };
# 418 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/lockdep.h" 3
static inline void early_init_irq_lock_class(void)
{
}







static inline void early_boot_irqs_off(void)
{
}
static inline void early_boot_irqs_on(void)
{
}
static inline void print_irqtrace_events(struct task_struct *curr)
{
}
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 2 3

typedef struct {
 raw_spinlock_t raw_lock;
# 32 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 3
} spinlock_t;



typedef struct {
 raw_rwlock_t raw_lock;
# 48 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_types.h" 3
} rwlock_t;
# 81 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3

extern int __attribute__((section(".spinlock.text"))) generic__raw_read_trylock(raw_rwlock_t *lock);







# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_up.h" 1 3
# 91 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3
# 137 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
static inline void smp_mb__after_lock(void) { barrier(); }
# 321 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 322 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3
# 330 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
extern int _atomic_dec_and_lock(atomic_t *atomic, spinlock_t *lock);
# 346 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock_api_up.h" 1 3
# 347 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/spinlock.h" 2 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/wait.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/numa.h" 1 3
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 1 3
# 32 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
typedef struct {
 unsigned sequence;
 spinlock_t lock;
} seqlock_t;
# 60 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline void write_seqlock(seqlock_t *sl)
{
 do { do { } while (0); (void)0; (void)(&sl->lock); } while (0);
 ++sl->sequence;
 barrier();
}

static inline void write_sequnlock(seqlock_t *sl)
{
 barrier();
 sl->sequence++;
 do { do { } while (0); (void)0; (void)(&sl->lock); } while (0);
}

static inline int write_tryseqlock(seqlock_t *sl)
{
 int ret = (({ do { do { } while (0); (void)0; (void)(&sl->lock); } while (0); 1; }));

 if (ret) {
  ++sl->sequence;
  barrier();
 }
 return ret;
}


static __inline __attribute__ ((__always_inline__)) unsigned read_seqbegin(const seqlock_t *sl)
{
 unsigned ret;

repeat:
 ret = (*(volatile typeof(sl->sequence) *)&(sl->sequence));
 if (unlikely(ret & 1)) {
  cpu_relax();
  goto repeat;
 }
 barrier();

 return ret;
}






static __inline __attribute__ ((__always_inline__)) int read_seqretry(const seqlock_t *sl, unsigned start)
{
 barrier();

 return unlikely(sl->sequence != start);
}
# 121 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
typedef struct seqcount {
 unsigned sequence;
} seqcount_t;





static inline void __seqcount_init(seqcount_t *s, const char *name,
       struct lock_class_key *key)
{
 s->sequence = 0;
}
# 148 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline unsigned __read_seqcount_begin(const seqcount_t *s)
{
 unsigned ret;

repeat:
 ret = s->sequence;
 if (unlikely(ret & 1)) {
  cpu_relax();
  goto repeat;
 }
 return ret;
}
# 170 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline unsigned read_seqcount_begin(const seqcount_t *s)
{
 unsigned ret = __read_seqcount_begin(s);
 barrier();
 return ret;
}
# 191 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline unsigned raw_seqcount_begin(const seqcount_t *s)
{
 unsigned ret = (*(volatile typeof(s->sequence) *)&(s->sequence));
 barrier();
 return ret & ~1;
}
# 212 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline int __read_seqcount_retry(const seqcount_t *s, unsigned start)
{
 return unlikely(s->sequence != start);
}
# 227 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline int read_seqcount_retry(const seqcount_t *s, unsigned start)
{
 barrier();

 return __read_seqcount_retry(s, start);
}






static inline void write_seqcount_begin(seqcount_t *s)
{
 s->sequence++;
 barrier();
}

static inline void write_seqcount_end(seqcount_t *s)
{
 barrier();
 s->sequence++;
}
# 258 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/seqlock.h" 3
static inline void write_seqcount_barrier(seqcount_t *s)
{
 barrier();
 s->sequence+=2;
}
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 1 3
# 93 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 94 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 2 3




typedef struct { DECLARE_BITMAP(bits, (1 << 0)); } nodemask_t;
extern nodemask_t _unused_nodemask_arg_;


static inline void __node_set(int node, volatile nodemask_t *dstp)
{
 set_bit(node, dstp->bits);
}


static inline void __node_clear(int node, volatile nodemask_t *dstp)
{
 clear_bit(node, dstp->bits);
}


static inline void __nodes_setall(nodemask_t *dstp, int nbits)
{
 bitmap_fill(dstp->bits, nbits);
}


static inline void __nodes_clear(nodemask_t *dstp, int nbits)
{
 bitmap_zero(dstp->bits, nbits);
}






static inline int __node_test_and_set(int node, nodemask_t *addr)
{
 return test_and_set_bit(node, addr->bits);
}



static inline void __nodes_and(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_and(dstp->bits, src1p->bits, src2p->bits, nbits);
}



static inline void __nodes_or(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_or(dstp->bits, src1p->bits, src2p->bits, nbits);
}



static inline void __nodes_xor(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_xor(dstp->bits, src1p->bits, src2p->bits, nbits);
}



static inline void __nodes_andnot(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_andnot(dstp->bits, src1p->bits, src2p->bits, nbits);
}



static inline void __nodes_complement(nodemask_t *dstp,
     const nodemask_t *srcp, int nbits)
{
 bitmap_complement(dstp->bits, srcp->bits, nbits);
}



static inline int __nodes_equal(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_equal(src1p->bits, src2p->bits, nbits);
}



static inline int __nodes_intersects(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_intersects(src1p->bits, src2p->bits, nbits);
}



static inline int __nodes_subset(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_subset(src1p->bits, src2p->bits, nbits);
}


static inline int __nodes_empty(const nodemask_t *srcp, int nbits)
{
 return bitmap_empty(srcp->bits, nbits);
}


static inline int __nodes_full(const nodemask_t *srcp, int nbits)
{
 return bitmap_full(srcp->bits, nbits);
}


static inline int __nodes_weight(const nodemask_t *srcp, int nbits)
{
 return bitmap_weight(srcp->bits, nbits);
}



static inline void __nodes_shift_right(nodemask_t *dstp,
     const nodemask_t *srcp, int n, int nbits)
{
 bitmap_shift_right(dstp->bits, srcp->bits, n, nbits);
}



static inline void __nodes_shift_left(nodemask_t *dstp,
     const nodemask_t *srcp, int n, int nbits)
{
 bitmap_shift_left(dstp->bits, srcp->bits, n, nbits);
}





static inline int __first_node(const nodemask_t *srcp)
{
 return min_t(int, (1 << 0), find_first_bit(srcp->bits, (1 << 0)));
}


static inline int __next_node(int n, const nodemask_t *srcp)
{
 return min_t(int,(1 << 0),find_next_bit(srcp->bits, (1 << 0), n+1));
}

static inline void init_nodemask_of_node(nodemask_t *mask, int node)
{
 __nodes_clear(&(*mask), (1 << 0));
 __node_set((node), &(*mask));
}
# 266 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
static inline int __first_unset_node(const nodemask_t *maskp)
{
 return min_t(int,(1 << 0),
   find_first_zero_bit(maskp->bits, (1 << 0)));
}
# 300 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
static inline int __nodemask_scnprintf(char *buf, int len,
     const nodemask_t *srcp, int nbits)
{
 return bitmap_scnprintf(buf, len, srcp->bits, nbits);
}



static inline int __nodemask_parse_user(const char *buf, int len,
     nodemask_t *dstp, int nbits)
{
 return bitmap_parse_user(buf, len, dstp->bits, nbits);
}



static inline int __nodelist_scnprintf(char *buf, int len,
     const nodemask_t *srcp, int nbits)
{
 return bitmap_scnlistprintf(buf, len, srcp->bits, nbits);
}


static inline int __nodelist_parse(const char *buf, nodemask_t *dstp, int nbits)
{
 return bitmap_parselist(buf, dstp->bits, nbits);
}



static inline int __node_remap(int oldbit,
  const nodemask_t *oldp, const nodemask_t *newp, int nbits)
{
 return bitmap_bitremap(oldbit, oldp->bits, newp->bits, nbits);
}



static inline void __nodes_remap(nodemask_t *dstp, const nodemask_t *srcp,
  const nodemask_t *oldp, const nodemask_t *newp, int nbits)
{
 bitmap_remap(dstp->bits, srcp->bits, oldp->bits, newp->bits, nbits);
}



static inline void __nodes_onto(nodemask_t *dstp, const nodemask_t *origp,
  const nodemask_t *relmapp, int nbits)
{
 bitmap_onto(dstp->bits, origp->bits, relmapp->bits, nbits);
}



static inline void __nodes_fold(nodemask_t *dstp, const nodemask_t *origp,
  int sz, int nbits)
{
 bitmap_fold(dstp->bits, origp->bits, sz, nbits);
}
# 374 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
enum node_states {
 N_POSSIBLE,
 N_ONLINE,
 N_NORMAL_MEMORY,



 N_HIGH_MEMORY = N_NORMAL_MEMORY,

 N_CPU,
 NR_NODE_STATES
};






extern nodemask_t node_states[NR_NODE_STATES];
# 437 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
static inline int node_state(int node, enum node_states state)
{
 return node == 0;
}

static inline void node_set_state(int node, enum node_states state)
{
}

static inline void node_clear_state(int node, enum node_states state)
{
}

static inline int num_node_state(enum node_states state)
{
 return 1;
}
# 502 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/nodemask.h" 3
struct nodemask_scratch {
 nodemask_t mask1;
 nodemask_t mask2;
};
# 17 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/pageblock-flags.h" 1 3
# 29 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/pageblock-flags.h" 3
enum pageblock_bits {
 PB_migrate,
 PB_migrate_end = PB_migrate + 3 - 1,




 NR_PAGEBLOCK_BITS
};
# 63 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/pageblock-flags.h" 3
struct page;


unsigned long get_pageblock_flags_group(struct page *page,
     int start_bitidx, int end_bitidx);
void set_pageblock_flags_group(struct page *page, unsigned long flags,
     int start_bitidx, int end_bitidx);
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/bounds.h" 1 3
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3
# 50 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
extern int page_group_by_mobility_disabled;

static inline int get_pageblock_migratetype(struct page *page)
{
 return get_pageblock_flags_group(page, PB_migrate, PB_migrate_end);
}

struct free_area {
 struct list_head free_list[5];
 unsigned long nr_free;
};

struct pglist_data;
# 79 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
enum zone_stat_item {

 NR_FREE_PAGES,
 NR_LRU_BASE,
 NR_INACTIVE_ANON = NR_LRU_BASE,
 NR_ACTIVE_ANON,
 NR_INACTIVE_FILE,
 NR_ACTIVE_FILE,
 NR_UNEVICTABLE,
 NR_MLOCK,
 NR_ANON_PAGES,
 NR_FILE_MAPPED,

 NR_FILE_PAGES,
 NR_FILE_DIRTY,
 NR_WRITEBACK,
 NR_SLAB_RECLAIMABLE,
 NR_SLAB_UNRECLAIMABLE,
 NR_PAGETABLE,
 NR_KERNEL_STACK,

 NR_UNSTABLE_NFS,
 NR_BOUNCE,
 NR_VMSCAN_WRITE,
 NR_WRITEBACK_TEMP,
 NR_ISOLATED_ANON,
 NR_ISOLATED_FILE,
 NR_SHMEM,
# 115 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 NR_ANON_TRANSPARENT_HUGEPAGES,
 NR_VM_ZONE_STAT_ITEMS };
# 131 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
enum lru_list {
 LRU_INACTIVE_ANON = 0,
 LRU_ACTIVE_ANON = 0 + 1,
 LRU_INACTIVE_FILE = 0 + 2,
 LRU_ACTIVE_FILE = 0 + 2 + 1,
 LRU_UNEVICTABLE,
 NR_LRU_LISTS
};





static inline int is_file_lru(enum lru_list l)
{
 return (l == LRU_INACTIVE_FILE || l == LRU_ACTIVE_FILE);
}

static inline int is_active_lru(enum lru_list l)
{
 return (l == LRU_ACTIVE_ANON || l == LRU_ACTIVE_FILE);
}

static inline int is_unevictable_lru(enum lru_list l)
{
 return (l == LRU_UNEVICTABLE);
}
# 169 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
typedef unsigned isolate_mode_t;

struct lruvec {
 struct list_head lists[NR_LRU_LISTS];
};

enum zone_watermarks {
 WMARK_MIN,
 WMARK_LOW,
 WMARK_HIGH,
 NR_WMARK
};





struct per_cpu_pages {
 int count;
 int high;
 int batch;


 struct list_head lists[3];
};

struct per_cpu_pageset {
 struct per_cpu_pages pcp;







} ;
# 214 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
enum zone_type {
# 249 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 ZONE_NORMAL,
# 261 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 ZONE_MOVABLE,
 __MAX_NR_ZONES
};
# 285 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
struct zone_reclaim_stat {
# 294 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 unsigned long recent_rotated[2];
 unsigned long recent_scanned[2];




 unsigned long nr_saved_scan[NR_LRU_LISTS];
};

struct zone {



 unsigned long watermark[NR_WMARK];







 unsigned long percpu_drift_mark;
# 326 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 unsigned long lowmem_reserve[4];
# 337 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 struct per_cpu_pageset pageset[1];




 spinlock_t lock;




 struct free_area free_area[11];






 unsigned long *pageblock_flags;
# 367 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3



 spinlock_t lru_lock;





 struct lruvec lruvec;


 struct zone_reclaim_stat reclaim_stat;

 unsigned long pages_scanned;
 unsigned long flags;


 atomic_long_t vm_stat[NR_VM_ZONE_STAT_ITEMS];
# 400 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 int prev_priority;





 unsigned int inactive_ratio;



# 436 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 wait_queue_head_t * wait_table;
 unsigned long wait_table_hash_nr_entries;
 unsigned long wait_table_bits;




 struct pglist_data *zone_pgdat;

 unsigned long zone_start_pfn;
# 457 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 unsigned long spanned_pages;
 unsigned long present_pages;




 const char *name;
# 475 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 unsigned long nr_migrate_reserve_block;
 unsigned long padding[12];



} ;

typedef enum {
 ZONE_ALL_UNRECLAIMABLE,
 ZONE_OOM_LOCKED,
 ZONE_CONGESTED,


} zone_flags_t;

static inline void zone_set_flag(struct zone *zone, zone_flags_t flag)
{
 set_bit(flag, &zone->flags);
}

static inline int zone_test_and_set_flag(struct zone *zone, zone_flags_t flag)
{
 return test_and_set_bit(flag, &zone->flags);
}

static inline void zone_clear_flag(struct zone *zone, zone_flags_t flag)
{
 clear_bit(flag, &zone->flags);
}

static inline int zone_is_all_unreclaimable(const struct zone *zone)
{
 return (__builtin_constant_p((ZONE_ALL_UNRECLAIMABLE)) ? constant_test_bit((ZONE_ALL_UNRECLAIMABLE), (&zone->flags)) : variable_test_bit((ZONE_ALL_UNRECLAIMABLE), (&zone->flags)));
}

static inline int zone_is_reclaim_congested(const struct zone *zone)
{
 return (__builtin_constant_p((ZONE_CONGESTED)) ? constant_test_bit((ZONE_CONGESTED), (&zone->flags)) : variable_test_bit((ZONE_CONGESTED), (&zone->flags)));
}

static inline int zone_is_oom_locked(const struct zone *zone)
{
 return (__builtin_constant_p((ZONE_OOM_LOCKED)) ? constant_test_bit((ZONE_OOM_LOCKED), (&zone->flags)) : variable_test_bit((ZONE_OOM_LOCKED), (&zone->flags)));
}
# 608 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
struct zonelist_cache;






struct zoneref {
 struct zone *zone;
 int zone_idx;
};
# 637 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
struct zonelist {
 struct zonelist_cache *zlcache_ptr;
 struct zoneref _zonerefs[((1 << 0) * 4) + 1];



};
# 655 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
extern struct page *mem_map;
# 669 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
struct bootmem_data;
typedef struct pglist_data {
 struct zone node_zones[4];
 struct zonelist node_zonelists[1];
 int nr_zones;






 struct bootmem_data *bdata;
# 691 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
 unsigned long node_start_pfn;
 unsigned long node_present_pages;
 unsigned long node_spanned_pages;

 int node_id;
 wait_queue_head_t kswapd_wait;
 struct task_struct *kswapd;
 int kswapd_max_order;
} pg_data_t;
# 717 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/memory_hotplug.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/memory_hotplug.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 1 3
# 12 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/errno.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/errno.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/errno.h" 2 3
# 13 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 1 3
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 2 3
# 49 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
struct mutex {

 atomic_t count;
 spinlock_t wait_lock;





 struct list_head wait_list;
# 70 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
};





struct mutex_waiter {
 struct list_head list;
 struct task_struct *task;



};
# 131 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
extern void __mutex_init(struct mutex *lock, const char *name,
    struct lock_class_key *key);







static inline int mutex_is_locked(struct mutex *lock)
{
 return atomic_read(&lock->count) != 1;
}
# 169 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
extern void mutex_lock(struct mutex *lock);
extern int __must_check mutex_lock_interruptible(struct mutex *lock);
extern int __must_check mutex_lock_killable(struct mutex *lock);
# 185 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mutex.h" 3
extern int mutex_trylock(struct mutex *lock);
extern void mutex_unlock(struct mutex *lock);

extern int atomic_dec_and_mutex_lock(atomic_t *cnt, struct mutex *lock);
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rwsem.h" 1 3
# 13 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rwsem.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rwsem.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rwsem.h" 2 3

struct rw_semaphore;




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/rwsem.h" 1 3
# 23 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rwsem.h" 2 3





extern void down_read(struct rw_semaphore *sem);




extern int down_read_trylock(struct rw_semaphore *sem);




extern void down_write(struct rw_semaphore *sem);




extern int down_write_trylock(struct rw_semaphore *sem);




extern void up_read(struct rw_semaphore *sem);




extern void up_write(struct rw_semaphore *sem);




extern void downgrade_write(struct rw_semaphore *sem);
# 15 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/srcu.h" 1 3
# 32 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/srcu.h" 3
struct srcu_struct_array {
 int c[2];
};

struct srcu_struct {
 int completed;
 struct srcu_struct_array *per_cpu_ref;
 struct mutex mutex;
};







int init_srcu_struct(struct srcu_struct *sp);
void cleanup_srcu_struct(struct srcu_struct *sp);
int srcu_read_lock(struct srcu_struct *sp) ;
void srcu_read_unlock(struct srcu_struct *sp, int idx) ;
void synchronize_srcu(struct srcu_struct *sp);
void synchronize_srcu_expedited(struct srcu_struct *sp);
long srcu_batches_completed(struct srcu_struct *sp);
# 16 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 2 3
# 50 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 3
struct notifier_block {
 int (*notifier_call)(struct notifier_block *, unsigned long, void *);
 struct notifier_block *next;
 int priority;
};

struct atomic_notifier_head {
 spinlock_t lock;
 struct notifier_block *head;
};

struct blocking_notifier_head {
 struct rw_semaphore rwsem;
 struct notifier_block *head;
};

struct raw_notifier_head {
 struct notifier_block *head;
};

struct srcu_notifier_head {
 struct mutex mutex;
 struct srcu_struct srcu;
 struct notifier_block *head;
};
# 89 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/notifier.h" 3
extern void srcu_init_notifier_head(struct srcu_notifier_head *nh);
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/memory_hotplug.h" 2 3

struct page;
struct zone;
struct pglist_data;
struct mem_section;
# 172 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/memory_hotplug.h" 3
static inline void pgdat_resize_lock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_unlock(struct pglist_data *p, unsigned long *f) {}
static inline void pgdat_resize_init(struct pglist_data *pgdat) {}

static inline unsigned zone_span_seqbegin(struct zone *zone)
{
 return 0;
}
static inline int zone_span_seqretry(struct zone *zone, unsigned iv)
{
 return 0;
}
static inline void zone_span_writelock(struct zone *zone) {}
static inline void zone_span_writeunlock(struct zone *zone) {}
static inline void zone_seqlock_init(struct zone *zone) {}

static inline int mhp_notimplemented(const char *func)
{
 printk(KERN_WARNING "%s() called, with CONFIG_MEMORY_HOTPLUG disabled\n", func);
 dump_stack();
 return -38;
}

static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}
# 206 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/memory_hotplug.h" 3
static inline int is_mem_section_removable(unsigned long pfn,
     unsigned long nr_pages)
{
 return 0;
}


extern int mem_online_node(int nid);
extern int add_memory(int nid, u64 start, u64 size);
extern int arch_add_memory(int nid, u64 start, u64 size);
extern int remove_memory(u64 start, u64 size);
extern int sparse_add_one_section(struct zone *zone, unsigned long start_pfn,
        int nr_pages);
extern void sparse_remove_one_section(struct zone *zone, struct mem_section *ms);
extern struct page *sparse_decode_mem_map(unsigned long coded_mem_map,
       unsigned long pnum);
# 718 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 2 3

extern struct mutex zonelists_mutex;
void get_zone_counts(unsigned long *active, unsigned long *inactive,
   unsigned long *free);
void build_all_zonelists(void *data);
void wakeup_kswapd(struct zone *zone, int order);
bool zone_watermark_ok(struct zone *z, int order, unsigned long mark,
  int classzone_idx, int alloc_flags);
bool zone_watermark_ok_safe(struct zone *z, int order, unsigned long mark,
  int classzone_idx, int alloc_flags);
enum memmap_context {
 MEMMAP_EARLY,
 MEMMAP_HOTPLUG,
};
extern int init_currently_empty_zone(struct zone *zone, unsigned long start_pfn,
         unsigned long size,
         enum memmap_context context);




static inline void memory_present(int nid, unsigned long start, unsigned long end) {}
# 751 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
static inline int populated_zone(struct zone *zone)
{
 return (!!zone->present_pages);
}

extern int movable_zone;

static inline int zone_movable_is_highmem(void)
{



 return 0;

}

static inline int is_highmem_idx(enum zone_type idx)
{




 return 0;

}

static inline int is_normal_idx(enum zone_type idx)
{
 return (idx == ZONE_NORMAL);
}







static inline int is_highmem(struct zone *zone)
{






 return 0;

}

static inline int is_normal(struct zone *zone)
{
 return zone == zone->zone_pgdat->node_zones + ZONE_NORMAL;
}

static inline int is_dma32(struct zone *zone)
{



 return 0;

}

static inline int is_dma(struct zone *zone)
{



 return 0;

}


struct ctl_table;
int free_kbytes_sysctl_handler(struct ctl_table *, int,
     void *, size_t *, loff_t *);
extern int sysctl_lowmem_reserve_ratio[4 -1];
int lowmem_reserve_ratio_sysctl_handler(struct ctl_table *, int,
     void *, size_t *, loff_t *);
int percpu_pagelist_fraction_sysctl_handler(struct ctl_table *, int,
     void *, size_t *, loff_t *);
int sysctl_min_unmapped_ratio_sysctl_handler(struct ctl_table *, int,
   void *, size_t *, loff_t *);
int sysctl_min_slab_ratio_sysctl_handler(struct ctl_table *, int,
   void *, size_t *, loff_t *);

extern int numa_zonelist_order_handler(struct ctl_table *, int,
   void *, size_t *, loff_t *);
extern char numa_zonelist_order[];




extern struct pglist_data contig_page_data;
# 854 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
extern struct pglist_data *first_online_pgdat(void);
extern struct pglist_data *next_online_pgdat(struct pglist_data *pgdat);
extern struct zone *next_zone(struct zone *zone);
# 886 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
static inline struct zone *zonelist_zone(struct zoneref *zoneref)
{
 return zoneref->zone;
}

static inline int zonelist_zone_idx(struct zoneref *zoneref)
{
 return zoneref->zone_idx;
}

static inline int zonelist_node_idx(struct zoneref *zoneref)
{




 return 0;

}
# 919 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
struct zoneref *next_zones_zonelist(struct zoneref *z,
     enum zone_type highest_zoneidx,
     nodemask_t *nodes,
     struct zone **zone);
# 936 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
static inline struct zoneref *first_zones_zonelist(struct zonelist *zonelist,
     enum zone_type highest_zoneidx,
     nodemask_t *nodes,
     struct zone **zone)
{
 return next_zones_zonelist(zonelist->_zonerefs, highest_zoneidx, nodes,
        zone);
}
# 979 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
static inline unsigned long early_pfn_to_nid(unsigned long pfn)
{
 return 0;
}
# 1160 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
void memory_present(int nid, unsigned long start, unsigned long end);
unsigned long __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) node_memmap_size_bytes(int, unsigned long, unsigned long);
# 1194 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmzone.h" 3
static inline int memmap_valid_within(unsigned long pfn,
     struct page *page, struct zone *zone)
{
 return 1;
}
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/topology.h" 1 3
# 33 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/topology.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/smp.h" 1 3
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/smp.h" 3
extern void cpu_idle(void);

struct call_single_data {
 struct list_head list;
 void (*func) (void *info);
 void *info;
 u16 flags;
 u16 priv;
};


extern unsigned int total_cpus;

int smp_call_function_single(int cpuid, void (*func) (void *info), void *info,
    int wait);
# 126 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/smp.h" 3
static inline void smp_send_stop(void) { }





static inline int up_smp_call_function(void (*func)(void *), void *info)
{
 return 0;
}
# 160 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/smp.h" 3
static inline void smp_send_reschedule(int cpu) { }




static inline void call_function_init(void) { }
# 197 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/smp.h" 3
extern void arch_disable_smp_support(void);

void smp_setup_processor_id(void);
# 34 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/topology.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/topology.h" 1 3
# 169 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/topology.h" 3
static inline int numa_node_id(void)
{
 return 0;
}

static inline int early_cpu_to_node(int cpu)
{
 return 0;
}

static inline void setup_node_to_cpumask_map(void) { }



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/topology.h" 1 3
# 184 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/topology.h" 2 3

extern const struct cpumask *cpu_coregroup_mask(int cpu);
# 197 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/topology.h" 3
static inline void arch_fix_phys_package_id(int num, u32 slot)
{
}

struct pci_bus;
int x86_pci_root_bus_node(int bus);
void x86_pci_root_bus_resources(int bus, struct list_head *resources);
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/topology.h" 2 3
# 48 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/topology.h" 3
int arch_update_cpu_topology(void);
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmdebug.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/autoconf.h" 1 3
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/mmdebug.h" 2 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 2 3

struct vm_area_struct;
# 126 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 3
static inline int allocflags_to_migratetype(gfp_t gfp_flags)
{
 WARN_ON((gfp_flags & ((( gfp_t)0x80000u)|(( gfp_t)0x08u))) == ((( gfp_t)0x80000u)|(( gfp_t)0x08u)));

 if (unlikely(page_group_by_mobility_disabled))
  return 0;


 return (((gfp_flags & (( gfp_t)0x08u)) != 0) << 1) |
  ((gfp_flags & (( gfp_t)0x80000u)) != 0);
}
# 221 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 3
static inline enum zone_type gfp_zone(gfp_t flags)
{
 enum zone_type z;
 int bit = flags & ((( gfp_t)0x01u)|(( gfp_t)0x02u)|(( gfp_t)0x04u)|(( gfp_t)0x08u));

 z = (( (ZONE_NORMAL << 0 * 2) | (ZONE_DMA << (( gfp_t)0x01u) * 2) | (ZONE_NORMAL << (( gfp_t)0x02u) * 2) | (ZONE_DMA32 << (( gfp_t)0x04u) * 2) | (ZONE_NORMAL << (( gfp_t)0x08u) * 2) | (ZONE_DMA << ((( gfp_t)0x08u) | (( gfp_t)0x01u)) * 2) | (ZONE_MOVABLE << ((( gfp_t)0x08u) | (( gfp_t)0x02u)) * 2) | (ZONE_DMA32 << ((( gfp_t)0x08u) | (( gfp_t)0x04u)) * 2)) >> (bit * 2)) &
      ((1 << 2) - 1);

 if (__builtin_constant_p(bit))
  ((void)sizeof(char[1 - 2 * !!((( 1 << ((( gfp_t)0x01u) | (( gfp_t)0x02u)) | 1 << ((( gfp_t)0x01u) | (( gfp_t)0x04u)) | 1 << ((( gfp_t)0x04u) | (( gfp_t)0x02u)) | 1 << ((( gfp_t)0x01u) | (( gfp_t)0x04u) | (( gfp_t)0x02u)) | 1 << ((( gfp_t)0x08u) | (( gfp_t)0x02u) | (( gfp_t)0x01u)) | 1 << ((( gfp_t)0x08u) | (( gfp_t)0x04u) | (( gfp_t)0x01u)) | 1 << ((( gfp_t)0x08u) | (( gfp_t)0x04u) | (( gfp_t)0x02u)) | 1 << ((( gfp_t)0x08u) | (( gfp_t)0x04u) | (( gfp_t)0x01u) | (( gfp_t)0x02u))) >> bit) & 1)]));
 else {



 }
 return z;
}
# 246 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 3
static inline int gfp_zonelist(gfp_t flags)
{
 if (0 && unlikely(flags & (( gfp_t)0x40000u)))
  return 1;

 return 0;
}
# 263 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 3
static inline struct zonelist *node_zonelist(int nid, gfp_t flags)
{
 return (&contig_page_data)->node_zonelists + gfp_zonelist(flags);
}


static inline void arch_free_page(struct page *page, int order) { }


static inline void arch_alloc_page(struct page *page, int order) { }


struct page *
__alloc_pages_nodemask(gfp_t gfp_mask, unsigned int order,
         struct zonelist *zonelist, nodemask_t *nodemask);

static inline struct page *
__alloc_pages(gfp_t gfp_mask, unsigned int order,
  struct zonelist *zonelist)
{
 return __alloc_pages_nodemask(gfp_mask, order, zonelist, ((void *)0));
}

static inline struct page *alloc_pages_node(int nid, gfp_t gfp_mask,
      unsigned int order)
{

 if (nid < 0)
  nid = (((void)(0),0));

 return __alloc_pages(gfp_mask, order, node_zonelist(nid, gfp_mask));
}

static inline struct page *alloc_pages_exact_node(int nid, gfp_t gfp_mask,
      unsigned int order)
{
 do { } while (0);

 return __alloc_pages(gfp_mask, order, node_zonelist(nid, gfp_mask));
}


extern struct page *alloc_pages_current(gfp_t gfp_mask, unsigned order);

static inline struct page *
alloc_pages(gfp_t gfp_mask, unsigned int order)
{
 return alloc_pages_current(gfp_mask, order);
}
extern struct page *alloc_pages_vma(gfp_t gfp_mask, int order,
       struct vm_area_struct *vma, unsigned long addr,
   int node);
# 327 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/gfp.h" 3
extern unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order);
extern unsigned long get_zeroed_page(gfp_t gfp_mask);

void *alloc_pages_exact(size_t size, gfp_t gfp_mask);
void free_pages_exact(void *virt, size_t size);

void *alloc_pages_exact_nid(int nid, size_t size, gfp_t gfp_mask);







extern void __free_pages(struct page *page, unsigned int order);
extern void free_pages(unsigned long addr, unsigned int order);
extern void free_hot_page(struct page *page);




void page_alloc_init(void);
void drain_zone_pages(struct zone *zone, struct per_cpu_pages *pcp);
void drain_all_pages(void);
void drain_local_pages(void *dummy);

extern gfp_t gfp_allowed_mask;

extern void pm_restrict_gfp_mask(void);
extern void pm_restore_gfp_mask(void);
# 13 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 2 3
# 93 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
void __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) kmem_cache_init(void);
int slab_is_available(void);

struct kmem_cache *kmem_cache_create(const char *, size_t, size_t,
   unsigned long,
   void (*)(void *));
void kmem_cache_destroy(struct kmem_cache *);
int kmem_cache_shrink(struct kmem_cache *);
void kmem_cache_free(struct kmem_cache *, void *);
unsigned int kmem_cache_size(struct kmem_cache *);
const char *kmem_cache_name(struct kmem_cache *);
int kmem_ptr_validate(struct kmem_cache *cachep, const void *ptr);
# 136 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
void * __must_check __krealloc(const void *, size_t, gfp_t);
void * __must_check krealloc(const void *, size_t, gfp_t);
void kfree(const void *);
void kzfree(const void *);
size_t ksize(const void *);
# 166 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 1 3
# 36 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 3
struct kmem_cache {

 struct array_cache *array[4096];

 unsigned int batchcount;
 unsigned int limit;
 unsigned int shared;

 unsigned int buffer_size;
 u32 reciprocal_buffer_size;


 unsigned int flags;
 unsigned int num;



 unsigned int gfporder;


 gfp_t gfpflags;

 size_t colour;
 unsigned int colour_off;
 struct kmem_cache *slabp_cache;
 unsigned int slab_size;
 unsigned int dflags;


 void (*ctor)(void *obj);


 const char *name;
 struct list_head next;
# 105 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 3
 struct kmem_list3 *nodelists[(1 << 0)];



};


struct cache_sizes {
 size_t cs_size;
 struct kmem_cache *cs_cachep;

 struct kmem_cache *cs_dmacachep;

};
extern struct cache_sizes malloc_sizes[];

void *kmem_cache_alloc(struct kmem_cache *, gfp_t);
void *__kmalloc(size_t size, gfp_t flags);


extern void *kmem_cache_alloc_notrace(struct kmem_cache *cachep, gfp_t flags);
extern void *kmem_cache_alloc_trace(size_t size,
        struct kmem_cache *cachep, gfp_t flags);
extern size_t slab_buffer_size(struct kmem_cache *cachep);
# 145 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 3
static __inline __attribute__ ((__always_inline__)) void *kmalloc(size_t size, gfp_t flags)
{
 struct kmem_cache *cachep;
 void *ret;

 if (__builtin_constant_p(size)) {
  int i = 0;

  if (!size)
   return ((void *)16);






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmalloc_sizes.h" 1 3

 if (size <= 32) goto found; else i++;

 if (size <= 64) goto found; else i++;



 if (size <= 128) goto found; else i++;

 if (size <= 192) goto found; else i++;

 if (size <= 256) goto found; else i++;
 if (size <= 512) goto found; else i++;
 if (size <= 1024) goto found; else i++;
 if (size <= 2048) goto found; else i++;
 if (size <= 4096) goto found; else i++;
 if (size <= 8192) goto found; else i++;
 if (size <= 16384) goto found; else i++;
 if (size <= 32768) goto found; else i++;
 if (size <= 65536) goto found; else i++;
 if (size <= 131072) goto found; else i++;

 if (size <= 262144) goto found; else i++;


 if (size <= 524288) goto found; else i++;


 if (size <= 1048576) goto found; else i++;


 if (size <= 2097152) goto found; else i++;


 if (size <= 4194304) goto found; else i++;
# 162 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 2 3

  return ((void *)0);
found:

  if (flags & (( gfp_t)0x01u))
   cachep = malloc_sizes[i].cs_dmacachep;
  else

   cachep = malloc_sizes[i].cs_cachep;

  ret = kmem_cache_alloc_trace(size, cachep, flags);

  return ret;
 }
 return __kmalloc(size, flags);
}


extern void *__kmalloc_node(size_t size, gfp_t flags, int node);
extern void *kmem_cache_alloc_node(struct kmem_cache *, gfp_t flags, int node);


extern void *kmem_cache_alloc_node_notrace(struct kmem_cache *cachep,
        gfp_t flags,
        int nodeid);
extern void *kmem_cache_alloc_node_trace(size_t size,
      struct kmem_cache *cachep,
      gfp_t flags,
      int nodeid);
# 205 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 3
static __inline __attribute__ ((__always_inline__)) void *kmalloc_node(size_t size, gfp_t flags, int node)
{
 struct kmem_cache *cachep;

 if (__builtin_constant_p(size)) {
  int i = 0;

  if (!size)
   return ((void *)16);






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmalloc_sizes.h" 1 3

 if (size <= 32) goto found; else i++;

 if (size <= 64) goto found; else i++;



 if (size <= 128) goto found; else i++;

 if (size <= 192) goto found; else i++;

 if (size <= 256) goto found; else i++;
 if (size <= 512) goto found; else i++;
 if (size <= 1024) goto found; else i++;
 if (size <= 2048) goto found; else i++;
 if (size <= 4096) goto found; else i++;
 if (size <= 8192) goto found; else i++;
 if (size <= 16384) goto found; else i++;
 if (size <= 32768) goto found; else i++;
 if (size <= 65536) goto found; else i++;
 if (size <= 131072) goto found; else i++;

 if (size <= 262144) goto found; else i++;


 if (size <= 524288) goto found; else i++;


 if (size <= 1048576) goto found; else i++;


 if (size <= 2097152) goto found; else i++;


 if (size <= 4194304) goto found; else i++;
# 221 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab_def.h" 2 3

  return ((void *)0);
found:

  if (flags & (( gfp_t)0x01u))
   cachep = malloc_sizes[i].cs_dmacachep;
  else

   cachep = malloc_sizes[i].cs_cachep;

  return kmem_cache_alloc_node_trace(size, cachep, flags, node);
 }
 return __kmalloc_node(size, flags, node);
}
# 167 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 2 3
# 220 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
static inline void *kmalloc_array(size_t n, size_t size, gfp_t flags)
{
 if (size != 0 && n > ULONG_MAX / size)
  return ((void *)0);
 return __kmalloc(n * size, flags);
}







static inline void *kcalloc(size_t n, size_t size, gfp_t flags)
{
 return kmalloc_array(n, size, flags | (( gfp_t)0x8000u));
}
# 314 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/slab.h" 3
static inline void *kmem_cache_zalloc(struct kmem_cache *k, gfp_t flags)
{
 return kmem_cache_alloc(k, flags | (( gfp_t)0x8000u));
}






static __inline __attribute__ ((__always_inline__)) void *kzalloc(size_t size, gfp_t flags)
{
 return kmalloc(size, flags | (( gfp_t)0x8000u));
}







static inline void *kzalloc_node(size_t size, gfp_t flags, int node)
{
 return kmalloc_node(size, flags | (( gfp_t)0x8000u), node);
}

void __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) kmem_cache_init_late(void);
# 3 "testlist.c" 2
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 1 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/stat.h" 1 3
# 11 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmod.h" 1 3
# 26 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmod.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/workqueue.h" 1 3







# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 1 3
# 24 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/time.h" 1 3
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/time.h" 3
struct timespec {
 __kernel_time_t tv_sec;
 long tv_nsec;
};


struct timeval {
 __kernel_time_t tv_sec;
 __kernel_suseconds_t tv_usec;
};

struct timezone {
 int tz_minuteswest;
 int tz_dsttime;
};
# 303 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/time.h" 3
struct itimerspec {
 struct timespec it_interval;
 struct timespec it_value;
};

struct itimerval {
 struct timeval it_interval;
 struct timeval it_value;
};
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timex.h" 1 3
# 64 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timex.h" 3
struct timex {
 unsigned int modes;
 long offset;
 long freq;
 long maxerror;
 long esterror;
 int status;
 long constant;
 long precision;
 long tolerance;


 struct timeval time;
 long tick;

 long ppsfreq;
 long jitter;
 int shift;
 long stabil;
 long jitcnt;
 long calcnt;
 long errcnt;
 long stbcnt;

 int tai;

 int :32; int :32; int :32; int :32;
 int :32; int :32; int :32; int :32;
 int :32; int :32; int :32;
};
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/param.h" 1 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/param.h" 1 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/param.h" 2 3
# 10 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 2 3
# 81 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 3
extern u64 __attribute__((section(".data"))) jiffies_64;
extern unsigned long volatile __attribute__((section(".data"))) jiffies;


u64 get_jiffies_64(void);
# 187 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 3
extern unsigned long preset_lpj;
# 300 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/jiffies.h" 3
extern unsigned int jiffies_to_msecs(const unsigned long j);
extern unsigned int jiffies_to_usecs(const unsigned long j);
extern unsigned long msecs_to_jiffies(const unsigned int m);
extern unsigned long usecs_to_jiffies(const unsigned int u);
extern unsigned long timespec_to_jiffies(const struct timespec *value);
extern void jiffies_to_timespec(const unsigned long jiffies,
    struct timespec *value);
extern unsigned long timeval_to_jiffies(const struct timeval *value);
extern void jiffies_to_timeval(const unsigned long jiffies,
          struct timeval *value);
extern clock_t jiffies_to_clock_t(long x);
extern unsigned long clock_t_to_jiffies(unsigned long x);
extern u64 jiffies_64_to_clock_t(u64 x);
extern u64 nsec_to_clock_t(u64 x);
extern u64 nsecs_to_jiffies64(u64 n);
extern unsigned long nsecs_to_jiffies(u64 n);
# 26 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 2 3
# 46 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
union ktime {
 s64 tv64;

 struct {



 s32 nsec, sec;

 } tv;

};

typedef union ktime ktime_t;
# 144 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline ktime_t ktime_set(const long secs, const unsigned long nsecs)
{
 return (ktime_t) { .tv = { .sec = secs, .nsec = nsecs } };
}
# 156 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline ktime_t ktime_sub(const ktime_t lhs, const ktime_t rhs)
{
 ktime_t res;

 res.tv64 = lhs.tv64 - rhs.tv64;
 if (res.tv.nsec < 0)
  res.tv.nsec += NSEC_PER_SEC;

 return res;
}
# 174 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline ktime_t ktime_add(const ktime_t add1, const ktime_t add2)
{
 ktime_t res;

 res.tv64 = add1.tv64 + add2.tv64;
# 187 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
 if (res.tv.nsec >= NSEC_PER_SEC)
  res.tv64 += (u32)-NSEC_PER_SEC;

 return res;
}
# 200 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
extern ktime_t ktime_add_ns(const ktime_t kt, u64 nsec);
# 209 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
extern ktime_t ktime_sub_ns(const ktime_t kt, u64 nsec);







static inline ktime_t timespec_to_ktime(const struct timespec ts)
{
 return (ktime_t) { .tv = { .sec = (s32)ts.tv_sec,
          .nsec = (s32)ts.tv_nsec } };
}







static inline ktime_t timeval_to_ktime(const struct timeval tv)
{
 return (ktime_t) { .tv = { .sec = (s32)tv.tv_sec,
       .nsec = (s32)tv.tv_usec * 1000 } };
}







static inline struct timespec ktime_to_timespec(const ktime_t kt)
{
 return (struct timespec) { .tv_sec = (time_t) kt.tv.sec,
       .tv_nsec = (long) kt.tv.nsec };
}







static inline struct timeval ktime_to_timeval(const ktime_t kt)
{
 return (struct timeval) {
  .tv_sec = (time_t) kt.tv.sec,
  .tv_usec = (suseconds_t) (kt.tv.nsec / NSEC_PER_USEC) };
}







static inline s64 ktime_to_ns(const ktime_t kt)
{
 return (s64) kt.tv.sec * NSEC_PER_SEC + kt.tv.nsec;
}
# 280 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline int ktime_equal(const ktime_t cmp1, const ktime_t cmp2)
{
 return cmp1.tv64 == cmp2.tv64;
}
# 295 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline int ktime_compare(const ktime_t cmp1, const ktime_t cmp2)
{
 if (cmp1.tv64 < cmp2.tv64)
  return -1;
 if (cmp1.tv64 > cmp2.tv64)
  return 1;
 return 0;
}
# 311 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline bool ktime_after(const ktime_t cmp1, const ktime_t cmp2)
{
 return ktime_compare(cmp1, cmp2) > 0;
}
# 323 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
static inline bool ktime_before(const ktime_t cmp1, const ktime_t cmp2)
{
 return ktime_compare(cmp1, cmp2) < 0;
}

static inline s64 ktime_to_us(const ktime_t kt)
{
 struct timeval tv = ktime_to_timeval(kt);
 return (s64) tv.tv_sec * USEC_PER_SEC + tv.tv_usec;
}

static inline s64 ktime_to_ms(const ktime_t kt)
{
 struct timeval tv = ktime_to_timeval(kt);
 return (s64) tv.tv_sec * MSEC_PER_SEC + tv.tv_usec / USEC_PER_MSEC;
}

static inline s64 ktime_us_delta(const ktime_t later, const ktime_t earlier)
{
       return ktime_to_us(ktime_sub(later, earlier));
}

static inline ktime_t ktime_add_us(const ktime_t kt, const u64 usec)
{
 return ktime_add_ns(kt, usec * 1000);
}

static inline ktime_t ktime_add_ms(const ktime_t kt, const u64 msec)
{
 return ktime_add_ns(kt, msec * NSEC_PER_MSEC);
}

static inline ktime_t ktime_sub_us(const ktime_t kt, const u64 usec)
{
 return ktime_sub_ns(kt, usec * 1000);
}

extern ktime_t ktime_add_safe(const ktime_t lhs, const ktime_t rhs);
# 372 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/ktime.h" 3
extern void ktime_get_ts(struct timespec *ts);




static inline ktime_t ns_to_ktime(u64 ns)
{
 static const ktime_t ktime_zero = { .tv64 = 0 };

 return ktime_add_ns(ktime_zero, ns);
}

static inline ktime_t ms_to_ktime(u64 ms)
{
 static const ktime_t ktime_zero = { .tv64 = 0 };

 return ktime_add_ms(ktime_zero, ms);
}
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/debugobjects.h" 1 3






enum debug_obj_state {
 ODEBUG_STATE_NONE,
 ODEBUG_STATE_INIT,
 ODEBUG_STATE_INACTIVE,
 ODEBUG_STATE_ACTIVE,
 ODEBUG_STATE_DESTROYED,
 ODEBUG_STATE_NOTAVAILABLE,
 ODEBUG_STATE_MAX,
};

struct debug_obj_descr;
# 26 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/debugobjects.h" 3
struct debug_obj {
 struct hlist_node node;
 enum debug_obj_state state;
 void *object;
 struct debug_obj_descr *descr;
};
# 45 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/debugobjects.h" 3
struct debug_obj_descr {
 const char *name;

 int (*fixup_init) (void *addr, enum debug_obj_state state);
 int (*fixup_activate) (void *addr, enum debug_obj_state state);
 int (*fixup_destroy) (void *addr, enum debug_obj_state state);
 int (*fixup_free) (void *addr, enum debug_obj_state state);
};
# 66 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/debugobjects.h" 3
static inline void
debug_object_init (void *addr, struct debug_obj_descr *descr) { }
static inline void
debug_object_init_on_stack(void *addr, struct debug_obj_descr *descr) { }
static inline void
debug_object_activate (void *addr, struct debug_obj_descr *descr) { }
static inline void
debug_object_deactivate(void *addr, struct debug_obj_descr *descr) { }
static inline void
debug_object_destroy (void *addr, struct debug_obj_descr *descr) { }
static inline void
debug_object_free (void *addr, struct debug_obj_descr *descr) { }

static inline void debug_objects_early_init(void) { }
static inline void debug_objects_mem_init(void) { }





static inline void
debug_check_no_obj_freed(const void *address, unsigned long size) { }
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 2 3


struct tvec_base;

struct timer_list {
 struct list_head entry;
 unsigned long expires;

 void (*function)(unsigned long);
 unsigned long data;

 struct tvec_base *base;

 void *start_site;
 char start_comm[16];
 int start_pid;




};

extern struct tvec_base boot_tvec_bases;
# 59 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 3
void init_timer_key(struct timer_list *timer,
      const char *name,
      struct lock_class_key *key);
void init_timer_deferrable_key(struct timer_list *timer,
          const char *name,
          struct lock_class_key *key);
# 125 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 3
static inline void destroy_timer_on_stack(struct timer_list *timer) { }
static inline void init_timer_on_stack_key(struct timer_list *timer,
        const char *name,
        struct lock_class_key *key)
{
 init_timer_key(timer, name, key);
}


static inline void setup_timer_key(struct timer_list * timer,
    const char *name,
    struct lock_class_key *key,
    void (*function)(unsigned long),
    unsigned long data)
{
 timer->function = function;
 timer->data = data;
 init_timer_key(timer, name, key);
}

static inline void setup_timer_on_stack_key(struct timer_list *timer,
     const char *name,
     struct lock_class_key *key,
     void (*function)(unsigned long),
     unsigned long data)
{
 timer->function = function;
 timer->data = data;
 init_timer_on_stack_key(timer, name, key);
}

extern void setup_deferrable_timer_on_stack_key(struct timer_list *timer,
      const char *name,
      struct lock_class_key *key,
      void (*function)(unsigned long),
      unsigned long data);
# 172 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 3
static inline int timer_pending(const struct timer_list * timer)
{
 return timer->entry.next != ((void *)0);
}

extern void add_timer_on(struct timer_list *timer, int cpu);
extern int del_timer(struct timer_list * timer);
extern int mod_timer(struct timer_list *timer, unsigned long expires);
extern int mod_timer_pending(struct timer_list *timer, unsigned long expires);
extern int mod_timer_pinned(struct timer_list *timer, unsigned long expires);
# 196 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 3
extern unsigned long get_next_timer_interrupt(unsigned long now);






extern int timer_stats_active;



extern void init_timer_stats(void);

extern void timer_stats_update_stats(void *timer, pid_t pid, void *startf,
         void *timerf, char *comm,
         unsigned int timer_flag);

extern void __timer_stats_timer_set_start_info(struct timer_list *timer,
            void *addr);

static inline void timer_stats_timer_set_start_info(struct timer_list *timer)
{
 if (likely(!timer_stats_active))
  return;
 __timer_stats_timer_set_start_info(timer, __builtin_return_address(0));
}

static inline void timer_stats_timer_clear_start_info(struct timer_list *timer)
{
 timer->start_site = ((void *)0);
}
# 241 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/timer.h" 3
extern void add_timer(struct timer_list *timer);


  extern int try_to_del_timer_sync(struct timer_list *timer);
  extern int del_timer_sync(struct timer_list *timer);







extern void init_timers(void);
extern void run_local_timers(void);
struct hrtimer;
extern enum hrtimer_restart it_real_fn(struct hrtimer *);

unsigned long __round_jiffies(unsigned long j, int cpu);
unsigned long __round_jiffies_relative(unsigned long j, int cpu);
unsigned long round_jiffies(unsigned long j);
unsigned long round_jiffies_relative(unsigned long j);

unsigned long __round_jiffies_up(unsigned long j, int cpu);
unsigned long __round_jiffies_up_relative(unsigned long j, int cpu);
unsigned long round_jiffies_up(unsigned long j);
unsigned long round_jiffies_up_relative(unsigned long j);
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/workqueue.h" 2 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 13 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/workqueue.h" 2 3

struct workqueue_struct;

struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);







enum {

 WORK_BUSY_PENDING = 1 << 0,
 WORK_BUSY_RUNNING = 1 << 1,
};

struct work_struct {
 atomic_long_t data;



 struct list_head entry;
 work_func_t func;



};



struct delayed_work {
 struct work_struct work;
 struct timer_list timer;
};

static inline struct delayed_work *to_delayed_work(struct work_struct *work)
{
 return container_of(work, struct delayed_work, work);
}

struct execute_work {
 struct work_struct work;
};
# 173 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/workqueue.h" 3
extern struct workqueue_struct *
__create_workqueue_key(const char *name, int singlethread,
         int freezeable, int rt, struct lock_class_key *key,
         const char *lock_name);

extern struct workqueue_struct *system_wq;
extern struct workqueue_struct *system_long_wq;
extern struct workqueue_struct *system_power_efficient_wq;
# 211 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/workqueue.h" 3
extern void destroy_workqueue(struct workqueue_struct *wq);

extern int queue_work(struct workqueue_struct *wq, struct work_struct *work);
extern int queue_work_on(int cpu, struct workqueue_struct *wq,
   struct work_struct *work);
extern int queue_delayed_work(struct workqueue_struct *wq,
   struct delayed_work *work, unsigned long delay);
extern int queue_delayed_work_on(int cpu, struct workqueue_struct *wq,
   struct delayed_work *work, unsigned long delay);

extern void flush_workqueue(struct workqueue_struct *wq);
extern void flush_scheduled_work(void);
extern void flush_delayed_work(struct delayed_work *work);

extern int schedule_work(struct work_struct *work);
extern int schedule_work_on(int cpu, struct work_struct *work);
extern int schedule_delayed_work(struct delayed_work *work, unsigned long delay);
extern int schedule_delayed_work_on(int cpu, struct delayed_work *work,
     unsigned long delay);
extern int schedule_on_each_cpu(work_func_t func);
extern int current_is_keventd(void);
extern int keventd_up(void);

extern void init_workqueues(void);
int execute_in_process_context(work_func_t fn, struct execute_work *);

extern int flush_work(struct work_struct *work);

extern int cancel_work_sync(struct work_struct *work);

extern unsigned int work_busy(struct work_struct *work);
extern bool mod_delayed_work(struct workqueue_struct *wq,
        struct delayed_work *dwork, unsigned long delay);







static inline int cancel_delayed_work(struct delayed_work *work)
{
 int ret;

 ret = del_timer_sync(&work->timer);
 if (ret)
  clear_bit(0, ((unsigned long *)(&(&work->work)->data)));
 return ret;
}






static inline int __cancel_delayed_work(struct delayed_work *work)
{
 int ret;

 ret = del_timer(&work->timer);
 if (ret)
  clear_bit(0, ((unsigned long *)(&(&work->work)->data)));
 return ret;
}

extern int cancel_delayed_work_sync(struct delayed_work *work);


static inline
void cancel_rearming_delayed_workqueue(struct workqueue_struct *wq,
     struct delayed_work *work)
{
 cancel_delayed_work_sync(work);
}


static inline
void cancel_rearming_delayed_work(struct delayed_work *work)
{
 cancel_delayed_work_sync(work);
}







long work_on_cpu(unsigned int cpu, long (*fn)(void *), void *arg);
# 27 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmod.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysctl.h" 1 3
# 28 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysctl.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 29 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysctl.h" 2 3



struct completion;






struct __sysctl_args {
 int *name;
 int nlen;
 void *oldval;
 size_t *oldlenp;
 void *newval;
 size_t newlen;
 unsigned long __unused[4];
};
# 59 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysctl.h" 3
enum
{
 CTL_KERN=1,
 CTL_VM=2,
 CTL_NET=3,
 CTL_PROC=4,
 CTL_FS=5,
 CTL_DEBUG=6,
 CTL_DEV=7,
 CTL_BUS=8,
 CTL_ABI=9,
 CTL_CPU=10,
 CTL_ARLAN=254,
 CTL_S390DBF=5677,
 CTL_SUNRPC=7249,
 CTL_PM=9899,
 CTL_FRV=9898,
};


enum
{
 CTL_BUS_ISA=1
};


enum
{
 INOTIFY_MAX_USER_INSTANCES=1,
 INOTIFY_MAX_USER_WATCHES=2,
 INOTIFY_MAX_QUEUED_EVENTS=3
};


enum
{
 KERN_OSTYPE=1,
 KERN_OSRELEASE=2,
 KERN_OSREV=3,
 KERN_VERSION=4,
 KERN_SECUREMASK=5,
 KERN_PROF=6,
 KERN_NODENAME=7,
 KERN_DOMAINNAME=8,

 KERN_PANIC=15,
 KERN_REALROOTDEV=16,

 KERN_SPARC_REBOOT=21,
 KERN_CTLALTDEL=22,
 KERN_PRINTK=23,
 KERN_NAMETRANS=24,
 KERN_PPC_HTABRECLAIM=25,
 KERN_PPC_ZEROPAGED=26,
 KERN_PPC_POWERSAVE_NAP=27,
 KERN_MODPROBE=28,
 KERN_SG_BIG_BUFF=29,
 KERN_ACCT=30,
 KERN_PPC_L2CR=31,

 KERN_RTSIGNR=32,
 KERN_RTSIGMAX=33,

 KERN_SHMMAX=34,
 KERN_MSGMAX=35,
 KERN_MSGMNB=36,
 KERN_MSGPOOL=37,
 KERN_SYSRQ=38,
 KERN_MAX_THREADS=39,
  KERN_RANDOM=40,
  KERN_SHMALL=41,
  KERN_MSGMNI=42,
  KERN_SEM=43,
  KERN_SPARC_STOP_A=44,
  KERN_SHMMNI=45,
 KERN_OVERFLOWUID=46,
 KERN_OVERFLOWGID=47,
 KERN_SHMPATH=48,
 KERN_HOTPLUG=49,
 KERN_IEEE_EMULATION_WARNINGS=50,
 KERN_S390_USER_DEBUG_LOGGING=51,
 KERN_CORE_USES_PID=52,
 KERN_TAINTED=53,
 KERN_CADPID=54,
 KERN_PIDMAX=55,
   KERN_CORE_PATTERN=56,
 KERN_PANIC_ON_OOPS=57,
 KERN_HPPA_PWRSW=58,
 KERN_HPPA_UNALIGNED=59,
 KERN_PRINTK_RATELIMIT=60,
 KERN_PRINTK_RATELIMIT_BURST=61,
 KERN_PTY=62,
 KERN_NGROUPS_MAX=63,
 KERN_SPARC_SCONS_PWROFF=64,
 KERN_HZ_TIMER=65,
 KERN_UNKNOWN_NMI_PANIC=66,
 KERN_BOOTLOADER_TYPE=67,
 KERN_RANDOMIZE=68,
 KERN_SETUID_DUMPABLE=69,
 KERN_SPIN_RETRY=70,
 KERN_ACPI_VIDEO_FLAGS=71,
 KERN_IA64_UNALIGNED=72,
 KERN_COMPAT_LOG=73,
 KERN_MAX_LOCK_DEPTH=74,
 KERN_NMI_WATCHDOG=75,
 KERN_PANIC_ON_NMI=76,
 KERN_PANIC_ON_WARN=77,
};




enum
{
 VM_UNUSED1=1,
 VM_UNUSED2=2,
 VM_UNUSED3=3,
 VM_UNUSED4=4,
 VM_OVERCOMMIT_MEMORY=5,
 VM_UNUSED5=6,
 VM_UNUSED7=7,
 VM_UNUSED8=8,
 VM_UNUSED9=9,
 VM_PAGE_CLUSTER=10,
 VM_DIRTY_BACKGROUND=11,
 VM_DIRTY_RATIO=12,
 VM_DIRTY_WB_CS=13,
 VM_DIRTY_EXPIRE_CS=14,
 VM_NR_PDFLUSH_THREADS=15,
 VM_OVERCOMMIT_RATIO=16,
 VM_PAGEBUF=17,
 VM_HUGETLB_PAGES=18,
 VM_SWAPPINESS=19,
 VM_LOWMEM_RESERVE_RATIO=20,
 VM_MIN_FREE_KBYTES=21,
 VM_MAX_MAP_COUNT=22,
 VM_LAPTOP_MODE=23,
 VM_BLOCK_DUMP=24,
 VM_HUGETLB_GROUP=25,
 VM_VFS_CACHE_PRESSURE=26,
 VM_LEGACY_VA_LAYOUT=27,
 VM_SWAP_TOKEN_TIMEOUT=28,
 VM_DROP_PAGECACHE=29,
 VM_PERCPU_PAGELIST_FRACTION=30,
 VM_ZONE_RECLAIM_MODE=31,
 VM_MIN_UNMAPPED=32,
 VM_PANIC_ON_OOM=33,
 VM_VDSO_ENABLED=34,
 VM_MIN_SLAB=35,
};



enum
{
 NET_CORE=1,
 NET_ETHER=2,
 NET_802=3,
 NET_UNIX=4,
 NET_IPV4=5,
 NET_IPX=6,
 NET_ATALK=7,
 NET_NETROM=8,
 NET_AX25=9,
 NET_BRIDGE=10,
 NET_ROSE=11,
 NET_IPV6=12,
 NET_X25=13,
 NET_TR=14,
 NET_DECNET=15,
 NET_ECONET=16,
 NET_SCTP=17,
 NET_LLC=18,
 NET_NETFILTER=19,
 NET_DCCP=20,
 NET_IRDA=412,
};


enum
{
 RANDOM_POOLSIZE=1,
 RANDOM_ENTROPY_COUNT=2,
 RANDOM_READ_THRESH=3,
 RANDOM_WRITE_THRESH=4,
 RANDOM_BOOT_ID=5,
 RANDOM_UUID=6
};


enum
{
 PTY_MAX=1,
 PTY_NR=2
};


enum
{
 BUS_ISA_MEM_BASE=1,
 BUS_ISA_PORT_BASE=2,
 BUS_ISA_PORT_SHIFT=3
};


enum
{
 NET_CORE_WMEM_MAX=1,
 NET_CORE_RMEM_MAX=2,
 NET_CORE_WMEM_DEFAULT=3,
 NET_CORE_RMEM_DEFAULT=4,

 NET_CORE_MAX_BACKLOG=6,
 NET_CORE_FASTROUTE=7,
 NET_CORE_MSG_COST=8,
 NET_CORE_MSG_BURST=9,
 NET_CORE_OPTMEM_MAX=10,
 NET_CORE_HOT_LIST_LENGTH=11,
 NET_CORE_DIVERT_VERSION=12,
 NET_CORE_NO_CONG_THRESH=13,
 NET_CORE_NO_CONG=14,
 NET_CORE_LO_CONG=15,
 NET_CORE_MOD_CONG=16,
 NET_CORE_DEV_WEIGHT=17,
 NET_CORE_SOMAXCONN=18,
 NET_CORE_BUDGET=19,
 NET_CORE_AEVENT_ETIME=20,
 NET_CORE_AEVENT_RSEQTH=21,
 NET_CORE_WARNINGS=22,
};







enum
{
 NET_UNIX_DESTROY_DELAY=1,
 NET_UNIX_DELETE_DELAY=2,
 NET_UNIX_MAX_DGRAM_QLEN=3,
};


enum
{
 NET_NF_CONNTRACK_MAX=1,
 NET_NF_CONNTRACK_TCP_TIMEOUT_SYN_SENT=2,
 NET_NF_CONNTRACK_TCP_TIMEOUT_SYN_RECV=3,
 NET_NF_CONNTRACK_TCP_TIMEOUT_ESTABLISHED=4,
 NET_NF_CONNTRACK_TCP_TIMEOUT_FIN_WAIT=5,
 NET_NF_CONNTRACK_TCP_TIMEOUT_CLOSE_WAIT=6,
 NET_NF_CONNTRACK_TCP_TIMEOUT_LAST_ACK=7,
 NET_NF_CONNTRACK_TCP_TIMEOUT_TIME_WAIT=8,
 NET_NF_CONNTRACK_TCP_TIMEOUT_CLOSE=9,
 NET_NF_CONNTRACK_UDP_TIMEOUT=10,
 NET_NF_CONNTRACK_UDP_TIMEOUT_STREAM=11,
 NET_NF_CONNTRACK_ICMP_TIMEOUT=12,
 NET_NF_CONNTRACK_GENERIC_TIMEOUT=13,
 NET_NF_CONNTRACK_BUCKETS=14,
 NET_NF_CONNTRACK_LOG_INVALID=15,
 NET_NF_CONNTRACK_TCP_TIMEOUT_MAX_RETRANS=16,
 NET_NF_CONNTRACK_TCP_LOOSE=17,
 NET_NF_CONNTRACK_TCP_BE_LIBERAL=18,
 NET_NF_CONNTRACK_TCP_MAX_RETRANS=19,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_CLOSED=20,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_WAIT=21,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_ECHOED=22,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_ESTABLISHED=23,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_SENT=24,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_RECD=25,
 NET_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_ACK_SENT=26,
 NET_NF_CONNTRACK_COUNT=27,
 NET_NF_CONNTRACK_ICMPV6_TIMEOUT=28,
 NET_NF_CONNTRACK_FRAG6_TIMEOUT=29,
 NET_NF_CONNTRACK_FRAG6_LOW_THRESH=30,
 NET_NF_CONNTRACK_FRAG6_HIGH_THRESH=31,
 NET_NF_CONNTRACK_CHECKSUM=32,
};


enum
{

 NET_IPV4_FORWARD=8,
 NET_IPV4_DYNADDR=9,

 NET_IPV4_CONF=16,
 NET_IPV4_NEIGH=17,
 NET_IPV4_ROUTE=18,
 NET_IPV4_FIB_HASH=19,
 NET_IPV4_NETFILTER=20,

 NET_IPV4_TCP_TIMESTAMPS=33,
 NET_IPV4_TCP_WINDOW_SCALING=34,
 NET_IPV4_TCP_SACK=35,
 NET_IPV4_TCP_RETRANS_COLLAPSE=36,
 NET_IPV4_DEFAULT_TTL=37,
 NET_IPV4_AUTOCONFIG=38,
 NET_IPV4_NO_PMTU_DISC=39,
 NET_IPV4_TCP_SYN_RETRIES=40,
 NET_IPV4_IPFRAG_HIGH_THRESH=41,
 NET_IPV4_IPFRAG_LOW_THRESH=42,
 NET_IPV4_IPFRAG_TIME=43,
 NET_IPV4_TCP_MAX_KA_PROBES=44,
 NET_IPV4_TCP_KEEPALIVE_TIME=45,
 NET_IPV4_TCP_KEEPALIVE_PROBES=46,
 NET_IPV4_TCP_RETRIES1=47,
 NET_IPV4_TCP_RETRIES2=48,
 NET_IPV4_TCP_FIN_TIMEOUT=49,
 NET_IPV4_IP_MASQ_DEBUG=50,
 NET_TCP_SYNCOOKIES=51,
 NET_TCP_STDURG=52,
 NET_TCP_RFC1337=53,
 NET_TCP_SYN_TAILDROP=54,
 NET_TCP_MAX_SYN_BACKLOG=55,
 NET_IPV4_LOCAL_PORT_RANGE=56,
 NET_IPV4_ICMP_ECHO_IGNORE_ALL=57,
 NET_IPV4_ICMP_ECHO_IGNORE_BROADCASTS=58,
 NET_IPV4_ICMP_SOURCEQUENCH_RATE=59,
 NET_IPV4_ICMP_DESTUNREACH_RATE=60,
 NET_IPV4_ICMP_TIMEEXCEED_RATE=61,
 NET_IPV4_ICMP_PARAMPROB_RATE=62,
 NET_IPV4_ICMP_ECHOREPLY_RATE=63,
 NET_IPV4_ICMP_IGNORE_BOGUS_ERROR_RESPONSES=64,
 NET_IPV4_IGMP_MAX_MEMBERSHIPS=65,
 NET_TCP_TW_RECYCLE=66,
 NET_IPV4_ALWAYS_DEFRAG=67,
 NET_IPV4_TCP_KEEPALIVE_INTVL=68,
 NET_IPV4_INET_PEER_THRESHOLD=69,
 NET_IPV4_INET_PEER_MINTTL=70,
 NET_IPV4_INET_PEER_MAXTTL=71,
 NET_IPV4_INET_PEER_GC_MINTIME=72,
 NET_IPV4_INET_PEER_GC_MAXTIME=73,
 NET_TCP_ORPHAN_RETRIES=74,
 NET_TCP_ABORT_ON_OVERFLOW=75,
 NET_TCP_SYNACK_RETRIES=76,
 NET_TCP_MAX_ORPHANS=77,
 NET_TCP_MAX_TW_BUCKETS=78,
 NET_TCP_FACK=79,
 NET_TCP_REORDERING=80,
 NET_TCP_ECN=81,
 NET_TCP_DSACK=82,
 NET_TCP_MEM=83,
 NET_TCP_WMEM=84,
 NET_TCP_RMEM=85,
 NET_TCP_APP_WIN=86,
 NET_TCP_ADV_WIN_SCALE=87,
 NET_IPV4_NONLOCAL_BIND=88,
 NET_IPV4_ICMP_RATELIMIT=89,
 NET_IPV4_ICMP_RATEMASK=90,
 NET_TCP_TW_REUSE=91,
 NET_TCP_FRTO=92,
 NET_TCP_LOW_LATENCY=93,
 NET_IPV4_IPFRAG_SECRET_INTERVAL=94,
 NET_IPV4_IGMP_MAX_MSF=96,
 NET_TCP_NO_METRICS_SAVE=97,
 NET_TCP_DEFAULT_WIN_SCALE=105,
 NET_TCP_MODERATE_RCVBUF=106,
 NET_TCP_TSO_WIN_DIVISOR=107,
 NET_TCP_BIC_BETA=108,
 NET_IPV4_ICMP_ERRORS_USE_INBOUND_IFADDR=109,
 NET_TCP_CONG_CONTROL=110,
 NET_TCP_ABC=111,
 NET_IPV4_IPFRAG_MAX_DIST=112,
  NET_TCP_MTU_PROBING=113,
 NET_TCP_BASE_MSS=114,
 NET_IPV4_TCP_WORKAROUND_SIGNED_WINDOWS=115,
 NET_TCP_DMA_COPYBREAK=116,
 NET_TCP_SLOW_START_AFTER_IDLE=117,
 NET_CIPSOV4_CACHE_ENABLE=118,
 NET_CIPSOV4_CACHE_BUCKET_SIZE=119,
 NET_CIPSOV4_RBM_OPTFMT=120,
 NET_CIPSOV4_RBM_STRICTVALID=121,
 NET_TCP_AVAIL_CONG_CONTROL=122,
 NET_TCP_ALLOWED_CONG_CONTROL=123,
 NET_TCP_MAX_SSTHRESH=124,
 NET_TCP_FRTO_RESPONSE=125,
};

enum {
 NET_IPV4_ROUTE_FLUSH=1,
 NET_IPV4_ROUTE_MIN_DELAY=2,
 NET_IPV4_ROUTE_MAX_DELAY=3,
 NET_IPV4_ROUTE_GC_THRESH=4,
 NET_IPV4_ROUTE_MAX_SIZE=5,
 NET_IPV4_ROUTE_GC_MIN_INTERVAL=6,
 NET_IPV4_ROUTE_GC_TIMEOUT=7,
 NET_IPV4_ROUTE_GC_INTERVAL=8,
 NET_IPV4_ROUTE_REDIRECT_LOAD=9,
 NET_IPV4_ROUTE_REDIRECT_NUMBER=10,
 NET_IPV4_ROUTE_REDIRECT_SILENCE=11,
 NET_IPV4_ROUTE_ERROR_COST=12,
 NET_IPV4_ROUTE_ERROR_BURST=13,
 NET_IPV4_ROUTE_GC_ELASTICITY=14,
 NET_IPV4_ROUTE_MTU_EXPIRES=15,
 NET_IPV4_ROUTE_MIN_PMTU=16,
 NET_IPV4_ROUTE_MIN_ADVMSS=17,
 NET_IPV4_ROUTE_SECRET_INTERVAL=18,
 NET_IPV4_ROUTE_GC_MIN_INTERVAL_MS=19,
};

enum
{
 NET_PROTO_CONF_ALL=-2,
 NET_PROTO_CONF_DEFAULT=-3


};

enum
{
 NET_IPV4_CONF_FORWARDING=1,
 NET_IPV4_CONF_MC_FORWARDING=2,
 NET_IPV4_CONF_PROXY_ARP=3,
 NET_IPV4_CONF_ACCEPT_REDIRECTS=4,
 NET_IPV4_CONF_SECURE_REDIRECTS=5,
 NET_IPV4_CONF_SEND_REDIRECTS=6,
 NET_IPV4_CONF_SHARED_MEDIA=7,
 NET_IPV4_CONF_RP_FILTER=8,
 NET_IPV4_CONF_ACCEPT_SOURCE_ROUTE=9,
 NET_IPV4_CONF_BOOTP_RELAY=10,
 NET_IPV4_CONF_LOG_MARTIANS=11,
 NET_IPV4_CONF_TAG=12,
 NET_IPV4_CONF_ARPFILTER=13,
 NET_IPV4_CONF_MEDIUM_ID=14,
 NET_IPV4_CONF_NOXFRM=15,
 NET_IPV4_CONF_NOPOLICY=16,
 NET_IPV4_CONF_FORCE_IGMP_VERSION=17,
 NET_IPV4_CONF_ARP_ANNOUNCE=18,
 NET_IPV4_CONF_ARP_IGNORE=19,
 NET_IPV4_CONF_PROMOTE_SECONDARIES=20,
 NET_IPV4_CONF_ARP_ACCEPT=21,
 NET_IPV4_CONF_ARP_NOTIFY=22,
 NET_IPV4_CONF_ACCEPT_LOCAL=23,
 NET_IPV4_CONF_SRC_VMARK=24,
 NET_IPV4_CONF_PROXY_ARP_PVLAN=25,
# 505 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysctl.h" 3
 NET_IPV4_CONF_ROUTE_LOCALNET=26,
 __NET_IPV4_CONF_MAX
};


enum
{
 NET_IPV4_NF_CONNTRACK_MAX=1,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_SYN_SENT=2,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_SYN_RECV=3,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_ESTABLISHED=4,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_FIN_WAIT=5,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_CLOSE_WAIT=6,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_LAST_ACK=7,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_TIME_WAIT=8,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_CLOSE=9,
 NET_IPV4_NF_CONNTRACK_UDP_TIMEOUT=10,
 NET_IPV4_NF_CONNTRACK_UDP_TIMEOUT_STREAM=11,
 NET_IPV4_NF_CONNTRACK_ICMP_TIMEOUT=12,
 NET_IPV4_NF_CONNTRACK_GENERIC_TIMEOUT=13,
 NET_IPV4_NF_CONNTRACK_BUCKETS=14,
 NET_IPV4_NF_CONNTRACK_LOG_INVALID=15,
 NET_IPV4_NF_CONNTRACK_TCP_TIMEOUT_MAX_RETRANS=16,
 NET_IPV4_NF_CONNTRACK_TCP_LOOSE=17,
 NET_IPV4_NF_CONNTRACK_TCP_BE_LIBERAL=18,
 NET_IPV4_NF_CONNTRACK_TCP_MAX_RETRANS=19,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_CLOSED=20,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_WAIT=21,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_COOKIE_ECHOED=22,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_ESTABLISHED=23,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_SENT=24,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_RECD=25,
  NET_IPV4_NF_CONNTRACK_SCTP_TIMEOUT_SHUTDOWN_ACK_SENT=26,
 NET_IPV4_NF_CONNTRACK_COUNT=27,
 NET_IPV4_NF_CONNTRACK_CHECKSUM=28,
};


enum {
 NET_IPV6_CONF=16,
 NET_IPV6_NEIGH=17,
 NET_IPV6_ROUTE=18,
 NET_IPV6_ICMP=19,
 NET_IPV6_BINDV6ONLY=20,
 NET_IPV6_IP6FRAG_HIGH_THRESH=21,
 NET_IPV6_IP6FRAG_LOW_THRESH=22,
 NET_IPV6_IP6FRAG_TIME=23,
 NET_IPV6_IP6FRAG_SECRET_INTERVAL=24,
 NET_IPV6_MLD_MAX_MSF=25,
};

enum {
 NET_IPV6_ROUTE_FLUSH=1,
 NET_IPV6_ROUTE_GC_THRESH=2,
 NET_IPV6_ROUTE_MAX_SIZE=3,
 NET_IPV6_ROUTE_GC_MIN_INTERVAL=4,
 NET_IPV6_ROUTE_GC_TIMEOUT=5,
 NET_IPV6_ROUTE_GC_INTERVAL=6,
 NET_IPV6_ROUTE_GC_ELASTICITY=7,
 NET_IPV6_ROUTE_MTU_EXPIRES=8,
 NET_IPV6_ROUTE_MIN_ADVMSS=9,
 NET_IPV6_ROUTE_GC_MIN_INTERVAL_MS=10
};

enum {
 NET_IPV6_FORWARDING=1,
 NET_IPV6_HOP_LIMIT=2,
 NET_IPV6_MTU=3,
 NET_IPV6_ACCEPT_RA=4,
 NET_IPV6_ACCEPT_REDIRECTS=5,
 NET_IPV6_AUTOCONF=6,
 NET_IPV6_DAD_TRANSMITS=7,
 NET_IPV6_RTR_SOLICITS=8,
 NET_IPV6_RTR_SOLICIT_INTERVAL=9,
 NET_IPV6_RTR_SOLICIT_DELAY=10,
 NET_IPV6_USE_TEMPADDR=11,
 NET_IPV6_TEMP_VALID_LFT=12,
 NET_IPV6_TEMP_PREFERED_LFT=13,
 NET_IPV6_REGEN_MAX_RETRY=14,
 NET_IPV6_MAX_DESYNC_FACTOR=15,
 NET_IPV6_MAX_ADDRESSES=16,
 NET_IPV6_FORCE_MLD_VERSION=17,
 NET_IPV6_ACCEPT_RA_DEFRTR=18,
 NET_IPV6_ACCEPT_RA_PINFO=19,
 NET_IPV6_ACCEPT_RA_RTR_PREF=20,
 NET_IPV6_RTR_PROBE_INTERVAL=21,
 NET_IPV6_ACCEPT_RA_RT_INFO_MAX_PLEN=22,
 NET_IPV6_PROXY_NDP=23,
 NET_IPV6_ACCEPT_SOURCE_ROUTE=25,
 __NET_IPV6_MAX
};


enum {
 NET_IPV6_ICMP_RATELIMIT=1
};


enum {
 NET_NEIGH_MCAST_SOLICIT=1,
 NET_NEIGH_UCAST_SOLICIT=2,
 NET_NEIGH_APP_SOLICIT=3,
 NET_NEIGH_RETRANS_TIME=4,
 NET_NEIGH_REACHABLE_TIME=5,
 NET_NEIGH_DELAY_PROBE_TIME=6,
 NET_NEIGH_GC_STALE_TIME=7,
 NET_NEIGH_UNRES_QLEN=8,
 NET_NEIGH_PROXY_QLEN=9,
 NET_NEIGH_ANYCAST_DELAY=10,
 NET_NEIGH_PROXY_DELAY=11,
 NET_NEIGH_LOCKTIME=12,
 NET_NEIGH_GC_INTERVAL=13,
 NET_NEIGH_GC_THRESH1=14,
 NET_NEIGH_GC_THRESH2=15,
 NET_NEIGH_GC_THRESH3=16,
 NET_NEIGH_RETRANS_TIME_MS=17,
 NET_NEIGH_REACHABLE_TIME_MS=18,
 __NET_NEIGH_MAX
};


enum {
 NET_DCCP_DEFAULT=1,
};


enum {
 NET_IPX_PPROP_BROADCASTING=1,
 NET_IPX_FORWARDING=2
};


enum {
 NET_LLC2=1,
 NET_LLC_STATION=2,
};


enum {
 NET_LLC2_TIMEOUT=1,
};


enum {
 NET_LLC_STATION_ACK_TIMEOUT=1,
};


enum {
 NET_LLC2_ACK_TIMEOUT=1,
 NET_LLC2_P_TIMEOUT=2,
 NET_LLC2_REJ_TIMEOUT=3,
 NET_LLC2_BUSY_TIMEOUT=4,
};


enum {
 NET_ATALK_AARP_EXPIRY_TIME=1,
 NET_ATALK_AARP_TICK_TIME=2,
 NET_ATALK_AARP_RETRANSMIT_LIMIT=3,
 NET_ATALK_AARP_RESOLVE_TIME=4
};



enum {
 NET_NETROM_DEFAULT_PATH_QUALITY=1,
 NET_NETROM_OBSOLESCENCE_COUNT_INITIALISER=2,
 NET_NETROM_NETWORK_TTL_INITIALISER=3,
 NET_NETROM_TRANSPORT_TIMEOUT=4,
 NET_NETROM_TRANSPORT_MAXIMUM_TRIES=5,
 NET_NETROM_TRANSPORT_ACKNOWLEDGE_DELAY=6,
 NET_NETROM_TRANSPORT_BUSY_DELAY=7,
 NET_NETROM_TRANSPORT_REQUESTED_WINDOW_SIZE=8,
 NET_NETROM_TRANSPORT_NO_ACTIVITY_TIMEOUT=9,
 NET_NETROM_ROUTING_CONTROL=10,
 NET_NETROM_LINK_FAILS_COUNT=11,
 NET_NETROM_RESET=12
};


enum {
 NET_AX25_IP_DEFAULT_MODE=1,
 NET_AX25_DEFAULT_MODE=2,
 NET_AX25_BACKOFF_TYPE=3,
 NET_AX25_CONNECT_MODE=4,
 NET_AX25_STANDARD_WINDOW=5,
 NET_AX25_EXTENDED_WINDOW=6,
 NET_AX25_T1_TIMEOUT=7,
 NET_AX25_T2_TIMEOUT=8,
 NET_AX25_T3_TIMEOUT=9,
 NET_AX25_IDLE_TIMEOUT=10,
 NET_AX25_N2=11,
 NET_AX25_PACLEN=12,
 NET_AX25_PROTOCOL=13,
 NET_AX25_DAMA_SLAVE_TIMEOUT=14
};


enum {
 NET_ROSE_RESTART_REQUEST_TIMEOUT=1,
 NET_ROSE_CALL_REQUEST_TIMEOUT=2,
 NET_ROSE_RESET_REQUEST_TIMEOUT=3,
 NET_ROSE_CLEAR_REQUEST_TIMEOUT=4,
 NET_ROSE_ACK_HOLD_BACK_TIMEOUT=5,
 NET_ROSE_ROUTING_CONTROL=6,
 NET_ROSE_LINK_FAIL_TIMEOUT=7,
 NET_ROSE_MAX_VCS=8,
 NET_ROSE_WINDOW_SIZE=9,
 NET_ROSE_NO_ACTIVITY_TIMEOUT=10
};


enum {
 NET_X25_RESTART_REQUEST_TIMEOUT=1,
 NET_X25_CALL_REQUEST_TIMEOUT=2,
 NET_X25_RESET_REQUEST_TIMEOUT=3,
 NET_X25_CLEAR_REQUEST_TIMEOUT=4,
 NET_X25_ACK_HOLD_BACK_TIMEOUT=5,
 NET_X25_FORWARD=6
};


enum
{
 NET_TR_RIF_TIMEOUT=1
};


enum {
 NET_DECNET_NODE_TYPE = 1,
 NET_DECNET_NODE_ADDRESS = 2,
 NET_DECNET_NODE_NAME = 3,
 NET_DECNET_DEFAULT_DEVICE = 4,
 NET_DECNET_TIME_WAIT = 5,
 NET_DECNET_DN_COUNT = 6,
 NET_DECNET_DI_COUNT = 7,
 NET_DECNET_DR_COUNT = 8,
 NET_DECNET_DST_GC_INTERVAL = 9,
 NET_DECNET_CONF = 10,
 NET_DECNET_NO_FC_MAX_CWND = 11,
 NET_DECNET_MEM = 12,
 NET_DECNET_RMEM = 13,
 NET_DECNET_WMEM = 14,
 NET_DECNET_DEBUG_LEVEL = 255
};


enum {
 NET_DECNET_CONF_LOOPBACK = -2,
 NET_DECNET_CONF_DDCMP = -3,
 NET_DECNET_CONF_PPP = -4,
 NET_DECNET_CONF_X25 = -5,
 NET_DECNET_CONF_GRE = -6,
 NET_DECNET_CONF_ETHER = -7


};


enum {
 NET_DECNET_CONF_DEV_PRIORITY = 1,
 NET_DECNET_CONF_DEV_T1 = 2,
 NET_DECNET_CONF_DEV_T2 = 3,
 NET_DECNET_CONF_DEV_T3 = 4,
 NET_DECNET_CONF_DEV_FORWARDING = 5,
 NET_DECNET_CONF_DEV_BLKSIZE = 6,
 NET_DECNET_CONF_DEV_STATE = 7
};


enum {
 NET_SCTP_RTO_INITIAL = 1,
 NET_SCTP_RTO_MIN = 2,
 NET_SCTP_RTO_MAX = 3,
 NET_SCTP_RTO_ALPHA = 4,
 NET_SCTP_RTO_BETA = 5,
 NET_SCTP_VALID_COOKIE_LIFE = 6,
 NET_SCTP_ASSOCIATION_MAX_RETRANS = 7,
 NET_SCTP_PATH_MAX_RETRANS = 8,
 NET_SCTP_MAX_INIT_RETRANSMITS = 9,
 NET_SCTP_HB_INTERVAL = 10,
 NET_SCTP_PRESERVE_ENABLE = 11,
 NET_SCTP_MAX_BURST = 12,
 NET_SCTP_ADDIP_ENABLE = 13,
 NET_SCTP_PRSCTP_ENABLE = 14,
 NET_SCTP_SNDBUF_POLICY = 15,
 NET_SCTP_SACK_TIMEOUT = 16,
 NET_SCTP_RCVBUF_POLICY = 17,
};


enum {
 NET_BRIDGE_NF_CALL_ARPTABLES = 1,
 NET_BRIDGE_NF_CALL_IPTABLES = 2,
 NET_BRIDGE_NF_CALL_IP6TABLES = 3,
 NET_BRIDGE_NF_FILTER_VLAN_TAGGED = 4,
 NET_BRIDGE_NF_FILTER_PPPOE_TAGGED = 5,
};


enum {
 NET_IRDA_DISCOVERY=1,
 NET_IRDA_DEVNAME=2,
 NET_IRDA_DEBUG=3,
 NET_IRDA_FAST_POLL=4,
 NET_IRDA_DISCOVERY_SLOTS=5,
 NET_IRDA_DISCOVERY_TIMEOUT=6,
 NET_IRDA_SLOT_TIMEOUT=7,
 NET_IRDA_MAX_BAUD_RATE=8,
 NET_IRDA_MIN_TX_TURN_TIME=9,
 NET_IRDA_MAX_TX_DATA_SIZE=10,
 NET_IRDA_MAX_TX_WINDOW=11,
 NET_IRDA_MAX_NOREPLY_TIME=12,
 NET_IRDA_WARN_NOREPLY_TIME=13,
 NET_IRDA_LAP_KEEPALIVE_TIME=14,
};



enum
{
 FS_NRINODE=1,
 FS_STATINODE=2,
 FS_MAXINODE=3,
 FS_NRDQUOT=4,
 FS_MAXDQUOT=5,
 FS_NRFILE=6,
 FS_MAXFILE=7,
 FS_DENTRY=8,
 FS_NRSUPER=9,
 FS_MAXSUPER=10,
 FS_OVERFLOWUID=11,
 FS_OVERFLOWGID=12,
 FS_LEASES=13,
 FS_DIR_NOTIFY=14,
 FS_LEASE_TIME=15,
 FS_DQSTATS=16,
 FS_XFS=17,
 FS_AIO_NR=18,
 FS_AIO_MAX_NR=19,
 FS_INOTIFY=20,
 FS_OCFS2=988,
};


enum {
 FS_DQ_LOOKUPS = 1,
 FS_DQ_DROPS = 2,
 FS_DQ_READS = 3,
 FS_DQ_WRITES = 4,
 FS_DQ_CACHE_HITS = 5,
 FS_DQ_ALLOCATED = 6,
 FS_DQ_FREE = 7,
 FS_DQ_SYNCS = 8,
 FS_DQ_WARNINGS = 9,
};




enum {
 DEV_CDROM=1,
 DEV_HWMON=2,
 DEV_PARPORT=3,
 DEV_RAID=4,
 DEV_MAC_HID=5,
 DEV_SCSI=6,
 DEV_IPMI=7,
};


enum {
 DEV_CDROM_INFO=1,
 DEV_CDROM_AUTOCLOSE=2,
 DEV_CDROM_AUTOEJECT=3,
 DEV_CDROM_DEBUG=4,
 DEV_CDROM_LOCK=5,
 DEV_CDROM_CHECK_MEDIA=6
};


enum {
 DEV_PARPORT_DEFAULT=-3
};


enum {
 DEV_RAID_SPEED_LIMIT_MIN=1,
 DEV_RAID_SPEED_LIMIT_MAX=2
};


enum {
 DEV_PARPORT_DEFAULT_TIMESLICE=1,
 DEV_PARPORT_DEFAULT_SPINTIME=2
};


enum {
 DEV_PARPORT_SPINTIME=1,
 DEV_PARPORT_BASE_ADDR=2,
 DEV_PARPORT_IRQ=3,
 DEV_PARPORT_DMA=4,
 DEV_PARPORT_MODES=5,
 DEV_PARPORT_DEVICES=6,
 DEV_PARPORT_AUTOPROBE=16
};


enum {
 DEV_PARPORT_DEVICES_ACTIVE=-3,
};


enum {
 DEV_PARPORT_DEVICE_TIMESLICE=1,
};


enum {
 DEV_MAC_HID_KEYBOARD_SENDS_LINUX_KEYCODES=1,
 DEV_MAC_HID_KEYBOARD_LOCK_KEYCODES=2,
 DEV_MAC_HID_MOUSE_BUTTON_EMULATION=3,
 DEV_MAC_HID_MOUSE_BUTTON2_KEYCODE=4,
 DEV_MAC_HID_MOUSE_BUTTON3_KEYCODE=5,
 DEV_MAC_HID_ADB_MOUSE_SENDS_KEYCODES=6
};


enum {
 DEV_SCSI_LOGGING_LEVEL=1,
};


enum {
 DEV_IPMI_POWEROFF_POWERCYCLE=1,
};


enum
{
 ABI_DEFHANDLER_COFF=1,
 ABI_DEFHANDLER_ELF=2,
 ABI_DEFHANDLER_LCALL7=3,
 ABI_DEFHANDLER_LIBCSO=4,
 ABI_TRACE=5,
 ABI_FAKE_UTSNAME=6,
};
# 28 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmod.h" 2 3






extern int __request_module(bool wait, const char *name, ...)
 __attribute__((format(printf, 2, 3)));
# 47 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kmod.h" 3
struct key;
struct file;

enum umh_wait {
 UMH_NO_WAIT = -1,
 UMH_WAIT_EXEC = 0,
 UMH_WAIT_PROC = 1,
};



struct subprocess_info {
 struct work_struct work;
 struct completion *complete;
 struct cred *cred;
 char *path;
 char **argv;
 char **envp;
 enum umh_wait wait;
 int retval;
 int (*init)(struct subprocess_info *info);
 void (*cleanup)(struct subprocess_info *info);
 void *data;
};


struct subprocess_info *call_usermodehelper_setup(char *path, char **argv,
        char **envp, gfp_t gfp_mask);


void call_usermodehelper_setkeys(struct subprocess_info *info,
     struct key *session_keyring);
int call_usermodehelper_stdinpipe(struct subprocess_info *sub_info,
      struct file **filp);
void call_usermodehelper_setfns(struct subprocess_info *info,
      int (*init)(struct subprocess_info *info),
      void (*cleanup)(struct subprocess_info *info),
      void *data);


int call_usermodehelper_exec(struct subprocess_info *info, enum umh_wait wait);



void call_usermodehelper_freeinfo(struct subprocess_info *info);

static inline int
call_usermodehelper_fns(char *path, char **argv, char **envp,
   enum umh_wait wait,
   int (*init)(struct subprocess_info *info),
   void (*cleanup)(struct subprocess_info *), void *data)
{
 struct subprocess_info *info;
 gfp_t gfp_mask = (wait == UMH_NO_WAIT) ? ((( gfp_t)0x20u)) : ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u));

 info = call_usermodehelper_setup(path, argv, envp, gfp_mask);
 if (info == ((void *)0))
  return -12;
 call_usermodehelper_setfns(info, init, cleanup, data);
 return call_usermodehelper_exec(info, wait);
}

static inline int
call_usermodehelper(char *path, char **argv, char **envp, enum umh_wait wait)
{
 return call_usermodehelper_fns(path, argv, envp,
           wait, ((void *)0), ((void *)0), ((void *)0));
}

static inline int
call_usermodehelper_keys(char *path, char **argv, char **envp,
    struct key *session_keyring, enum umh_wait wait)
{
 struct subprocess_info *info;
 gfp_t gfp_mask = (wait == UMH_NO_WAIT) ? ((( gfp_t)0x20u)) : ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u));

 info = call_usermodehelper_setup(path, argv, envp, gfp_mask);
 if (info == ((void *)0))
  return -12;

 call_usermodehelper_setkeys(info, session_keyring);
 return call_usermodehelper_exec(info, wait);
}

extern struct ctl_table usermodehelper_table[];

extern void usermodehelper_init(void);

extern int usermodehelper_disable(void);
extern void usermodehelper_enable(void);
# 14 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 1 3




# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf-em.h" 1 3
# 6 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 2 3




struct file;
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 3
typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;


typedef __u64 Elf64_Addr;
typedef __u16 Elf64_Half;
typedef __s16 Elf64_SHalf;
typedef __u64 Elf64_Off;
typedef __s32 Elf64_Sword;
typedef __u32 Elf64_Word;
typedef __u64 Elf64_Xword;
typedef __s64 Elf64_Sxword;
# 149 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 3
typedef struct dynamic{
  Elf32_Sword d_tag;
  union{
    Elf32_Sword d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;

typedef struct {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;
# 172 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 3
typedef struct elf32_rel {
  Elf32_Addr r_offset;
  Elf32_Word r_info;
} Elf32_Rel;

typedef struct elf64_rel {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
} Elf64_Rel;

typedef struct elf32_rela{
  Elf32_Addr r_offset;
  Elf32_Word r_info;
  Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct elf64_rela {
  Elf64_Addr r_offset;
  Elf64_Xword r_info;
  Elf64_Sxword r_addend;
} Elf64_Rela;

typedef struct elf32_sym{
  Elf32_Word st_name;
  Elf32_Addr st_value;
  Elf32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct elf64_sym {
  Elf64_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf64_Half st_shndx;
  Elf64_Addr st_value;
  Elf64_Xword st_size;
} Elf64_Sym;




typedef struct elf32_hdr{
  unsigned char e_ident[16];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
  unsigned char e_ident[16];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;







typedef struct elf32_phdr{
  Elf32_Word p_type;
  Elf32_Off p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
} Elf32_Phdr;

typedef struct elf64_phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;
# 311 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 3
typedef struct elf32_shdr {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct elf64_shdr {
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;
# 402 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/elf.h" 3
typedef struct elf32_note {
  Elf32_Word n_namesz;
  Elf32_Word n_descsz;
  Elf32_Word n_type;
} Elf32_Nhdr;


typedef struct elf64_note {
  Elf64_Word n_namesz;
  Elf64_Word n_descsz;
  Elf64_Word n_type;
} Elf64_Nhdr;
# 15 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 1 3
# 21 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysfs.h" 1 3
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysfs.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 20 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysfs.h" 2 3

struct kobject;
struct module;






struct attribute {
 const char *name;
 struct module *owner;
 mode_t mode;




};
# 50 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysfs.h" 3
struct attribute_group {
 const char *name;
 mode_t (*is_visible)(struct kobject *,
           struct attribute *, int);
 struct attribute **attrs;
};
# 81 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/sysfs.h" 3
struct file;
struct vm_area_struct;

struct bin_attribute {
 struct attribute attr;
 size_t size;
 void *private;
 ssize_t (*read)(struct file *, struct kobject *, struct bin_attribute *,
   char *, loff_t, size_t);
 ssize_t (*write)(struct file *,struct kobject *, struct bin_attribute *,
    char *, loff_t, size_t);
 int (*mmap)(struct file *, struct kobject *, struct bin_attribute *attr,
      struct vm_area_struct *vma);
};



struct sysfs_ops {
 ssize_t (*show)(struct kobject *, struct attribute *,char *);
 ssize_t (*store)(struct kobject *,struct attribute *,const char *, size_t);
};

struct sysfs_dirent;



int sysfs_schedule_callback(struct kobject *kobj, void (*func)(void *),
       void *data, struct module *owner);

int __must_check sysfs_create_dir(struct kobject *kobj);
void sysfs_remove_dir(struct kobject *kobj);
int __must_check sysfs_rename_dir(struct kobject *kobj, const char *new_name);
int __must_check sysfs_move_dir(struct kobject *kobj,
    struct kobject *new_parent_kobj);

int __must_check sysfs_create_file(struct kobject *kobj,
       const struct attribute *attr);
int __must_check sysfs_create_files(struct kobject *kobj,
       const struct attribute **attr);
int __must_check sysfs_chmod_file(struct kobject *kobj, struct attribute *attr,
      mode_t mode);
void sysfs_remove_file(struct kobject *kobj, const struct attribute *attr);
void sysfs_remove_files(struct kobject *kobj, const struct attribute **attr);

int __must_check sysfs_create_bin_file(struct kobject *kobj,
           struct bin_attribute *attr);
void sysfs_remove_bin_file(struct kobject *kobj, struct bin_attribute *attr);

int __must_check sysfs_create_link(struct kobject *kobj, struct kobject *target,
       const char *name);
int __must_check sysfs_create_link_nowarn(struct kobject *kobj,
       struct kobject *target,
       const char *name);
void sysfs_remove_link(struct kobject *kobj, const char *name);

int __must_check sysfs_create_group(struct kobject *kobj,
        const struct attribute_group *grp);
int sysfs_update_group(struct kobject *kobj,
         const struct attribute_group *grp);
void sysfs_remove_group(struct kobject *kobj,
   const struct attribute_group *grp);
int sysfs_add_file_to_group(struct kobject *kobj,
   const struct attribute *attr, const char *group);
void sysfs_remove_file_from_group(struct kobject *kobj,
   const struct attribute *attr, const char *group);
int sysfs_merge_group(struct kobject *kobj,
         const struct attribute_group *grp);
void sysfs_unmerge_group(struct kobject *kobj,
         const struct attribute_group *grp);

void sysfs_notify(struct kobject *kobj, const char *dir, const char *attr);
void sysfs_notify_dirent(struct sysfs_dirent *sd);
struct sysfs_dirent *sysfs_get_dirent(struct sysfs_dirent *parent_sd,
          const unsigned char *name);
struct sysfs_dirent *sysfs_get(struct sysfs_dirent *sd);
void sysfs_put(struct sysfs_dirent *sd);
void sysfs_printk_last_file(void);
int __must_check sysfs_init(void);

static inline int sysfs_create_groups(struct kobject *kobj,
          const struct attribute_group **groups)
{
 int error = 0;
 int i;

 if (!groups)
  return 0;

 for (i = 0; groups[i]; i++) {
  error = sysfs_create_group(kobj, groups[i]);
  if (error) {
   while (--i >= 0)
    sysfs_remove_group(kobj, groups[i]);
   break;
  }
 }
 return error;
}
# 22 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kref.h" 1 3
# 21 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kref.h" 3
struct kref {
 atomic_t refcount;
};

void kref_set(struct kref *kref, int num);
void kref_init(struct kref *kref);
void kref_get(struct kref *kref);
int kref_put(struct kref *kref, void (*release) (struct kref *kref));
int kref_put_spinlock_irqsave(struct kref *kref,
 void (*release)(struct kref *kref), spinlock_t *lock);
int kref_sub(struct kref *kref, unsigned int count,
      void (*release) (struct kref *kref));
# 50 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kref.h" 3
static inline int __must_check kref_get_unless_zero(struct kref *kref)
{
 return atomic_add_unless(&kref->refcount, 1, 0);
}
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 26 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 28 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 2 3






extern char uevent_helper[];


extern u64 uevent_seqnum;
# 49 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 3
enum kobject_action {
 KOBJ_ADD,
 KOBJ_REMOVE,
 KOBJ_CHANGE,
 KOBJ_MOVE,
 KOBJ_ONLINE,
 KOBJ_OFFLINE,
 KOBJ_MAX
};

struct kobject {
 const char *name;
 struct list_head entry;
 struct kobject *parent;
 struct kset *kset;
 struct kobj_type *ktype;
 struct sysfs_dirent *sd;
 struct kref kref;
 unsigned int state_initialized:1;
 unsigned int state_in_sysfs:1;
 unsigned int state_add_uevent_sent:1;
 unsigned int state_remove_uevent_sent:1;
 unsigned int uevent_suppress:1;
};

extern int kobject_set_name(struct kobject *kobj, const char *name, ...)
       __attribute__((format(printf, 2, 3)));
extern int kobject_set_name_vargs(struct kobject *kobj, const char *fmt,
      va_list vargs);

static inline const char *kobject_name(const struct kobject *kobj)
{
 return kobj->name;
}

extern void kobject_init(struct kobject *kobj, struct kobj_type *ktype);
extern int __must_check kobject_add(struct kobject *kobj,
        struct kobject *parent,
        const char *fmt, ...);
extern int __must_check kobject_init_and_add(struct kobject *kobj,
          struct kobj_type *ktype,
          struct kobject *parent,
          const char *fmt, ...);

extern void kobject_del(struct kobject *kobj);

extern struct kobject * __must_check kobject_create(void);
extern struct kobject * __must_check kobject_create_and_add(const char *name,
      struct kobject *parent);

extern int __must_check kobject_rename(struct kobject *, const char *new_name);
extern int __must_check kobject_move(struct kobject *, struct kobject *);

extern struct kobject *kobject_get(struct kobject *kobj);
extern void kobject_put(struct kobject *kobj);

extern char *kobject_get_path(struct kobject *kobj, gfp_t flag);

struct kobj_type {
 void (*release)(struct kobject *kobj);
 const struct sysfs_ops *sysfs_ops;
 struct attribute **default_attrs;
};

struct kobj_uevent_env {
 char *envp[32];
 int envp_idx;
 char buf[2048];
 int buflen;
};

struct kset_uevent_ops {
 int (*filter)(struct kset *kset, struct kobject *kobj);
 const char *(*name)(struct kset *kset, struct kobject *kobj);
 int (*uevent)(struct kset *kset, struct kobject *kobj,
        struct kobj_uevent_env *env);
};

struct kobj_attribute {
 struct attribute attr;
 ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr,
   char *buf);
 ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t count);
};

extern const struct sysfs_ops kobj_sysfs_ops;
# 154 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kobject.h" 3
struct kset {
 struct list_head list;
 spinlock_t list_lock;
 struct kobject kobj;
 struct kset_uevent_ops *uevent_ops;
};

extern void kset_init(struct kset *kset);
extern int __must_check kset_register(struct kset *kset);
extern void kset_unregister(struct kset *kset);
extern struct kset * __must_check kset_create_and_add(const char *name,
      struct kset_uevent_ops *u,
      struct kobject *parent_kobj);

static inline struct kset *to_kset(struct kobject *kobj)
{
 return kobj ? container_of(kobj, struct kset, kobj) : ((void *)0);
}

static inline struct kset *kset_get(struct kset *k)
{
 return k ? to_kset(kobject_get(&k->kobj)) : ((void *)0);
}

static inline void kset_put(struct kset *k)
{
 kobject_put(&k->kobj);
}

static inline struct kobj_type *get_ktype(struct kobject *kobj)
{
 return kobj->ktype;
}

extern struct kobject *kset_find_obj(struct kset *, const char *);
extern struct kobject *kset_find_obj_hinted(struct kset *, const char *,
      struct kobject *);


extern struct kobject *kernel_kobj;

extern struct kobject *mm_kobj;

extern struct kobject *hypervisor_kobj;

extern struct kobject *power_kobj;

extern struct kobject *firmware_kobj;


int kobject_uevent(struct kobject *kobj, enum kobject_action action);
int kobject_uevent_env(struct kobject *kobj, enum kobject_action action,
   char *envp[]);

int add_uevent_var(struct kobj_uevent_env *env, const char *format, ...)
 __attribute__((format (printf, 2, 3)));

int kobject_action_type(const char *buf, size_t count,
   enum kobject_action *type);
# 17 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 1 3





# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 7 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 2 3
# 32 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 3
struct kernel_param;


typedef int (*param_set_fn)(const char *val, struct kernel_param *kp);

typedef int (*param_get_fn)(char *buffer, struct kernel_param *kp);




struct kernel_param {
 const char *name;
 u16 perm;
 u16 flags;
 param_set_fn set;
 param_get_fn get;
 union {
  void *arg;
  const struct kparam_string *str;
  const struct kparam_array *arr;
 };
};


struct kparam_string {
 unsigned int maxlen;
 char *string;
};


struct kparam_array
{
 unsigned int max;
 unsigned int *num;
 param_set_fn set;
 param_get_fn get;
 unsigned int elemsize;
 void *elem;
};
# 148 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 3
extern int parse_args(const char *name,
        char *args,
        struct kernel_param *params,
        unsigned num,
        int (*unknown)(char *param, char *val,
         const char *doing));



extern void destroy_params(const struct kernel_param *params, unsigned num);
# 171 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 3
extern int param_set_byte(const char *val, struct kernel_param *kp);
extern int param_get_byte(char *buffer, struct kernel_param *kp);


extern int param_set_short(const char *val, struct kernel_param *kp);
extern int param_get_short(char *buffer, struct kernel_param *kp);


extern int param_set_ushort(const char *val, struct kernel_param *kp);
extern int param_get_ushort(char *buffer, struct kernel_param *kp);


extern int param_set_int(const char *val, struct kernel_param *kp);
extern int param_get_int(char *buffer, struct kernel_param *kp);


extern int param_set_uint(const char *val, struct kernel_param *kp);
extern int param_get_uint(char *buffer, struct kernel_param *kp);


extern int param_set_long(const char *val, struct kernel_param *kp);
extern int param_get_long(char *buffer, struct kernel_param *kp);


extern int param_set_ulong(const char *val, struct kernel_param *kp);
extern int param_get_ulong(char *buffer, struct kernel_param *kp);


extern int param_set_charp(const char *val, struct kernel_param *kp);
extern int param_get_charp(char *buffer, struct kernel_param *kp);



extern int param_set_bool(const char *val, struct kernel_param *kp);
extern int param_get_bool(char *buffer, struct kernel_param *kp);
# 214 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 3
extern int param_set_invbool(const char *val, struct kernel_param *kp);
extern int param_get_invbool(char *buffer, struct kernel_param *kp);
# 232 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/moduleparam.h" 3
extern int param_array_set(const char *val, struct kernel_param *kp);
extern int param_array_get(char *buffer, struct kernel_param *kp);

extern int param_set_copystring(const char *val, struct kernel_param *kp);
extern int param_get_string(char *buffer, struct kernel_param *kp);



struct module;


extern int module_param_sysfs_setup(struct module *mod,
        struct kernel_param *kparam,
        unsigned int num_params);

extern void module_param_sysfs_remove(struct module *mod);
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/tracepoint.h" 1 3
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/tracepoint.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 1 3
# 42 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/completion.h" 1 3
# 25 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/completion.h" 3
struct completion {
 unsigned int done;
 wait_queue_head_t wait;
};
# 73 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/completion.h" 3
static inline void init_completion(struct completion *x)
{
 x->done = 0;
 init_waitqueue_head(&x->wait);
}

extern void wait_for_completion(struct completion *);
extern int wait_for_completion_interruptible(struct completion *x);
extern int wait_for_completion_killable(struct completion *x);
extern unsigned long wait_for_completion_timeout(struct completion *x,
         unsigned long timeout);
extern unsigned long wait_for_completion_interruptible_timeout(
   struct completion *x, unsigned long timeout);
extern bool try_wait_for_completion(struct completion *x);
extern bool completion_done(struct completion *x);

extern void complete(struct completion *);
extern void complete_all(struct completion *);
# 43 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 2 3






struct rcu_head {
 struct rcu_head *next;
 void (*func)(struct rcu_head *head);
};







extern void synchronize_rcu_bh(void);
extern void synchronize_sched(void);
extern void rcu_barrier(void);
extern void rcu_barrier_bh(void);
extern void rcu_barrier_sched(void);
extern void synchronize_sched_expedited(void);
extern int sched_expedited_torture_stats(char *page);


extern void rcu_init(void);
extern void rcu_scheduler_starting(void);
extern int rcu_needs_cpu(int cpu);
extern int rcu_scheduler_active;


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcutree.h" 1 3
# 33 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcutree.h" 3
struct notifier_block;

extern void rcu_sched_qs(int cpu);
extern void rcu_bh_qs(int cpu);
extern int rcu_cpu_notify(struct notifier_block *self,
     unsigned long action, void *hcpu);
extern int rcu_needs_cpu(int cpu);
extern int rcu_expedited_torture_stats(char *page);






static inline void rcu_virt_note_context_switch(int cpu)
{
 rcu_sched_qs(cpu);
}
# 60 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcutree.h" 3
static inline void __rcu_read_lock(void)
{
 do { } while (0);
}

static inline void __rcu_read_unlock(void)
{
 do { } while (0);
}



static inline void exit_rcu(void)
{
}



static inline void __rcu_read_lock_bh(void)
{
 local_bh_disable();
}
static inline void __rcu_read_unlock_bh(void)
{
 local_bh_enable();
}

extern void call_rcu_sched(struct rcu_head *head,
      void (*func)(struct rcu_head *rcu));
extern void synchronize_rcu_expedited(void);

static inline void synchronize_rcu_bh_expedited(void)
{
 synchronize_sched_expedited();
}

extern void __rcu_init(void);
extern void rcu_check_callbacks(int cpu, int user);

extern long rcu_batches_completed(void);
extern long rcu_batches_completed_bh(void);
extern long rcu_batches_completed_sched(void);


void rcu_enter_nohz(void);
void rcu_exit_nohz(void);
# 116 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcutree.h" 3
static inline int rcu_blocking_is_gp(void)
{
 return 1 == 1;
}
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 2 3
# 138 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
static inline void rcu_read_lock(void)
{
 __rcu_read_lock();
 (void)0;
 do { } while (0);
}
# 160 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
static inline void rcu_read_unlock(void)
{
 do { } while (0);
 (void)0;
 __rcu_read_unlock();
}
# 178 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
static inline void rcu_read_lock_bh(void)
{
 __rcu_read_lock_bh();
 (void)0;
 do { } while (0);
}






static inline void rcu_read_unlock_bh(void)
{
 do { } while (0);
 (void)0;
 __rcu_read_unlock_bh();
}
# 206 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
static inline void rcu_read_lock_sched(void)
{
 do { } while (0);
 (void)0;
 do { } while (0);
}


static inline __attribute__((no_instrument_function)) void rcu_read_lock_sched_notrace(void)
{
 do { } while (0);
 (void)0;
}






static inline void rcu_read_unlock_sched(void)
{
 do { } while (0);
 (void)0;
 do { } while (0);
}


static inline __attribute__((no_instrument_function)) void rcu_read_unlock_sched_notrace(void)
{
 (void)0;
 do { } while (0);
}
# 293 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
struct rcu_synchronize {
 struct rcu_head head;
 struct completion completion;
};

extern void wakeme_after_rcu(struct rcu_head *head);
# 311 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
extern void call_rcu(struct rcu_head *head,
         void (*func)(struct rcu_head *head));
# 332 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/rcupdate.h" 3
extern void call_rcu_bh(struct rcu_head *head,
   void (*func)(struct rcu_head *head));


extern void kfree(const void *);







static inline void __rcu_reclaim(struct rcu_head *head)
{
 unsigned long offset = (unsigned long)head->func;

 if (((offset) < 4096))
  kfree((void *)head - offset);
 else
  head->func(head);
}
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/tracepoint.h" 2 3

struct module;
struct tracepoint;

struct tracepoint {
 const char *name;
 int state;
 void (*regfunc)(void);
 void (*unregfunc)(void);
 void **funcs;
} __attribute__((aligned(32)));
# 40 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/tracepoint.h" 3
extern int tracepoint_probe_register(const char *name, void *probe);





extern int tracepoint_probe_unregister(const char *name, void *probe);

extern int tracepoint_probe_register_noupdate(const char *name, void *probe);
extern int tracepoint_probe_unregister_noupdate(const char *name, void *probe);
extern void tracepoint_probe_update_all(void);

struct tracepoint_iter {
 struct module *module;
 struct tracepoint *tracepoint;
};

extern void tracepoint_iter_start(struct tracepoint_iter *iter);
extern void tracepoint_iter_next(struct tracepoint_iter *iter);
extern void tracepoint_iter_stop(struct tracepoint_iter *iter);
extern void tracepoint_iter_reset(struct tracepoint_iter *iter);
extern int tracepoint_get_iter_range(struct tracepoint **tracepoint,
 struct tracepoint *begin, struct tracepoint *end);






static inline void tracepoint_synchronize_unregister(void)
{
 synchronize_sched();
}




extern void tracepoint_update_probe_range(struct tracepoint *begin,
 struct tracepoint *end);
# 19 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 1 3







# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/pfn.h" 1 3
# 9 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 2 3
# 68 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 3
extern void *pcpu_base_addr;
extern const unsigned long *pcpu_unit_offsets;

struct pcpu_group_info {
 int nr_units;
 unsigned long base_offset;
 unsigned int *cpu_map;

};

struct pcpu_alloc_info {
 size_t static_size;
 size_t reserved_size;
 size_t dyn_size;
 size_t unit_size;
 size_t atom_size;
 size_t alloc_size;
 size_t __ai_size;
 int nr_groups;
 struct pcpu_group_info groups[];
};

enum pcpu_fc {
 PCPU_FC_AUTO,
 PCPU_FC_EMBED,
 PCPU_FC_PAGE,

 PCPU_FC_NR,
};
extern const char *pcpu_fc_names[PCPU_FC_NR];

extern enum pcpu_fc pcpu_chosen_fc;

typedef void * (*pcpu_fc_alloc_fn_t)(unsigned int cpu, size_t size,
         size_t align);
typedef void (*pcpu_fc_free_fn_t)(void *ptr, size_t size);
typedef void (*pcpu_fc_populate_pte_fn_t)(unsigned long addr);
typedef int (pcpu_fc_cpu_distance_fn_t)(unsigned int from, unsigned int to);

extern struct pcpu_alloc_info * __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_alloc_alloc_info(int nr_groups,
            int nr_units);
extern void __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_free_alloc_info(struct pcpu_alloc_info *ai);

extern struct pcpu_alloc_info * __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_build_alloc_info(
    size_t reserved_size, ssize_t dyn_size,
    size_t atom_size,
    pcpu_fc_cpu_distance_fn_t cpu_distance_fn);

extern int __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_setup_first_chunk(const struct pcpu_alloc_info *ai,
      void *base_addr);


extern int __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_embed_first_chunk(size_t reserved_size, ssize_t dyn_size,
    size_t atom_size,
    pcpu_fc_cpu_distance_fn_t cpu_distance_fn,
    pcpu_fc_alloc_fn_t alloc_fn,
    pcpu_fc_free_fn_t free_fn);



extern int __attribute__ ((__section__(".init.text"))) __attribute__((no_instrument_function)) pcpu_page_first_chunk(size_t reserved_size,
    pcpu_fc_alloc_fn_t alloc_fn,
    pcpu_fc_free_fn_t free_fn,
    pcpu_fc_populate_pte_fn_t populate_pte_fn);
# 141 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 3
extern void *__alloc_reserved_percpu(size_t size, size_t align);
# 182 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 3
extern void *__alloc_percpu(size_t size, size_t align);
extern void free_percpu(void *__pdata);
extern phys_addr_t per_cpu_ptr_to_phys(void *addr);
# 246 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/percpu.h" 3
extern void __bad_size_call_parameter(void);
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 2 3


# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/atomic.h" 1 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 2 3


typedef struct {
 atomic_long_t a;
} local_t;






static inline void local_inc(local_t *l)
{
 asm volatile(" " "incq" " " "%0"
       : "+m" (l->a.counter));
}

static inline void local_dec(local_t *l)
{
 asm volatile(" " "decq" " " "%0"
       : "+m" (l->a.counter));
}

static inline void local_add(long i, local_t *l)
{
 asm volatile(" " "addq" " " "%1,%0"
       : "+m" (l->a.counter)
       : "ir" (i));
}

static inline void local_sub(long i, local_t *l)
{
 asm volatile(" " "subq" " " "%1,%0"
       : "+m" (l->a.counter)
       : "ir" (i));
}
# 54 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
static inline int local_sub_and_test(long i, local_t *l)
{
 unsigned char c;

 asm volatile(" " "subq" " " "%2,%0; sete %1"
       : "+m" (l->a.counter), "=qm" (c)
       : "ir" (i) : "memory");
 return c;
}
# 72 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
static inline int local_dec_and_test(local_t *l)
{
 unsigned char c;

 asm volatile(" " "decq" " " "%0; sete %1"
       : "+m" (l->a.counter), "=qm" (c)
       : : "memory");
 return c != 0;
}
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
static inline int local_inc_and_test(local_t *l)
{
 unsigned char c;

 asm volatile(" " "incq" " " "%0; sete %1"
       : "+m" (l->a.counter), "=qm" (c)
       : : "memory");
 return c != 0;
}
# 109 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
static inline int local_add_negative(long i, local_t *l)
{
 unsigned char c;

 asm volatile(" " "addq" " " "%2,%0; sets %1"
       : "+m" (l->a.counter), "=qm" (c)
       : "ir" (i) : "memory");
 return c;
}
# 126 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
static inline long local_add_return(long i, local_t *l)
{
 long __i;






 __i = i;
 asm volatile(" " "xaddq" " " "%0, %1;"
       : "+r" (i), "+m" (l->a.counter)
       : : "memory");
 return i + __i;
# 149 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/local.h" 3
}

static inline long local_sub_return(long i, local_t *l)
{
 return local_add_return(-i, l);
}
# 21 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/module.h" 1 3



# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/asm-generic/module.h" 1 3







struct mod_arch_specific
{
};
# 5 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/arch/x86/include/asm/module.h" 2 3
# 22 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3

# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 1 3






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/tracepoint.h" 1 3
# 8 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 2 3



struct module;






extern struct tracepoint
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_load
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ; static inline void
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 trace_module_load
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (struct module *mod) { if (unlikely(
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_load
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 .state)) do { void **it_func; rcu_read_lock_sched_notrace(); it_func = ({ typeof((&__tracepoint_module_load)->funcs) _________p1 = (*(volatile typeof((&__tracepoint_module_load)->funcs) *)&((&__tracepoint_module_load)->funcs)); do { } while (0); (_________p1); }); if (it_func) { do { ((void(*)(struct module *mod))(*it_func))(mod); } while (*(++it_func)); } rcu_read_unlock_sched_notrace(); } while (0); } static inline int
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 register_trace_module_load
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod)) { return tracepoint_probe_register("module_load", (void *)probe); } static inline int
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 unregister_trace_module_load
# 18 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod)) { return tracepoint_probe_unregister("module_load", (void *)probe); }
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ;

extern struct tracepoint
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_free
# 37 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ; static inline void
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 trace_module_free
# 37 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (struct module *mod) { if (unlikely(
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_free
# 37 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 .state)) do { void **it_func; rcu_read_lock_sched_notrace(); it_func = ({ typeof((&__tracepoint_module_free)->funcs) _________p1 = (*(volatile typeof((&__tracepoint_module_free)->funcs) *)&((&__tracepoint_module_free)->funcs)); do { } while (0); (_________p1); }); if (it_func) { do { ((void(*)(struct module *mod))(*it_func))(mod); } while (*(++it_func)); } rcu_read_unlock_sched_notrace(); } while (0); } static inline int
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 register_trace_module_free
# 37 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod)) { return tracepoint_probe_register("module_free", (void *)probe); } static inline int
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 unregister_trace_module_free
# 37 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod)) { return tracepoint_probe_unregister("module_free", (void *)probe); }
# 52 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ;


# 74 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ;

extern struct tracepoint




 __tracepoint_module_get
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ; static inline void




 trace_module_get
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (struct module *mod, unsigned long ip, int refcnt) { if (unlikely(




 __tracepoint_module_get
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 .state)) do { void **it_func; rcu_read_lock_sched_notrace(); it_func = ({ typeof((&__tracepoint_module_get)->funcs) _________p1 = (*(volatile typeof((&__tracepoint_module_get)->funcs) *)&((&__tracepoint_module_get)->funcs)); do { } while (0); (_________p1); }); if (it_func) { do { ((void(*)(struct module *mod, unsigned long ip, int refcnt))(*it_func))(mod, ip, refcnt); } while (*(++it_func)); } rcu_read_unlock_sched_notrace(); } while (0); } static inline int




 register_trace_module_get
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod, unsigned long ip, int refcnt)) { return tracepoint_probe_register("module_get", (void *)probe); } static inline int




 unregister_trace_module_get
# 76 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod, unsigned long ip, int refcnt)) { return tracepoint_probe_unregister("module_get", (void *)probe); }




 ;

extern struct tracepoint




 __tracepoint_module_put
# 83 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ; static inline void




 trace_module_put
# 83 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (struct module *mod, unsigned long ip, int refcnt) { if (unlikely(




 __tracepoint_module_put
# 83 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 .state)) do { void **it_func; rcu_read_lock_sched_notrace(); it_func = ({ typeof((&__tracepoint_module_put)->funcs) _________p1 = (*(volatile typeof((&__tracepoint_module_put)->funcs) *)&((&__tracepoint_module_put)->funcs)); do { } while (0); (_________p1); }); if (it_func) { do { ((void(*)(struct module *mod, unsigned long ip, int refcnt))(*it_func))(mod, ip, refcnt); } while (*(++it_func)); } rcu_read_unlock_sched_notrace(); } while (0); } static inline int




 register_trace_module_put
# 83 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod, unsigned long ip, int refcnt)) { return tracepoint_probe_register("module_put", (void *)probe); } static inline int




 unregister_trace_module_put
# 83 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(struct module *mod, unsigned long ip, int refcnt)) { return tracepoint_probe_unregister("module_put", (void *)probe); }




 ;

extern struct tracepoint
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_request
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ; static inline void
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 trace_module_request
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (char *name, bool wait, unsigned long ip) { if (unlikely(
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 __tracepoint_module_request
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 .state)) do { void **it_func; rcu_read_lock_sched_notrace(); it_func = ({ typeof((&__tracepoint_module_request)->funcs) _________p1 = (*(volatile typeof((&__tracepoint_module_request)->funcs) *)&((&__tracepoint_module_request)->funcs)); do { } while (0); (_________p1); }); if (it_func) { do { ((void(*)(char *name, bool wait, unsigned long ip))(*it_func))(name, wait, ip); } while (*(++it_func)); } rcu_read_unlock_sched_notrace(); } while (0); } static inline int
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 register_trace_module_request
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(char *name, bool wait, unsigned long ip)) { return tracepoint_probe_register("module_request", (void *)probe); } static inline int
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 unregister_trace_module_request
# 90 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 (void (*probe)(char *name, bool wait, unsigned long ip)) { return tracepoint_probe_unregister("module_request", (void *)probe); }
# 110 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 3
 ;






# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/define_trace.h" 1 3
# 118 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/trace/events/module.h" 2 3
# 24 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 2 3
# 35 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
struct kernel_symbol
{
 unsigned long value;
 const char *name;
};

struct modversion_info
{
 unsigned long crc;
 char name[(64 - sizeof(unsigned long))];
};

struct rheldata
{

 u8 rhel_major;
 u8 rhel_minor;
 u16 rhel_release;
};

struct module;

struct module_attribute {
        struct attribute attr;
        ssize_t (*show)(struct module_attribute *, struct module *, char *);
        ssize_t (*store)(struct module_attribute *, struct module *,
    const char *, size_t count);
 void (*setup)(struct module *, const char *);
 int (*test)(struct module *);
 void (*free)(struct module *);
};

struct module_version_attribute {
 struct module_attribute mattr;
 const char *module_name;
 const char *version;
};

struct module_kobject
{
 struct kobject kobj;
 struct module *mod;
 struct kobject *drivers_dir;
 struct module_param_attrs *mp;
};


extern int init_module(void);
extern void cleanup_module(void);


struct exception_table_entry;

const struct exception_table_entry *
search_extable(const struct exception_table_entry *first,
        const struct exception_table_entry *last,
        unsigned long value);
void sort_extable(struct exception_table_entry *start,
    struct exception_table_entry *finish);
void sort_main_extable(void);
void trim_init_extable(struct module *m);
# 210 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
const struct exception_table_entry *search_exception_tables(unsigned long add);

struct notifier_block;




void *__symbol_get(const char *symbol);
void *__symbol_get_gpl(const char *symbol);
# 267 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
enum module_state
{
 MODULE_STATE_LIVE,
 MODULE_STATE_COMING,
 MODULE_STATE_GOING,
};

struct module
{
 enum module_state state;


 struct list_head list;


 char name[(64 - sizeof(unsigned long))];


 struct module_kobject mkobj;
 struct module_attribute *modinfo_attrs;
 const char *version;
 const char *srcversion;
 struct kobject *holders_dir;


 const struct kernel_symbol *syms;
 const unsigned long *crcs;
 unsigned int num_syms;


 struct kernel_param *kp;
 unsigned int num_kp;


 unsigned int num_gpl_syms;
 const struct kernel_symbol *gpl_syms;
 const unsigned long *gpl_crcs;
# 318 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
 const struct kernel_symbol *gpl_future_syms;
 const unsigned long *gpl_future_crcs;
 unsigned int num_gpl_future_syms;


 unsigned int num_exentries;
 struct exception_table_entry *extable;


 int (*init)(void);


 void *module_init;


 void *module_core;


 unsigned int init_size, core_size;


 unsigned int init_text_size, core_text_size;


 struct mod_arch_specific arch;

 unsigned int taints;


 int gpgsig_ok;



 unsigned num_bugs;
 struct list_head bug_list;
 struct bug_entry *bug_table;
# 362 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
 Elf64_Sym *symtab, *core_symtab;
 unsigned int num_symtab, core_num_syms;
 char *strtab, *core_strtab;


 struct module_sect_attrs *sect_attrs;


 struct module_notes_attrs *notes_attrs;



 void *percpu;



 char *args;

 struct tracepoint *tracepoints;
 unsigned int num_tracepoints;



 const char **trace_bprintk_fmt_start;
 unsigned int num_trace_bprintk_fmt;


 struct ftrace_event_call *trace_events;
 unsigned int num_trace_events;


 unsigned long *ftrace_callsites;
 unsigned int num_ftrace_callsites;




 struct list_head modules_which_use_me;


 struct task_struct *waiter;


 void (*exit)(void);


 char *refptr;







 ctor_fn_t *ctors;
 unsigned int num_ctors;

};




extern struct mutex module_mutex;




static inline int module_is_live(struct module *mod)
{
 return mod->state != MODULE_STATE_GOING;
}

struct module *__module_text_address(unsigned long addr);
struct module *__module_address(unsigned long addr);
bool is_module_address(unsigned long addr);
bool is_module_text_address(unsigned long addr);

static inline int within_module_core(unsigned long addr, struct module *mod)
{
 return (unsigned long)mod->module_core <= addr &&
        addr < (unsigned long)mod->module_core + mod->core_size;
}

static inline int within_module_init(unsigned long addr, struct module *mod)
{
 return (unsigned long)mod->module_init <= addr &&
        addr < (unsigned long)mod->module_init + mod->init_size;
}


struct module *find_module(const char *name);

struct symsearch {
 const struct kernel_symbol *start, *stop;
 const unsigned long *crcs;
 enum {
  NOT_GPL_ONLY,
  GPL_ONLY,
  WILL_BE_GPL_ONLY,
 } licence;
 bool unused;
};


const struct kernel_symbol *find_symbol(const char *name,
     struct module **owner,
     const unsigned long **crc,
     bool gplok,
     bool warn);


bool each_symbol(bool (*fn)(const struct symsearch *arr, struct module *owner,
       unsigned int symnum, void *data), void *data);



int module_get_kallsym(unsigned int symnum, unsigned long *value, char *type,
   char *name, char *module_name, int *exported);


unsigned long module_kallsyms_lookup_name(const char *name);

int module_kallsyms_on_each_symbol(int (*fn)(void *, const char *,
          struct module *, unsigned long),
       void *data);

extern void __module_put_and_exit(struct module *mod, long code)
 __attribute__((noreturn));



unsigned int module_refcount(struct module *mod);
void __symbol_put(const char *symbol);

void symbol_put_addr(void *addr);

static inline local_t *__module_ref_addr(struct module *mod, int cpu)
{

 return (local_t *) (mod->refptr + per_cpu_offset(cpu));



}



static inline void __module_get(struct module *module)
{
 if (module) {
  unsigned int cpu = ({ do { } while (0); 0; });
  local_inc(__module_ref_addr(module, cpu));
  trace_module_get(module, _THIS_IP_,
     atomic_long_read(&(__module_ref_addr(module, cpu))->a));
  do { } while (0);
 }
}

static inline int try_module_get(struct module *module)
{
 int ret = 1;

 if (module) {
  unsigned int cpu = ({ do { } while (0); 0; });
  if (likely(module_is_live(module))) {
   local_inc(__module_ref_addr(module, cpu));
   trace_module_get(module, _THIS_IP_,
    atomic_long_read(&(__module_ref_addr(module, cpu))->a));
  }
  else
   ret = 0;
  do { } while (0);
 }
 return ret;
}

extern void module_put(struct module *module);
# 555 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
int use_module(struct module *a, struct module *b);
# 567 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
const char *module_address_lookup(unsigned long addr,
       unsigned long *symbolsize,
       unsigned long *offset,
       char **modname,
       char *namebuf);
int lookup_module_symbol_name(unsigned long addr, char *symname);
int lookup_module_symbol_attrs(unsigned long addr, unsigned long *size, unsigned long *offset, char *modname, char *name);


const struct exception_table_entry *search_module_extables(unsigned long addr);

int register_module_notifier(struct notifier_block * nb);
int unregister_module_notifier(struct notifier_block * nb);

extern void print_modules(void);

extern void module_update_tracepoints(void);
extern int module_get_iter_tracepoints(struct tracepoint_iter *iter);
# 708 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
struct device_driver;

struct module;

extern struct kset *module_kset;
extern struct kobj_type module_ktype;
extern int module_sysfs_initialized;

int mod_sysfs_init(struct module *mod);
int mod_sysfs_setup(struct module *mod,
      struct kernel_param *kparam,
      unsigned int num_params);
int module_add_modinfo_attrs(struct module *mod);
void module_remove_modinfo_attrs(struct module *mod);
# 755 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/module.h" 3
int module_bug_finalize(const Elf64_Ehdr *, const Elf64_Shdr *,
    struct module *);
void module_bug_cleanup(struct module *);
# 4 "testlist.c" 2
# 1 "/usr/src/kernels/2.6.32-573.3.1.el6.x86_64/include/linux/kernel.h" 1 3
# 5 "testlist.c" 2


;
struct student
{
    int id;
    char* name;
    struct list_head list;
};

void print_student(struct student*);

static int testlist_init(void)
{
    struct student *stu1, *stu2, *stu3, *stu4;
    struct student *stu;


    struct list_head stu_head = { &(stu_head), &(stu_head) };


    stu1 = kmalloc(sizeof(*stu1), ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u)));
    stu1->id = 1;
    stu1->name = "wyb";
    INIT_LIST_HEAD(&stu1->list);

    stu2 = kmalloc(sizeof(*stu2), ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u)));
    stu2->id = 2;
    stu2->name = "wyb2";
    INIT_LIST_HEAD(&stu2->list);

    stu3 = kmalloc(sizeof(*stu3), ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u)));
    stu3->id = 3;
    stu3->name = "wyb3";
    INIT_LIST_HEAD(&stu3->list);

    stu4 = kmalloc(sizeof(*stu4), ((( gfp_t)0x10u) | (( gfp_t)0x40u) | (( gfp_t)0x80u)));
    stu4->id = 4;
    stu4->name = "wyb4";
    INIT_LIST_HEAD(&stu4->list);


    list_add (&stu1->list, &stu_head);
    list_add (&stu2->list, &stu_head);
    list_add (&stu3->list, &stu_head);
    list_add (&stu4->list, &stu_head);


    for (stu = container_of((&stu_head)->next, typeof(*stu), list); &stu->list != (&stu_head); stu = container_of(stu->list.next, typeof(*stu), list))
    {
        print_student(stu);
    }

    for (stu = container_of((&stu_head)->prev, typeof(*stu), list); &stu->list != (&stu_head); stu = container_of(stu->list.prev, typeof(*stu), list))
    {
        print_student(stu);
    }


    list_del(&stu2->list);
    for (stu = container_of((&stu_head)->next, typeof(*stu), list); &stu->list != (&stu_head); stu = container_of(stu->list.next, typeof(*stu), list))
    {
        print_student(stu);
    }


    list_replace(&stu3->list, &stu2->list);
    for (stu = container_of((&stu_head)->next, typeof(*stu), list); &stu->list != (&stu_head); stu = container_of(stu->list.next, typeof(*stu), list))
    {
        print_student(stu);
    }

    return 0;
}

static void testlist_exit(void)
{
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "testlist is exited!\n");
    printk(KERN_ALERT "*************************\n");
}

void print_student(struct student *stu)
{
    printk (KERN_ALERT "======================\n");
    printk (KERN_ALERT "id  =%d\n", stu->id);
    printk (KERN_ALERT "name=%s\n", stu->name);
    printk (KERN_ALERT "======================\n");
}

static initcall_t __initcall_testlist_init6 __used __attribute__((__section__(".initcall" "6" ".init"))) = testlist_init;;
static exitcall_t __exitcall_testlist_exit __used __attribute__ ((__section__(".exitcall.exit"))) = testlist_exit;;
