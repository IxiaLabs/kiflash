/*
 * Copyright Keysight Technologies, All Rights Reserved
 * Keysight Confidential
 */

#include "FlashProviderNodeAdapter.h"
#include "NanCheckinAsyncWorker.h"
#include "NanCheckoutAsyncWorker.h"
#include "NanGetAvailableFeatureListAsyncWorker.h"
#include "NanSetLicenseSearchPathAsyncWorker.h"
#include "NanGetLicenseServerStatusAsyncWorker.h"

#ifdef WIN32
#include "Win64/FlashProviderWrapperWindows.h"
#else
#include "Linux/FlashProviderWrapperLinux.h"
#endif

#include "ValidateArguments.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>

Nan::Persistent<v8::Function> FlashProviderNodeAdapter::s_constructor;
static constexpr auto ObjectName = "FlashProviderWrapper";
const int LM_DUP_NONE = 0x4000;
const int LM_DUP_HOST = 0x0002;

// Initialize native extension
void FlashProviderNodeAdapter::Init(v8::Local<v8::Object> exports)
{
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New(ObjectName).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "checkOut", CheckOut);
    Nan::SetPrototypeMethod(tpl, "checkIn", CheckIn);
    Nan::SetPrototypeMethod(tpl, "getAvailableFeatureList", GetAvailableFeatureList);
    Nan::SetPrototypeMethod(tpl, "getLicenseServerStatus", GetLicenseServerStatus);
    Nan::SetPrototypeMethod(tpl, "setLicenseSearchPath", SetLicenseSearchPath);

    s_constructor.Reset(tpl->GetFunction());

    exports->Set(Nan::New(ObjectName).ToLocalChecked(), tpl->GetFunction());
}

// Request to create native object to
void FlashProviderNodeAdapter::New(const InfoT& info)
{
    ValidateArguments { info, 2 };

    if (!info[0]->IsObject())
    {
        Nan::ThrowTypeError("Argument must be an object");
        return;
    }

    if (!info[1]->IsString())
    {
         Nan::ThrowTypeError("Argument must be a string");
         return;
    }

    v8::Local<v8::Object> obj = info[0]->ToObject();
    auto productIdV8 = Nan::Get(obj, Nan::New<v8::String>("productId").ToLocalChecked());
    auto pathV8 = Nan::Get(obj, Nan::New<v8::String>("path").ToLocalChecked());

    using V8StringT = v8::String::Utf8Value;

    std::string workingDir(*V8StringT(info[1]));
    std::string productId(*V8StringT(productIdV8.ToLocalChecked()));
    std::string path(*V8StringT(pathV8.ToLocalChecked()));

    try
    {
        auto provider = new FlashProviderNodeAdapter(productId, path, workingDir);

        provider->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    }
    catch (std::exception& e)
    {
        auto err = std::string("Failed to create the FlashProviderNodeAdapter: ") + std::string(e.what());
        Nan::ThrowTypeError(err.c_str());
        return;
    }
}

