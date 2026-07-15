#ifndef FILEOPS_H
#define FILEOPS_H

#include <windows.h>

/**
    Save a log entry to a file with the filename given
    Returns TRUE if successful or FALSE if any errors arise
*/
BOOL saveLogToFile(const SYSTEMTIME *time, const char *text, char *errorReport, size_t errorReportSize);

#endif
