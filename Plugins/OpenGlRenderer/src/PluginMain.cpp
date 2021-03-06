//////////////////////////////////////////////////////////////////////
//
//  PluginMain.cpp
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2017 Luk2010
//  Created on 11/02/2017.
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
#include "OpenGlRenderer.h"

#ifdef GrePlatformUnix

bool glewinited = false ;

GLenum initGlew ( void )
{
    //////////////////////////////////////////////////////////////////////
    // Initializes GLEW in experimental mode when on linux-like platforms.

    GreDebug( "[INFO] Initializing GLEW." ) << Gre::gendl ;
    glewExperimental = GL_TRUE ;

    GLenum err = glewInit();

    if ( GLEW_OK != err )
    GreDebug ( "[ERRO] Can't initialize GLEW : " ) << glewGetErrorString(err) << Gre::gendl ;

    else
    glewinited = true ;
}

#endif

Gre::PluginInfo info ;

extern "C" Gre::PluginInfo * GetPluginInfo ( void )
{
    info.name = "OpenGlRenderer Plugin" ;
    info.author = "Luk2010" ;
    info.version = GRE_PLUGIN_VERSION ;
    
    uuid_parse("56704513-f05d-4a4a-af96-c16ccccfdfe2", info.uuid);
    
    return & info ;
}

extern "C" void StartPlugin ( void )
{
    // Creates an OpenGlRendererLoader instance and register it to the
    // RendererManager. The user should load correctly a Renderer, but it has
    // no use of 'install' function as the loader already calls it.
    
    OpenGlRendererLoader* loader = new OpenGlRendererLoader () ;
    if ( !loader ) {
        GreDebug("[WARN]") << "Can't create 'OpenGlRendererLoader' instance." << Gre::gendl ;
        return ;
    }
    
    auto & factory = Gre::ResourceManager::Get () ->getRendererManager()->getFactory() ;
    factory.registers("OpenGlRendererLoader", loader);
    
    GreDebug("[INFO] OpenGlRenderer Plugin started.") << Gre::gendl ;
}

extern "C" void StopPlugin ( void )
{
    auto & factory = Gre::ResourceManager::Get() ->getRendererManager()->getFactory() ;
    factory.unregister("OpenGlRendererLoader");
    
    GreDebug("[INFO] OpenGlRenderer Plugin stopped.") << Gre::gendl ;
}
