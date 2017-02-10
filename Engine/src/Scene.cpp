//////////////////////////////////////////////////////////////////////
//
//  Scene.cpp
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2017 Luk2010
//  Created on 14/12/2015.
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

#include "Scene.h"
#include "RenderingQuery.h"

GreBeginNamespace

RenderScene::RenderScene(const std::string& name, const RenderSceneOptions& options)
: Gre::Resource(name) , iRootNode(nullptr)
{
    
}

RenderScene::~RenderScene()
{
    
}

const RenderNodeUser RenderScene::getRootNode() const
{
    return iRootNode;
}

const RenderNodeHolder& RenderScene::setRootNode(const RenderNodeHolder &rendernode)
{
    GreAutolock ;
    
    if ( !iRootNode.isInvalid() )
    {
        Resource::removeListener( EventProceederUser(iRootNode) );
    }
    
    iRootNode = rendernode;
    
    if ( !iRootNode.isInvalid() )
    {
        Resource::addListener( EventProceederUser(iRootNode) );
    }
    
    return iRootNode;
}

RenderNodeHolder RenderScene::createNode( const std::string & name ) const
{
    return RenderNodeHolder ( new RenderNode(name) );
}

void RenderScene::addNode(const RenderNodeHolder &rendernode)
{
    GreAutolock ;
    
    if ( !iRootNode.isInvalid() )
    {
        iRootNode->addNode(rendernode);
    }
    
    else
    {
        setRootNode ( rendernode ) ;
    }
}

void RenderScene::removeNode( const RenderNodeHolder & holder )
{
    if ( !holder.isInvalid() )
    {
        GreAutolock ;
        
        // See if this node has a parent.
        
        RenderNodeHolder parent = holder->getParent () ;
        
        if ( !parent.isInvalid() )
        {
            parent -> remove ( holder-> getIdentifier() ) ;
        }
        
        // Now this child will be destroyed when the last holder holding it
        // will be destroyed. Normally, this holder is used by the USER , and at
        // the end of its function, should be destroyed.
        
        // Note : This function version also destroys every children of the
        // RenderNode to destroy. See 'RenderScene::removeNodeNoChildren()' to remove
        // a RenderNode without removing its children.
    }
}

void RenderScene::removeNodeNoChildren ( const RenderNodeHolder & holder )
{
    if ( !holder.isInvalid() )
    {
        GreAutolock ;
        
        // See if this node has a parent.
        
        RenderNodeHolder parent = holder->getParent () ;
        
        if ( !parent.isInvalid() )
        {
            parent -> removeNotRecursive ( holder-> getIdentifier() ) ;
        }
        
        // Now this child will be destroyed when the last holder holding it
        // will be destroyed. Normally, this holder is used by the USER , and at
        // the end of its function, should be destroyed.
        
        // Note : This function destroys only the given RenderNode. Its children
        // are distribuited in the parent's children using 'RenderNode::addNode()'.
    }
}

void RenderScene::removeNodeFromIdentifier(const Gre::ResourceIdentifier &identifier)
{
    GreAutolock ;
    
    // Try to find a Node with given identifier. This function is therefore much more
    // time-cost than 'RenderScene::removeNode()'.
    
    RenderNodeHolder node = findHolder ( identifier ) ;
    
    if ( !node.isInvalid() )
    {
        removeNode(node);
    }
    
#ifdef GreIsDebugMode
    else
    {
        GreDebugPretty() << "Invalid ResourceIdentifier given : '" << (uint32_t) identifier << "'." << Gre::gendl;
    }
#endif
    
}

RenderNodeHolder RenderScene::findHolder ( const Gre::ResourceIdentifier & identifier )
{
    GreAutolock ;
    
    if ( !iRootNode.isInvalid() )
    {
        if ( iRootNode->getIdentifier() == identifier )
            return iRootNode ;
        
        return iRootNode -> find(identifier) ;
    }
    
    return RenderNodeHolder ( nullptr ) ;
}

RenderNodeUser RenderScene::findNode ( const Gre::ResourceIdentifier & identifier)
{
    GreAutolock ;
    
    if ( !iRootNode.isInvalid() )
    {
        if ( iRootNode->getIdentifier() == identifier )
            return iRootNode ;
        
        return iRootNode -> find(identifier) ;
    }
    
    return RenderNodeHolder ( nullptr ) ;
}

