#ifndef FILEOPS_H
#define FILEOPS_H

#include <windows.h>
#include <stddef.h>


typedef struct
{
    const char *heading;
    const char *text;
} LogSection;
/**
    Save a log entry to a file with the filename given
    Returns TRUE if successful or FALSE if any errors arise
*/
BOOL saveLogToFile(
    const SYSTEMTIME *time,
    const LogSection *sections,
    size_t sectionCount,
    char *errorReport,
    size_t errorReportSize
);

#endif
