#pragma once
#include <iostream>

/* https://github.com/kauefraga/cpp-colors/pull/1/changes */
// @brief Namespace for ANSI color codes
namespace cpp_colors
{
  /// @brief A collection of ANSI escape code to control foreground colors.
  namespace foreground {
    constexpr std::string_view black = "\033[30m";
    constexpr std::string_view red = "\033[31m";
    constexpr std::string_view green = "\033[32m";
    constexpr std::string_view yellow = "\033[33m";
    constexpr std::string_view blue = "\033[34m";
  }

  /// @brief A collection of ANSI escape code to apply styles to output text.
  namespace style {
    constexpr std::string_view reset = "\033[0m";
    constexpr std::string_view bold = "\033[1m";
    constexpr std::string_view normal = "\033[22m";
    constexpr std::string_view italic = "\033[3m";
    constexpr std::string_view strikethrough = "\033[9m";
    constexpr std::string_view underline = "\033[4m";

    /// @brief An alias for style::bold.
    constexpr std::string_view b = bold;

    /// @brief An alias for style::italic.
    constexpr std::string_view i = italic;

    /// @brief An alias for style::underline.
    constexpr std::string_view u = underline;

    /// @brief An alias for style::strikethrough.
    constexpr std::string_view st = strikethrough;
  }
}
