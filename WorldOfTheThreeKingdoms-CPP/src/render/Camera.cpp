#include "Camera.h"

namespace WO3K {

Camera& Camera::Instance() {
    static Camera inst;
    return inst;
}

Camera::Camera() {
}

void Camera::SetView(int x, int y, int w, int h) {
    std::lock_guard<std::mutex> lk(m_lock);
    m_x = x; m_y = y; m_w = w; m_h = h;
}

void Camera::GetView(int& x, int& y, int& w, int& h) {
    std::lock_guard<std::mutex> lk(m_lock);
    x = m_x; y = m_y; w = m_w; h = m_h;
}

} // namespace WO3K
