#pragma once
#include <cmath>
namespace my_vec
{
    template<typename T>
    class Vector2
    {
    public:
        Vector2() : x(T{}), y(T{}) {}
        Vector2(T x, T y) : x(x), y(y) {}
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
        Vector2& operator+=(const Vector2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }
        Vector2 operator*(T scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        float vector_length() const {
            return std::sqrt(static_cast<float>(x * x + y * y));
        }
        Vector2<float> normalized() const {
            float len = vector_length();
            if (len > 0) {
                return Vector2<float>(static_cast<float>(x) / len, static_cast<float>(y) / len);
            }
            return Vector2<float>(0, 0);
        }
        T getX() const { return x; }
        T getY() const { return y; }
        void setX(T value) { x = value; }
        void setY(T value) { y = value; }
    private:
        T x, y;
    };
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
    using Vector2u = Vector2<unsigned int>;
    using Vector2d = Vector2<double>;
}  
