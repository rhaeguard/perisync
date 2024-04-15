#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <linux/uinput.h>

#define TARGET_FILE "<TARGET_FILE>"

// sources: 
// https://unix.stackexchange.com/a/422705
// https://kernel.org/doc/html/v4.12/input/uinput.html

void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

int main(void)
{
   struct uinput_setup usetup;

   int device = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

   /* enable mouse button left and relative events */
   ioctl(device, UI_SET_EVBIT, EV_KEY);
   ioctl(device, UI_SET_KEYBIT, BTN_LEFT);

   ioctl(device, UI_SET_EVBIT, EV_REL);
   ioctl(device, UI_SET_RELBIT, REL_X);
   ioctl(device, UI_SET_RELBIT, REL_Y);

   memset(&usetup, 0, sizeof(usetup));
   usetup.id.bustype = BUS_USB;
   usetup.id.vendor = 0x1234;  /* sample vendor */
   usetup.id.product = 0x5678; /* sample product */
   strcpy(usetup.name, "Example device");

   ioctl(device, UI_DEV_SETUP, &usetup);
   ioctl(device, UI_DEV_CREATE);

   /*
    * On UI_DEV_CREATE the kernel will create the device node for this
    * device. We are inserting a pause here so that userspace has time
    * to detect, initialize the new device, and can start listening to
    * the event, otherwise it will not notice the event we are about
    * to send. This pause is only needed in our example code!
    */
   sleep(1);
   FILE *target_file = fopen(TARGET_FILE, "r");
   char buf[128];

   if (target_file == NULL)
   {
      printf("error opening the target file\n");
      exit(-1);
   }

   int nbytes;
   int px = 0;
   int py = 0;
   while (1)
   {
      int x, y;
      fscanf(target_file, "%d %d\n", &x, &y);

      if (x == 0 && y == 0)
      {
         break;
      }

      int dx = x - px;
      int dy = y - py;

      px = x;
      py = y;

      emit(device, EV_REL, REL_X, dx);
      emit(device, EV_REL, REL_Y, dy);
      emit(device, EV_SYN, SYN_REPORT, 0);
      usleep(1000);

      // reset back to the beginning to read the new contents
      clearerr(target_file);
      fseek(target_file, 0, SEEK_SET);
   }

   /*
    * Give userspace some time to read the events before we destroy the
    * device with UI_DEV_DESTOY.
    */
   sleep(1);

   ioctl(device, UI_DEV_DESTROY);
   close(device);
   fclose(target_file);

   return 0;
}
