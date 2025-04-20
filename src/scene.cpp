/**
 * @file scene.cpp
 * @author Benjamin von Snarski (@benvonh)
 * @brief Source for scene class implementation except for manager.
 * @version 0.1
 * @date 2025-04-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "dt/scene/view.hpp"
#include "dt/scene/manager.hpp"

namespace dt
{
  namespace scene
  {
    template <View::Direction D>
    void View::Move(const double dt)
    {
      pxr::UsdTimeCode now = Manager::GetTimeCode();
      pxr::UsdTimeCode before = now.GetValue() - dt;
      std::lock_guard<std::mutex> lock(Manager::mutex);

      float pivot;
      pxr::GfVec3d translation;
      this->rotateY.Get(&pivot, before);
      this->translate.Get(&translation, before);

      pivot *= M_PI / 180.f;
      const double s = this->speed * dt;
      pxr::GfVec3d t;

      if constexpr (D == View::Direction::FORWARD)
      {
        t = pxr::GfVec3d(-std::sin(pivot) * s, 0, -std::cos(pivot) * s);
      }
      if constexpr (D == View::Direction::BACKWARD)
      {
        t = pxr::GfVec3d(std::sin(pivot) * s, 0, std::cos(pivot) * s);
      }
      if constexpr (D == View::Direction::LEFT)
      {
        t = pxr::GfVec3d(-std::cos(pivot) * s, 0, std::sin(pivot) * s);
      }
      if constexpr (D == View::Direction::RIGHT)
      {
        t = pxr::GfVec3d(std::cos(pivot) * s, 0, -std::sin(pivot) * s);
      }
      if constexpr (D == View::Direction::UP)
      {
        t = pxr::GfVec3d(0, s, 0);
      }
      if constexpr (D == View::Direction::DOWN)
      {
        t = pxr::GfVec3d(0, -s, 0);
      }

      this->translate.Set(translation + t, now);
    }

    template void View::Move<View::Direction::FORWARD>(const double dt);
    template void View::Move<View::Direction::BACKWARD>(const double dt);
    template void View::Move<View::Direction::LEFT>(const double dt);
    template void View::Move<View::Direction::RIGHT>(const double dt);
    template void View::Move<View::Direction::UP>(const double dt);
    template void View::Move<View::Direction::DOWN>(const double dt);

    void View::Look(const float dx, const float dy, const float dt)
    {
      pxr::UsdTimeCode now = Manager::GetTimeCode();
      pxr::UsdTimeCode before = now.GetValue() - dt;
      std::lock_guard<std::mutex> lock(Manager::mutex);

      float x, y;
      this->rotateX.Get(&x, before);
      this->rotateY.Get(&y, before);

      const float q = -this->sensitivity * dt;
      x += dy * q;
      y += dx * q;

      x = std::clamp(x, -90.f, 90.f);

      this->rotateX.Set(x, now);
      this->rotateY.Set(y, now);
    }
  }
}