FlashProviderNodeAdapter::FlashProviderNodeAdapter(std::string productId, std::string path, std::string workingDir) :
    m_isValid(false)
{
    try
    {
        m_pWrapper = new FlashProviderWrapperLinux(workingDir);

        if (productId != "undefined")
        {
            m_pWrapper->SetLicenseSearchPathByProduct(productId);
        }
        else if (path != "undefined")
        {
            m_pWrapper->SetLicenseSearchPath(path);
        }
        else
        {
            throw std::runtime_error("No license path options were specified");
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        throw(e);
    }
}

FlashProviderNodeAdapter::~FlashProviderNodeAdapter()
{
    delete m_pWrapper;
}

void FlashProviderNodeAdapter::CheckOut(const InfoT& info)
{
    ValidateArguments{info, 4};

    auto adapter = ObjectWrap::Unwrap<FlashProviderNodeAdapter>(info.Holder());

    if (!info[0]->IsString())
    {
        // feature
        Nan::ThrowTypeError("Feature must be a string");
        return;
    }

    if (!info[1]->IsString())
    {
        // version
        Nan::ThrowTypeError("Version must be a string");
        return;
    }

    if (!info[2]->IsNumber())
    {
        // totalNumberOfLicenses
        Nan::ThrowTypeError("Total Number of Licenses must be a number");
        return;
    }

    std::string feature = *v8::String::Utf8Value(info[0]);
    std::string version = *v8::String::Utf8Value(info[1]);
    int totalNumberOfLicenses = info[2]->Int32Value();

    // callback
    if (info[3]->IsFunction())
    {
         Nan::Callback *callBack = new Nan::Callback(info[3].As<v8::Function>());
         Nan::AsyncQueueWorker(new NanCheckoutAsyncWorker(callBack, info.GetIsolate(), adapter, feature, version, totalNumberOfLicenses));
    }
}

void FlashProviderNodeAdapter::CheckIn(const InfoT& info)
{
    ValidateArguments{info, 2};

    auto adapter = ObjectWrap::Unwrap<FlashProviderNodeAdapter>(info.Holder());

    if (!info[0]->IsString())
    {
        // feature
        Nan::ThrowTypeError("Feature must be a string");
        return;
    }

    if (!info[1]->IsFunction())
    {
        Nan::ThrowTypeError("Callback must be a string");
        return;
    }

    std::string feature = *v8::String::Utf8Value(info[0]);

    Nan::Callback *callBack = new Nan::Callback(info[1].As<v8::Function>());
    Nan::AsyncQueueWorker(new NanCheckinAsyncWorker(callBack, info.GetIsolate(), adapter, feature));

}

void FlashProviderNodeAdapter::GetAvailableFeatureList(const InfoT& info)
{
    ValidateArguments{info, 1};

    auto adapter = ObjectWrap::Unwrap<FlashProviderNodeAdapter>(info.Holder());

    // callback
    if (info[0]->IsFunction())
    {
        Nan::Callback *callBack = new Nan::Callback(info[0].As<v8::Function>());
        Nan::AsyncQueueWorker(new NanGetAvailableFeatureListAsyncWorker(callBack, info.GetIsolate(), adapter));
    }
}

void FlashProviderNodeAdapter::GetLicenseServerStatus(const InfoT& info)
{
    ValidateArguments{ info, 1 };

    auto adapter = ObjectWrap::Unwrap<FlashProviderNodeAdapter>(info.Holder());

    // callback
    if (info[0]->IsFunction())
    {
        Nan::Callback *callBack = new Nan::Callback(info[0].As<v8::Function>());
        Nan::AsyncQueueWorker(new NanGetLicenseServerStatusAsyncWorker(callBack, info.GetIsolate(), adapter));
    }

}

void FlashProviderNodeAdapter::SetLicenseSearchPath(const InfoT& info)
{
    ValidateArguments{ info, 2 };

    auto adapter = ObjectWrap::Unwrap<FlashProviderNodeAdapter>(info.Holder());

        if (!info[0]->IsString())
        {
            Nan::ThrowTypeError("License path must be a string");
            return;
        }

        if (!info[1]->IsFunction())
        {
            Nan::ThrowTypeError("Callback must be a string");
            return;
        }

    std::string serverPath = *v8::String::Utf8Value(info[0]);

    Nan::Callback *callBack = new Nan::Callback(info[1].As<v8::Function>());
    Nan::AsyncQueueWorker(new NanSetLicenseSearchPathAsyncWorker(callBack, info.GetIsolate(), adapter, serverPath));
}

bool FlashProviderNodeAdapter::IsCheckedOut(std::string feature)
{
    int result = m_pWrapper->FloCheckIsCheckedOut((char*) feature.c_str());
    return result == 0;
}

int FlashProviderNodeAdapter::GetLastErrorNumber()
{
    return m_pWrapper->FloGetLastErrorNumber();
}

std::string FlashProviderNodeAdapter::GetLastErrorMessage()
{
    std::string result;

    int bufferSize;
    FLO_ERROR errorNumber = m_pWrapper->FloGetLastErrorMessage(nullptr, &bufferSize);
    if (errorNumber == 0)
    {
        char* buffer = new char[bufferSize];
        errorNumber = m_pWrapper->FloGetLastErrorMessage(buffer, &bufferSize);
        if (errorNumber == 0)
        {
            result.assign(buffer);
        }

        delete [] buffer;
    }

    return result;
}

void FlashProviderNodeAdapter::CheckIn(FeatureT feature)
{
    m_pWrapper->FloCheckIn((char*) feature.c_str());

    SetFeatureCheckedIn(feature);
}

int FlashProviderNodeAdapter::GetFeatureCount()
{
    int count;
    const FLO_ERROR errorNumber = m_pWrapper->FloGetFeatureCount(&count);
    if (errorNumber == 0)
    {
        return count;
    }

    return errorNumber < 0 ? errorNumber : -1;
}

void FlashProviderNodeAdapter::CheckOut(FeatureT feature, std::string version, int totalNumberOfLicenses)
{
    m_pWrapper->FloCheckOut((char*) feature.c_str(), (char*) version.c_str(), totalNumberOfLicenses, LM_DUP_NONE);
}

bool FlashProviderNodeAdapter::CheckOutConfig(FeatureT feature)
{
    return m_pWrapper->FloLoadConfigBufferWithCheckedOutFeature((char*) feature.c_str()) == 0;
}

std::string FlashProviderNodeAdapter::ConfigInfo(std::string fieldName)
{
    std::string returnVal;
    int size;
    FLO_ERROR err = m_pWrapper->FloGetFieldFromConfigBuffer(fieldName.c_str(), nullptr, &size);
    if (err == 0)
    {
        char* buffer = new char[size];
        err = m_pWrapper->FloGetFieldFromConfigBuffer(fieldName.c_str(), buffer, &size);
        if (err == 0)
        {
            returnVal = buffer;
        }

        delete[] buffer;
    }

    return returnVal;
}

bool FlashProviderNodeAdapter::ConfigFeature(FeatureT featureName, bool first)
{
    return (m_pWrapper->FloLoadConfigBufferWithFeature(featureName.c_str(), first) == 0);
}

void FlashProviderNodeAdapter::SetLicenseSearchPath(std::string path)
{
    auto licenses = this->GetLicenses();
    for (auto license : licenses)
    {
        this->CheckIn(license.first);
    }

    m_pWrapper->FloClose();

    return m_pWrapper->SetLicenseSearchPath(path);
}

std::string FlashProviderNodeAdapter::GetFeatureName(int idx)
{
    std::string result;

    int bufferSize;
    FLO_ERROR errorNumber = m_pWrapper->FloGetFeatureName(idx, nullptr, &bufferSize);
    if (errorNumber == 0)
    {
        char* buffer = new char[bufferSize];
        errorNumber = m_pWrapper->FloGetFeatureName(idx, buffer, &bufferSize);
        if (errorNumber == 0)
        {
            result.assign(buffer);
        }

        delete [] buffer;
    }

    return result;
}

LicenseConfig FlashProviderNodeAdapter::CreateLicenseConfig()
{
    LicenseConfig config;

    config.SetVersion(this->ConfigInfo("version"));

    try
    {
        auto version = std::stod(config.GetVersion());
        config.SetVersionAsDouble(version);
    }
    catch (std::exception&)
    {
        std::cout << "Error parsing version string to double: " << std::endl;
        config.SetVersionAsDouble(0.0);
    }

    auto users = this->ConfigInfo("users");

    try
    {
        int count = std::stoi(users);
        config.SetIsCounted(true);
        config.SetTotalLicenseCount(count);
    }
    catch (std::exception&)
    {
        config.SetIsCounted(false);
        config.SetTotalLicenseCount(0);
    }

    config.SetServerName(this->ConfigInfo("server_name"));

    try
    {
        auto expirationInDays = std::stoi(this->ConfigInfo("expire_days"));
        auto date = this->ConfigInfo("date");

        std::transform(date.begin(), date.end(), date.begin(), ::tolower);

        if (date == "1-jan-0" || expirationInDays > 10000)
        {
            struct tm expiration = { 0 };
            expiration.tm_mday = 1;
            expiration.tm_mon = 0;
            expiration.tm_year = 2100 - 1900;
            expiration.tm_isdst = -1;

            config.SetExpirationDate(mktime(&expiration));
        }
        else
        {
            size_t startPos = 0;
            auto pos = date.find('-', startPos);
            auto day = std::stoi(date.substr(startPos, pos - startPos));

            startPos = pos + 1;
            pos = date.find('-', startPos );
            auto monthStr = date.substr(startPos, pos - startPos);

            auto month = 1;
            if (monthStr == "jan")
            {
                month = 1;
            }
            else if (monthStr == "feb")
            {
                month = 2;
            }
            else if (monthStr == "mar")
            {
                month = 3;
            }
            else if (monthStr == "apr")
            {
                month = 4;
            }
            else if (monthStr == "may")
            {
                month = 5;
            }
            else if (monthStr == "jun")
            {
                month = 6;
            }
            else if (monthStr == "jul")
            {
                month = 7;
            }
            else if (monthStr == "aug")
            {
                month = 8;
            }
            else if (monthStr == "sep")
            {
                month = 9;
            }
            else if (monthStr == "oct")
            {
                month = 10;
            }
            else if (monthStr == "nov")
            {
                month = 11;
            }
            else if (monthStr == "dec")
            {
                month = 12;
            }

            startPos = pos + 1;
            pos = date.find('-', startPos);
            auto year = std::stoi(date.substr(startPos, pos - startPos));

            struct tm expiration = { 0 };
            expiration.tm_mday = day;
            expiration.tm_mon = month - 1;
            expiration.tm_year = year - 1900;
            expiration.tm_isdst = -1;

            config.SetExpirationDate(mktime(&expiration));
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Failed to parse the expire_days in the license structure: " << e.what() << std::endl;
        config.SetExpirationDate(0);
    }

    config.SetAnnotation(this->ConfigInfo("vendor_def"));

    return config;
}

void FlashProviderNodeAdapter::SetFeatureCheckedOut(LicenseConfig config)
{
    m_licenses[config.GetFeature()] = config.GetServerName();
}

void FlashProviderNodeAdapter::SetFeatureCheckedIn(FeatureT feature)
{
    if (m_licenses.find(feature) != m_licenses.end())
    {
        m_licenses.erase(feature);
    }
}

std::tuple<int, int, int> FlashProviderNodeAdapter::GetConnectionStatus(FeatureT feature)
{
    CONNECT_STATE connectState = CONNECT_STATE::CONN_OK;
    int pass = 0;
    int totalAttempts = 0;
    int interval = 0;

    const int err = m_pWrapper->FloGetConnectStatusForFeature(feature.c_str(), &connectState, &pass, &totalAttempts, &interval);
    if (err != 0)
    {
        connectState = CONNECT_STATE::CONN_RECONNECT_TRY;
    }

    return std::tuple<int, int, int>(int(connectState), pass, interval);
}
