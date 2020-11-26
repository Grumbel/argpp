// ArgParse - A Command Line Argument Parser for C++
// Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_ARGPARSER_ARGUMENT_HPP
#define HEADER_ARGPARSER_ARGUMENT_HPP

#include <string_view>

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

class ArgumentBase
{
public:
  ArgumentBase(std::string name) :
    m_name(name)
  {}
  virtual ~ArgumentBase() {}

  std::string const& get_name() const { return m_name; }

  virtual void call(std::string_view text) = 0;

private:
  std::string m_name;
};

template<typename T>
class Argument : public ArgumentBase
{
public:
  using type = T;

public:
  Argument(std::string name) :
    ArgumentBase(name),
    m_convert_func(from_string<T>),
    m_callback()
  {}

  template<typename F>
  Argument(std::string name, F func) :
    ArgumentBase(name),
    m_convert_func(func)
  {}

  void on(std::function<void (T)> callback) {
    assert(m_callback);
    m_callback = callback;
  }

  void call(std::string_view text) override
  {
    m_callback(m_convert_func(text));
  }

private:
  std::function<T (std::string_view)> m_convert_func;
  std::function<void (T)> m_callback;
};

} // namespace argparser

#endif

/* EOF */
