/// @file Color.cpp
/// @brief Stream operator definitions for the Color enum.
#include "Color.h"
#include <string>

std::ostream& operator<<(std::ostream& stream, Color color) {
  stream << (color == Color::Red ? COLOR_NAME_RED : COLOR_NAME_BLUE);
  return stream;
}

std::istream& operator>>(std::istream& stream, Color& color) {
  std::string word;
  stream >> word;
  if (word == COLOR_NAME_RED)
    color = Color::Red;
  else if (word == COLOR_NAME_BLUE)
    color = Color::Blue;
  else
    stream.setstate(std::ios::failbit);
  return stream;
}
