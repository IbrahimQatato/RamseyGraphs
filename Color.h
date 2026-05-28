#ifndef COLOR_H
#define COLOR_H

/// @file Color.h
/// @brief Defines the Color enum and its stream operators for graph edge coloring.

#include <istream>
#include <ostream>
#include <string>

/// @brief Represents the two possible colors for Ramsey graph edge coloring.
enum class Color { Red, Blue };

/// @brief Writes "Red" or "Blue" to the stream.
/// @param stream Output stream.
/// @param color Color to write.
/// @return Reference to the stream.
std::ostream& operator<<(std::ostream& stream, Color color);

/// @brief Reads "Red" or "Blue" from the stream. Sets failbit on unknown input.
/// @param stream Input stream.
/// @param color Color to read into.
/// @return Reference to the stream.
std::istream& operator>>(std::istream& stream, Color& color);

#endif
