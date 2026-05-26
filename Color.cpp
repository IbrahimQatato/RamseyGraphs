#include "Color.h"
#include <string>

std::ostream& operator<<(std::ostream& stream, Color color) {
  stream << (color == Color::Red ? "Red" : "Blue");
  return stream;
}

std::istream& operator>>(std::istream& stream, Color& color) {
  std::string word;
  stream >> word;
  if (word == "Red")
    color = Color::Red;
  else if (word == "Blue")
    color = Color::Blue;
  else
    stream.setstate(std::ios::failbit);
  return stream;
}
