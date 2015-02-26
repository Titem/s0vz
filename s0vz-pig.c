#define DAEMON_NAME "s0vz-pig"
#define DAEMON_VERSION "1.0"
#define DAEMON_BUILD "1"

#include <stdio.h>
#include <stdlib.h>             /* standard library for the C programming language, */
#include <string.h>             /* functions implementing operations on strings  */
#include <unistd.h>             /* provides access to the POSIX operating system API */
#include <sys/stat.h>           /* declares the stat() functions; umask */
#include <fcntl.h>              /* file descriptors */
#include <syslog.h>             /* send messages to the system logger */
#include <errno.h>              /* macros to report error conditions through error codes */
#include <signal.h>             /* signal processing */
#include <stddef.h>             /* defines the macros NULL and offsetof as well as the types ptrdiff_t, wchar_t, and size_t */

#include <curl/curl.h>          /* multiprotocol file transfer library */
#include <pigpio.h>
#include <libconfig.h>          /* reading, manipulating, and writing structured configuration files */

#define BUF_LEN 64

void alert(int pin, int level, uint32_t tick);
void cfile();

int pidFilehandle, vzport, i, len, running_handles, rc;

const char *vzserver, *vzpath, *vzuuid[64];
char url[128];

char gpio_pin_id[] = {5, 6};

int inputs = sizeof(gpio_pin_id)/sizeof(gpio_pin_id[0]);

struct timeval tv;

CURL *easyhandle[sizeof(gpio_pin_id)/sizeof(gpio_pin_id[0])];
CURLM *multihandle;
CURLMcode multihandle_res;

static char errorBuffer[CURL_ERROR_SIZE+1];

void cfile() {

	config_t cfg;

	//config_setting_t *setting;

	config_init(&cfg);

	int chdir(const char *path);

	chdir ("/etc");

	if(!config_read_file(&cfg, DAEMON_NAME".cfg"))
	{
		syslog(LOG_INFO, "Config error > /etc/%s - %s\n", config_error_file(&cfg),config_error_text(&cfg));
		config_destroy(&cfg);
		/* daemonShutdown(); */
		exit(EXIT_FAILURE);
	}

	if (!config_lookup_string(&cfg, "vzserver", &vzserver))
	{
		syslog(LOG_INFO, "Missing 'VzServer' setting in configuration file.");
		config_destroy(&cfg);
		/* daemonShutdown(); */
		exit(EXIT_FAILURE);
	}
	else
	syslog(LOG_INFO, "VzServer:%s", vzserver);

	if (!config_lookup_int(&cfg, "vzport", &vzport))
	{
		syslog(LOG_INFO, "Missing 'VzPort' setting in configuration file.");
		config_destroy(&cfg);
		/* daemonShutdown(); */
		exit(EXIT_FAILURE);
	}
	else
	syslog(LOG_INFO, "VzPort:%d", vzport);


	if (!config_lookup_string(&cfg, "vzpath", &vzpath))
	{
		syslog(LOG_INFO, "Missing 'VzPath' setting in configuration file.");
		config_destroy(&cfg);
		/* daemonShutdown(); */
		exit(EXIT_FAILURE);
	}
	else
	syslog(LOG_INFO, "VzPath:%s", vzpath);

	for (i=0; i<inputs; i++)
	{
		char gpio[6];
		sprintf ( gpio, "GPIO%01d", i );
		if ( config_lookup_string( &cfg, gpio, &vzuuid[i]) == CONFIG_TRUE )
		syslog ( LOG_INFO, "%s = %s", gpio, vzuuid[i] );
	}

}

void alert(int pin, int level, uint32_t tick)
{
	if(level == 0) 
	{
		printf("/**********************/\n");
		printf("Pin:\t%u\n", pin);
		printf("Level:\t%u\n", level);
		printf("Tick:\t%u\n", tick);
		printf("/**********************/\n");
	}
}

int setup() 
{
	freopen( "/dev/null", "r", stdin);
	freopen( "/dev/null", "w", stdout);
	freopen( "/dev/null", "w", stderr);

	FILE* devnull = NULL;		
	devnull = fopen("/dev/null", "w+");
		
	setlogmask(LOG_UPTO(LOG_INFO));
	openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);

	syslog ( LOG_INFO, "S0/Impulse to Volkszaehler RaspberryPI deamon %s.%s", DAEMON_VERSION, DAEMON_BUILD );
	/* GPIO Init */
	if (gpioInitialise()<0) return 1;

	/* Config lesen */
	/*cfile(); */

	/* Jeden definierten Eingang initalisieren */
	int i;
	for (i = 0; i < inputs; i++)
	{
		gpioSetAlertFunc(gpio_pin_id[i], alert);
	}
}

int main(int argc, char *argv[])
{
	setup();
	
	while(1)
	{
		gpioDelay(10000);
	}
	return 0;
}