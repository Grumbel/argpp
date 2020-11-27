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

#ifndef HEADER_ARGPARSER_POSITIONAL_ITEM_HPP
#define HEADER_ARGPARSER_POSITIONAL_ITEM_HPP

namespace argparser {

class PositionalItem : public Item
{
public:
  PositionalItem() {}

  virtual std::string const& get_name() const = 0;
  virtual std::string const& get_help() const = 0;
  virtual void call(std::string_view text) = 0;
};

template<typename T>
class TPositionalItem : public PositionalItem
{
public:
  TPositionalItem(Argument<T> argument, std::string help) :
    m_argument(argument),
    m_help(std::move(help)),
    m_callback()
  {}

  std::string const& get_name() const override {
    return m_argument.get_name();
  }

  std::string const& get_help() const override {
    return m_help;
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
  std::string m_help;
  std::function<void (T)> m_callback;
};

class RestItem : public Item
{
public:
  RestItem(std::string name) :
    m_name(std::move(name)),
    m_callback()
  {}

  std::string const& get_name() const { return m_name; }
  void call(std::string_view text) {}

private:
  std::string m_name;
  std::function<void (std::string_view)> m_callback;
};

} // namespace argparser

#endif

/* EOF */