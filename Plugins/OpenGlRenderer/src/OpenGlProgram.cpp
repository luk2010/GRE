//////////////////////////////////////////////////////////////////////
//
//  OpenGlProgram.cpp
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

#include "OpenGlRenderer.h"

GLenum translateGlAttribType ( const Gre::VertexAttribType & type )
{
    if ( type == Gre::VertexAttribType::Byte )
        return GL_BYTE ;
    else if ( type == Gre::VertexAttribType::UnsignedByte )
        return GL_UNSIGNED_BYTE ;
    else if ( type == Gre::VertexAttribType::Short )
        return GL_SHORT ;
    else if ( type == Gre::VertexAttribType::UnsignedShort )
        return GL_UNSIGNED_SHORT ;
    else if ( type == Gre::VertexAttribType::Int )
        return GL_INT ;
    else if ( type == Gre::VertexAttribType::UnsignedInt )
        return GL_UNSIGNED_INT ;
    else if ( type == Gre::VertexAttribType::Float )
        return GL_FLOAT ;
    else if ( type == Gre::VertexAttribType::Double )
        return GL_DOUBLE ;

    else
        return GL_INVALID_ENUM ;
}

Gre::HdwProgVarType translateGlUniformType ( const GLenum & type )
{
    if ( type == GL_FLOAT ) return Gre::HdwProgVarType::Float1 ;
    else if ( type == GL_FLOAT_VEC2 ) return Gre::HdwProgVarType::Float2 ;
    else if ( type == GL_FLOAT_VEC3 ) return Gre::HdwProgVarType::Float3 ;
    else if ( type == GL_FLOAT_VEC4 ) return Gre::HdwProgVarType::Float4 ;

    else if ( type == GL_INT ) return Gre::HdwProgVarType::Int1 ;
    else if ( type == GL_INT_VEC2 ) return Gre::HdwProgVarType::Int2 ;
    else if ( type == GL_INT_VEC3 ) return Gre::HdwProgVarType::Int3 ;
    else if ( type == GL_INT_VEC4 ) return Gre::HdwProgVarType::Int4 ;

    else if ( type == GL_UNSIGNED_INT ) return Gre::HdwProgVarType::UnsignedInt1 ;
    else if ( type == GL_UNSIGNED_INT_VEC2 ) return Gre::HdwProgVarType::UnsignedInt2 ;
    else if ( type == GL_UNSIGNED_INT_VEC3 ) return Gre::HdwProgVarType::UnsignedInt3 ;
    else if ( type == GL_UNSIGNED_INT_VEC4 ) return Gre::HdwProgVarType::UnsignedInt4 ;

    else if ( type == GL_BOOL ) return Gre::HdwProgVarType::Bool1 ;
    else if ( type == GL_BOOL_VEC2 ) return Gre::HdwProgVarType::Bool2 ;
    else if ( type == GL_BOOL_VEC3 ) return Gre::HdwProgVarType::Bool3 ;
    else if ( type == GL_BOOL_VEC4 ) return Gre::HdwProgVarType::Bool4 ;

    else if ( type == GL_FLOAT_MAT2 ) return Gre::HdwProgVarType::Matrix2 ;
    else if ( type == GL_FLOAT_MAT3 ) return Gre::HdwProgVarType::Matrix3 ;
    else if ( type == GL_FLOAT_MAT4 ) return Gre::HdwProgVarType::Matrix4 ;

    else if ( type == GL_SAMPLER_1D || type == GL_SAMPLER_1D_SHADOW || type == GL_INT_SAMPLER_1D || type == GL_UNSIGNED_INT_SAMPLER_1D )
    return Gre::HdwProgVarType::Sampler1D ;

    else if ( type == GL_SAMPLER_2D || type == GL_SAMPLER_2D_SHADOW || type == GL_INT_SAMPLER_2D || type == GL_UNSIGNED_INT_SAMPLER_2D )
    return Gre::HdwProgVarType::Sampler2D ;

    else if ( type == GL_SAMPLER_3D || type == GL_INT_SAMPLER_3D || type == GL_UNSIGNED_INT_SAMPLER_3D )
    return Gre::HdwProgVarType::Sampler3D ;

    else if ( type == GL_SAMPLER_CUBE || type == GL_SAMPLER_CUBE_SHADOW || type == GL_INT_SAMPLER_CUBE || type == GL_UNSIGNED_INT_SAMPLER_CUBE )
    return Gre::HdwProgVarType::SamplerCube ;

    return Gre::HdwProgVarType::None ;
}

