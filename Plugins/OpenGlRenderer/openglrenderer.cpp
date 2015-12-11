//
//  openglrenderer.cpp
//  GResource
//
//  Created by Jacques Tronconi on 08/11/2015.
//  Copyright (c) 2015 Atlanti's Corporation. All rights reserved.
//

#include "ResourceManager.h"
#include <OpenGL/gl.h>
#include <OpenGl/gl3.h>
#include <OpenGL/glu.h>
#include <sstream>

using namespace Gre;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

class OpenGlUtils
{
public:
    
    static GLenum PrimitiveTypeToGl (PrimitiveType ptype)
    {
        switch (ptype)
        {
            case PrimitiveType::Points:
                return GL_POINTS;
            case PrimitiveType::Lines:
                return GL_LINES;
            case PrimitiveType::LinesAdjacency:
                return GL_LINES_ADJACENCY_EXT;
            case PrimitiveType::LinesLoop:
                return GL_LINE_LOOP;
            case PrimitiveType::LinesStrip:
                return GL_LINE_STRIP;
            case PrimitiveType::LinesStripAdjacency:
                return GL_LINE_STRIP_ADJACENCY_EXT;
            case PrimitiveType::Patches:
                return GL_POINTS;
            case PrimitiveType::Triangles:
                return GL_TRIANGLES;
            case PrimitiveType::TrianglesAdjacency:
                return GL_TRIANGLES_ADJACENCY_EXT;
            case PrimitiveType::TrianglesFan:
                return GL_TRIANGLE_FAN;
            case PrimitiveType::TrianglesStrip:
                return GL_TRIANGLE_STRIP;
            case PrimitiveType::TrianglesStripAdjacency:
                return GL_TRIANGLE_STRIP_ADJACENCY_EXT;
            default:
                return GL_POINTS;
        }
    }
    
    static GLenum StorageTypeToGl (StorageType stype)
    {
        switch (stype)
        {
            case StorageType::UnsignedByte:
                return GL_UNSIGNED_BYTE;
            case StorageType::UnsignedShort:
                return GL_UNSIGNED_SHORT;
            case StorageType::UnsignedInt:
                return GL_UNSIGNED_INT;
            default:
                return GL_UNSIGNED_INT;
        }
    }
};

class OpenGlVertexBuffer : public HardwareVertexBufferPrivate
{
public:
    
    POOLED(Pools::HwdBuffer)
    
    OpenGlVertexBuffer()
    : _mVertexBufferId(0)
    {
        glGenBuffers(1, &_mVertexBufferId);
    }
    
    ~OpenGlVertexBuffer()
    {
        if(_mVertexBufferId)
        {
            glDeleteBuffers(1, &_mVertexBufferId);
        }
    }
    
    void bind() const
    {
        if(_mVertexBufferId)
            glBindBuffer(GL_ARRAY_BUFFER, _mVertexBufferId);
    }
    
    void unbind() const
    {
        if(_mVertexBufferId)
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    bool isInvalid() const
    {
        return _mVertexBufferId == 0;
    }
    
    void update() const
    {
        if(_mVertexBufferId)
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _mVertexs.size(), _mVertexs.data(), GL_STREAM_DRAW);
            setDirty(false);
        }
    }
    
private:
    
    GLuint _mVertexBufferId;
    
};

class OpenGlIndexBuffer : public HardwareIndexBufferPrivate
{
public:
    
    POOLED(Pools::HwdBuffer)
    
    OpenGlIndexBuffer()
    : _mIndexBufferId(0)
    {
        glGenBuffers(1, &_mIndexBufferId);
    }
    
    OpenGlIndexBuffer(PrimitiveType ptype, StorageType stype)
    : HardwareIndexBufferPrivate(ptype, stype), _mIndexBufferId(0)
    {
        glGenBuffers(1, &_mIndexBufferId);
    }
    
    ~OpenGlIndexBuffer()
    {
        if(_mIndexBufferId)
        {
            glDeleteBuffers(1, &_mIndexBufferId);
        }
    }
    
    void bind() const
    {
        if(_mIndexBufferId)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mIndexBufferId);
    }
    
    void unbind() const
    {
        if(_mIndexBufferId)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    bool isInvalid() const
    {
        return _mIndexBufferId == 0;
    }
    
    void update() const
    {
        if(_mIndexBufferId)
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, getSize(), nullptr, GL_STREAM_DRAW);
            char* ptr = 0;
            for(auto indexedFace : _mFaces.indexedFaces)
            {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) ptr, sizeof(GLint) * indexedFace.indices.size(), (void*) indexedFace.indices.data());
                ptr += sizeof(GLint) * indexedFace.indices.size();
            }
            setDirty(false);
        }
    }
    
