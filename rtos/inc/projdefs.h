#ifndef PROJDEFS_H
#define PROJDEFS_H

typedef void (*TaskFunction_t)( void * );

#define pdFALSE                                  ( ( BaseType_t ) 0 )
#define pdTRUE                                   ( ( BaseType_t ) 1 )

#define pdPASS                                   ( pdTRUE )
#define pdFAIL                                   ( pdFALSE )
#define errQUEUE_EMPTY                           ( ( BaseType_t ) 0 )
#define errQUEUE_FULL                            ( ( BaseType_t ) 0 )

/* FreeRTOS error definitions. */
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY    ( -1 )
#define errQUEUE_BLOCKED                         ( -4 )
#define errQUEUE_YIELD                           ( -5 )


#endif /* PROJDEFS_H */

