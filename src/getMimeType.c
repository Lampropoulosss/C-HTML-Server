#include "getMimeType.h"

#include <string.h>

const char *get_mime_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
    {
        return "application/octet-stream"; // Default binary type
    }

    if (strcasecmp(ext, ".html") == 0)
    {
        return "text/html";
    }
    else if (strcasecmp(ext, ".css") == 0)
    {
        return "text/css";
    }
    else if (strcasecmp(ext, ".js") == 0)
    {
        return "application/javascript";
    }
    else if (strcasecmp(ext, ".png") == 0)
    {
        return "image/png";
    }
    else if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcasecmp(ext, ".gif") == 0)
    {
        return "image/gif";
    }
    else if (strcasecmp(ext, ".ico") == 0)
    {
        return "image/x-icon";
    }
    else if (strcasecmp(ext, ".json") == 0)
    {
        return "application/json";
    }
    else if (strcasecmp(ext, ".pdf") == 0)
    {
        return "application/pdf";
    }
    else if (strcasecmp(ext, ".svg") == 0)
    {
        return "image/svg+xml";
    }
    else if (strcasecmp(ext, ".xml") == 0)
    {
        return "application/xml";
    }
    else if (strcasecmp(ext, ".mp4") == 0)
    {
        return "video/mp4";
    }
    else if (strcasecmp(ext, ".webm") == 0)
    {
        return "video/webm";
    }
    else if (strcasecmp(ext, ".wav") == 0)
    {
        return "audio/wav";
    }
    else if (strcasecmp(ext, ".mp3") == 0)
    {
        return "audio/mpeg";
    }
    else if (strcasecmp(ext, ".txt") == 0)
    {
        return "text/plain";
    }

    return "application/octet-stream"; // Default binary type
}