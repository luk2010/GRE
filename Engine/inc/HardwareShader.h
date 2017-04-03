//////////////////////////////////////////////////////////////////////
//
//  HardwareShader.h
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2017 Luk2010
//  Created on 06/01/2016.
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

#ifndef GRE_HardwareShader_h
#define GRE_HardwareShader_h

#include "Pools.h"
#include "Resource.h"
#include "ResourceLoader.h"

GreBeginNamespace

//////////////////////////////////////////////////////////////////////
/// @brief Shader's possibles Types.
//////////////////////////////////////////////////////////////////////
enum class ShaderType
{
    /// @brief A Null Type. This describes an invalid Shader.
    Null,

    /// @brief Shader stage in the rendering pipeline that handles the
    /// processing of individual vertices.
    /// A vertex shader receives a single vertex from the vertex stream and
    /// generates a single vertex to the output vertex stream.
    Vertex,

    /// @brief Shader stage that will processes a Fragment generated by the
    /// Rasterization into a set of colors and a single depth value.
    Fragment
};

//////////////////////////////////////////////////////////////////////
/// @brief A Shader table is a set of pathes to load a particular Shader
/// type. This structure is mainly used by the Technique parser to load
/// shaders in the program manager.
struct ShaderPathTable
{
    /// @brief Shader's type.
    ShaderType type ;

    /// @brief Pathes used by the manager to load one shader. The First
    /// value is the compiler ( "GLSL" , "HLSL" , ... ) that will be recognized
    /// by the manager and then the path. This path is relative to already loaded
    /// bundles.
    std::map < std::string , std::string > pathes ;
};

//////////////////////////////////////////////////////////////////////
/// @brief A list of tables.
struct ShaderPathTableList
{
    /// @brief Tables holded.
    std::vector < ShaderPathTable > tables ;

    /// @brief Adds a list of pathes for given shader type.
    void add ( const ShaderType & type , const std::map < std::string , std::string > & pathes ) ;
};

//////////////////////////////////////////////////////////////////////
/// @brief Represents a Shader interface.
///
/// This Shader class has the only purpose to hold a handle to the
/// specific-shader implementation. The Shader object is created, then
/// a source is linked, and the Shader is then compiled.
///
/// The Shader object is owned by HardwareProgramManager. You can load
/// a Shader using this manager.
///
/// Load a Shader
///  - HardwareProgramManager:: loadShader().
///  - HardwareShader:: setSource().
///  - HardwareShader:: compile().
///
/// setSource() and compile() can be done in HardwareProgramManager::loadShader()
/// if provided right args.
//////////////////////////////////////////////////////////////////////
class DLL_PUBLIC HardwareShader : public Resource
{
public:

    POOLED(Pools::Resource)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    HardwareShader(const std::string& name, const ShaderType& type, const std::string& text);

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~HardwareShader() noexcept ( false ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the Type of this Shader.
    //////////////////////////////////////////////////////////////////////
    virtual ShaderType getType() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the source for this Shader.
    //////////////////////////////////////////////////////////////////////
    virtual const std::string& getSource() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns true if the Shader is successfully compiled.
    //////////////////////////////////////////////////////////////////////
    virtual bool isCompiled() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the origin filepath this HardwareShader was loaded,
    /// if it was provided.
    //////////////////////////////////////////////////////////////////////
    virtual const std::string& getFilepath() const ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the errors logs when compiled.
    //////////////////////////////////////////////////////////////////////
    virtual const std::string& getErrorLog () const = 0 ;

protected:

    /// @brief Holds the Shader's type.
    ShaderType iType;

    /// @brief Holds the source text of this Shader.
    std::string iSource;

    /// @brief Holds the origin Filepath, if provided.
    std::string iFilepath;

    /// @brief Flag 'Is the Shader compiled ?'
    mutable bool iCompiled;
};

/// @brief SpecializedCountedObjectHolder for HardwareShaderPrivate.
typedef SpecializedCountedObjectHolder<HardwareShader> HardwareShaderHolder;

/// @brief SpecializedResourceHolderList for HardwareShaderPrivate.
typedef SpecializedResourceHolderList<HardwareShader> HardwareShaderHolderList;

/// @brief SpecializedCountedObjectUser for HardwareShader.
typedef SpecializedCountedObjectUser<HardwareShader> HardwareShaderUser;

GreEndNamespace

#endif
