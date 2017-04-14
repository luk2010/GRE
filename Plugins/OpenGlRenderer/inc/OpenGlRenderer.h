//////////////////////////////////////////////////////////////////////
//
//  OpenGlRenderer.h
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

#ifndef OpenGlRenderer_h
#define OpenGlRenderer_h

#include <Renderer.h>

//////////////////////////////////////////////////////////////////////
// On Darwin OS , we only need to include the GL framework. Extension 
// management is done within the framework.

#ifdef GrePlatformDarwin
#   include <OpenGL/OpenGL.h>
#   include <OpenGL/gl3.h>
#endif // GrePlatformDarwin

//////////////////////////////////////////////////////////////////////
// On Linux , we use GLEW library to manage OpenGl extensions. Notes
// libglew should be linked.

#ifdef GrePlatformUnix
#   include <GL/glew.h>
#endif

GLenum translateGlTexture ( const Gre::TextureType & type ) ;
GLenum translateGlAttachement ( const Gre::RenderFramebufferAttachement & att ) ;
std::string translateGlError ( GLenum err ) ;

class OpenGlRenderer ;

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Framebuffer.
//////////////////////////////////////////////////////////////////////
class OpenGlFramebuffer : public Gre::RenderFramebuffer
{
public:

    POOLED ( Gre::Pools::Render )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlFramebuffer ( const std::string & name ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlFramebuffer () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Make the FrameBuffer usable.
    //////////////////////////////////////////////////////////////////////
    virtual void bind() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Make the FrameBuffer unusable.
    //////////////////////////////////////////////////////////////////////
    virtual void unbind() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Attachs the given texture object to the given Attachement.
    //////////////////////////////////////////////////////////////////////
    virtual void setAttachement(const Gre::RenderFramebufferAttachement& attachement, Gre::TextureHolder& holder) ;
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Attaches the given texture but don't cache it.
    //////////////////////////////////////////////////////////////////////
    virtual void setAttachementNoCache (const Gre::RenderFramebufferAttachement & attachement ,
                                        const Gre::TextureHolder & holder ) const ;

protected:

    /// @brief OpenGl 's framebuffer id.
    GLuint iGlFramebuffer ;
    
    /// @brief Pushed surface when changing the viewport.
    mutable GLint iGlViewport[4] ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Framebuffer Creator.
//////////////////////////////////////////////////////////////////////
class OpenGlFramebufferCreator : public Gre::RenderFramebufferInternalCreator
{
public:

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlFramebufferCreator ( const OpenGlRenderer * parent ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlFramebufferCreator () ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual Gre::RenderFramebuffer* load ( const std::string & name , const Gre::ResourceLoaderOptions& options ) const ;

protected:

    /// @brief Parent's renderer.
    const OpenGlRenderer* iRenderer ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Texture.
//////////////////////////////////////////////////////////////////////
class OpenGlTexture : public Gre::Texture
{
public:

    POOLED ( Gre::Pools::Render )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlTexture (const std::string & name , const Gre::TextureType & type ,
                   const Gre::SoftwarePixelBufferHolderList & buffers) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlTexture () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns 'iGlTexture != 0'.
    //////////////////////////////////////////////////////////////////////
    virtual bool isGlTextureValid () const ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual GLuint getGlTexture () const ;

protected:

    //////////////////////////////////////////////////////////////////////
    /// @brief Should bind the Texture to the correct target.
    //////////////////////////////////////////////////////////////////////
    virtual void _bind () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Should unbind the Texture from its target.
    //////////////////////////////////////////////////////////////////////
    virtual void _unbind () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Sets the GPU texture buffer to given Texture.
    //////////////////////////////////////////////////////////////////////
    virtual void _setBuffer ( ) const ;

    virtual void _setParameters ( GLenum target ) const ;
    virtual void _applySwizzling ( GLenum target , const Gre::SoftwarePixelBufferHolder& buffer ) const ;

protected:

    /// @brief We create only one iGlTexture id. When using cube map, the
    /// texture id does not vary.
    mutable GLuint iGlTexture ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Texture Manager.
//////////////////////////////////////////////////////////////////////
class OpenGlTextureCreator : public Gre::TextureInternalCreator
{
public:

    POOLED ( Gre::Pools::Manager )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlTextureCreator ( const Gre::Renderer* renderer ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlTextureCreator () ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Loads a Texture from a SoftwarePixelBuffer.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::Texture* load (const std::string & name ,
                                  const Gre::SoftwarePixelBufferHolderList & buffers ,
                                  const Gre::TextureType & type ,
                                  const Gre::ResourceLoaderOptions & ops ) const ;

protected:

    /// @brief
    const Gre::Renderer* iRenderer ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Shader.
//////////////////////////////////////////////////////////////////////
class OpenGlShader : public Gre::HardwareShader
{
public:

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlShader ( const std::string & name , const Gre::ShaderType& type , const std::string & src ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlShader () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the errors logs when compiled.
    //////////////////////////////////////////////////////////////////////
    virtual const std::string& getErrorLog () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the gl shader ID .
    //////////////////////////////////////////////////////////////////////
    virtual GLuint getGlShader () const ;

protected:

    /// @brief
    std::string iErrorLog ;

    /// @brief
    GLuint iGlShader ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Program.
//////////////////////////////////////////////////////////////////////
class OpenGlProgram : public Gre::HardwareProgram
{
public:

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlProgram ( const std::string & name ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlProgram () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Binds the HardwareProgram.
    //////////////////////////////////////////////////////////////////////
    virtual void _bind () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Unbinds the HardwareProgram.
    //////////////////////////////////////////////////////////////////////
    virtual void _unbind () const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Attach the given shader to this Program.
    //////////////////////////////////////////////////////////////////////
    virtual bool _attachShader ( const Gre::HardwareShaderHolder & hwdShader ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Finalize the Program.
    //////////////////////////////////////////////////////////////////////
    virtual bool _finalize() ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Destroys the Program internal object.
    //////////////////////////////////////////////////////////////////////
    virtual void _deleteProgram () ;

public:

    //////////////////////////////////////////////////////////////////////
    /// @brief Looks for the given attribute name and bind it to defined
    /// component , enabling it.
    //////////////////////////////////////////////////////////////////////
    virtual void setVertexAttrib (const std::string & attrib ,
                                  size_t elements ,
                                  Gre::VertexAttribType type ,
                                  bool normalize ,
                                  size_t stride ,
                                  void * pointer) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Should bind the texture unit with given number .
    //////////////////////////////////////////////////////////////////////
    virtual void bindTextureUnit ( int unit ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Internally sets the uniform to the program object.
    //////////////////////////////////////////////////////////////////////
    virtual bool _setUniform ( int location , const Gre::HdwProgVarType & type , const Gre::RealProgramVariable & value ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the maximum number of lights supported by the
    /// program driver.
    //////////////////////////////////////////////////////////////////////
    virtual unsigned int getMaximumLights () const ;

protected:

    /// @brief Holds the OpenGl ID for the program created. If invalid, this
    /// number is 0 .
    GLuint iGlProgram ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl HardwareProgramManagerCreator.
//////////////////////////////////////////////////////////////////////
class OpenGlProgramManagerCreator : public Gre::HardwareProgramManagerInternalCreator
{
public:

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlProgramManagerCreator ( const OpenGlRenderer* renderer ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlProgramManagerCreator () ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Loads a Shader file with name , type and source.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::HardwareShaderHolder loadShader (const Gre::ShaderType & type ,
                                                  const std::string & name ,
                                                  const std::string & source) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Loads a Program with every shaders that may be part of it.
    /// Notes that at least a Vertex Shader and a Fragment Shader should
    /// be present.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::HardwareProgramHolder loadProgram (const std::string & name ,
                                                    const Gre::HardwareShaderHolderList & shaders) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the Compiler String the bundle will use to get a valid
    /// path to the shader file. For example , OpenGl 'GLSL' Compiler has
    /// String Value "GLSL" . Shaders files will so be in folder 'GLSL/'.
    //////////////////////////////////////////////////////////////////////
    virtual const std::string getCompiler () const ;

protected:

    /// @brief
    const OpenGlRenderer* iRenderer ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Index Buffer.
//////////////////////////////////////////////////////////////////////
class OpenGlHardwareIndexBuffer : public Gre::HardwareIndexBuffer
{
public:

    POOLED ( Gre::Pools::HdwBuffer )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlHardwareIndexBuffer ( const void* data , size_t sz , const Gre::IndexDescriptor& desc ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    ~OpenGlHardwareIndexBuffer () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Bind the Hardware Buffer in order to use it.
    //////////////////////////////////////////////////////////////////////
    virtual void bind() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Unbind the Hardware Buffer after it has been used.
    //////////////////////////////////////////////////////////////////////
    virtual void unbind() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Adds some data to this Buffer. Size is the total size in
    /// bytes, not the number of Vertex.
    //////////////////////////////////////////////////////////////////////
    virtual void addData(const char* vdata, size_t sz);

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the data in this SoftwareVertexBuffer.
    //////////////////////////////////////////////////////////////////////
    virtual const char* getData() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clears the data in the buffer.
    //////////////////////////////////////////////////////////////////////
    virtual void clearData();

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the size of the buffer, in bytes.
    //////////////////////////////////////////////////////////////////////
    virtual size_t getSize() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the number of elements in the buffer.
    //////////////////////////////////////////////////////////////////////
    virtual size_t count() const;

protected:

    /// @brief
    GLuint iGlBuffer ;

    /// @brief
    size_t iSize ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Vertex Buffer.
//////////////////////////////////////////////////////////////////////
class OpenGlHardwareVertexBuffer : public Gre::HardwareVertexBuffer
{
public:

    POOLED ( Gre::Pools::HdwBuffer )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlHardwareVertexBuffer ( const void* data , size_t sz , const Gre::VertexDescriptor& desc ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlHardwareVertexBuffer () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Bind the Hardware Buffer in order to use it.
    //////////////////////////////////////////////////////////////////////
    virtual void bind() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Unbind the Hardware Buffer after it has been used.
    //////////////////////////////////////////////////////////////////////
    virtual void unbind() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Adds some data to this Buffer. Size is the total size in
    /// bytes, not the number of Vertex.
    //////////////////////////////////////////////////////////////////////
    virtual void addData(const char* vdata, size_t sz);

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the data in this SoftwareVertexBuffer.
    //////////////////////////////////////////////////////////////////////
    virtual const char* getData() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clears the data in the buffer.
    //////////////////////////////////////////////////////////////////////
    virtual void clearData();

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the size of the buffer, in bytes.
    //////////////////////////////////////////////////////////////////////
    virtual size_t getSize() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the number of elements in the buffer.
    //////////////////////////////////////////////////////////////////////
    virtual size_t count() const;

protected:

    /// @brief
    GLuint iGlBuffer ;

    /// @brief
    size_t iSize ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Gre::MeshManager.
///
/// As the MeshManager takes care of creating buffers for the meshes
/// (and so also for the MeshLoader) , the Renderer used is stored in
/// order to retrieve the associated RenderContext.
///
//////////////////////////////////////////////////////////////////////
class OpenGlMeshManager : public Gre::MeshManager
{
public:

    POOLED ( Gre::Pools::Manager )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlMeshManager ( const OpenGlRenderer* renderer ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlMeshManager () noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a HardwareVertexBuffer with given data.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::HardwareVertexBufferHolder createVertexBuffer ( const void* data , size_t sz , const Gre::VertexDescriptor& desc ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a HardwareIndexBuffer with given data.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::HardwareIndexBufferHolder createIndexBuffer ( const void* data , size_t sz , const Gre::IndexDescriptor& desc ) const  ;
    
protected:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a MeshBinder to use with every newly created mesh.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::MeshBinder * loadBinder () const ;

protected:

    /// @brief
    const OpenGlRenderer* iRenderer ;
};

//////////////////////////////////////////////////////////////////////
/// @brief OpenGl Gre::Renderer implementation.
//////////////////////////////////////////////////////////////////////
class OpenGlRenderer : public Gre::Renderer
{
public:

    POOLED ( Gre::Pools::Render )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlRenderer ( const std::string& name , const Gre::RendererOptions& options ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlRenderer () noexcept ( false ) ;

public:
    
    //////////////////////////////////////////////////////////////////////
    /// @brief Sets only the given region for clearing the buffers. Also
    /// called a Scissor - box .
    //////////////////////////////////////////////////////////////////////
    virtual void setClearRegion ( const Gre::Surface & box ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Sets the current viewport.
    //////////////////////////////////////////////////////////////////////
    virtual void setViewport ( const Gre::Viewport & viewport ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Sets the binded framebuffer color used to clear the color
    /// buffer.
    //////////////////////////////////////////////////////////////////////
    virtual void setClearColor ( const Gre::Color & color ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Sets the default's current framebuffer depth value when the
    /// depth buffer is cleared.
    //////////////////////////////////////////////////////////////////////
    virtual void setClearDepth ( float value ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Clears the binded framebuffer buffers.
    //////////////////////////////////////////////////////////////////////
    virtual void clearBuffers ( const Gre::ClearBuffers & buffers ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Draw the given mesh to the framebuffer currently used.
    ///
    /// @note
    /// This function should not bind vertex shader attributes , as this should
    /// be done in the 'Mesh::use()' function. The node calls the Mesh if it
    /// is considered visible and the mesh sends the attributes data to the
    /// program used by the technique. This allows us to customize easily the
    /// process by making the user able to overwrite 'Mesh::use()' and so
    /// change what is sent to the program.
    ///
    /// @note
    /// Also , this function should check if the mesh is indexed , or if it
    /// has some custom drawing capabilities we can use to draw the mesh
    /// quicker. Use the right function in the right situation.
    ///
    //////////////////////////////////////////////////////////////////////
    virtual void drawMesh ( const Gre::MeshHolder & mesh ) const ;

public:

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a MeshManager for this Renderer.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::MeshManagerHolder iCreateMeshManager ( ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a HardwareProgramManager for this Renderer.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::HardwareProgramManagerInternalCreator * iCreateProgramManagerCreator ( ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a TextureManager for this Renderer.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::TextureInternalCreator* iCreateTextureCreator ( ) const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates the Framebuffer internal creator.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::RenderFramebufferInternalCreator* iCreateFramebufferCreator ( ) const ;
};

//////////////////////////////////////////////////////////////////////
/// @brief An OpenGlRenderer Loader.
//////////////////////////////////////////////////////////////////////
class OpenGlRendererLoader : public Gre::RendererLoader
{
public:

    POOLED ( Gre::Pools::Loader )

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    OpenGlRendererLoader () ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~OpenGlRendererLoader () noexcept ( false ) ;

    ////////////////////////////////////////////////////////////////////////
    /// @brief Returns a clone of this object.
    ////////////////////////////////////////////////////////////////////////
    virtual Gre::ResourceLoader* clone() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns false.
    //////////////////////////////////////////////////////////////////////
    virtual bool isLoadable( const std::string& filepath ) const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns 'true' if the given RendererOptions is compatible
    /// with the Renderer loaded by this object.
    //////////////////////////////////////////////////////////////////////
    virtual bool isCompatible ( const Gre::RendererOptions& options ) const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a new Renderer Object.
    //////////////////////////////////////////////////////////////////////
    virtual Gre::RendererHolder load ( const std::string& name , const Gre::RendererOptions& options ) const;
};

#endif /* OpenGlRenderer_h */
