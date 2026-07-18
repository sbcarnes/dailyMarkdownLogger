#include "fileops.h"
#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

BOOL saveLogToFile(const SYSTEMTIME *time, const LogSection *sections, size_t sectionCount, char *errorReport, size_t errorReportSize)
{
    
    char fileName[14];
    char filePath[128];
    
    int formatFileName = snprintf(fileName, sizeof(fileName), "%04d-%02d-%02d.md", time->wYear, time->wMonth, time->wDay);
    int formatFilePath = snprintf(filePath, sizeof(filePath), "%s/%s", LOG_DIRECTORY, fileName);
    
    if((formatFileName < 0 || formatFileName >= sizeof(fileName)) || (formatFilePath < 0 || formatFilePath >= sizeof(filePath))) {
        snprintf(errorReport, errorReportSize, "Error formatting file name \n\n errNo: %d\n errMsg: %s", errno, strerror(errno));
        return FALSE;
    }
    
    FILE *file = fopen(filePath, "wb");
    
    if (!file) {
        snprintf(errorReport, errorReportSize, "Could not open file: '%s' \n\n errNo: %d\n errMsg: %s",
                                    fileName, errno, strerror(errno));
        return FALSE;
    }
    
    if (fprintf(file, "# Daily Log - %04d-%02d-%02d\n\n", time->wYear, time->wMonth, time->wDay) < 0) {
        snprintf(errorReport, errorReportSize, "The log could not be written \n\n errNo: %d\n errMsg: %s",
                 errno, strerror(errno));
        fclose(file);
        return FALSE;
    }
    
    for (size_t i = 0; i < sectionCount; ++i)
    {
        if(fprintf(file, "## %s\n\n%s\n\n", sections[i].heading, sections[i].text) < 0) {
            snprintf(errorReport, errorReportSize, "The log could not be written \n\n errNo: %d\n errMsg: %s",
                 errno, strerror(errno));
        fclose(file);
        return FALSE;
        }
    }
    
    if (fclose(file) != 0) {
        snprintf(errorReport, errorReportSize, "Error closing log file \n\n errNo: %d\n errMsg: %s",
                 errno, strerror(errno));
        return FALSE;
    }
    
    return TRUE;
}
