#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <mach/adc.h>


#define MODULE_VERSION "1.0"
#define MODULE_NAME "fhsbattery"


static int fhsbattery_read(char *page, char **start, off_t offset,
                           int count, int *eof, void *data)
{
	int reads;

	off_t len;
	int battery_level_raw, battery_level = 400; // 4.00V
	int charging_status = 0;

	battery_level_raw = 0;	

	for (reads=0;reads<2;reads++)
	{
		battery_level_raw += lpc31xx_adc_read(0);
	}

	battery_level_raw = battery_level_raw/reads;

  	battery_level = (2*330*battery_level_raw)/1024;
	
	len = sprintf(page,"%3d\n%s\n",battery_level,charging_status?"charging":"discharging");
	
	*eof = 1;
	
	return len;

}

static int __init fhsbattery_init(void)
{
	static struct proc_dir_entry *entry = NULL;
	int dummy;
	entry = create_proc_entry("fhsbattery",S_IWUSR,NULL);
	if (entry) 
		{
			entry->read_proc = fhsbattery_read;
		}
		
	lpc31xx_adc_init();
	
	
		
	return 0;
}

static void __init fhsbattery_cleanup(void)
{
	int dummy;
	remove_proc_entry("fhsbattery",NULL);
	lpc31xx_adc_close();
}

module_init(fhsbattery_init);
module_exit(fhsbattery_cleanup);

MODULE_AUTHOR("Miguel Angel Ajo Pelayo <miguelangel@nbee.es>");
MODULE_DESCRIPTION("fhs battery monitor");


