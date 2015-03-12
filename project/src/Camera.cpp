#include "Camera.hpp"

Camera::Camera()
{
    m_phi = 3.14/2.f;
    m_theta = 3.14/2.f;
    m_radius = 10.f;
    compute();
}

Camera::~Camera()
{

}

void Camera::zoom(float factor)
{
   m_radius += factor * m_radius;
   if (m_radius < 0.1)
   {
       m_radius = 10.f;
       o = eye + glm::normalize(o - eye) * m_radius;
   }
   compute();
}

void Camera::turn(float phi, float theta)
{
    m_theta += 1.f * theta;
    m_phi   -= 1.f * phi;
    if (m_phi >= (2 * M_PI) - 0.1 )
    {
        m_phi = 0.00001;
    }
    else if (m_phi <= 0 )
    {
        m_phi = 2 * M_PI - 0.1;
    }
    compute();
}

void Camera::pan(float x, float y)
{
    glm::vec3 up(0.f, m_phi < M_PI ? 1.f : -1.f, 0.f);
    glm::vec3 fwd = glm::normalize(o - eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, up));
    up = glm::normalize(glm::cross(side, fwd));
    o[0] += up[0] * y * m_radius * 2;
    o[1] += up[1] * y * m_radius * 2;
    o[2] += up[2] * y * m_radius * 2;
    o[0] -= side[0] * x * m_radius * 2;
    o[1] -= side[1] * x * m_radius * 2;
    o[2] -= side[2] * x * m_radius * 2;
    compute();
}

// private methods
void Camera::compute()
{
    eye.x = cos(m_theta) * sin(m_phi) * m_radius + o.x;
    eye.y = cos(m_phi) * m_radius + o.y ;
    eye.z = sin(m_theta) * sin(m_phi) * m_radius + o.z;
    up = glm::vec3(0.f, m_phi < M_PI ? 1.f : -1.f, 0.f);
}
