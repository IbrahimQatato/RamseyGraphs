#ifndef COLOR_H
#define COLOR_H

#include <istream>
#include <ostream>
#include <string>

enum class Color { Red, Blue };

std::ostream& operator<<(std::ostream& stream, Color color);
std::istream& operator>>(std::istream& stream, Color& color);

#endif
