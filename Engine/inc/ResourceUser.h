//////////////////////////////////////////////////////////////////////
//
//  ResourceUser.h
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2016 Luk2010
//  Created on 02/05/2016.
//
//////////////////////////////////////////////////////////////////////
/*
 -----------------------------------------------------------------------------
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */

#ifndef GRE_ResourceUser_h
#define GRE_ResourceUser_h

#include "ResourceHolder.h"

GreBeginNamespace

//////////////////////////////////////////////////////////////////////
/// @class ResourceUser
/// @brief Permits easier use of Resource Objects.
///
/// The ResourceUser is a proxy to use Resource's Objects without having
/// to manage if the Object is still alive or not. The ResourceUser Object
/// holds a pointer to the Resource Object, and also one to the
/// ReferenceCounter Object.
///
/// The ResourceUser object is not allowed to destroy the Resource. This
/// job has to be done by the Resource itself, commiting suicide. This
/// suicide is managed by ResourceHolder, calling Resource::release().
///
/// The ResourceUser is in charge for destroying the ReferenceCounter when
/// ReferenceCounter::getUserCount() reachs 0.
//////////////////////////////////////////////////////////////////////
class DLL_PUBLIC ResourceUser
{
public:
    
    POOLED(Pools::Resource)
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a User from a raw Resource.
    //////////////////////////////////////////////////////////////////////
    ResourceUser(const Resource* resource);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a User from a holder.
    //////////////////////////////////////////////////////////////////////
    ResourceUser(const ResourceHolder& holder);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Copy another ResourceUser.
    //////////////////////////////////////////////////////////////////////
    ResourceUser(const ResourceUser& user);
    
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~ResourceUser() noexcept(false);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    ResourceHolder lock();
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    const ResourceHolder lock() const;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns true if the ReferenceCounter::getHolderCount() is 0,
    /// or if the ReferenceCounter is null.
    //////////////////////////////////////////////////////////////////////
    virtual bool isInvalid() const;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns true if ReferenceCounter::getHolderCount() is 0.
    //////////////////////////////////////////////////////////////////////
    virtual bool isExpired() const;
    operator bool() const;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Reset the ResourceUser.
    //////////////////////////////////////////////////////////////////////
    void reset();
    
public:
    
    ////////////////////////////////////////////////////////////////////////
    /// @brief Returns the Name of this Resource.
    ////////////////////////////////////////////////////////////////////////
    std::string getName() const;
    
    ////////////////////////////////////////////////////////////////////////
    /// @brief Returns the Type of this Resource.
    ////////////////////////////////////////////////////////////////////////
    Resource::Type getType() const;
    
public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Add an action to the Listener.
    /// An Action is a function which do something when an event of the correct
    /// type reach it. You can use it thanks to lambda functions.
    //////////////////////////////////////////////////////////////////////
    void addAction(EventType etype, std::function<void (const Event&)> eaction);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Removes every functions in the Listener.
    /// @note The Listener continues to listen to objects it has been
    /// registered to.
    //////////////////////////////////////////////////////////////////////
    void resetActions();
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Calls every methods registered in the Listener.
    /// When overwriting this method, if you still want to add actions, use
    /// ::onEvent(e) to call this class function which correctly calls every
    /// methods.
    //////////////////////////////////////////////////////////////////////
    void onEvent (const Event& e);
    
public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Adds a listener giving him a new name.
    /// This function creates a new Generic Listener and returns it.
    //////////////////////////////////////////////////////////////////////
    ResourceUser& addListener(const std::string& name);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Adds a listener to the emitter list, and returns a reference
    /// to the generic listener added.
    //////////////////////////////////////////////////////////////////////
    ResourceUser& addListener(const ResourceUser& listener);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns a shared_pointer to the Generic Listener pointer
    ///by its name.
    //////////////////////////////////////////////////////////////////////
    ResourceUser getListener(const std::string& name);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Remove the given Listener from the list.
    //////////////////////////////////////////////////////////////////////
    void removeListener(const std::string& name);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Send an event to every listeners.
    //////////////////////////////////////////////////////////////////////
    void sendEvent(const Event& e);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Sets this property to true if the Resource object should
    /// transmit its received Event to its listeners.
    //////////////////////////////////////////////////////////////////////
    void setShouldTransmitEvents(bool p);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns true if the Resource object should transmit its
    /// received Event to its listeners.
    //////////////////////////////////////////////////////////////////////
    bool shouldTransmitEvents() const;
    
public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Increase the Reference Counter by one.
    /// If the Counter wasn't initialized, sets iCounterInitialized to true.
    //////////////////////////////////////////////////////////////////////
    void acquire();
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Decrease the Reference Counter by one.
    /// If the Counter reach 0, commit suicide.
    //////////////////////////////////////////////////////////////////////
    void release();
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the number of ResourceHolder holding this Resource.
    /// @see ReferenceCounter::getHolderCount().
    //////////////////////////////////////////////////////////////////////
    int getCounterValue() const;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the ReferenceCounter Object, or null if uninitialized.
    //////////////////////////////////////////////////////////////////////
    ReferenceCounter* getReferenceCounter();
    
public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Return the Variant for given Custom Data's entry.
    //////////////////////////////////////////////////////////////////////
    Variant& getCustomData(const std::string& entry);
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Return the Variant for given Custom Data's entry.
    //////////////////////////////////////////////////////////////////////
    const Variant& getCustomData(const std::string& entry) const;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Sets the Variant for given Custom Data's entry.
    //////////////////////////////////////////////////////////////////////
    void setCustomData(const std::string &entry, const Variant& data);
    
public:
    
