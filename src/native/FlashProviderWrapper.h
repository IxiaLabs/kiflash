#pragma once
#include "Include/FlashProvider.h"
#include <nan.h>

class FlashProviderWrapper
{
public:
    FlashProviderWrapper();
    virtual ~FlashProviderWrapper();
    
    virtual void SetFlashSearchPath(const std::string& path);
    virtual void ReloadFlashSearchPath();
	
	virtual void FlashOpen(void);
    virtual void FlashClose(void);
/*    
    virtual FLO_ERROR FloGetLastErrorNumber();
    virtual FLO_ERROR FloGetLastErrorMessage(char *buffer, int *bufferSizePtr);
    virtual FLO_ERROR FloSetLicenseSearchPath(const char *path);
    virtual FLO_ERROR FloCheckOut(const char *feature, const char *version, int count, int duplicatesMask);
    virtual void FloCheckIn(const char *feature);
    virtual FLO_ERROR FloGetFeatureCount(int *countPtr);
    virtual FLO_ERROR FloGetFeatureName(int featureIndex, char *buffer, int *bufferSize);
    virtual FLO_ERROR FloCheckIsCheckedOut(const char *feature);
    virtual FLO_ERROR FloGetFieldFromConfigBuffer(const char *fieldName, char *buffer, int *bufferSizePtr);
    virtual FLO_ERROR FloLoadConfigBufferWithFeature(const char *feature, int firstInstance);
    virtual FLO_ERROR FloLoadConfigBufferWithCheckedOutFeature(const char *feature);
    virtual FLO_ERROR FloGetConnectStatusForFeature(const char *feature, CONNECT_STATE *connectState, int *pass, int *totalAttempts, int *interval);
    virtual FLO_ERROR FloGetUserListForFeature(const char *feature, LIC_USERS_PTR userArray, int *userArraySize);
*/    

protected:
    void LoadLibrary();

	// FloUnlockT *m_fnFloUnlock;
	FlashOpenT *m_fnFlashOpen;
	FlashCloseT *m_fnFlashClose;
/*    
	FloGetLastErrorNumberT *m_fnFloGetLastErrorNumber;
	FloGetLastErrorMessageT *m_fnFloGetLastErrorMessage;
	FloSetLicenseSearchPathT *m_fnFloSetLicenseSearchPath;
	FloCheckOutT *m_fnFloCheckOut;
	FloCheckInT *m_fnFloCheckIn;
	FloGetFeatureCountT *m_fnFloGetFeatureCount;
	FloGetFeatureNameT *m_fnFloGetFeatureName;
	FloCheckIsCheckedOutT *m_fnFloCheckIsCheckedOut;
	FloGetFieldFromConfigBufferT *m_fnFloGetFieldFromConfigBuffer;
	FloLoadConfigBufferWithFeatureT *m_fnFloLoadConfigBufferWithFeature;
	FloLoadConfigBufferWithCheckedOutFeatureT *m_fnFloLoadConfigBufferWithCheckedOutFeature;
	FloGetConnectStatusForFeatureT *m_fnFloGetConnectStatusForFeature;
    FloGetUserListForFeatureT *m_fnFloGetUserListForFeature;
*/    

    std::string m_path;
};
