//////////////////////////////////////////////////////////////////////
//
//  Event.h
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2016 Luk2010
//  Created on 23/11/2015.
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

#ifndef GRE_Event_h
#define GRE_Event_h

#include "Version.h"
#include "Pools.h"
#include "Key.h"
#include "Variant.h"
#include "ReferenceCountedObject.h"

GreBeginNamespace

class EventProceeder ;

//////////////////////////////////////////////////////////////////////
/// @brief Determines an Event Type.
/// Also list every events available in the Engine.
//////////////////////////////////////////////////////////////////////
enum class EventType
{
    KeyDown, KeyUp,

    LeftMousePress , LeftMouseRelease ,
    RightMousePress , RightMouseRelease ,
    MouseExitedWindow , MouseEnteredWindow ,
    CursorMoved ,

    Update,

    WindowSized ,
    WindowMoved ,
    WindowExposed ,
    WindowWillClose ,
    WindowTitleChanged ,
    WindowAttachContext ,
    WindowDetachContext ,
    WindowFocused ,
    WindowUnfocused ,
    WindowClosed ,
    LastWindowClosed ,

    RenderTargetWillClose ,
    RenderTargetClosed ,
    RenderTargetChangedRenderContext ,
    RenderTargetChangedFramebuffer ,

    RendererRegisteredTarget ,
    RendererUnregisteredTarget ,

    RenderScenePreRender ,
    RenderScenePostRender ,

    ResourceUnloaded ,

    PositionChanged , DirectionChanged ,

    Custom
};

