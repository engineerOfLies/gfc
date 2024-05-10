#include "gfc_callbacks.h"
#include "simple_logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

GFC_Callback *gfc_callback_new(void (*callback)(void *data),void *data)
{
    GFC_Callback *c;
    c = (GFC_Callback *)malloc(sizeof(GFC_Callback));
    if (!c)
    {
        slog("failed to allocate callback");
        return NULL;
    }
    memset(c,0,sizeof(GFC_Callback));
    c->callback = callback;
    c->data = data;
    return c;
}

void gfc_callback_free(GFC_Callback *callback)
{
    if (!callback)return;
    free(callback);
}

void gfc_callback_call(GFC_Callback *callback)
{
    if (!callback)return;
    if (callback->callback)
    {
        callback->callback(callback->data);
    }
}

/*eol@eof*/
