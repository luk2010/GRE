//////////////////////////////////////////////////////////////////////
//
//  PluginMain.cpp
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2017 Luk2010
//  Created on 09/02/2017.
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

#include <ResourceManager.h>
#include "macOSWindowManager.h"
#include "macOSWindow.h"

using namespace Gre ;

class DefaultApplicationLoader : public ApplicationLoader
{
public:
	
	DefaultApplicationLoader () { }
	virtual ~DefaultApplicationLoader () noexcept ( false ) { }
	virtual ApplicationHolder load (const std::string& name , const std::string& author , const std::string& desc) const 
	{
		return ApplicationHolder ( new Application(name, author, desc) );
	}
	virtual ResourceLoader* clone () const 
	{
		return new DefaultApplicationLoader () ;
	}
	virtual bool isLoadable ( const std::string& file ) const
	{
		return false ;
	}
};

PluginInfo info ;

extern "C" PluginInfo * GetPluginInfo ( void )
{
    info.name = "DarwinApplication Plugin" ;
    info.author = "Luk2010" ;
    info.version = GRE_PLUGIN_VERSION ;
    
    uuid_parse("e6cbad59-8beb-4ebc-9f0b-7ec694dd7d0b", info.uuid);
    
    return & info ;
}

extern "C" void StartPlugin (void)
{
    // Create a Window Manager and initialize Appkit.
    macOSWindowManager* wmanager = new macOSWindowManager() ;
    initializeAppkit ( wmanager ) ;
    
    // Registers the Window Loader.
    wmanager->getFactory().registers("macOS Window Loader", new macOSWindowLoader());
    
    // Here , we only have to register the macOSWindowManager.
    ResourceManager::Get()->setWindowManager(WindowManagerHolder(wmanager));
	
	// Also, think about the ApplicationLoader. Finally, this is the WindowManager wich will 
	// process OS X events. So, just load a default Application loader.
	ResourceManager::Get()->getApplicationFactory().registers("DefaultApplicationLoader", new DefaultApplicationLoader());
}

extern "C" void StopPlugin (void)
{
    // Retrieve the Window Manager and terminate appkit.
    macOSWindowManager* manager = reinterpret_cast<macOSWindowManager*>(ResourceManager::Get()->getWindowManager().getObject());
    terminateAppkit(manager);
    
    GreDebug ( "[INFO] Mac OS X Window Plugin stopped." ) << Gre::gendl ;
}