    /// @brief A Null ReferenceUser.
    static ResourceUser Null;
    
protected:
    
    /// @brief The Resource pointer.
    Resource* iResource;
    
    /// @brief The Reference Counter.
    mutable ReferenceCounter* iCounter;
};

//////////////////////////////////////////////////////////////////////
/// @brief Helps locking the Resource Object.
//////////////////////////////////////////////////////////////////////
template<typename Class>
class ResourceLockerHelper
{
public:
    
    /// @brief Common typedef for templated holder.
    typedef SpecializedResourceHolder<Class> ClassHolder;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs the ResourceLockerHelper from Resource.
    //////////////////////////////////////////////////////////////////////
    ResourceLockerHelper(const Class* resource = nullptr)
    {
        iClass = const_cast<Class*>(resource);
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Copy the Helper.
    //////////////////////////////////////////////////////////////////////
    ResourceLockerHelper(const ResourceLockerHelper<Class>& helper)
    {
        iClass = helper.iClass;
    }
    
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ~ResourceLockerHelper() { }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    ClassHolder lock(ReferenceCounter* counter)
    {
        return SpecializedResourceHolder<Class>(iClass, counter);
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    const SpecializedResourceHolder<Class> lock(ReferenceCounter* counter) const
    {
        return SpecializedResourceHolder<Class>(iClass, counter);
    }
    
    /// @brief iClass properties, pointer to the Specialized Resource.
    Class* iClass;
};

//////////////////////////////////////////////////////////////////////
/// @brief A template Class to help creating Specialized ResourceUser.
//////////////////////////////////////////////////////////////////////
template<typename Class>
class SpecializedResourceUser : virtual public ResourceUser
{
public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs the ResourceUser from raw Resource.
    //////////////////////////////////////////////////////////////////////
    SpecializedResourceUser(const Class* resource = nullptr)
    : ResourceUser(dynamic_cast<const Resource*>(resource)), iHelper(resource)
    {
        
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs a SpecializedResourceUser from a 
    /// SpecializedResourceHolder of same type.
    //////////////////////////////////////////////////////////////////////
    SpecializedResourceUser(const SpecializedResourceHolder<Class>& holder)
    : ResourceUser(holder), iHelper(holder.get())
    {
    	
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs from a ResourceUser.
    //////////////////////////////////////////////////////////////////////
    SpecializedResourceUser(const ResourceUser& user)
    : ResourceUser(user), iHelper(reinterpret_cast<const Class*>(user.lock().get()))
    {
        
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Copy this class.
    //////////////////////////////////////////////////////////////////////
    SpecializedResourceUser(const SpecializedResourceUser<Class>& user)
    : ResourceUser(user), iHelper(user.iHelper)
    {
        
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs from another Specialized.
    //////////////////////////////////////////////////////////////////////
    template<typename Class2>
    SpecializedResourceUser(const SpecializedResourceUser<Class2>& user)
    : ResourceUser(user), iHelper(reinterpret_cast<const Class*>(user.lock().get()))
    {
        
    }
    
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~SpecializedResourceUser() noexcept(false) { }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    SpecializedResourceHolder<Class> lock()
    {
        return iHelper.lock(iCounter);
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new ResourceHolder in order to use the Resource.
    //////////////////////////////////////////////////////////////////////
    const SpecializedResourceHolder<Class> lock() const
    {
        return iHelper.lock(iCounter);
    }
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Reset the ResourceUser.
    //////////////////////////////////////////////////////////////////////
    void reset()
    {
        ResourceUser::reset();
        iHelper.iClass = nullptr;
    }
    
private:
    
    /// @brief Holds the specialized Resource.
    ResourceLockerHelper<Class> iHelper;
};

/// @brief Should permit a simple lock() for a Child user-class.
///
/// @param holdertype : Class the lock() function should return.
/// @param privatetype : Class the holder handles. ( Your private resource )
/// @param baseusertype : The base class your Child comes from.
///
/// To handle multiple inheritance, just set 'baseusertype' as your first
/// ResourceUser parent, as it is used in order to retrieve a pointer from the
/// Resource object.
#define GreUserLockCast( holdertype , privatetype , baseusertype ) \
    holdertype ( reinterpret_cast < privatetype * > ( baseusertype ::lock().get() ) )

/// @brief Should permit a simple const-lock() for a Child user-class.
/// @param holdertype : Class the lock() function should return.
/// @param privatetype : Class the holder handles. ( Your private resource )
/// @param baseusertype : The base class your Child comes from.
///
/// To handle multiple inheritance, just set 'baseusertype' as your first
/// ResourceUser parent, as it is used in order to retrieve a pointer from the
/// Resource object.
///
/// @note This Constant derived macro use 'const_cast', because the return type
/// of the function using this one should be 'const ResourceHolderType' . The
/// constant is preserved.
#define GreUserConstLockCast( holdertype , privatetype , baseusertype ) \
    holdertype ( const_cast < privatetype * > ( reinterpret_cast < const privatetype * > ( baseusertype ::lock().get() ) ) )

GreEndNamespace

#endif