const RenderNodeUser RenderScene::findNode ( const Gre::ResourceIdentifier & identifier) const
{
    GreAutolock ;
    
    if ( !iRootNode.isInvalid() )
    {
        if ( iRootNode->getIdentifier() == identifier )
            return iRootNode ;
        
        return iRootNode -> find(identifier) ;
    }
    
    return RenderNodeHolder ( nullptr ) ;
}

void RenderScene::draw(const EventHolder &elapsed) const
{
    GreAutolock ;
    
    if ( iRenderTarget.isInvalid() )
        return ;
    
    if ( elapsed.isInvalid() || elapsed->getType() != EventType::Update )
        return ;
    
    if ( !iTechnique.isInvalid() )
    {
        iRenderTarget -> bind () ;
        
        {
            EventHolder e = EventHolder ( new RenderScenePreRenderEvent(this) ) ;
            sendEvent ( e ) ;
            
            const RendererHolder rholder = iRenderer.lock() ;
            if ( !rholder.isInvalid() ) {
                rholder->preRender (iClearColor) ;
            }
            
            _preRender () ;
        }
        
        {
            drawTechnique(iTechnique, elapsed) ;
        }
        
        {
            EventHolder e = EventHolder ( new RenderScenePostRenderEvent(this) ) ;
            sendEvent ( e ) ;
            
            const RendererHolder rholder = iRenderer.lock() ;
            if ( !rholder.isInvalid() ) {
                rholder->postRender () ;
            }
            
            _postRender () ;
        }
    
        iRenderTarget -> swapBuffers () ;
        iRenderTarget -> unbind () ;
    }
}

void RenderScene::setTechnique(const TechniqueUser &technique)
{
    GreAutolock ; iTechnique = technique.lock() ;
#ifdef GreIsDebugMode
    if ( technique.isInvalid() ) {
        GreDebug("[WARN] Setting invalid Technique to RenderScene '") << getName() << "'." << Gre::gendl ;
    }
#endif
}

void RenderScene::setRenderer ( const RendererUser& renderer )
{
    GreAutolock ; iRenderer = renderer ;
#ifdef GreIsDebugMode
    if ( renderer.isInvalid() ) {
        GreDebug("[WARN] Setting invalid Renderer to RenderScene '") << getName() << "'." << Gre::gendl ;
    }
#endif
}

void RenderScene::setRenderTarget(const RenderTargetUser &target)
{
    GreAutolock ; iRenderTarget = target.lock() ;
#ifdef GreIsDebugMode
    if ( target.isInvalid() ) {
        GreDebug("[WARN] Setting invalid RenderTarget to RenderScene '") << getName() << "'." << Gre::gendl ;
    }
#endif
}

void RenderScene::setClearColor ( const Color& color )
{
    GreAutolock ; iClearColor = color ;
}

void RenderScene::drawTechnique( const TechniqueHolder& technique , const EventHolder &elapsed) const
{
#ifdef GreIsDebugMode
    if ( technique.isInvalid() ) {
        GreDebug("[WARN] Invalid Technique to draw RenderScene '") << getName() << "'." << Gre::gendl ;
        return ;
    } else if ( elapsed.isInvalid() ) {
        GreDebug("[WARN] Invalid EventHolder to draw RenderScene '") << getName() << "'." << Gre::gendl ;
        return ;
    }
#endif
    
    if ( technique->isActivated() )
    {
        for ( auto & pass : technique->getPasses() )
        {
            RenderingQuery query ;
            query.setRenderScene ( RenderSceneUser(this) ) ;
            query.setRenderPass ( pass ) ;
            query.setCamera ( technique->getCamera() ) ;
            query.setHardwareProgram ( pass->getHardwareProgram() ) ;
            query.setViewport ( technique->getViewport() ) ;
            
            const UpdateEvent& u = elapsed->to<UpdateEvent>() ;
            query.setElapsedTime ( u.elapsedTime ) ;
            
            if ( technique->isExclusive() ) {
                query.setRenderedNodes ( technique->getNodes() ) ;
            } else {
                query.setRenderedNodes ( { getRootNode().lock() } ) ;
            }
            
            RendererHolder rholder = iRenderer.lock() ;
            if ( rholder.isInvalid() )
                return ;
            
            rholder -> draw ( query ) ;
        }
        
        if ( technique->hasSubtechniques() )
        {
            for ( const TechniqueHolder & tech : technique->getSubtechniques() )
            {
                drawTechnique(tech, elapsed) ;
            }
        }
    }
}

