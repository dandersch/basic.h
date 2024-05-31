#pragma once

#undef ERROR // defined in wingdi.h as 0

#if !defined(LOG_ENTRY_FILE)

  #define LOG_ENTRIES_SEVERITY \
    LOG_ENTRY_SEVERITY(TRACE,       (1<<0), "[TRACE]", LOG_COLOR_GRAY)   \
    LOG_ENTRY_SEVERITY(INFO,        (1<<1), "[INFO ]", LOG_COLOR_GREEN)  \
    LOG_ENTRY_SEVERITY(WARN,        (1<<2), "[WARN ]", LOG_COLOR_YELLOW) \
    LOG_ENTRY_SEVERITY(ERROR,       (1<<3), "[ERROR]", LOG_COLOR_RED)    \
    LOG_ENTRY_SEVERITY(FATAL,       (1<<4), "[FATAL]", LOG_COLOR_PURPLE) \
    LOG_ENTRY_SEVERITY(EMPTY0,           0, "[     ]", LOG_COLOR_GRAY)

  #define LOG_ENTRIES_SUBSYSTEM \
    LOG_ENTRY_SUBSYSTEM(PLATFORM,   (1<<12), "[PLATF]", LOG_COLOR_YELLOW) \
    LOG_ENTRY_SUBSYSTEM(AUDIO,      (1<<13), "[AUDIO]", LOG_COLOR_YELLOW) \
    LOG_ENTRY_SUBSYSTEM(VIDEO,      (1<<14), "[VIDEO]", LOG_COLOR_YELLOW) \
    LOG_ENTRY_SUBSYSTEM(EMPTY1,           0, "[     ]", LOG_COLOR_GRAY)

  #define LOG_ENTRIES_CATEGORY \
    LOG_ENTRY_CATEGORY(INIT,       (1<<20), "[INIT ]", LOG_COLOR_PURPLE) \
    LOG_ENTRY_CATEGORY(SHUTDOWN,   (1<<21), "[SHUTD]", LOG_COLOR_OFF)    \
    LOG_ENTRY_CATEGORY(ACTIVATE,   (1<<22), "[ACTIV]", LOG_COLOR_PURPLE) \
    LOG_ENTRY_CATEGORY(DEACTIVATE, (1<<23), "[DEACT]", LOG_COLOR_PURPLE) \
    LOG_ENTRY_CATEGORY(EMPTY2,           0, "[     ]", LOG_COLOR_GRAY)

#endif

#include <stdio.h>
#include <time.h>

#if !defined(LOG_VARIABLE_NAME)
  #define LOG_VARIABLE_NAME log_verbosity_level
#endif

extern int LOG_VARIABLE_NAME;

#ifdef LOG_USE_NO_COLOR
  #define LOG_COLOR_OFF    ""
  #define LOG_COLOR_GRAY   ""
  #define LOG_COLOR_BLUE   ""
  #define LOG_COLOR_CYAN   ""
  #define LOG_COLOR_GREEN  ""
  #define LOG_COLOR_YELLOW ""
  #define LOG_COLOR_RED    ""
  #define LOG_COLOR_PURPLE ""
#else
  #define LOG_COLOR_OFF    "\x1b[0m"
  #define LOG_COLOR_GRAY   "\x1b[38;5;245m"
  #define LOG_COLOR_BLUE   "\x1b[94m"
  #define LOG_COLOR_CYAN   "\x1b[36m"
  #define LOG_COLOR_GREEN  "\x1b[32m"
  #define LOG_COLOR_YELLOW "\x1b[33m"
  #define LOG_COLOR_RED    "\x1b[31m"
  #define LOG_COLOR_PURPLE "\x1b[35m"
#endif

enum
{
  /* severity */
  #define LOG_ENTRY_SEVERITY(name, value, string, color) LOG_SEVERITY_##name = value,
  #define LOG_ENTRY_SUBSYSTEM(name, value, string, color)
  #define LOG_ENTRY_CATEGORY(name, value, string, color)
  #ifdef LOG_ENTRY_FILE
    #include LOG_ENTRY_FILE
  #else
    LOG_ENTRIES_SEVERITY
  #endif
  #undef LOG_ENTRY_SEVERITY