private:
    
    GLuint _mIndexBufferId;
};

class OpenGlTexture : public TexturePrivate
{
public:
    
    OpenGlTexture (const std::string& name, const Image& img)
    : TexturePrivate(name, img), _mTextureId(0)
    {
        if(!img.isEmpty())
        {
            const PixelBatch& pbatch = img.getPixelBatch();
            
            glPixelStorei(GL_UNPACK_ROW_LENGTH, pbatch.width);
            glPixelStorei(GL_UNPACK_ALIGNMENT,  pbatch.pixelsAlignment);
            
            glGenTextures(1, &_mTextureId);
            glBindTexture(GL_TEXTURE_2D, _mTextureId);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            if(pbatch.samplesPerPixel == 3 || pbatch.samplesPerPixel == 4)
            {
                glTexImage2D(GL_TEXTURE_2D, 0,
                             pbatch.samplesPerPixel == 4 ? GL_RGBA8 : GL_RGB8,
                             image.getWidth(),
                             image.getHeight(),
                             0,
                             pbatch.samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
                             GL_UNSIGNED_BYTE,
                             img.getPixelBatch().pixels);
            }
            
            glBindTexture(GL_TEXTURE_2D, 0);
            image.unloadCache();
        }
    }
    
    ~OpenGlTexture()
    {
        if(_mTextureId)
        {
            glDeleteTextures(1, &_mTextureId);
        }
    }
    
    void bind() const
    {
        if(_mTextureId) {
            glBindTexture(GL_TEXTURE_2D, _mTextureId);
            setBinded(true);
        }
    }
    
    void unbind() const
    {
        if(_mTextureId) {
            glBindTexture(GL_TEXTURE_2D, 0);
            setBinded(false);
        }
    }
    
    bool isInvalid() const
    {
        return _mTextureId == 0;
    }
    
private:
    
    GLuint _mTextureId;
};

class OpenGlRenderer : public RendererResource
{
private:
    
    GLfloat _mClearColor [4];
    GLfloat _mClearDepth;
    GLint   _gl_major;
    GLint   _gl_minor;
    std::vector<std::string> _extensions; ///< @brief Supported extensions.
    
    bool _suportVbo;
    
public:
    
    OpenGlRenderer (const std::string& name)
    : RendererResource(name)
    {
        _mClearColor[0] = 1.0f;
        _mClearColor[1] = 1.0f;
        _mClearColor[2] = 1.0f;
        _mClearColor[3] = 0.0f;
        _mClearDepth    = 1.0f;
        
        GLint GL_major, GL_minor;

#ifdef GL_VERSION_1_1
        GL_major = 1;
        GL_minor = 1;
#endif
#ifdef GL_VERSION_1_2
        GL_minor = 2;
#endif
#ifdef GL_VERSION_1_3
        GL_minor = 3;
#endif
#ifdef GL_VERSION_1_4
        GL_minor = 4;
#endif
#ifdef GL_VERSION_1_5
        GL_minor = 5;
#endif
#ifdef GL_VERSION_2_0
        GL_major = 2;
        GL_minor = 0;
#endif
#ifdef GL_VERSION_2_1
        GL_minor = 1;
#endif
#ifdef GL_VERSION_3_0
        GL_major = 3;
        GL_minor = 0;
#endif
#ifdef GL_VERSION_3_1
        GL_minor = 1;
#endif
#ifdef GL_VERSION_3_2
        GL_minor = 2;
#endif
#ifdef GL_VERSION_3_3
        GL_minor = 3;
#endif
#ifdef GL_VERSION_4_0
        GL_major = 4;
        GL_minor = 0;
#endif
#ifdef GL_VERSION_4_1
        GL_minor = 1;
#endif
        
        _gl_major = GL_major;
        _gl_minor = GL_minor;
        
        initExtensions();
        
        char* GL_version  = (char*) glGetString(GL_VERSION);
        char* GL_vendor   = (char*) glGetString(GL_VENDOR);
        char* GL_renderer = (char*) glGetString(GL_RENDERER);
        
        std::cout << "[OpenGl] Version  : " << GL_version << std::endl;
        std::cout << "[OpenGl] Vendor   : " << GL_vendor << std::endl;
        std::cout << "[OpenGl] Renderer : " << GL_renderer << std::endl;
        std::cout << "[OpenGl] Num Ext  : " << _extensions.size() << std::endl;
        
        _suportVbo = hasExtension("GL_ARB_vertex_buffer_object");
        if(!_suportVbo) {
            std::cout << "[OpenGl] Warning : Current Hardware does not support Vertex Buffer Objects ! This can be followed by very bad performances." << std::endl;
        }
    }
    
