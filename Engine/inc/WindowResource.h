//
//  WindowResource.h
//  GRE
//
//  Created by Jacques Tronconi on 15/11/2015.
//
//

#ifndef GRE_WindowResource_h
#define GRE_WindowResource_h

#include "Resource.h"
#include "Renderer.h"
#include "Window.h"

struct WindowPrivate
{
    
};

class DLL_PUBLIC WindowResource : public Resource
{
public:
    
    POOLED(Pools::Resource)
    
    WindowResource (const std::string& name, const WindowPrivate& winData);
    
    virtual ~WindowResource();
    
    virtual bool pollEvent() { return false; }
    
    virtual bool isClosed() const { return true; }
    
    virtual const std::string recommendedRenderer() const { return ""; }
    
    virtual void associate(Renderer& renderer) { _associatedRenderer = renderer; }
    
    Renderer& getAssociatedRenderer() { return _associatedRenderer; }
    
    virtual void setTitle(const std::string& title) { }
    
    virtual void swapBuffers() { }
    
    virtual WindowSize getWindowSize() const { return std::make_pair(0, 0); }
    
protected:
    
    WindowPrivate _data;
    Renderer      _associatedRenderer;
};

#endif
