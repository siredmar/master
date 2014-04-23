#ifndef __LPC313x_PWM_H__
#define __LPC313x_PWM_H__


#define PWM_AUTHOR        "Michael Schwarz"
#define PWM_DESCRIPTION   "PWM Driver for GNUBLIN"

#define PWM_PHYS    (0x13009000)

#define PWM_TMR_REG     __REG (PWM_PHYS + 0x00)
#define PWM_CNTL_REG    __REG (PWM_PHYS + 0x04)


#define PWM_MR_MASK ((1<<12)-1)

#define PWM_CLKDIV_1     0
#define PWM_CLKDIV_2     1
#define PWM_CLKDIV_4     2
#define PWM_CLKDIV_8     3

#define PWM_FORCE_HIGH   (1 << 4)
#define PWM_LOOP         (1 << 6)
#define PWM_PDM          (1 << 7)


// defaults
#define PWM_TMR_DEFAULT   0
#define PWM_CNTL_DEFAULT  0


// device
#define DEVICE_NAME           "lpc313x_pwm"
#define DEVICE_LEN            4


static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);


#endif // __LPC313x_PWM_H__