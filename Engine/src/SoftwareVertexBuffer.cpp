//////////////////////////////////////////////////////////////////////
//
//  SoftwareVertexBuffer.cpp
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2016 Luk2010
//  Created on 11/07/2016.
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

#include "SoftwareVertexBuffer.h"

GreBeginNamespace

SoftwareVertexBuffer::SoftwareVertexBuffer(const std::string& name)
: HardwareVertexBuffer(name)
, iVertexData(nullptr)
, iSize(0)
, iBoundingBoxInvalid(false)
{
    
}

SoftwareVertexBuffer::~SoftwareVertexBuffer()
{
    clearData();
}

void SoftwareVertexBuffer::bind() const
{
    
}

void SoftwareVertexBuffer::unbind() const
{
    
}

void SoftwareVertexBuffer::update()
{
    if ( iBoundingBoxInvalid )
    {
        makeBoundingBox();
    }
}

bool SoftwareVertexBuffer::isDataInvalid() const
{
    return !( iVertexData && iSize );
}

void SoftwareVertexBuffer::addData(const char *vdata, size_t sz)
{
    if ( iVertexData )
    {
        char* newdata = (char*) malloc(iSize + sz);
        memcpy(newdata, iVertexData, iSize);
        memcpy(newdata + iSize, vdata, sz);
        
        iSize = iSize + sz;
        iVertexData = newdata;
        iBoundingBoxInvalid = true;
    }
    
    else
    {
        iVertexData = (char*) malloc(sz);
        memcpy(iVertexData, vdata, sz);
        
        iSize = sz;
        iBoundingBoxInvalid = true;
    }
}

const char* SoftwareVertexBuffer::getData() const
{
    return iVertexData;
}

void SoftwareVertexBuffer::clearData()
{
    if ( iVertexData && iSize )
    {
        free(iVertexData);
        iVertexData = nullptr;
        iSize = 0;
        iBoundingBox.clear();
        iBoundingBoxInvalid = false;
    }
    
    else if ( iVertexData )
    {
#ifdef GreIsDebugMode
        GreDebugPretty() << "Freeing 'iVertexData' but 'iSize' is 0." << Gre::gendl;
#endif
        free(iVertexData);
        iVertexData = nullptr;
        iSize = 0;
        iBoundingBox.clear();
        iBoundingBoxInvalid = false;
    }
}

size_t SoftwareVertexBuffer::getSize() const
{
    return iSize;
}

size_t SoftwareVertexBuffer::count() const
{
    if ( iVertexData && iSize )
    {
        return iSize / iComponents.getSize();
    }
    
    else
    {
        return 0;
    }
}

const BoundingBox& SoftwareVertexBuffer::getBoundingBox() const
{
    return iBoundingBox;
}

void SoftwareVertexBuffer::makeBoundingBox()
{
    if ( iBoundingBoxInvalid || iBoundingBox.isInvalid() )
    {
        const VertexDescriptor& desc = getVertexDescriptor();
        iBoundingBox.clear();
        
        if ( desc.getSize() && iVertexData && iSize )
        {
            const VertexAttribComponent position = desc.findComponent ( VertexAttribAlias::Position ) ;
            
            if ( position.size > 0 )
            {
                const char* end = iVertexData + iSize ;
                const char* data = iVertexData + desc.getOffset(position) ;
                size_t stride = desc.getStride(position) ;
                
                do
                {
                    const Vector3& vec = * ( (const Vector3*) data );
                    iBoundingBox.add(vec);
                    data = data + stride;
                    
                } while ( data < end );
            }
            
#ifdef GreIsDebugMode
            else
            {
                GreDebugPretty() << "No 'VertexComponentType::Position' in VertexDescriptor." << Gre::gendl;
            }
#endif
            
        }
        
        iBoundingBoxInvalid = false;
    }
    
#ifdef GreIsDebugMode
    else
    {
        GreDebugPretty() << "No need to update BoundingBox as 'iBoundingBoxInvalid' is false." << Gre::gendl;
    }
#endif
}

void SoftwareVertexBuffer::setData(const HardwareVertexBufferHolder &holder)
{
    clearData();
    
    if ( !holder.isInvalid() )
    {
        setVertexDescriptor(holder->getVertexDescriptor());
        addData(holder->getData(), holder->getSize());
    }
}

void SoftwareVertexBuffer::onUpdateEvent(const Gre::UpdateEvent &e)
{
    HardwareVertexBuffer::onUpdateEvent(e);
    
    if ( iBoundingBoxInvalid )
    {
        makeBoundingBox();
    }
}

GreEndNamespace
