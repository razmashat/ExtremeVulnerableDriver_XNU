//
//  ExtremeVulnerableDriverUserClient.cpp
//  ExtremeVulnerableDriver
//
//  Created by Nguyễn Hoàng Trung on 3/2/19.
//  Copyright © 2019 Trung Nguyen. All rights reserved.
//
#include <IOKit/IOLib.h>
#include "ExtremeVulnerableDriver.hpp"
#include "ExtremeVulnerableDriverUserClient.hpp"

// Define the superclass
#define super IOUserClient

OSDefineMetaClassAndStructors(ExtremeVulnerableDriverUserClient, IOUserClient)
bool    ExtremeVulnerableDriverUserClient::initWithTask (task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties)
{
    if (!owningTask)
        return false;
    this->owningTask = owningTask;
    IOLog("userClient::initWithTask\n");
    if (! super::initWithTask(owningTask, securityToken , type, properties))
        return false;
    
    return true;
}

bool    ExtremeVulnerableDriverUserClient::start (IOService* provider)
{
    if (! super::start(provider))
        return false;
    IOLog("userClient::start\n");
    return true;
}

void    ExtremeVulnerableDriverUserClient::stop (IOService* provider)
{
    IOLog("userClient::stop\n");
    super::stop(provider);
}

void    ExtremeVulnerableDriverUserClient::free (void)
{
    IOLog("userClient::free\n");
    super::free();
}

IOReturn    ExtremeVulnerableDriverUserClient::clientClose (void)
{
    terminate();
    return kIOReturnSuccess;
}

IOReturn    ExtremeVulnerableDriverUserClient::clientDied (void)
{
    IOLog("userClient::clientDied\n");
    return super::clientDied();
}
const IOExternalMethodDispatch ExtremeVulnerableDriverUserClient::sMethods[5] =
{
    { sCareless_read, 2, 0 , 0, 0 },
    
    { sCareless_write, 2, 0, 0, 0 },
    
    { sCreateShmem, 1, 0, 0, sizeof(struct ExtremeVulnerableShmemData)},
    
    { sDeleteShmem, 1, 0, 0, 0},
    
    { sProcessData, 0, sizeof(struct ExtremeVulnerableSubmitData), 0, 0},
};
IOReturn    ExtremeVulnerableDriverUserClient::externalMethod (uint32_t selector, IOExternalMethodArguments* arguments,
                                                                      IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)
{
    IOLog("ExtremeVulnerableDriverUserClient::externalMethod\n");
    // Ensure the requested control selector is within range
    if (selector >= 5)
        return kIOReturnUnsupported;
    dispatch = (IOExternalMethodDispatch*)&sMethods[selector];
    target = this;
    reference = NULL;
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

IOReturn    ExtremeVulnerableDriverUserClient::sCareless_read (OSObject* target, void* reference, IOExternalMethodArguments* arguments)
{
    ExtremeVulnerableDriverUserClient*    me = (ExtremeVulnerableDriverUserClient*)target;
    IOLog("ExtremeVulnerableDriverUserClient::sCareless_read\n");
    return me->careless_read((user_addr_t)arguments->scalarInput[0], (size_t)arguments->scalarInput[1]);
}
IOReturn    ExtremeVulnerableDriverUserClient::sCareless_write (OSObject* target, void* reference, IOExternalMethodArguments* arguments)
{
    ExtremeVulnerableDriverUserClient*    me = (ExtremeVulnerableDriverUserClient*)target;
    
    return me->careless_write((user_addr_t)arguments->scalarInput[0], (size_t)arguments->scalarInput[1]);
}

IOReturn ExtremeVulnerableDriverUserClient::sCreateShmem(OSObject *target, void *reference, IOExternalMethodArguments *arguments){
    ExtremeVulnerableDriverUserClient* me = (ExtremeVulnerableDriverUserClient*)target;
    IOLog("ExtremeVulnerableDriverUserClient::sCreateShmem\n");
    return me->createShmem(me->owningTask, arguments->scalarInput[0], (ExtremeVulnerableShmemData*)arguments->structureOutput);
}

IOReturn ExtremeVulnerableDriverUserClient::sDeleteShmem(OSObject *target, void *reference, IOExternalMethodArguments *arguments){
    ExtremeVulnerableDriverUserClient* me = (ExtremeVulnerableDriverUserClient*)target;
    IOLog("ExtremeVulnerableDriverUserClient::sDeleteShmem\n");
    return me->deleteShmem((uint32_t)arguments->scalarInput[0]);
}

IOReturn ExtremeVulnerableDriverUserClient::sProcessData(OSObject *target, void *reference, IOExternalMethodArguments *arguments){
    ExtremeVulnerableDriverUserClient* me = (ExtremeVulnerableDriverUserClient*)target;
    IOLog("ExtremeVulnerableDriverUserClient::sProcessData\n");
    return me->processData((struct ExtremeVulnerableSubmitData*)arguments->structureInput);
}

IOReturn ExtremeVulnerableDriverUserClient::careless_read(user_addr_t udaddr,size_t len){
    IOLog("ExtremeVulnerableDriverUserClient::careless_read\n");
    char buf[1];
    void * addr=buf;
    return copyout(addr, udaddr, len);
}
IOReturn ExtremeVulnerableDriverUserClient::careless_write(user_addr_t udaddr,size_t len)
{
    char buf[1];
    void * addr=buf;
    return copyin(udaddr, addr, len);
}

IOReturn ExtremeVulnerableDriverUserClient::createShmem(task_t inTask, vm_size_t size, ExtremeVulnerableShmemData* outShmemData){
    ExtremeVulnerableDriver* provider = (ExtremeVulnerableDriver*)this->getProvider();
    if(!provider){
        return kIOReturnError;
    }
    return provider->createShmem(inTask, size, outShmemData);
}

IOReturn ExtremeVulnerableDriverUserClient::deleteShmem(uint32_t shmemID){
    ExtremeVulnerableDriver* provider = (ExtremeVulnerableDriver*)this->getProvider();
    if(!provider){
        return kIOReturnError;
    }
    return provider->deleteShmem(shmemID);
}

IOReturn ExtremeVulnerableDriverUserClient::processData(struct ExtremeVulnerableSubmitData* data){
    ExtremeVulnerableDriver* provider = (ExtremeVulnerableDriver*)this->getProvider();
    if(!provider){
        return kIOReturnError;
    }
    return provider->processData(data);
}
