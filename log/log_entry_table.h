#define LOG_ENTRIES_SEVERITY \
LOG_ENTRY_SEVERITY(   TRACE,        (1<< 0),    "[TRACE]",     LOG_COLOR_GRAY    )\
LOG_ENTRY_SEVERITY(   INFO,         (1<< 1),    "[INFO ]",     LOG_COLOR_GREEN   )\
LOG_ENTRY_SEVERITY(   WARN,         (1<< 2),    "[WARN ]",     LOG_COLOR_YELLOW  )\
LOG_ENTRY_SEVERITY(   ERROR,        (1<< 3),    "[ERROR]",     LOG_COLOR_RED     )\
LOG_ENTRY_SEVERITY(   FATAL,        (1<< 4),    "[FATAL]",     LOG_COLOR_PURPLE  )\
LOG_ENTRY_SEVERITY(   EMPTY0,             0,    "[     ]",     LOG_COLOR_GRAY    )
#define LOG_ENTRIES_SUBSYSTEM \
LOG_ENTRY_SUBSYSTEM(  PLATFORM,     (1<< 8),    "[PLATF]",     LOG_COLOR_CYAN    )\
LOG_ENTRY_SUBSYSTEM(  AUDIO,        (1<< 9),    "[AUDIO]",     LOG_COLOR_GREEN   )\
LOG_ENTRY_SUBSYSTEM(  VIDEO,        (1<<10),    "[VIDEO]",     LOG_COLOR_RED     )\
LOG_ENTRY_SUBSYSTEM(  NETWORK,      (1<<11),    "[NETWK]",     LOG_COLOR_PURPLE  )\
LOG_ENTRY_SUBSYSTEM(  TEST,         (1<<12),    "[TEST ]",     LOG_COLOR_GREEN   )\
LOG_ENTRY_SUBSYSTEM(  EMPTY1,             0,    "[     ]",     LOG_COLOR_GRAY    )
#define LOG_ENTRIES_CATEGORY \
LOG_ENTRY_CATEGORY(   INIT,         (1<<18),    "[INIT ]",     LOG_COLOR_GRAY    )\
LOG_ENTRY_CATEGORY(   SHUTDOWN,     (1<<19),    "[SHUTD]",     LOG_COLOR_GRAY    )\
LOG_ENTRY_CATEGORY(   ACTIVATE,     (1<<20),    "[ACTIV]",     LOG_COLOR_GRAY    )\
LOG_ENTRY_CATEGORY(   DEACTIVATE,   (1<<21),    "[DEACT]",     LOG_COLOR_GRAY    )\
LOG_ENTRY_CATEGORY(   EMPTY2,             0,    "[     ]",     LOG_COLOR_GRAY    )