  #define LOG_ENTRY_SEVERITY(name, value, string, color) value |
  #define LOG_SEVERITY_MASK LOG_ENTRIES_SEVERITY LOG_ENTRIES_CATEGORY 0
  #ifdef LOG_ENTRY_FILE
    LOG_SEVERITY =
      #include LOG_ENTRY_FILE
    0,
  #else
    LOG_SEVERITY = LOG_SEVERITY_MASK,
  #endif
  #undef LOG_ENTRY_SEVERITY
  #undef LOG_ENTRY_SUBSYSTEM
  #undef LOG_ENTRY_CATEGORY

  /* subsystems */
  #define LOG_ENTRY_SEVERITY(name, value, string, color)
  #define LOG_ENTRY_SUBSYSTEM(name, value, string, color) LOG_SUBSYSTEM_##name = value,
  #define LOG_ENTRY_CATEGORY(name, value, string, color)
  #ifdef LOG_ENTRY_FILE
    #include LOG_ENTRY_FILE
  #else
    LOG_ENTRIES_SUBSYSTEM
  #endif
  #undef LOG_ENTRY_SUBSYSTEM

  #define LOG_ENTRY_SUBSYSTEM(name, value, string, color) value |
  #define LOG_SUBSYSTEM_MASK LOG_ENTRIES_SUBSYSTEM 0
  #ifdef LOG_ENTRY_FILE
    LOG_SUBSYSTEMS =
      #include LOG_ENTRY_FILE
    0,
  #else
    LOG_SUBSYSTEMS = LOG_SUBSYSTEM_MASK,
  #endif
  #undef LOG_ENTRY_SEVERITY
  #undef LOG_ENTRY_SUBSYSTEM
  #undef LOG_ENTRY_CATEGORY

  /* categories */
  #define LOG_ENTRY_SEVERITY(name, value, string, color)
  #define LOG_ENTRY_SUBSYSTEM(name, value, string, color)
  #define LOG_ENTRY_CATEGORY(name, value, string, color) LOG_CATEGORY_##name = value,
  #ifdef LOG_ENTRY_FILE
    #include LOG_ENTRY_FILE
  #else
    LOG_ENTRIES_CATEGORY
  #endif
  #undef LOG_ENTRY_CATEGORY

  #define LOG_ENTRY_CATEGORY(name, value, string, color) value |
  #define LOG_CATEGORY_MASK LOG_ENTRIES_CATEGORY 0
  #ifdef LOG_ENTRY_FILE
    LOG_CATEGORIES =
      #include LOG_ENTRY_FILE
    0,
  #else
    LOG_CATEGORIES = LOG_CATEGORY_MASK,
  #endif
  #undef LOG_ENTRY_SEVERITY
  #undef LOG_ENTRY_SUBSYSTEM
  #undef LOG_ENTRY_CATEGORY

  /* useful masks */
  LOG_EVERYTHING       = 0x7fffffff,
};

#if !defined(LOG_TIME_FORMAT)
  #define LOG_TIME_FORMAT "%H:%M:%S "
#endif

