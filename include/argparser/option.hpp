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

#ifndef HEADER_ARGPARSER_OPTION_HPP
#define HEADER_ARGPARSER_OPTION_HPP

#include <string>
#include <string_view>

#include "item.hpp"

namespace argparser {

class Option : public Item
{
public:
  Option(char short_name, std::string long_name, std::string help) :
    m_short_name(short_name),
    m_long_name(long_name),
    m_help(help)
  {}

  std::string const& get_help() const { return m_help; }

  char get_short_name() const { return m_short_name; }
  std::string const& get_long_name() const { return m_long_name; }

  virtual bool requires_argument() const = 0;

private:
  char m_short_name;
  std::string m_long_name;
  std::string m_help;
};

class OptionWithoutArg : public Option
{
public:
  OptionWithoutArg(char short_name, std::string long_name, std::string help) :
    Option(short_name, long_name, help),
    m_callback()
  {}

  bool requires_argument() const override { return false; }

  void call() {
    m_callback();
  }

  void then(std::function<void ()> cb) {
    assert(!requires_argument());

    m_callback = cb;
  }

  template<typename T>
  void store(T& place, T const value) {
    assert(!requires_argument());

    then([&place, value = std::move(value)]{
      place = value;
    });
  }

private:
  std::function<void ()> m_callback;
};

class OptionWithArg : public Option
{
public:
  using Option::Option;
  virtual std::string const& get_argument_name() const = 0;
  virtual void call(std::string_view text) = 0;
};

template<typename T>
class TOptionWithArg : public OptionWithArg
{
public:
  TOptionWithArg(char short_name, std::string long_name, Argument<T> argument, std::string help) :
    OptionWithArg(short_name, long_name, help),
    m_argument(std::move(argument)),
    m_callback()
  {}

  bool requires_argument() const override { return true; }

  std::string const& get_argument_name() const override {
    return m_argument.get_name();
  }

  void call(std::string_view text) override {
    if (m_callback) {
      m_callback(m_argument.convert(text));
    }
  }

  template<typename F>
  void then(F func) {
    assert(!m_callback);
    m_callback = func;
  }

private:
  Argument<T> m_argument;
  std::function<void (T)> m_callback;
};

} // namespace argparser

#endif

/* EOF */
