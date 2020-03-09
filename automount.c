#include <linux/string.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

#define PROCFS_MAX_SIZE 1024
#define PROCFS_NAME "mount_info"
//#define USERSPACE_MONITORING "/home/wh75er/projects/b-device-automount/user-space/monitoring/monitoring.out"
//#define USERSPACE_MOUNT "/home/wh75er/projects/b-device-automount/user-space/mount/mount.out"

MODULE_AUTHOR("wh75er");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

static char *str = NULL;
static char dev_path[100];
static char dev_name[100];
static char dev_fs[100];

static char* envp[] = {
    "HOME=/",
    "TERM=linux",
    "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

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

  sscanf(str, "%s %s %s\n", dev_path, dev_name, dev_fs);
  printk(KERN_INFO "Detected Path: %s Filename: %s with FS: %s\n", dev_path, dev_name, dev_fs);

  char mnt_path[200];
  strcpy(mnt_path, "");
  strcat(mnt_path, "/mnt/");
  strcat(mnt_path, dev_name);

  char* argv[] = {USERSPACE_MOUNT, mnt_path, dev_path, dev_fs, NULL};
  call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);

  return count;
}

static struct file_operations fileops =
{
  .owner = THIS_MODULE,
  .write = procfile_write,
  .open = procfile_open,
  .read = seq_read,
};

static int __init mod_init(void) {
  struct proc_dir_entry *Mount_Info_File;
  Mount_Info_File = proc_create(PROCFS_NAME, 0666, NULL, &fileops);
  if(!Mount_Info_File)
    return -1;
  printk(KERN_INFO "+Module was loaded\n");

  char* argv[] = {USERSPACE_MONITORING, NULL};
  call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

  return 0;
}

static void __exit mod_exit(void) {
  remove_proc_entry(PROCFS_NAME, NULL);
  printk(KERN_INFO "+Module was unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);
