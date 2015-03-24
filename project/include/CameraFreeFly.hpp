#pragma once

#include "CameraFreeFly.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "iostream"

struct CameraFreeFly
{
    glm::vec3 m_Position;
    float m_fDistance; // La distance par rapport au centre de la voiture
    float m_fPhi, m_fTheta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

    CameraFreeFly(): m_Position(0, 0, 10), m_fDistance(0.5), m_fPhi(M_PI), m_fTheta(0.f) {
        computeDirectionVectors();
    }

    CameraFreeFly(glm::vec3& pos, float x, float y): m_Position(0.f), m_fPhi(x), m_fTheta(y) {
        computeDirectionVectors();
    }

    void changeDistance(float delta)
    {
        m_fDistance -= delta;
        m_fDistance = glm::max(0.5f, m_fDistance);
        m_fDistance = glm::min(2.f, m_fDistance);
    }

    void moveFront(float delta) {
        m_Position += m_FrontVector * delta;
    }

    void moveLeft(float delta) {
        m_Position += m_LeftVector * delta;
    }

    void rotateLeft(float degrees) {
        m_fPhi += glm::radians(degrees);
        computeDirectionVectors();
    }

    void rotateUp(float degrees) {
        m_fTheta += glm::radians(degrees);
        computeDirectionVectors();
    }

    void computeDirectionVectors() {
        float PIoverTwo = M_PI * 0.5f;
        float cTheta = cos(m_fTheta);
        m_FrontVector = glm::vec3(cTheta * sin(m_fPhi), sin(m_fTheta), cTheta * cos(m_fPhi));
        m_LeftVector = glm::vec3(sin(m_fPhi + PIoverTwo), 0, cos(m_fPhi + PIoverTwo));
        m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
    }
};