// ---------------------------------------------------------------------------

OpenGlProgram::OpenGlProgram ( const std::string & name ) : Gre::HardwareProgram ( name )
{
    CHECK_GLEW

    //////////////////////////////////////////////////////////////////////
    // Tries to create a GL ID for the program object.
    iGlProgram = glCreateProgram () ;
}

OpenGlProgram::~OpenGlProgram() noexcept ( false )
{
    _deleteProgram() ;
}

void OpenGlProgram::_bind() const
{
    GreAutolock ;

    if ( !iBinded )
    {
        glUseProgram(iGlProgram) ;
        iBinded = true ;
    }
}

void OpenGlProgram::_unbind() const
{
    GreAutolock ;

    if ( iBinded )
    {
        glUseProgram(0);
        iBinded = false ;
    }
}

bool OpenGlProgram::_attachShader(const Gre::HardwareShaderHolder & hwdShader)
{
    GreAutolock ;

    //////////////////////////////////////////////////////////////////////
    // Checks given shader , GL program and link status.

    if ( iLinked )
        return false ;

    if ( !iGlProgram )
    {
        iGlProgram = glCreateProgram () ;
        if ( !iGlProgram ) return false ;
    }

    if ( hwdShader.isInvalid() )
        return false ;

    //////////////////////////////////////////////////////////////////////
    // Gets the OpenGlShader , GL id , and calls 'glAttachShader()'.

    const OpenGlShader * glshader = reinterpret_cast<const OpenGlShader*>(hwdShader.getObject()) ;
    GLuint glid = glshader -> getGlShader () ;

    glAttachShader ( iGlProgram , glid ) ;
    GLenum err = glGetError () ;

    if ( err != GL_NO_ERROR )
    {
#ifdef GreIsDebugMode
        GreDebug ( "[WARN] glAttachShader() : '") << getName() << "' : " << translateGlError (err) << Gre::gendl ;
#endif
        return false ;
    }

    return true ;
}

