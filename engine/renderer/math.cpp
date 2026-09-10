#include "math.hpp"
#include <algorithm>
namespace gta4web::math {
Mat4 Mat4::identity(){ Mat4 r{}; r.m[0]=r.m[5]=r.m[10]=r.m[15]=1.0f; return r; }
Mat4 Mat4::perspective(float fov,float aspect,float near_p,float far_p){ const float f=1.0f/std::tan(fov*0.5f); const float a=std::max(aspect,0.001f); Mat4 r{}; r.m[0]=f/a; r.m[5]=f; r.m[10]=(far_p+near_p)/(near_p-far_p); r.m[11]=-1.0f; r.m[14]=2.0f*far_p*near_p/(near_p-far_p); return r; }
Mat4 Mat4::translation(Vec3 v){ Mat4 r=identity(); r.m[12]=v.x; r.m[13]=v.y; r.m[14]=v.z; return r; }
Mat4 Mat4::rotation_x(float a){ const float c=std::cos(a),s=std::sin(a); Mat4 r=identity(); r.m[5]=c;r.m[6]=s;r.m[9]=-s;r.m[10]=c;return r; }
Mat4 Mat4::rotation_y(float a){ const float c=std::cos(a),s=std::sin(a); Mat4 r=identity(); r.m[0]=c;r.m[2]=-s;r.m[8]=s;r.m[10]=c;return r; }
Mat4 Mat4::rotation_z(float a){ const float c=std::cos(a),s=std::sin(a); Mat4 r=identity(); r.m[0]=c;r.m[1]=s;r.m[4]=-s;r.m[5]=c;return r; }
Mat4 Mat4::scale(Vec3 v){ Mat4 r{}; r.m[0]=v.x;r.m[5]=v.y;r.m[10]=v.z;r.m[15]=1.0f;return r; }
Mat4 Mat4::look_at(Vec3 eye,Vec3 target,Vec3 up){ const Vec3 f=normalize(target-eye),r=normalize(cross(f,up)),u=cross(r,f); Mat4 m=identity(); m.m[0]=r.x;m.m[4]=r.y;m.m[8]=r.z;m.m[1]=u.x;m.m[5]=u.y;m.m[9]=u.z;m.m[2]=-f.x;m.m[6]=-f.y;m.m[10]=-f.z;m.m[12]=-dot(r,eye);m.m[13]=-dot(u,eye);m.m[14]=dot(f,eye);return m; }
Mat4 multiply(const Mat4&a,const Mat4&b){ Mat4 r{}; for(int c=0;c<4;++c)for(int row=0;row<4;++row){float v=0;for(int k=0;k<4;++k)v+=a.m[k*4+row]*b.m[c*4+k];r.m[c*4+row]=v;}return r; }
}
