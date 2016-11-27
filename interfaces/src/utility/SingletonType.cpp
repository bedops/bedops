#include <cstdlib>

namespace Ext {

template <typename T>
T* SingletonType<T>::_instance = 0;

template <typename T>
void SingletonType<T>::Cleanup() { delete _instance; _instance = 0; }

template <typename T>
SingletonType<T>::SingletonType() { std::atexit(&Cleanup); }

} // namespace Ext
