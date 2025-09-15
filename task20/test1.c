#include <linux/module.h>
#include <linux/kernel.h>
int init_test(void)
{
  pr_info("Test module loaded\n");
  return 0;
}
void cleanup_test(void)
{
  pr_info("Test module unloaded\n");
}
module_init(init_test);
module_exit(cleanup_test);
MODULE_LICENSE("GPL");
