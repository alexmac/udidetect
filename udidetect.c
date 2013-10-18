#include <string.h>
#include "MobileDevice.h"

void detect_screen(char *screen, const char *product_type, const char *ios_version_full);
void handle_device(am_device *device);
void device_callback(struct am_device_notification_callback_info *info, void *arg);

int screentype;

void detect_screen(char *screen, const char *product_type, const char *ios_version_full)
{
	char device_name[15];
	char device_gen[2];
	char ios_version[2];
	
	char *resolution;

	memcpy(device_name, product_type, strlen(product_type)-3);
	strncpy(device_gen, strtok((char *)product_type, device_name), 1);
	strncpy(ios_version, ios_version_full, 1);

	if(strcmp(device_name, "iPhone") == 0)
	{
		if(atoi(device_gen) > 2) resolution = "retina";
		else resolution = "low";
	}
	else if(strcmp(device_name, "iPod") == 0)
	{
		if(atoi(device_gen) > 3) resolution = "retina";
		else resolution = "low";
	}
	else if(strcmp(device_name, "iPad") == 0) {

		if(atoi(device_gen) >= 3) resolution = "ipad_retina";
		else resolution = "ipad";
	}
	
	strcat(screen, resolution);
	strcat(screen, "_ios");
	strcat(screen, ios_version);
}

void handle_device(am_device *device)
{
	CFStringEncoding encoding = CFStringGetSystemEncoding();
	const char *udid          = CFStringGetCStringPtr(AMDeviceCopyDeviceIdentifier(device), encoding);
	
	if(screentype)
	{
		AMDeviceConnect(device);
		assert(AMDeviceIsPaired(device));
		assert(AMDeviceValidatePairing(device) == 0);
		assert(AMDeviceStartSession(device) == 0);
	
		const char *device_name  = CFStringGetCStringPtr(AMDeviceCopyValue(device, 0, CFSTR("DeviceName")),     encoding);
		const char *product_type = CFStringGetCStringPtr(AMDeviceCopyValue(device, 0, CFSTR("ProductType")),    encoding);
		const char *ios_version  = CFStringGetCStringPtr(AMDeviceCopyValue(device, 0, CFSTR("ProductVersion")), encoding);
	   
		char screen[15] = "";
		detect_screen(screen, product_type, ios_version);
		
		printf("%s\n%s\n", udid, screen);
		fflush(stdout);

		assert(AMDeviceStopSession(device) == 0);
		AMDeviceDisconnect(device);

		exit(0);
	}
	else
	{
		printf("%s\n", udid);
		fflush(stdout);
		exit(0);
	}
}

void device_callback(struct am_device_notification_callback_info *info, void *arg)
{
	switch (info->msg) {
			case ADNCI_MSG_CONNECTED:
				handle_device(info->dev);
			default:
				break;
	}  
}	 

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if(strcmp(argv[1], "-h") == 0)
		{
			printf("usage: %s [-z]\n", argv[0]);
			exit(1);
		}
		else if(strcmp(argv[1], "-s") == 0)
		{
		  screentype = 1;
		}
	}

	struct am_device_notification *notify;
	AMDeviceNotificationSubscribe(&device_callback, 0, 0, NULL, &notify); 
	CFRunLoopRun();	 
}
