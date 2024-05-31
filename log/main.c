#define LOG_ENTRY_FILE "log_entry_table.h"
#define LOG_USE_SHORT_NAMES_GLOBALLY
#define LOG_TIME_FORMAT LOG_COLOR_GRAY "%H:%M:%S "
#include "log.h"

int log_verbosity_level = LOG_EVERYTHING; // define globally once

int main()
{
    LOG(TRACE|PLATFORM|INIT,"Started engine");
    LOG(INFO|VIDEO|INIT, "Successfully initialized video subsystem");
    LOG(AUDIO|WARN|ACTIVATE, "Insufficient memory for normal-sized buffer");
    LOG(ERROR|NETWORK|INIT, "Couldn't init network");
    LOG(FATAL|PLATFORM|INIT, "Fatal error occured. Aborting...");
}
