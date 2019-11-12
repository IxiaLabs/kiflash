
#include "FlashProviderWrapper.h"
// #include "FloCalculateKey.h"
#include <iostream>
#include <exception>
#include <stdexcept>

FlashProviderWrapperLinux::FlashProviderWrapperLinux(std::string libraryPath)
{
    try
    {
        // if not passed in from JS-land
        if (libraryPath.empty())
        {
            LoadLibrary("build/Release");
        }
        else
        {
            LoadLibrary(libraryPath);
        }
    }
    catch (std::exception& e)
    {
        throw(e);
    }
}

FlashProviderWrapperLinux::~FlashProviderWrapperLinux()
{
    if (m_hLibrary)
    {
        dlclose(m_hLibrary);
    }
}

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

void FlashProviderWrapper::LoadLibraryFunctions(SharedLibraryHandleT hLibrary)
{
    // m_fnFloUnlock = (FloUnlockT *)dlsym(hLibrary, STR(floUnlock));
	FlashOpenT *m_fnFlashOpen;
	FlashCloseT *m_fnFlashClose;    
    m_fnFlashOpen = (FlashOpenT *)dlsym(hLibrary, STR(flashOpen));
    m_fnFlashClose = (FlashCloseT *)dlsym(hLibrary, STR(flashClose));
/*    m_fnFloGetLastErrorNumber = (FloGetLastErrorNumberT *)dlsym(hLibrary, STR(floGetLastErrorNumber));
    m_fnFloGetLastErrorMessage = (FloGetLastErrorMessageT *)dlsym(hLibrary, STR(floGetLastErrorMessage));
    m_fnFloSetLicenseSearchPath = (FloSetLicenseSearchPathT *)dlsym(hLibrary, STR(floSetLicenseSearchPath));
    m_fnFloCheckOut= (FloCheckOutT *)dlsym(hLibrary, STR(floCheckOut));
    m_fnFloCheckIn = (FloCheckInT *)dlsym(hLibrary, STR(floCheckIn));
    m_fnFloGetFeatureCount = (FloGetFeatureCountT *)dlsym(hLibrary, STR(floGetFeatureCount));
    m_fnFloGetFeatureName = (FloGetFeatureNameT *)dlsym(hLibrary, STR(floGetFeatureName));
    m_fnFloCheckIsCheckedOut = (FloCheckIsCheckedOutT *)dlsym(hLibrary, STR(floCheckIsCheckedOut));
    m_fnFloGetFieldFromConfigBuffer = (FloGetFieldFromConfigBufferT *)dlsym(hLibrary, STR(floGetFieldFromConfigBuffer));
    m_fnFloLoadConfigBufferWithFeature = (FloLoadConfigBufferWithFeatureT *)dlsym(hLibrary, STR(floLoadConfigBufferWithFeature));
    m_fnFloLoadConfigBufferWithCheckedOutFeature = (FloLoadConfigBufferWithCheckedOutFeatureT *)dlsym(hLibrary, STR(floLoadConfigBufferWithCheckedOutFeature));
    m_fnFloGetConnectStatusForFeature = (FloGetConnectStatusForFeatureT *)dlsym(hLibrary, STR(floGetConnectStatusForFeature));
    m_fnFloGetUserListForFeature = (FloGetUserListForFeatureT *)dlsym(hLibrary, STR(floGetUserListForFeature));
*/    

    if (//!m_fnFloUnlock ||
        !m_fnFlashOpen ||
        !m_fnFlashClose )
        //!m_fnFloGetLastErrorNumber ||
        //!m_fnFloGetLastErrorMessage ||
        //!m_fnFloSetLicenseSearchPath ||
        //!m_fnFloCheckOut||
        //!m_fnFloCheckIn ||
        //!m_fnFloGetFeatureCount ||
        //!m_fnFloGetFeatureName ||
        //!m_fnFloCheckIsCheckedOut ||
        //!m_fnFloGetFieldFromConfigBuffer ||
        //!m_fnFloLoadConfigBufferWithFeature ||
        //!m_fnFloLoadConfigBufferWithCheckedOutFeature ||
        //!m_fnFloGetConnectStatusForFeature ||
        //!m_fnFloGetUserListForFeature)
    {
        auto err = "The functions failed to load! ";
        std::cout << err << std::endl;
        throw std::runtime_error(err);
    }
}

void FlashProviderWrapper::LoadLibrary(std::string path)
{
    path += "/libFlashProvider.so";
    m_hLibrary = dlopen(path.c_str(), RTLD_LAZY);

    if (!m_hLibrary)
    {
        char cwd[1024];
        getcwd(cwd, 1024);
        std::stringstream err;
        err << "Failed to load the library " << path << " my current path is: " << cwd << std::endl;
        throw std::runtime_error(err.str());
    };

    LoadLibraryFunctions(m_hLibrary);
    FlashProviderWrapper::LoadLibrary();
}