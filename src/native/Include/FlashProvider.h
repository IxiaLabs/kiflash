#pragma once


#include <time.h>




typedef int CONNECT_STATE_RETURN;

typedef struct {
	int fd;
    int  map_size;
	void * map_base;
} PCIMEM, *PCIMEM_PTR;



typedef enum class connect_state : int {
    CONN_OK,
    CONN_RECONNECT_TRY,
    CONN_RECONNECT,
    CONN_RECONNECT_DONE
} CONNECT_STATE;

typedef int FLO_BOOL;

// Function typedefs for clients that may want to store dynamically loaded functions in a variable
typedef void FlashOpenT(PCIMEM_PTR i_pcimem);
typedef void FlashCloseT(void);
typedef int FlashGetId(char *buffer, int *bufferSizePtr);
typedef int FloErase(int die);
typedef FLO_ERROR WINAPI FloSetLicenseSearchPathT(const char *path);
typedef FLO_ERROR WINAPI FloCheckOutT(const char *feature, const char *version, int count, int duplicatesMask);

typedef FLO_ERROR WINAPI FloGetFeatureCountT(int *countPtr);
typedef FLO_ERROR WINAPI FloGetFeatureNameT(int featureIndex, char *buffer, int *bufferSizePtr);
typedef FLO_ERROR WINAPI FloCheckIsCheckedOutT(const char *feature);
typedef FLO_ERROR WINAPI FloGetFieldFromConfigBufferT(const char *fieldName, char *buffer, int *bufferSizePtr);
typedef FLO_ERROR WINAPI FloLoadConfigBufferWithFeatureT(const char *feature, FLO_BOOL firstInstance);
typedef FLO_ERROR WINAPI FloLoadConfigBufferWithCheckedOutFeatureT(const char *feature);
typedef FLO_ERROR WINAPI FloGetConnectStatusForFeatureT(const char *feature, CONNECT_STATE *connectState, int* pass, int* totalAttempts, int *interval);
typedef FLO_ERROR WINAPI FloGetUserListForFeatureT(const char *feature, LIC_USERS_PTR userArray, int *userArrayCountPtr);
typedef FLO_ERROR WINAPI FloBorrowRequestT(const char *feature, const char *version, int requestedTime);
typedef FLO_ERROR WINAPI FloBorrowReturnT(const char *feature);
typedef FLO_ERROR WINAPI floGetHostIdT(char* buffer, int *bufferSizePtr);

#if defined(__cplusplus) || defined(c_plusplus)
    extern "C" {
#endif

    // long      WINAPI floUnlock(long protectKey); 
    void      WINAPI floOpen(void);
    void      WINAPI floClose(void);
    /*FLO_ERROR WINAPI floGetLastErrorNumber(void);
    FLO_ERROR WINAPI floGetLastErrorMessage(char *buffer, int *bufferSizePtr);
    FLO_ERROR WINAPI floSetLicenseSearchPath(const char *path);
    FLO_ERROR WINAPI floCheckOut(const char *feature, const char *version, int count, int duplicatesMask);
    void      WINAPI floCheckIn(const char *feature);
    FLO_ERROR WINAPI floGetFeatureCount(int *countPtr);
    FLO_ERROR WINAPI floGetFeatureName(int featureIndex, char *buffer, int *bufferSizePtr);
    FLO_ERROR WINAPI floCheckIsCheckedOut(const char *feature);
    FLO_ERROR WINAPI floGetFieldFromConfigBuffer(const char *fieldName, char *buffer, int *bufferSizePtr);
    FLO_ERROR WINAPI floLoadConfigBufferWithFeature(const char *feature, FLO_BOOL firstInstance);
    FLO_ERROR WINAPI floLoadConfigBufferWithCheckedOutFeature(const char *feature);
    FLO_ERROR WINAPI floGetConnectStatusForFeature(const char *feature, CONNECT_STATE *connectState, int *pass, int *totalAttempts, int *interval);
    FLO_ERROR WINAPI floGetUserListForFeature(const char *feature, LIC_USERS_PTR userArray, int *userArrayCountPtr);
    FLO_ERROR WINAPI floBorrowRequest(const char *feature, const char *version, int requestedTime);
    FLO_ERROR WINAPI floBorrowReturn(const char *feature);
    FLO_ERROR WINAPI floGetHostId(char* buffer, int *bufferSizePtr);
    */

#if defined(__cplusplus) || defined(c_plusplus)
    };
#endif
