#ifndef WE_ECS_I_SYSTEM_H
#define WE_ECS_I_SYSTEM_H

namespace WE {
class Application;
}

namespace WE::ECS {

class ISystem {

public:
  virtual void Update(float delta_time) = 0;
  virtual ~ISystem() = default;

  ISystem(WE::Application &application) : application_(application) {}

  inline WE::Application &GetApplication() const { return application_; }

private:
  WE::Application &application_;
};
} // namespace WE::ECS

#endif // !WE_ECS_I_SYSTEM_H
