/// Functions provided that use serial code, but are more independent 

#include "libSerial/serialUtils.h"

#if defined __cplusplus
extern "C"
{
#endif 


void sendString(struct usart_ctl *ctl, char const *string, int length)
{
  for (int i = 0; i < length; i++)
    PutCharSerial(ctl, string[i]);
}

void initReadLine(editBuffer *context, char *buffer, int limit, char readLines)
{
  context->buffer = buffer;
  context->limit = limit;
  context->counter = 0;
  context->complete = 0;
  context->readLines = readLines; // if readlines , then we look for \r at the end of a line and skip control chars 
  context->buffer[0] = 0;
}

editBufferReturn  readLine(struct usart_ctl *ctl, editBuffer *context)
{
  int cnt = PollSerial(ctl);
  if (!cnt)
    return EDIT_NULL;

  while (cnt && context->counter != context->limit)
  {
    char c = GetCharSerial(ctl); --cnt;
    if (!context->readLines || c >= ' ')
    {
      context->buffer[context->counter++] =c;
      if (context->counter != context->limit)
        context->buffer[context->counter] = 0;
    }
    if (context->readLines)
    {
      if (c == '\r')
      {
        context->complete = 1;
        return EDIT_CR;
      }
      else if (c < ' ')
      {
        context->counter = 0;
        context->buffer[0] = 0;
      }
    }
  }

  return (context->counter == context->limit) ? EDIT_DONE : EDIT_NULL;
}


void  resetInput(editBuffer * context)
{
  context->counter = 0; 
  context->complete = 0; 
  context->buffer[0] = 0;
}

int charCount(editBuffer * context)
{
  return context->counter; 
}


#if defined __cplusplus
}
#endif 