    ~OpenGlRenderer ()
    {
        
    }
    
    void initExtensions()
    {
        char* exts = (char*) glGetString(GL_EXTENSIONS);
        _extensions = split(std::string(exts), ' ');
    }
    
    bool hasExtension(const std::string& ext) const
    {
        return std::find(_extensions.begin(), _extensions.end(), ext) != _extensions.end();
    }
    
    void setClearColor (const Color& color)
    {
        _mClearColor[0] = color.getRed();
        _mClearColor[1] = color.getGreen();
        _mClearColor[2] = color.getBlue();
        _mClearColor[3] = color.getAlpha();
        glClearColor(_mClearColor[0], _mClearColor[1], _mClearColor[2], _mClearColor[3]);
    }
    
    void setClearDepth (float depth)
    {
        _mClearDepth = depth;
        glClearDepth(_mClearDepth);
    }
    
protected:
    void _preRender()
    {
        // [TODO] Handle resizing in a separate method.
        WindowSize sz = _window.getWindowSize();
        GLsizei width = sz.first;
        GLsizei height = sz.second;
        
        // Set the viewport.
        glViewport(0, 0, width, height);
        
        // Set the Projection Matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        // Calculate aspect ratio of the window
        gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
        
        // Reset ModelView Matrix.
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // --------------------------------------------
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
        glLoadIdentity();									// Reset The Current Modelview Matrix
        glEnable(GL_TEXTURE_2D);
    }
    
    void _render ()
    {
        glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
        glBegin(GL_TRIANGLES);								// Drawing Using Triangles
            glVertex3f( 0.0f, 1.0f, 0.0f);					// Top
            glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
            glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
        glEnd();											// Finished Drawing The Triangle
        glTranslatef(3.0f,0.0f,0.0f);						// Move Right 3 Units
        glBegin(GL_QUADS);									// Draw A Quad
            glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
            glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
            glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
            glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
        glEnd();
    }
    
    void _postRender()
    {
        glFlush();
    }
    
public:
    
    void translate(float x, float y, float z)
    {
        glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    }
    
    void rotate(float angle, float x, float y, float z)
    {
        glRotatef(angle, x, y, z);
    }
    
    void drawTriangle(float sz, const Color& color1, const Color& color2, const Color& color3)
    {
        glBegin(GL_TRIANGLES);								// Drawing Using Triangles
            glColor4fv(color1.getData());
            glVertex3f( 0.0f, sz, 0.0f);					// Top
            glColor4fv(color2.getData());
            glVertex3f(-sz,  -sz, 0.0f);					// Bottom Left
            glColor4fv(color3.getData());
            glVertex3f( sz,  -sz, 0.0f);					// Bottom Right
        glEnd();											// Finished Drawing The Triangle
    }
    
    void drawQuad(float sz, const Color& color1, const Color& color2, const Color& color3, const Color& color4)
    {
        glBegin(GL_QUADS);                                  // Draw A Quad
            glColor3f(color1.getRed(), 0.0f, 0.0f);
            glVertex3f(-sz,  sz, 0.0f);                     // Top Left
            glColor3f(color2.getRed(), 0.0f, 0.0f);
            glVertex3f( sz,  sz, 0.0f);                     // Top Right
            glColor3f(color3.getRed(), 0.0f, 0.0f);
            glVertex3f( sz, -sz, 0.0f);                     // Bottom Right
            glColor3f(color4.getRed(), 0.0f, 0.0f);
            glVertex3f(-sz, -sz, 0.0f);                     // Bottom Left
        glEnd();
    }
    
    void draw(const Mesh& mesh)
    {
        if(mesh.getType() == MeshPrivate::Type::Buffered)
        {
            auto vbuf = mesh.getVertexBuffer();
            vbuf.bind();
            
            if(vbuf.isDirty())
            {
                vbuf.update();
            }
            
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char*) NULL);
            
            if(vbuf.isColorActivated())
            {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(4, GL_FLOAT, sizeof(Vertex), (char*) (sizeof(Vector3) + sizeof(Vector2)));
            }
            
