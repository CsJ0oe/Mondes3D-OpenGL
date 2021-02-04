
#include "camera.h"

using namespace Eigen;

Camera::Camera()
{
  mViewMatrix.setIdentity();
  setPerspective(M_PI/2,0.1,10000);
  setViewport(0,0);
}

void Camera::lookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
{
  mTarget = target;

  // Create a new coordinate system:
  Vector3f X, Y, Z;
  Z = (position - target).normalized();
  X = up.normalized().cross(Z);
  Y = Z.cross(X);

  // Fill the view matrix
  mViewMatrix(0,0) = X.x();
  mViewMatrix(0,1) = X.y();
  mViewMatrix(0,2) = X.z();
  mViewMatrix(0,3) = -X.dot(position);
  mViewMatrix(1,0) = Y.x();
  mViewMatrix(1,1) = Y.y();
  mViewMatrix(1,2) = Y.z();
  mViewMatrix(1,3) = -Y.dot(position);
  mViewMatrix(2,0) = Z.x();
  mViewMatrix(2,1) = Z.y();
  mViewMatrix(2,2) = Z.z();
  mViewMatrix(2,3) = -Z.dot(position);
  mViewMatrix(3,0) = position.x();
  mViewMatrix(3,1) = position.y();
  mViewMatrix(3,2) = position.z();
  mViewMatrix(3,3) = 1.0f;
}

void Camera::setPerspective(float fovY, float near, float far)
{
  m_fovY = fovY;
  m_near = near;
  m_far = far;
}

void Camera::setViewport(int width, int height)
{
  mVpWidth = width;
  mVpHeight = height;
}

void Camera::zoom(float x)
{
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  mViewMatrix = Affine3f(Translation3f(Vector3f(0,0,x*t.norm())).inverse()) * mViewMatrix;
}

void Camera::rotateAroundTarget(float angle, Vector3f axis)
{
    Vector3f pos = Vector3f(mViewMatrix(3,0),mViewMatrix(3,1),mViewMatrix(3,2));
    Vector3f up  = Vector3f(mViewMatrix(1,0),mViewMatrix(1,1),mViewMatrix(1,2));
    pos = Affine3f(AngleAxisf(-angle, axis)) * (pos-mTarget) + mTarget;
    lookAt(pos, mTarget, up);
}

Camera::~Camera()
{
}

const Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

Matrix4f Camera::projectionMatrix() const
{
  float aspect = float(mVpWidth)/float(mVpHeight);
  float theta = m_fovY*0.5;
  float range = m_far - m_near;
  float invtan = 1./std::tan(theta);

  Matrix4f projMat;
  projMat.setZero();
  projMat(0,0) = invtan / aspect;
  projMat(1,1) = invtan;
  projMat(2,2) = -(m_near + m_far) / range;
  projMat(2,3) = -2 * m_near * m_far / range;
  projMat(3,2) = -1;

  return projMat;
}
