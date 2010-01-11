#pragma once

#include <pragma/graphics/types.h>

namespace pragma
{

	class Camera
	{
	public:
		typedef matrix4x4<Real> Transform;
	public:
		Camera() { }
		
		void				SetTransform	( const Point& aPosition, const Point& aLookAt, const Vector& aUp );
		void				SetProjection	( Real aFOV, Real aAspectRatio, Real aNearPlane, Real aFarPlane );

		const Point&		GetPosition		() const { return mPosition; }
		const Point&		GetLookAt		() const { return mLookAt; }
		const Vector&		GetUp			() const { return mUp; }

		Real				GetFOV			() const { return mFov; }
		Real				GetAspectRatio	() const { return mAspectRatio; }
		Real				GetNearPlane	() const { return mNearPlane; }
		Real				GetFarPlane		() const { return mFarPlane; }

		const Transform&	GetTransform	() const { return mTransform; }
		const Transform&	GetView			() const { return mView; }
		const Transform&	GetProjection	() const { return mProjection; }
	private:
		Point			mPosition;
		Point			mLookAt;
		Vector			mUp;

		Real			mFov;
		Real			mAspectRatio;
		Real			mNearPlane;
		Real			mFarPlane;

		Transform		mProjection;
		Transform		mView;
		Transform		mTransform;	// Complete matrix transform for this camera (mProjection * mView)
	};

}
