//
//  Resource.cpp
//  GResource
//
//  Created by Jacques Tronconi on 05/11/2015.
//  Copyright (c) 2015 Atlanti's Corporation. All rights reserved.
//

#include "Resource.h"

GRE_BEGIN_NAMESPACE

Resource::Resource (const std::string& name) :
_name (name)
{
    VariantDescriptor nulldescriptor;
    nulldescriptor.variant = Variant();
    _mVariantDatas[-1] = nulldescriptor;
    
#ifdef GreIsDebugMode
    GreDebugPretty() << name << " : Constructed." << std::endl;
#endif
}

Resource::~Resource ()
{
#ifdef GreIsDebugMode
    GreDebugPretty() << _name << " : Destroyed." << std::endl;
#endif
}

const std::string& Resource::getName() const
{
    return _name;
}

const void* Resource::_getData() const
{
    return nullptr;
}

const void* Resource::getCustomData(const std::string &dataname) const
{
    return nullptr;
}

void Resource::storeVariantData(int index, const Variant &data)
{
    if(index >= 0)
        _mVariantDatas[index].variant = data;
}

Variant& Resource::getVariantData(int index)
{
    if(index >= 0)
        return _mVariantDatas[index].variant;
    else
        return _mVariantDatas[-1].variant;
}

const Variant& Resource::getVariantData(int index) const
{
    if(index >= 0)
        return _mVariantDatas.at(index).variant;
    else
        return _mVariantDatas.at(-1).variant;
}

// ---------------------------------------------------------------------------------------------------

ResourceUser ResourceUser::Null = ResourceUser(std::weak_ptr<Resource>());

ResourceUser::ResourceUser (std::weak_ptr<Resource> r) :
_resource ( std::move(r) )
{
#ifdef DEBUG
//    GreDebugPretty() << "[ResourceUser:" << ( _resource.lock() ? _resource.lock()->getName() : "null" ) << "] Constructed." << std::endl;
#endif
}

ResourceUser::~ResourceUser ()
{
#ifdef DEBUG
//    GreDebugPretty() << "[ResourceUser:" << ( _resource.lock() ? _resource.lock()->getName() : "null" ) << "] Destroyed." << std::endl;
#endif
}

bool ResourceUser::expired () const
{
    return _resource.expired();
}

const std::string& ResourceUser::getName() const
{
    auto ptr = _resource.lock();
    return ptr->getName();
}


std::shared_ptr<Resource> ResourceUser::lock()
{
    return _resource.lock();
}
const std::shared_ptr<Resource> ResourceUser::lock() const
{
    return _resource.lock();
}

const void* ResourceUser::getCustomData(const std::string &dataname) const
{
    auto ptr = _resource.lock();
    if(ptr)
        return ptr->getCustomData(dataname);
    return nullptr;
}

void ResourceUser::storeVariantData(int index, const Gre::Variant &data)
{
    auto ptr = _resource.lock();
    if(ptr)
        ptr->storeVariantData(index, data);
}

Variant& ResourceUser::getVariantData(int index)
{
    auto ptr = _resource.lock();
    if(ptr)
        return ptr->getVariantData(index);
    return Variant::Null;
}

const Variant& ResourceUser::getVariantData(int index) const
{
    auto ptr = _resource.lock();
    if(ptr)
        return ptr->getVariantData(index);
    return Variant::Null;
}

// ---------------------------------------------------------------------------------------------------

ResourceLoader::ResourceLoader()
{
}
ResourceLoader::ResourceLoader(const ResourceLoader&)
{
}

ResourceLoader::~ResourceLoader()
{
}

bool ResourceLoader::isTypeSupported (Resource::Type type) const
{
    return false;
}

ResourceLoader* ResourceLoader::clone() const
{
    return nullptr;
}

GRE_END_NAMESPACE