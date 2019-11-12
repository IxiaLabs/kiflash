/*
 * Copyright Keysight Technologies, All Rights Reserved
 * Keysight Confidential
 */

#pragma once
#include <nan.h>
#include <memory>
#include <string>
#include <map>
#include <mutex>
// #include "LicenseConfig.h"
#include "FlashProviderWrapper.h"

struct NanCheckThreadAsyncWorker;

struct FlashProviderNodeAdapter : public Nan::ObjectWrap
{
	using InfoT = Nan::FunctionCallbackInfo<v8::Value>;
	using StringT = std::string;
	using FeatureT = std::string;
	using ServerT = std::string;

	static void Init(v8::Local<v8::Object> exports);

// JS methods
private:
	static void New(const InfoT& info);

	static void CheckOut(const InfoT& info);
	static void CheckIn(const InfoT& info);
	static void GetAvailableFeatureList(const InfoT& info);
	static void GetLicenseServerStatus(const InfoT& info);
	static void SetLicenseSearchPath(const InfoT& info);

	static Nan::Persistent<v8::Function> s_constructor;

// C++
private:
	explicit FlashProviderNodeAdapter(std::string productId, std::string networkAddress, std::string workingDir);
	~FlashProviderNodeAdapter();

	bool m_isValid;

	void CheckActiveLicenses();

	std::map<FeatureT, ServerT> m_licenses;

	FlashProviderWrapper* m_pWrapper;

public:
	int GetLastErrorNumber();
    std::string GetLastErrorMessage();    
	bool IsCheckedOut(FeatureT feature);
	void CheckOut(FeatureT feature, std::string version, int totalNumberOfLicenses);
	bool CheckOutConfig(FeatureT feature);
	void CheckIn(FeatureT feature);
	int GetFeatureCount();
	FeatureT GetFeatureName(int idx);
	bool ConfigFeature(FeatureT featureName, bool first);
	std::string ConfigInfo(std::string fieldName);
	LicenseConfig CreateLicenseConfig();
	void SetFeatureCheckedOut(LicenseConfig config);
	void SetFeatureCheckedIn(FeatureT feature);
	std::tuple<int, int, int> GetConnectionStatus(FeatureT feature);
	void SetLicenseSearchPath(std::string path);

	std::map<FeatureT, ServerT> GetLicenses() { return m_licenses; }
	std::mutex& GetJobMutex() { return m_jobMutex; }

private:
	std::mutex m_jobMutex;
};
