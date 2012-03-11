#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
// Thanks wikipedia ;)
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__)
// And these are kind of public.
#define DEBUGPRINT(_fmt)  DEBUGPRINT2(WHERESTR _fmt "\n", WHEREARG)
#define DEBUGPRINTARGS(_fmt, ...)  DEBUGPRINT2(WHERESTR _fmt "\n", WHEREARG, __VA_ARGS__)
#else
// In normal mode, macro's are empty.
#define DEBUGPRINT(_fmt) 
#define DEBUGPRINTARGS(_fmt, ...) 
#endif

#endif
