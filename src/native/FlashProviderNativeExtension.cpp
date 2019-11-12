/*
 * Copyright Keysight Technologies, All Rights Reserved
 * Keysight Confidential
 */

#include "FlashProviderNodeAdapter.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll)
{
	FlashProviderNodeAdapter::Init(target);
}

NODE_MODULE(FlashProviderNativeExtension, InitAll)
