#include <linux/module.h>
#include <linux/kernel.h>

int hello_init(void)
{
  printk("hello world!\n");
  return 0;
}

void hello_exit(void)
{
  printk("goodbye!\n");
}
MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_exit);