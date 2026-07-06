#pragma once
#include "libSerial/serial.h"

#if defined __cplusplus
extern "C"
{
#endif

/// @brief storage for a string reader
typedef struct
{
    char *buffer;   // pointer to workspace
    int limit;      // string limit
    int counter;    // byte counter
    char complete;  // flag complete
    char readLines; // true if reading lines with CR at end
} editBuffer;

/// @brief Use this for the readlines parameter  if reading up to buffer length or using CR
static const char READLINES_CR = 1;
/// @brief Use this for the readlines parameter if reading up to buffer length
static const char READLINES_BIN = 0; 

///
typedef enum
{
    EDIT_NULL = 0, ///< No result
    EDIT_DONE,     ///< All characters read
    EDIT_CR        ///< Characters up to CR read
} editBufferReturn;

/// @brief Send a string to the user
/// @param ctl Handle of usart 
/// @param string String to send 
/// @param length Length of string 
extern void sendString(struct usart_ctl *ctl, char const *string, int length);

/// @brief Prepare the line buffer reader
/// @param context editBuffer object to initialise
/// @param buffer  Pointer to data: externally allocated memory buffer.
/// @param limit   Maximum number of bytes in externally allocated memory buffer.
/// @param readLines if 1 then we are reading text with <CR> as line terminator, control codes ignored.
extern void initReadLine(editBuffer *context, char *buffer, int limit, char readLines);

/// @brief Poll the line buffer reader
/// @param ctl   pointer to a USART control structure
/// @param context pointer to the editBuffer context
/// @return what kind of state the buffer is now in
extern editBufferReturn readLine(struct usart_ctl *ctl, editBuffer *context);

/// @brief Reset the state of the line buffer reader for the next command  
/// @param context editBuffer object to reset 
extern void resetInput(editBuffer * context);

/// @brief Return number of characters in context
/// @param context 
/// @return number of characters in the context  
extern int charCount(editBuffer * context);

#if defined __cplusplus
}
#endif 
