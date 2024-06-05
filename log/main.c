#define LOG_TIME_FORMAT LOG_COLOR_GRAY "%H:%M:%S "

//#define LOG_ENTRY_FILE "log_entry_table.h"

#define LOG_USE_PLAIN_ENTRY_FILE
#define LOG_USE_SHORT_NAMES_GLOBALLY
#define LOG_ENTRY_FILE "log_entry_plain.h"

//#define LOG_ENTRIES_SEVERITY \
//LOG_ENTRY_SEVERITY(   INFO,         (1<< 0),    "[INFO ]",     LOG_COLOR_GREEN   )\
//LOG_ENTRY_SEVERITY(   WARN,         (1<< 1),    "[WARN ]",     LOG_COLOR_YELLOW  )\
//LOG_ENTRY_SEVERITY(   ERROR,        (1<< 2),    "[ERROR]",     LOG_COLOR_RED     )\
//LOG_ENTRY_SEVERITY(   FATAL,        (1<< 3),    "[FATAL]",     LOG_COLOR_PURPLE  )\
//LOG_ENTRY_SEVERITY(   EMPTY0,             0,    "[     ]",     LOG_COLOR_GRAY    )
//#define LOG_ENTRIES_SUBSYSTEM \
//LOG_ENTRY_SUBSYSTEM(  PLATFORM,     (1<< 8),    "[PLATF]",     LOG_COLOR_CYAN    )\
//LOG_ENTRY_SUBSYSTEM(  AUDIO,        (1<< 9),    "[AUDIO]",     LOG_COLOR_GREEN   )\
//LOG_ENTRY_SUBSYSTEM(  VIDEO,        (1<<10),    "[VIDEO]",     LOG_COLOR_RED     )\
//LOG_ENTRY_SUBSYSTEM(  EMPTY1,             0,    "[     ]",     LOG_COLOR_GRAY    )
//#define LOG_ENTRIES_CATEGORY \
//LOG_ENTRY_CATEGORY(   INIT,         (1<<18),    "[INIT ]",     LOG_COLOR_GRAY    )\
//LOG_ENTRY_CATEGORY(   SHUTDOWN,     (1<<19),    "[SHUTD]",     LOG_COLOR_GRAY    )\
//LOG_ENTRY_CATEGORY(   EMPTY2,             0,    "[     ]",     LOG_COLOR_GRAY    )

#include "log.h"

int log_verbosity_level = LOG_EVERYTHING; // define globally once

int main()
{
    /* usage */
    LOG(INFO|PLATFORM|INIT,      "Started engine");
    LOG(VIDEO|INIT,              "Successfully initialized video subsystem");

    unsigned int buffer_size = 1024;
    LOG(AUDIO|WARN|INIT,         "Insufficient memory for buffer of size %u", buffer_size);

    /* only log warnings or worse from here */
    LOG_SET_MASK(WARN | ERROR | FATAL); // short form of log_verbosity_level = LOG_SEVERITY_WARN | LOG_SEVERITY_ERROR | LOG_SEVERITY_FATAL;

    LOG(INFO|VIDEO,              "Won't be displayed");
    LOG(FATAL|PLATFORM|SHUTDOWN, "Fatal error occured. Aborting...");
}
