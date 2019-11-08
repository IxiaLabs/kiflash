
#include "FlashProviderWrapper.h"
// #include "FloCalculateKey.h"
#include <iostream>
#include <exception>
#include <stdexcept>

/*void FlashProviderWrapper::LoadLibrary()
{
    bool m_isValid = (FloUnlock(floCalculateKey(1956)) == floCalculateKey(1013));

    if (!m_isValid)
    {
        throw std::runtime_error("Failed to unlock library");
    }

    FloGetLastErrorNumber();
}*/


void FlashProviderWrapper::SetFlashSearchPath(const std::string& path)
{
    if (m_path != path)
    {
        m_path = path;
    }

    FloSetLicenseSearchPath((char*) m_path.c_str());
}

void FlashProviderWrapper::ReloadFlashSearchPath()
{
    if (!m_path.empty())
    {
        SetFlashSearchPath(m_path);
    }
}

/*long FlashProviderWrapper::FloUnlock(long tamperKey)
{
    return m_fnFloUnlock(tamperKey);
}*/

void FlashProviderWrapper::FlashOpen(void)
{
    //m_fnFloOpen();
    m_fnFlashOpen();
}

void FlashProviderWrapper::FloClose(void)
{
    //m_fnFloClose();
    m_fnFlashClose();
}

/*FLO_ERROR FlashProviderWrapper::FloGetLastErrorNumber()
{
    return m_fnFloGetLastErrorNumber();
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetLastErrorMessage(char *buffer, int *bufferSizePtr)
{
    return m_fnFloGetLastErrorMessage(buffer, bufferSizePtr);
}*/

/*FLO_ERROR FlashProviderWrapper::FloSetLicenseSearchPath(const char *path)
{
    return m_fnFloSetLicenseSearchPath(path);
}*/

/*FLO_ERROR FlashProviderWrapper::FloCheckOut(const char *feature, const char *version, int count, int duplicatesMask)
{
    return m_fnFloCheckOut(feature, version, count, duplicatesMask);
}*/

/*void FlashProviderWrapper::FloCheckIn(const char *feature)
{
    return m_fnFloCheckIn(feature);
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetFeatureCount(int *countPtr)
{
    // This is a bit of a hack to get the connection refreshed if we keep asking for licenses count.
    // When the provider goes away, the client will remove the license count from display.
    //
    ReloadLicenseSearchPath();

    return m_fnFloGetFeatureCount(countPtr);
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetFeatureName(int featureIndex, char *buffer, int *bufferSize)
{
    return m_fnFloGetFeatureName(featureIndex, buffer, bufferSize);
}*/

/*FLO_ERROR FlashProviderWrapper::FloCheckIsCheckedOut(const char *feature)
{
    return m_fnFloCheckIsCheckedOut(feature);
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetFieldFromConfigBuffer(const char *fieldName, char *buffer, int *bufferSize)
{
    return m_fnFloGetFieldFromConfigBuffer(fieldName, buffer, bufferSize);
}*/

/*FLO_ERROR FlashProviderWrapper::FloLoadConfigBufferWithFeature(const char *feature, int firstInstance)
{
    return m_fnFloLoadConfigBufferWithFeature(feature, firstInstance);
}*/

/*FLO_ERROR FlashProviderWrapper::FloLoadConfigBufferWithCheckedOutFeature(const char *feature)
{
    return m_fnFloLoadConfigBufferWithCheckedOutFeature(feature);
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetConnectStatusForFeature(const char *feature, CONNECT_STATE *connectState, int *pass, int *totalAttempts, int *interval)
{
    return m_fnFloGetConnectStatusForFeature(feature, connectState, pass, totalAttempts, interval);
}*/

/*FLO_ERROR FlashProviderWrapper::FloGetUserListForFeature(const char *feature, LIC_USERS_PTR userArray, int *userArraySize)
{
    return m_fnFloGetUserListForFeature(feature, userArray, userArraySize);
}*/
