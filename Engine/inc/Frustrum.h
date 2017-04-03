//////////////////////////////////////////////////////////////////////
//
//  Frustrum.h
//  This source file is part of Gre
//		(Gang's Resource Engine)
//
//  Copyright (c) 2015 - 2017 Luk2010
//  Created on 05/07/2016.
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

#ifndef Frustrum_h
#define Frustrum_h

#include "Pools.h"
#include "BoundingBox.h"

GreBeginNamespace

////////////////////////////////////////////////////////////////////////
/// @brief Projection Matrix type.
enum class ProjectionType : int
{
    Orthogonal, Perspective
};

//////////////////////////////////////////////////////////////////////
/// @brief Structures representing the 6-planes from the Camera
/// point of view.
///
/// The Camera object should contains and updates one object of this
/// kind. This object can be accessed using Camera::getFrustrum().
///
//////////////////////////////////////////////////////////////////////
class DLL_PUBLIC Frustrum
{
public:

    POOLED(Pools::Default)

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    Frustrum();

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates an Orthogonal Frustrum.
    //////////////////////////////////////////////////////////////////////
    Frustrum ( const OrthogonalVolume & orthovolume , float near , float far ) ;

    //////////////////////////////////////////////////////////////////////
    /// @brief Creates a Perspective Frustrum.
    //////////////////////////////////////////////////////////////////////
    Frustrum ( float fovy , float aspect , float near , float far ) ;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    Frustrum(const Frustrum& rhs);

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    virtual ~Frustrum();

    //////////////////////////////////////////////////////////////////////
    /// @brief Changes the iPerspective Matrix.
    //////////////////////////////////////////////////////////////////////
    virtual void setPerspective(const Matrix4& perspective);

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the iPerspective Matrix.
    //////////////////////////////////////////////////////////////////////
    virtual const Matrix4 & getPerspective() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Changes the View Matrix.
    //////////////////////////////////////////////////////////////////////
    virtual void setView(const Matrix4& view);

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns the View Matrix.
    //////////////////////////////////////////////////////////////////////
    virtual const Matrix4 & getView() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Computes planes from the iPerspective and iView Matrix.
    //////////////////////////////////////////////////////////////////////
    virtual void computePlanes(bool normalize = true);

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns one Plane.
    //////////////////////////////////////////////////////////////////////
    virtual Plane getPlane(int i) const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns every Plane (size = 6).
    //////////////////////////////////////////////////////////////////////
    virtual Plane* getPlanes();

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns every Plane (size = 6).
    //////////////////////////////////////////////////////////////////////
    virtual const Plane* getPlanes() const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns true if the 6 planes have a positive distance with
    /// the point.
    //////////////////////////////////////////////////////////////////////
    virtual bool contains(const Vector3& point) const;

    //////////////////////////////////////////////////////////////////////
    /// @brief Returns an IntersectionResult.
    //////////////////////////////////////////////////////////////////////
    virtual IntersectionResult intersect ( const BoundingBox& bbox ) const;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    virtual const ProjectionType & getProjectionType () const ;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    virtual void setProjectionType ( const ProjectionType& type ) ;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    float getNear () const ;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    void setNear ( float near ) ;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    float getFar () const ;

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    void setFar ( float far ) ;
    
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    float getAspectRatio () const ;
    
    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    void setAspectRatio ( float v ) ;

protected:

    /// @brief The perspective Matrix. ( = Projection matrix )
    Matrix4 iPerspective;

    /// @brief The view Matrix.
    Matrix4 iView;

    /// @brief The ViewProjection Matrix. (note : this property only change when ::computePlanes() is called)
    Matrix4 iViewProjection;

    /// @brief The 6 Planes for the frustrum. (note : this property only change when ::computePlanes() is called)
    Plane iPlanes[6];

    /// @brief The projection type.
    ProjectionType iProjectionType ;

    /// @brief
    float iFovy ;

    /// @brief Aspect ratio (usually 4/3 or 16/9).
    float iAspect ;

    /// @brief Near plane.
    float iNear ;

    /// @brief Far plane.
    float iFar ;

    /// @brief When using Orthogonal projection, this surface will be used
    /// instead of iFovy and iAspect.
    OrthogonalVolume iOrthoSurface ;
};

GreEndNamespace

#endif /* Frustrum_h */
