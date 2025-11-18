#pragma once
#include <mutex>

namespace WO3K {

class Camera {
public:
    static Camera& Instance();

    void SetView(int x, int y, int w, int h);
    void GetView(int& x, int& y, int& w, int& h);

    int X() const { return m_x; }
    int Y() const { return m_y; }
    int W() const { return m_w; }
    int H() const { return m_h; }

private:
    Camera();
    int m_x = 0, m_y = 0, m_w = 1024, m_h = 768;
    std::mutex m_lock;
};

} // namespace WO3K