//////////////////////////////////////////////////////////////////////
/// @brief Defines a basic event.
///
/// ### Introduction
///
/// In GRE , an Event is like a notification object. An Event is sent
/// to a Resource Object if this one listens to another Resource. The
/// Resource object that is listening is called a "Receiver" , the
/// Resource object that is listened is called an "Emitter" . Every
/// Resource can have those two roles.
///
/// The Application Object is responsible for sending EventType::Update
/// notifications to the different Managers available in the
/// ResourceManager singleton .
///
/// ### Subclassing 'Gre::Event'
///
/// You can define a new Event just in subclassing the main Event class.
/// Please see the implementation of simple Events ( as 'UpdateEvent' )
/// to see how to implement an Event .
///
/// You can also use the CustomEvent class, but this is not recommended
/// as the Properties map is a big structure and can lead to performance
/// reducing.
///
/// ### Sending a new Event
///
/// To send an Event , you just have to create it using 'new' , and use
/// the 'Resource::sendEvent()' function. This will send the Event to
/// every listeners registered in the Resource object.
///
/// @note
/// You can set the 'destroy' argument to false if you want to destroy the
/// Event yourself.
///
/// You also can send an Event to your listeners but asynchronously. This is
/// made by pushing the Event to the GlobalEventDispatcher.
///
/// ### Receiving an Event
///
/// When a Resource listens to an Emitter , every Events received will be
/// treated in 'Resource::onEvent()' . This function is already implemented
/// by the Resource base class.
///
/// @note
/// You can overwrite it if you want , for example , to take care of CustomEvent
/// before taking care of other Events , or if you want another behaviour.
///
/// The 'Resource::onEvent()' function do those differents things :
///
///   - Sends the Event to listeners registered. If one of the listeners set
///   the 'Event::iMustStopPropagating' property to true , the function stops here.
///   - Calls the correct protected specific Event function depending on
///   the Event Type.
///   - Calls every functions registered by 'Resource::onNextEvent()'.
///   - Calls every functions registered as Actions.
///
//////////////////////////////////////////////////////////////////////
class Event : public ReferenceCountedObject
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    /// @brief Constructs an Event wich has the given EventProceeder for
    /// Emitter . Please note this Emitter should not be destroyed or
    /// altered while creating the Event.
    //////////////////////////////////////////////////////////////////////
    Event ( const EventProceeder * emitter , const EventType& etype ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~Event() noexcept(false) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the Emitter EventProceederUser .
    //////////////////////////////////////////////////////////////////////
    const Holder < EventProceeder > & getEmitter () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns a pointer to the emitter.
    //////////////////////////////////////////////////////////////////////
    virtual const EventProceeder* getEmitterPointer () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the event's type.
    //////////////////////////////////////////////////////////////////////
    const EventType& getType () const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns 'iShouldStopPropagating' property.
    //////////////////////////////////////////////////////////////////////
    bool shouldStopPropagating () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Changes the 'iShouldStopPropagating' property.
    //////////////////////////////////////////////////////////////////////
    void setShouldStopPropagating ( bool value ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns 'iNoSublisteners'.
    //////////////////////////////////////////////////////////////////////
    bool noSublisteners () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Changes 'iNoSublisteners'.
    //////////////////////////////////////////////////////////////////////
    void setNoSublisteners ( bool value ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the event but according to its real type.
    //////////////////////////////////////////////////////////////////////
    template < typename T > const T & to() const
    {
        if ( std::is_base_of<Event, T>::value )
        {
            return * reinterpret_cast< T* > ( const_cast< Event* >(this) ) ;
        }
        else
        {
            throw GreExceptionWithText ( "Bad Event Type translation." ) ;
        }
    }

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    virtual Event* clone () const = 0 ;

protected:

    /// @brief Emitter for this event .
    Holder < EventProceeder > * iEmitter ;

    /// @brief Type for this event .
    EventType iType;

    /// @brief True if you want to stop the propagation of this Event to other
    /// listeners of the emitter. At construction , this value is always 'false'.
    bool iShouldStopPropagating ;

    /// @brief True if this event should not be send to sublisteners.
    bool iNoSublisteners ;
};

/// @brief A Generic Event callback.
typedef std::function < void ( const Event& ) > EventCallback;

typedef Holder<Event> EventHolder ;

/// @brief A callback based on EventHolder .
typedef std::function < void ( const EventHolder & ) > EventHolderCallback ;

//////////////////////////////////////////////////////////////////////
/// @brief A generic update event that notify the listener it should be
/// updated.
//////////////////////////////////////////////////////////////////////
class UpdateEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    UpdateEvent( const EventProceeder* emitter , const Duration & t ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief Elapsed Time (in seconds) .
    Duration elapsedTime;
};

//////////////////////////////////////////////////////////////////////
/// @brief KeyDown Event .
//////////////////////////////////////////////////////////////////////
class KeyDownEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    KeyDownEvent ( const EventProceeder* emitter , Key key , int mods = 0 ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief Key component .
    Key iKey ;

    /// @brief Modifiers flags .
    int iModifiers ;
};

//////////////////////////////////////////////////////////////////////
/// @brief KeyUp Event .
//////////////////////////////////////////////////////////////////////
class KeyUpEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    KeyUpEvent ( const EventProceeder* emitter , Key key , int mods = 0 ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief Key component .
    Key iKey ;

    /// @brief Modifiers flags.
    int iModifiers ;
};

//////////////////////////////////////////////////////////////////////
/// @brief CursorMoved Event.
//////////////////////////////////////////////////////////////////////
class CursorMovedEvent : public Event
{
public:

    POOLED ( Pools::Event )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    CursorMovedEvent ( const EventProceeder* emitter , const float deltax , const float deltay ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief Movement along the X axis.
    float DeltaX ;

    /// @brief Movement along the Y axis.
    float DeltaY ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Window Moved Event .
//////////////////////////////////////////////////////////////////////
class WindowMovedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowMovedEvent ( const EventProceeder* emitter , int left , int top ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief The Left ( x ) component .
    int Left ;

    /// @brief The Top ( y ) component .
    int Top ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Window Sized Event .
//////////////////////////////////////////////////////////////////////
class WindowSizedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowSizedEvent ( const EventProceeder* emitter , int width , int height ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief Width component .
    int Width ;

    /// @brief Height component .
    int Height ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Window Exposed Event .
//////////////////////////////////////////////////////////////////////
class WindowExposedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowExposedEvent ( const EventProceeder* emitter , const Surface& surface ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief The surface it has now to be .
    Surface iSurface ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Window Hidden Event .
//////////////////////////////////////////////////////////////////////
class WindowHiddenEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowHiddenEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Window WillClose Event .
//////////////////////////////////////////////////////////////////////
class WindowWillCloseEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowWillCloseEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowTitleChanged Event .
//////////////////////////////////////////////////////////////////////
class WindowTitleChangedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowTitleChangedEvent ( const EventProceeder* emitter , const std::string& title ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief The new title component .
    std::string iTitle ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowAttachContext Event .
//////////////////////////////////////////////////////////////////////
class WindowAttachContextEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowAttachContextEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowDetachContext Event .
//////////////////////////////////////////////////////////////////////
class WindowDetachContextEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowDetachContextEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowFocused Event .
//////////////////////////////////////////////////////////////////////
class WindowFocusedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowFocusedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowUnfocused Event .
//////////////////////////////////////////////////////////////////////
class WindowUnfocusedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowUnfocusedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowClosed Event .
//////////////////////////////////////////////////////////////////////
class WindowClosedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    WindowClosedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief WindowClosed Event .
//////////////////////////////////////////////////////////////////////
class LastWindowClosedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    LastWindowClosedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderTargetWillClose Event .
//////////////////////////////////////////////////////////////////////
class RenderTargetWillCloseEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderTargetWillCloseEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderTargetClosed Event .
//////////////////////////////////////////////////////////////////////
class RenderTargetClosedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderTargetClosedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderTargetChangedRenderContext Event .
//////////////////////////////////////////////////////////////////////
class RenderTargetChangedRenderContextEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderTargetChangedRenderContextEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderTargetChangedFramebuffer Event .
//////////////////////////////////////////////////////////////////////
class RenderTargetChangedFramebufferEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderTargetChangedFramebufferEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RendererRegisteredTarget Event .
//////////////////////////////////////////////////////////////////////
class RendererRegisteredTargetEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RendererRegisteredTargetEvent ( const EventProceeder* emitter , Holder < EventProceeder > * target ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ~RendererRegisteredTargetEvent () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event.
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief A pointer to a valid (?) EventProceederUser object. This EventProceederUser
    /// represents a RenderTargetPrivate object. The EventProceederUser is created by the
    /// user and destroyed in the Event destructor.
    Holder < EventProceeder > * Target ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RendererUnregisteredTarget Event .
//////////////////////////////////////////////////////////////////////
class RendererUnregisteredTargetEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RendererUnregisteredTargetEvent ( const EventProceeder* emitter , Holder < EventProceeder > * target ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ~RendererUnregisteredTargetEvent () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event.
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief A pointer to a valid EventProceeder object. This EventProceeder
    /// represents a RenderTargetPrivate object. The EventProceederUser is created by the
    /// user and destroyed in the Event destructor.
    Holder < EventProceeder > * Target ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderScenePreRenderEvent Event .
//////////////////////////////////////////////////////////////////////
class RenderScenePreRenderEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderScenePreRenderEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief RenderScenePostRenderEvent Event .
//////////////////////////////////////////////////////////////////////
class RenderScenePostRenderEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    RenderScenePostRenderEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief ResourceUnloaded Event .
/// This special event is sent when the 'emitter' Resource has its
/// 'unload' function called.
//////////////////////////////////////////////////////////////////////
class ResourceUnloadedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ResourceUnloadedEvent ( const EventProceeder* emitter ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ~ResourceUnloadedEvent () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event.
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief PositionChanged Event .
//////////////////////////////////////////////////////////////////////
class PositionChangedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    PositionChangedEvent ( const EventProceeder* emitter , const Vector3& pos ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief New Position.
    Vector3 Position ;
};

//////////////////////////////////////////////////////////////////////
/// @brief DirectionChanged Event .
//////////////////////////////////////////////////////////////////////
class DirectionChangedEvent : public Event
{
public:

    POOLED(Pools::Event)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    DirectionChangedEvent ( const EventProceeder* emitter , const Vector3& dir ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event* clone () const ;

    /// @brief New Direction.
    Vector3 Direction ;
};

//////////////////////////////////////////////////////////////////////
/// @brief A Custom Event .
//////////////////////////////////////////////////////////////////////
class CustomEvent : public Event
{
public:

    POOLED ( Pools::Event )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    CustomEvent ( const EventProceeder* emitter , const std::map < std::string , Variant > & properties ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clones the Event .
    //////////////////////////////////////////////////////////////////////
    Event * clone () const ;

    /// @brief A map of Properties for this Event. Those are defined by the user
    /// which created the Event.
    std::map < std::string , Variant > Properties ;
};

GreEndNamespace
#endif