            for(auto indexbuf : mesh.getIndexBufferBatch().batchs)
            {
                indexbuf.bind();
                
                if(indexbuf.isDirty())
                {
                    indexbuf.update();
                }
                
                prepareMaterial(indexbuf.getMaterial());
                if(indexbuf.getMaterial().texture.isBinded())
                {
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (char*) (sizeof(Vector3)));
                }
                
                GLenum mode = OpenGlUtils::PrimitiveTypeToGl(indexbuf.getPrimitiveType());
                GLenum stype = OpenGlUtils::StorageTypeToGl(indexbuf.getStorageType());
                GLsizei sz = (GLsizei) indexbuf.count();
                glDrawElements(mode, sz, stype, 0);
                
                if(indexbuf.getMaterial().texture.isBinded())
                {
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    indexbuf.getMaterial().texture.unbind();
                }
                
                indexbuf.unbind();
            }
            
            if(vbuf.isColorActivated())
                glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            vbuf.unbind();
        }
    }
    
    void prepareMaterial(const Material& mat)
    {
        if(mat.texture.isInvalid())
        {
            
        }
        
        else
        {
            mat.texture.bind();
        }
    }
    
    void renderExample ()
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_ALWAYS);
        
        glClearColor(_mClearColor[0], _mClearColor[1], _mClearColor[2], _mClearColor[3]);
        glClearDepth(_mClearDepth);
        glClear(GL_COLOR_BUFFER_BIT);
        
        WindowSize sz = _window.getWindowSize();
        
        glViewport(0,0,sz.first,sz.second);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,(float)sz.first-1,(float)sz.second-1,0,-1,1);
        
        int x1,y1,x2,y2;
        int r,g,b;
        
        x1=rand()%sz.first;
        y1=rand()%sz.second;
        x2=rand()%sz.first;
        y2=rand()%sz.second;
        r=rand()%256;
        g=rand()%256;
        b=rand()%256;
        
        glColor3ub(r,g,b);
        glBegin(GL_LINES);
        glVertex2i(x1,y1);
        glVertex2i(x2,y2);
        glEnd();
        glFlush();
    }
    
    HardwareVertexBuffer createVertexBuffer()
    {
        std::string hwdname = ResourceManager::Get().getNameGenerator().generateName("HardwareVertexBuffer");
        HardwareVertexBuffer oglbuffer = HardwareVertexBuffer(ResourceManager::Get().addResource(Resource::Type::HwdBuffer, hwdname, std::make_shared<OpenGlVertexBuffer>()));
        return oglbuffer;
    }
    
    HardwareIndexBuffer  createIndexBuffer(PrimitiveType ptype, StorageType stype)
    {
        std::string hwdname = ResourceManager::Get().getNameGenerator().generateName("HardwareIndexBuffer");
        HardwareIndexBuffer oglbuffer = HardwareIndexBuffer(ResourceManager::Get().addResource(Resource::Type::HwdBuffer, hwdname, std::make_shared<OpenGlIndexBuffer>(ptype, stype)));
        return oglbuffer;
    }
    
    Texture createTexture(const std::string& name, const std::string& file)
    {
        auto loader = ResourceManager::Get().getImageLoaderFactory().findBestLoader(file);
        if(loader == nullptr)
            return Texture::Null;
        
        std::string imgname = ResourceManager::Get().getNameGenerator().generateName(name + "-img");
        Image img = Image(ResourceManager::Get().loadResourceWith(loader, Resource::Type::Image, imgname, file));
        if(img.expired())
            return Texture::Null;
        
        std::string texname = ResourceManager::Get().getNameGenerator().generateName(name);
        Texture texture = Texture(ResourceManager::Get().addResource(Resource::Type::Texture, texname, std::make_shared<OpenGlTexture>(texname, img)));
        return texture;
    }
    
private:
    
    
};

class OpenGlRendererLoader : public RendererLoader
{
public:
    
    OpenGlRendererLoader () { }
    
    ResourceLoader* clone() const
    { return (ResourceLoader*) new OpenGlRendererLoader; }
    
    Resource* load (Resource::Type type, const std::string& name) const
    {
        return dynamic_cast<Resource*>(new OpenGlRenderer(name));
    }
};

extern "C" DLL_PUBLIC void* GetPluginName (void)
{
    return (void*) "OpenGl Renderer";
}

extern "C" DLL_PUBLIC void StartPlugin (void)
{
    ResourceManager::Get().getRendererLoaderFactory().registers("OpenGlLoader", new OpenGlRendererLoader);
}

extern "C" DLL_PUBLIC void StopPlugin (void)
{
    
}
