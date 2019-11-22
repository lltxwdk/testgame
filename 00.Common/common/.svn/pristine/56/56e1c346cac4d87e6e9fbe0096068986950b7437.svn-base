#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSProcessInfo.h>
#include <string.h>

int GetIOSDataDirectory(char* data, const int data_len) {
    if(data == NULL )
    {
        return -1;
    }
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    if([paths count] == 1)
    {
        return -1;
    }
    
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    
    const char *temp = [basePath UTF8String];
    if(temp == NULL || data_len <= strlen(temp))
    {
        return -1;
    }
    
    memcpy(data, temp, strlen(temp));
    return (int)strlen(temp);
}

int GetIOSTempDirectory(char* data, const int data_len) {
    if(data == NULL )
    {
        return -1;
    }
    
    NSString *basePath = NSTemporaryDirectory();
    const char *temp = [basePath UTF8String];
    if(temp == NULL || data_len <= strlen(temp))
    {
        return -1;
    }
    
    memcpy(data, temp, strlen(temp));
    return (int)strlen(temp);
}

int GetIOSDocDirectory(char* data, const int data_len) {
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    if([paths count] == 0)
    {
        return -1;
    }
    
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    if(data == NULL )
    {
        return -1;
    }
    
    const char *temp = [basePath UTF8String];
    if(temp == NULL || data_len <= strlen(temp))
    {
        return -1;
    }
    memcpy(data, temp, strlen(temp));
    return (int)strlen(temp);
}


