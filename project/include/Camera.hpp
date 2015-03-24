#pragma once
#include "glm/glm.hpp"

class CameraTrackball
{
public:
    CameraTrackball();
    ~CameraTrackball();

    void zoom(float factor);
    void turn(float phi, float theta);
    void pan(float x, float y);

    glm::vec3 o;
    glm::vec3 eye;
    glm::vec3 up;

private:
    float m_radius;
    float m_theta;
    float m_phi;

    void compute();
};