bool OpenGlProgram::_finalize()
{
    GreAutolock ;

    if ( !iGlProgram || iLinked )
        return false ;

    //////////////////////////////////////////////////////////////////////
    // Links the program.

    glLinkProgram(iGlProgram);

    //////////////////////////////////////////////////////////////////////
    // Gets errors from the linking process.

    GLint status ; glGetProgramiv(iGlProgram, GL_LINK_STATUS, &status) ;

    if ( status == GL_FALSE ) {

#ifdef GreIsDebugMode
        GLint infosz ; glGetProgramiv(iGlProgram, GL_INFO_LOG_LENGTH, &infosz) ;
        GLsizei lenght ;
        GLchar* buf = (GLchar*) malloc ( infosz + 1 ) ;
        glGetProgramInfoLog(iGlProgram, infosz, &lenght, buf) ;
        buf[lenght] = '\0' ;

        GreDebug("[WARN] glLinkProgram() : '") << getName() << "' : " << std::string(buf) << Gre::gendl ;
#endif

        return false ;
    }

    //////////////////////////////////////////////////////////////////////
    // Successfully linked.

    iLinked = true ;

    //////////////////////////////////////////////////////////////////////
    // Tries to iterate through the program uniforms.

    GLint uniformcount = 0 ;
    GLint uniformmaxlenght = 0 ;

    glGetProgramiv(iGlProgram, GL_ACTIVE_UNIFORMS, &uniformcount) ;
    GreDebug("[INFO] glGetProgramiv() : Found ") << uniformcount << " uniforms." << Gre::gendl ;

    if ( uniformcount )
    {
        glGetProgramiv(iGlProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformmaxlenght) ;
        // GreDebug("[INFO] glGetProgramiv() : Uniform Max Lenght = ") << uniformmaxlenght << Gre::gendl ;

        if ( uniformmaxlenght )
        {
            for ( int i = 0 ; i < uniformcount ; ++i )
            {
                char* buf = (char*) malloc ( uniformmaxlenght+1 ) ;
                GLsizei lenght = 0 ; GLint uniformsize ; GLenum uniformtype ;

                glGetActiveUniform(iGlProgram, i,
                                   uniformmaxlenght, &lenght,
                                   &uniformsize, &uniformtype,
                                   buf);

                buf [lenght] = '\0' ;

                GLint location = glGetUniformLocation(iGlProgram, buf) ;

                // GreDebug("[INFO] glGetActiveUniformName() : Uniform name = '") << std::string(buf) << "'." << Gre::gendl ;
                // GreDebug("[INFO] glGetUniformLocation() : Uniform location = ") << location << "." << Gre::gendl ;

                //////////////////////////////////////////////////////////////////////
                // We can save the uniform for saving time on 'glGetUniformLocation'.
                // We don't need any other informations for now on the uniform.

                Gre::HardwareProgramVariable var ;
                var.name = std::string(buf) ;
                var.location = location ;
                var.type = translateGlUniformType ( uniformtype ) ;
                iUniforms [std::string(buf)] = var ;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Tries to iterate through Attributes.

    GLint attrcount = 0 ;
    GLint attrmaxlenght = 0 ;

    glGetProgramiv(iGlProgram, GL_ACTIVE_ATTRIBUTES, &attrcount) ;
    GreDebug("[INFO] glGetProgramiv() : Found ") << attrcount << " attributes." << Gre::gendl ;

    if ( attrcount )
    {
        glGetProgramiv(iGlProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrmaxlenght) ;
        // GreDebug("[INFO] glGetProgramiv() : AttrMaxLenght = ") << attrmaxlenght << Gre::gendl ;

        if ( attrmaxlenght )
        {
            for ( int i = 0 ; i < attrcount ; ++i )
            {
                char* buf = (char*) malloc ( attrmaxlenght+1 ) ;
                GLsizei lenght = 0 ;

                glGetActiveAttrib(iGlProgram, i, attrmaxlenght, &lenght, NULL, NULL, buf) ;
                buf [lenght] = '\0' ;

                GLint location = glGetAttribLocation(iGlProgram, buf) ;

                // GreDebug("ActivAttrib = '") << std::string(buf) << "'." << Gre::gendl ;
                // GreDebug("Location = ") << location << Gre::gendl ;

                iAttribsLocation [std::string(buf)] = location ;
            }
        }
    }

    return iLinked ;
}

void OpenGlProgram::_deleteProgram()
{
    GreAutolock ;
    if ( !iGlProgram )
        return ;

    glDeleteProgram(iGlProgram) ;
    iGlProgram = 0 ;
    iLinked = false ;
    iBinded = false ;
}

bool OpenGlProgram::binded () const
{
    return iBinded ;
}

void OpenGlProgram::setVertexAttrib(const std::string &attrib, size_t elements, Gre::VertexAttribType type, bool normalize, size_t stride, void *pointer) const
{
    //////////////////////////////////////////////////////////////////////
    // Try to find the attribute in lookup table.

    auto it = iAttribsLocation.find(attrib) ;

    if ( it != iAttribsLocation.end() )
    {
        int loc = it->second ;

        if ( loc >= 0 )
        {
            //////////////////////////////////////////////////////////////////////
            // Enables Vertex Attribute and points the correct elements. Notes that
            // a Vertex Array Object should be binded when using this function.

            glEnableVertexAttribArray(loc) ;

            if ( type == Gre::VertexAttribType::Int )
            {
                glVertexAttribIPointer(loc, elements,
                                       translateGlAttribType(type),
                                       stride, pointer);
            }

            else
            {
                glVertexAttribPointer(loc, elements,
                                      translateGlAttribType(type),
                                      normalize,
                                      stride, pointer);
            }
        }
    }
}

void OpenGlProgram::disableVertexAttribs () const
{
    GreAutolock ;

    if ( !iGlProgram || !binded() )
    return ;

    //////////////////////////////////////////////////////////////////////
    // Loop through each attributes to disable it.

    for ( auto it : iAttribsLocation )
    glDisableVertexAttribArray ( it.second ) ;
}

void OpenGlProgram::bindTextureUnit(int unit) const
{
    GreAutolock ;

    if ( iGlProgram ) {
        glActiveTexture(GL_TEXTURE0+unit);
    }
}

bool OpenGlProgram::_setUniform(int location, const Gre::HdwProgVarType &type, const Gre::RealProgramVariable &value) const
{
    GreAutolock ;

    if ( iGlProgram )
    {

        if ( location < 0 ) {
            return false ;
        }

        if ( type == Gre::HdwProgVarType::Float1 ) {
            glUniform1fv(location, 1, & value.f1);
        }

        else if ( type == Gre::HdwProgVarType::Float2 ) {
            glUniform2fv(location, 1, glm::value_ptr( value.f2 ) );
        }

        else if ( type == Gre::HdwProgVarType::Float3 ) {
            glUniform3fv(location, 1, glm::value_ptr( value.f3 ) );
        }

        else if ( type == Gre::HdwProgVarType::Float4 ) {
            glUniform4fv(location, 1, glm::value_ptr( value.f4 ) );
        }

        else if ( type == Gre::HdwProgVarType::Int1 ) {
            glUniform1iv(location, 1, & value.i1);
        }

        else if ( type == Gre::HdwProgVarType::Int2 ) {
            glUniform2iv(location, 1, & value.i2[0]);
        }

        else if ( type == Gre::HdwProgVarType::Int3 ) {
            glUniform3iv(location, 1, & value.i3[0]);
        }

        else if ( type == Gre::HdwProgVarType::Int4 ) {
            glUniform4iv(location, 1, & value.i4[0]);
        }

        else if ( type == Gre::HdwProgVarType::UnsignedInt1 )
        glUniform1uiv ( location , 1 , & value.u1 ) ;
        else if ( type == Gre::HdwProgVarType::UnsignedInt2 )
        glUniform2uiv ( location , 1 , glm::value_ptr(value.u2) ) ;
        else if ( type == Gre::HdwProgVarType::UnsignedInt3 )
        glUniform3uiv ( location , 1 , glm::value_ptr(value.u3) ) ;
        else if ( type == Gre::HdwProgVarType::UnsignedInt4 )
        glUniform4uiv ( location , 1 , glm::value_ptr(value.u4) ) ;

        else if ( type == Gre::HdwProgVarType::Bool1 )
        glUniform1i ( location , value.b1 ) ;
        else if ( type == Gre::HdwProgVarType::Bool2 )
        glUniform2i ( location , value.b2[0] , value.b2[1] ) ;
        else if ( type == Gre::HdwProgVarType::Bool3 )
        glUniform3i ( location , value.b3[0] , value.b3[1] , value.b3[2] ) ;
        else if ( type == Gre::HdwProgVarType::Bool4 )
        glUniform4i ( location , value.b4[0] , value.b4[1] , value.b4[2] , value.b4[3] ) ;

        else if ( type == Gre::HdwProgVarType::Matrix2 ) {
            glUniformMatrix2fv(location, 1, false, glm::value_ptr( value.m2 ) );
        }

        else if ( type == Gre::HdwProgVarType::Matrix3 ) {
            glUniformMatrix3fv(location, 1, false, glm::value_ptr( value.m3) );
        }

        else if ( type == Gre::HdwProgVarType::Matrix4 ) {
            glUniformMatrix4fv(location, 1, false, glm::value_ptr( value.m4 ) );
        }

        else if ( type == Gre::HdwProgVarType::Sampler1D || type == Gre::HdwProgVarType::Sampler2D ||
                  type == Gre::HdwProgVarType::Sampler3D || type == Gre::HdwProgVarType::SamplerCube )
        glUniform1iv ( location , 1 , & value.i1 ) ;

        GLenum err = glGetError() ;
        if ( err == GL_NONE ) return true ;

    }

    return false ;
}

unsigned int OpenGlProgram::getMaximumLights() const
{
    return 10 ;
}
