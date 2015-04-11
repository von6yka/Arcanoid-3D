#include "Engine/pch.h"

#include "Engine/Math/Quat.h"
#include "Engine/Math/Math.h"


namespace engine
{


const Quat Quat::ZERO(0.0, 0.0, 0.0, 0.0);
const Quat Quat::IDENTITY;


Quat Quat::fromAngleAxis(const Vec3& vec3, float angleDegree)
{
    const float halfAngleRadian = Math::toRadian(angleDegree * 0.5f);

    const float sin = std::sin(halfAngleRadian);

    const float x = vec3.x * sin;
    const float y = vec3.y * sin;
    const float z = vec3.z * sin;
    const float w = std::cos(halfAngleRadian);

    return Quat(x, y, z, w);
}


Quat Quat::operator * (const Quat& other) const
{
    return Quat(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y + y * other.w + z * other.x - x * other.z,
        w * other.z + z * other.w + x * other.y - y * other.x,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}


Vec3 Quat::operator * (const Vec3& v) const
{
    const Vec3 qvec(x, y, z);

    Vec3 uv = qvec.crossProduct(v);
    Vec3 uuv = qvec.crossProduct(uv);

    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}


Mat4 Quat::toMat4() const
{
    const float fTx  = x + x;
    const float fTy  = y + y;
    const float fTz  = z + z;
    const float fTxx = fTx * x;
    const float fTxy = fTy * x;
    const float fTxz = fTz * x;
    const float fTyy = fTy * y;
    const float fTyz = fTz * y;
    const float fTzz = fTz * z;
    const float fTwx = fTx * w;
    const float fTwy = fTy * w;
    const float fTwz = fTz * w;

    const float m00 = 1.0f - (fTyy + fTzz);
    const float m01 = fTxy - fTwz;
    const float m02 = fTxz + fTwy;
    const float m10 = fTxy + fTwz;
    const float m11 = 1.0f - (fTxx + fTzz);
    const float m12 = fTyz - fTwx;
    const float m20 = fTxz - fTwy;
    const float m21 = fTyz + fTwx;
    const float m22 = 1.0f - (fTxx + fTyy);

    return Mat4(
        m00, m01, m02, 0,
        m10, m11, m12, 0,
        m20, m21, m22, 0,
        0,   0,   0,   1
    );
}


}//namespace engine
