#ifndef FDS_EXAMPLE_H__
#define FDS_EXAMPLE_H__

#include <stdint.h>

/* File ID and Key used for the configuration record. */

#define CONFIG_FILE     (0x8010)
#define CONFIG_REC_KEY  (0x7010)

/* Colors used to print on the console. */

#define COLOR_GREEN     "\033[1;32m"
#define COLOR_YELLOW    "\033[1;33m"
#define COLOR_CYAN      "\033[1;36m"

/* Macros to print on the console using colors. */

#define NRF_LOG_CYAN(...)   NRF_LOG_INFO(COLOR_CYAN   __VA_ARGS__)
#define NRF_LOG_YELLOW(...) NRF_LOG_INFO(COLOR_YELLOW __VA_ARGS__)
#define NRF_LOG_GREEN(...)  NRF_LOG_INFO(COLOR_GREEN  __VA_ARGS__)


/* A dummy structure to save in flash. */
typedef struct
{
    uint32_t boot_count;
    char     device_name[16];
    bool     config1_on;
    bool     config2_on;
} configuration_t;


/* Defined in main.c */

void delete_all_begin(void);

/* Defined in cli.c */

void cli_init(void);
void cli_start(void);
void cli_process(void);
bool record_delete_next(void);


#endif
