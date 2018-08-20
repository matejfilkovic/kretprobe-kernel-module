#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/limits.h>

static int firstReading;
static int secondReading;

void initializePMU(void *d)
{
   asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(0x00000007));
   asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r"(0x00000000));
   asm volatile("mcr p15, 0, %0, c9, c13, 1" : : "r"(0x00000008));
   asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r"(0x00000001));
}

static int enterHandler(struct kretprobe_instance *i, struct pt_regs *regs) {
   asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r"(0x0));
   asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r"(firstReading));

   return 0;
}

static int returnHandler(struct kretprobe_instance *i, struct pt_regs *regs) {
   asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r"(0x0));
   asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r"(secondReading));

   printk("kretprobe_module: exiting from udp_v4_get_port, first reading: %d, second reading %d .\n",
          firstReading,
          secondReading);

   return 0;
}

static struct kretprobe kretProbe = {
   .handler = enterHandler,
   .entry_handler = returnHandler,
   .maxactive = 1,
};

int init_module(void)
{
   int ret;

   on_each_cpu(initializePMU, NULL, 1);

   kretProbe.kp.symbol_name = "udp_v4_get_port";
   ret = register_kretprobe(&kretProbe);
   if (ret < 0)
   {
      printk("kretprobe initialization failure!");
      return -1;
   }

   printk("kretprobe set to %s: %p\n", kretProbe.kp.symbol_name, kretProbe.kp.addr);

   return 0;
}

void cleanup_module(void)
{
   unregister_kretprobe(&kretProbe);
   printk("kretprobe attached to %p removed!\n", kretProbe.kp.addr);
}

MODULE_LICENSE("GPL");
