// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2008-2020 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_ARGPARSER_FROM_STRING_HPP
#define HEADER_ARGPARSER_FROM_STRING_HPP

namespace argparser {

template<typename T> inline
T from_string(std::string_view text)
{
  return T(text);
}

template<> inline
bool from_string<bool>(std::string_view text)
{
  if (text == "0") {
    return false;
  } else {
    return true;
  }
}

template<> inline
int from_string<int>(std::string_view text)
{
  return std::stoi(std::string(text));
}

template<> inline
float from_string<float>(std::string_view text)
{
  return std::stof(std::string(text));
}

} // namespace argparser

#endif

/* EOF */
