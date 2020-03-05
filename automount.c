#include <linux/string.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "mount_info"

MODULE_AUTHOR("wh75er");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

static char *str = NULL;

static int fileproc_show(struct seq_file *m, void *v)
{
  seq_printf(m, "%s\n", str);
  return 0;
}

static int procfile_open(struct inode *inode, struct file *file)
{
    return single_open(file, fileproc_show, NULL);
}

static ssize_t procfile_write(struct file *file,
                  const char __user *buffer,
                  size_t count,
                  loff_t *ppos)
{

  char *tmp = kzalloc((count+1), GFP_KERNEL);
  if(!tmp)
    return -ENOMEM;
  if(copy_from_user(tmp, buffer, count)){
    kfree(tmp);
    return EFAULT;
  }
  kfree(str);
  str=tmp;
  return count;
}

static struct file_operations fileops =
{
  .owner = THIS_MODULE,
  .write = procfile_write,
  .open = procfile_open,
  .read = seq_read,
};

static int i = 0;

static int __init mod_init(void) {
  struct proc_dir_entry *Mount_Info_File;
  Mount_Info_File = proc_create(PROCFS_NAME, 0666, NULL, &fileops);
  if(!Mount_Info_File)
    return -1;
  printk(KERN_INFO "+Module was started with i equals %d!\n", i);
  return 0;
}

static void __exit mod_exit(void) {
  remove_proc_entry(PROCFS_NAME, NULL);
  printk(KERN_INFO "+Module was unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);