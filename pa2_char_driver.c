
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

MODULE_AUTHOR("Jonathan Wick");
MODULE_LICENSE("GPL");

#define BUFFER_SIZE 1024

// Define device_buffer and global data
#define MAJOR_NUMBER 240
int open_count = 0;
int close_count = 0;
char *device_buffer;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer:  Userspace buffer where you are writing the data you want to be written in the device file
	 		length:  Length of the userspace buffer
	 		current: Position of the opened file
	  	copy_from_user: Destination is device_buffer and source is *buffer */
if( length + *offset > BUFFER_SIZE){
	printk(KERN_ALERT "Error! Reading too much!!!\n");
	return -1;
}
else{
	printk("Reading from device.\n");
	copy_to_user(buffer, device_buffer + *offset, length);
	*offset += length;
	printk("%d bytes read.\n", length);
	return length;
}
return 0;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
/* *buffer:  Userspace buffer where you are writing the data you want to be written in the device file
 		length:  Length of the userspace buffer
 		current: Position of the opened file
  	copy_from_user: Destination is device_buffer and source is *buffer */
if(length + *offset > BUFFER_SIZE){
	printk(KERN_ALERT "Error! Not enough buffer space!!!\n");
	return -1;
}
else{
	printk("Writing to device.\n");
	copy_from_user(device_buffer + *offset, buffer, length);
	*offset += length;
	printk("%d bytes written.\n", length);
}
return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
/* print to the log file that the device is opened and also print the number of times this device has been closed until now*/
open_count++;
printk("The device has been opened.\n");
printk("The device has been opened %d times.\n", open_count);
return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
close_count++;
printk("The device has been closed.\n");
printk("The device has been closed %d times.\n", close_count);
return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
/* Update open file position according to the values of offset and whence */
loff_t position = 0;
switch(whence){
	case 0:  // SEEK_SET
		position = offset;
		break;
	case 1:  // SEEK_CUR
		position = pfile->f_pos + offset;
		break;
	case 2:  // SEEK_END
		position = BUFFER_SIZE - offset;
		break;
}

if (position < 0){
	printk(KERN_ALERT "Error! Seeking before the beginning of the file!!!\n");
	return -1;
}
if(position > BUFFER_SIZE){
	printk(KERN_ALERT "Error! Seeking beyond the file!!!\n");
	return -2;
}
pfile->f_pos = position;
return 0;
}

struct file_operations pa2_char_driver_file_operations = {
.owner   = THIS_MODULE,
.open = pa2_char_driver_open,
.release = pa2_char_driver_close,
.read = pa2_char_driver_read,
.write = pa2_char_driver_write,
.llseek = pa2_char_driver_seek
// add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code
};

static int pa2_char_driver_init(void)
{
/* print to the log file that the init function is called.*/
/* register the device */
register_chrdev(MAJOR_NUMBER, "simple_character_device", &pa2_char_driver_file_operations);
device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
printk("pa2_char_driver has initialized.\n");
return 0;
}

static void pa2_char_driver_exit(void)
{
/* print to the log file that the exit function is called.*/
/* unregister  the device using the register_chrdev() function. */
unregister_chrdev(MAJOR_NUMBER, "simple_character_device");
kfree(device_buffer);
printk("pa2_char_driver has exited.\n");
}

/* add module_init and module_exit to point to the corresponding init and exit function*/

module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