/* the core of the log macro */
#define _LOG(flags, format, ...)                                                                  \
  if ((flags) & LOG_VARIABLE_NAME)                                                                \
  {                                                                                               \
    /* get a timestamp string */                                                                  \
    time_t t = time(NULL); struct tm* time = localtime(&t);                                       \
    char buf[32]; buf[strftime(buf, sizeof(buf), LOG_TIME_FORMAT, time)] = '\0';                  \
    printf("%s" LOG_COLOR_OFF "%s%s%s %12.12s:%4i " format LOG_COLOR_OFF"\n", buf,                \
           _log_severity_label(flags), _log_subsystem_label(flags), _log_category_label(flags), \
           __FILE__, __LINE__, ##__VA_ARGS__);                                                    \
  }

#if defined(LOG_USE_SHORT_NAMES_GLOBALLY) && defined(LOG_ENTRY_FILE)
  /* NOTE fill the global namespace with unprefixed names of log entries (e.g. TRACE instead of LOG_SEVERITY_TRACE) */
  #define LOG_ENTRY_SEVERITY(name, value, string, color)  name = LOG_SEVERITY_##name ,
  #define LOG_ENTRY_SUBSYSTEM(name, value, string, color) name = LOG_SUBSYSTEM_##name ,
  #define LOG_ENTRY_CATEGORY(name, value, string, color)  name = LOG_CATEGORY_##name ,
  enum {
      #include LOG_ENTRY_FILE
  };
  #undef LOG_ENTRY_SEVERITY
  #undef LOG_ENTRY_SUBSYSTEM
  #undef LOG_ENTRY_CATEGORY
#endif

#define LOG_ENTRY_SEVERITY(name, value, string, color) name = LOG_SEVERITY_##name ,
#define LOG_ENTRY_SUBSYSTEM(name, value, string, color) name = LOG_SUBSYSTEM_##name ,
#define LOG_ENTRY_CATEGORY(name, value, string, color) name = LOG_CATEGORY_##name ,
#if defined(LOG_ENTRY_FILE)
  #define LOG(flags, format, ...)          \
    {                                      \
        _LOG(flags, format, ##__VA_ARGS__) \
    }
#else
  #define LOG(flags, format, ...)           \
    {                                       \
        enum {                              \
        /* remove LOG_ prefix */            \
        LOG_ENTRIES_SEVERITY                \
        LOG_ENTRIES_SUBSYSTEM               \
        LOG_ENTRIES_CATEGORY                \
        };                                  \
        _LOG(flags, format, ##__VA_ARGS__)  \
    }
#endif
/* NOTE redefined to above #define at bottom of file */
#undef LOG_ENTRY_SEVERITY
#undef LOG_ENTRY_SUBSYSTEM
#undef LOG_ENTRY_CATEGORY

#define LOG_ENTRY_SEVERITY(name, value, string, color) case LOG_SEVERITY_##name: return color string LOG_COLOR_OFF;
#define LOG_ENTRY_SUBSYSTEM(name, value, string, color)
#define LOG_ENTRY_CATEGORY(name, value, string, color)
static inline const char* _log_severity_label(int flags)
{
  switch (flags & LOG_SEVERITY)
  {
    #ifdef LOG_ENTRY_FILE
      #include LOG_ENTRY_FILE
    #else
      LOG_ENTRIES_SEVERITY
    #endif
    default: return "ERROR: COMBINING LOG SEVERITIES NOT SUPPORTED";
  }
}
#undef LOG_ENTRY_SEVERITY
#undef LOG_ENTRY_SUBSYSTEM
#undef LOG_ENTRY_CATEGORY

#define LOG_ENTRY_SEVERITY(name, value, string, color)
#define LOG_ENTRY_SUBSYSTEM(name, value, string, color) case LOG_SUBSYSTEM_##name: return color string LOG_COLOR_OFF;
#define LOG_ENTRY_CATEGORY(name, value, string, color)
static inline const char* _log_subsystem_label(int flags)
{
  switch (flags & LOG_SUBSYSTEMS)
  {
    #ifdef LOG_ENTRY_FILE
      #include LOG_ENTRY_FILE
    #else
      LOG_ENTRIES_SUBSYSTEM
    #endif
    default: return "ERROR: COMBINING LOG SUBSYSTEMS NOT SUPPORTED";
  }
}
#undef LOG_ENTRY_SEVERITY
#undef LOG_ENTRY_SUBSYSTEM
#undef LOG_ENTRY_CATEGORY

#define LOG_ENTRY_SEVERITY(name, value, string, color)
#define LOG_ENTRY_SUBSYSTEM(name, value, string, color)
#define LOG_ENTRY_CATEGORY(name, value, string, color) case LOG_CATEGORY_##name: return color string LOG_COLOR_OFF;
static inline const char* _log_category_label(int flags)
{
  switch (flags & LOG_CATEGORIES)
  {
    #ifdef LOG_ENTRY_FILE
      #include LOG_ENTRY_FILE
    #else
      LOG_ENTRIES_CATEGORY
    #endif
    default: return "ERROR: COMBINING LOG CATEGORIES NOT SUPPORTED";
  }
}
#undef LOG_ENTRY_SEVERITY
#undef LOG_ENTRY_SUBSYSTEM
#undef LOG_ENTRY_CATEGORY

/* NOTE these #define's are used in the LOG macro, so we cannot keep them #undef'ed */
#define LOG_ENTRY_SEVERITY(name, value, string, color)  name = LOG_SEVERITY_##name,
#define LOG_ENTRY_SUBSYSTEM(name, value, string, color) name = LOG_SUBSYSTEM_##name,
#define LOG_ENTRY_CATEGORY(name, value, string, color)  name = LOG_CATEGORY_##name,