void RenderScene::_preRender () const
{
    
}

void RenderScene::_postRender () const
{
    
}

// ---------------------------------------------------------------------------------------------------

RenderSceneLoader::RenderSceneLoader()
{
    
}

RenderSceneLoader::~RenderSceneLoader()
{
    
}

ResourceLoader* RenderSceneLoader::clone() const
{
    return new RenderSceneLoader () ;
}

bool RenderSceneLoader::isLoadable(const std::string &filepath) const
{
    return false;
}

RenderSceneHolder RenderSceneLoader::load(const std::string &name, const RenderSceneOptions &options) const
{
    return RenderSceneHolder ( new RenderScene(name, options) ) ;
}

// ---------------------------------------------------------------------------------------------------

RenderSceneManager::RenderSceneManager ( const std::string& name )
: SpecializedResourceManager<Gre::RenderScene, Gre::RenderSceneLoader>(name)
{
    GreAutolock ;
    
    iLoaders.registers( "Default" , new RenderSceneLoader() );
}

RenderSceneManager::~RenderSceneManager() noexcept ( false )
{
    
}

RenderSceneUser RenderSceneManager::load(const RenderSceneHolder &holder)
{
    if ( !holder.isInvalid() )
    {
        GreAutolock ;
        
        {
            RenderSceneHolder tmp = findHolder ( holder->getIdentifier() ) ;
            
            if ( !tmp.isInvalid() )
            {
#ifdef GreIsDebugMode
                GreDebugPretty() << "RenderScene '" << holder->getName() << "' already registered." << Gre::gendl;
                GreDebugPretty() << "  Resource Identifier was : " << holder->getIdentifier() << "." << Gre::gendl ;
#endif
                return RenderSceneUser ( nullptr );
            }
        }
        
#ifdef GreIsDebugMode
        GreDebugPretty() << "RenderScene '" << holder->getName() << "' registered." << Gre::gendl;
#endif
        
        iHolders.add(holder);
        addListener(EventProceederUser(holder));
        return holder;
    }
    
    return RenderSceneUser ( nullptr );
}

RenderSceneUser RenderSceneManager::load ( const std::string & name , const RenderSceneOptions & options )
{
#ifdef GreIsDebugMode
    if ( name.empty() ) {
        GreDebug("[WARN] Initialized RenderScene with no name.") << Gre::gendl;
    }
#endif
    
    RenderSceneLoader* loader = nullptr ;
    auto it = options.find("Loader" );
    
    if ( it != options.end() ) {
        loader = findLoader (it->second.toString()) ;
    } else {
        loader = findLoader () ;
    }
    
    if ( !loader )
    {
#ifdef GreIsDebugMode
        GreDebug("[WARN] Can't find a loader for RenderScene '") << name << "'." << Gre::gendl;
#endif
        return RenderSceneUser ( nullptr ) ;
    }
    
    RenderSceneHolder scene = loader -> load(name, options) ;
    if ( scene.isInvalid() ) {
#ifdef GreIsDebugMode
        GreDebug("[WARN] Can't load RenderScene '") << name << "'." << Gre::gendl ;
#endif 
        return RenderSceneUser ( nullptr ) ;
    }
    
    std::string technique = options.find("Technique") == options.end() ? "Default" : options.at("Technique").toString() ;
    if ( technique.empty() ) technique = "Default" ;
    
    scene -> setTechnique ( findTechnique(technique) ) ;
    
    iHolders.push_back(scene);
    addListener(EventProceederUser(scene));
    
#ifdef GreIsDebugMode
    GreDebug("[INFO] RenderScene '") << name << "' registered." << Gre::gendl ;
#endif
    
    return scene ;
}

void RenderSceneManager::drawScenes( const EventHolder& e ) const
{
    GreAutolock ;
    
    for ( auto it : iHolders )
    {
        if ( !it.isInvalid() )
        {
            it->draw ( e ) ;
        }
    }
}

TechniqueUser RenderSceneManager::findTechnique ( const std::string & name )
{
    for ( TechniqueHolder& tech : iTechniques ) {
        if ( !tech.isInvalid() ) {
            if ( tech->getName() == name ) return tech ;
        }
    }
    
    return TechniqueUser ( nullptr ) ;
}

GreEndNamespace
