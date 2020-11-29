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

#include "option_group.hpp"

#include <fmt/format.h>

#include "alias_item.hpp"
#include "command_item.hpp"
#include "error.hpp"
#include "positional_item.hpp"
#include "text_item.hpp"

namespace argparser {

OptionGroup::OptionGroup() :
  m_items(),
  m_short_options(),
  m_long_options(),
  m_positionals(),
  m_rest(nullptr)
{
}

void
OptionGroup::add_group(std::string name)
{
  add_newline();
  m_items.push_back(std::make_unique<TextItem>(std::move(name)));
}

void
OptionGroup::add_text(std::string text)
{
  m_items.push_back(std::make_unique<TextItem>(std::move(text)));
}

void
OptionGroup::add_newline()
{
  m_items.push_back(std::make_unique<TextItem>(""));
}

void
OptionGroup::add_pseudo(std::string name, std::string help)
{
  m_items.push_back(std::make_unique<PseudoItem>(std::move(name), std::move(help)));
}

Option&
OptionGroup::add_option(std::unique_ptr<Option> option)
{
  if (option->get_short_name()) {
    add_short_option(*option->get_short_name(), *option);
  }

  if (option->get_long_name()) {
    add_long_option(*option->get_long_name(), *option);
  }

  Option& option_ref = *option;
  m_items.push_back(std::move(option));
  return option_ref;
}

CommandItem&
OptionGroup::add_command(std::string name, std::string help)
{
  for (auto const& item : m_items) {
    if (dynamic_cast<PositionalItem*>(item.get()) != nullptr ||
        dynamic_cast<RestItem*>(item.get()) != nullptr)
    {
      throw Error("can't mix positional arguments with commands");
    }
  }

  auto command_item = std::make_unique<CommandItem>(std::move(name), std::move(help));
  CommandItem& command_item_ref = *command_item;
  m_items.push_back(std::move(command_item));
  return command_item_ref;
}

void
OptionGroup::add_alias(char name, Option const& option)
{
  add_short_option(name, option);
}

void
OptionGroup::add_alias(std::string name, Option const& option)
{
  add_long_option(std::move(name), option);
}

void
OptionGroup::add_short_option(char name, Option const& option)
{
  if (m_short_options.find(name) != m_short_options.end()) {
    throw Error(fmt::format("duplicate short option '{}'", name));
  }

  m_short_options[name] = &option;
}

void
OptionGroup::add_long_option(std::string name, Option const& option)
{
  if (m_long_options.find(name) != m_long_options.end()) {
    throw Error(fmt::format("duplicate long option '{}'", name));
  }

  m_long_options[std::move(name)] = &option;
}

CommandItem const&
OptionGroup::lookup_command(std::string_view name) const
{
  for (auto const& item : m_items) {
    Item* ptr = item.get();
    if (auto* command_item = dynamic_cast<CommandItem*>(ptr)) {
      if (command_item->get_name() == name) {
        return *command_item;
      }
    }
  }
  throw Error(fmt::format("command item '{}' not found", name));
}

PositionalItem const&
OptionGroup::lookup_positional(int i) const
{
  int positional_count = 0;
  for (auto const& item : m_items) {
    Item* ptr = item.get();
    if (auto* positional_item = dynamic_cast<PositionalItem*>(ptr)) {
      if (positional_count == i) {
        return *positional_item;
      }
      positional_count += 1;
    }
  }
  throw Error(fmt::format("positional item {} not found", i));
}

RestItem const&
OptionGroup::lookup_rest() const
{
  assert(m_rest);
  return *m_rest;
}

Option const&
OptionGroup::lookup_short_option(char name) const
{
  if (auto it = m_short_options.find(name); it != m_short_options.end()) {
    return *it->second;
  } else {
    throw Error(fmt::format("short option '{}' not found", name));
  }
}

Option const&
OptionGroup::lookup_long_option(std::string_view name) const
{
  if (auto it = m_long_options.find(std::string(name)); it != m_long_options.end()) {
    return *it->second;
  } else {
    throw Error(fmt::format("long option '{}' not found", name));
  }
}

bool
OptionGroup::has_options() const
{
  return !m_short_options.empty() || !m_long_options.empty();
}

bool
OptionGroup::has_commands() const
{
  for (auto const& item : m_items) {
    if (dynamic_cast<CommandItem*>(item.get()) != nullptr) {
      return true;
    }
  }
  return false;
}

bool
OptionGroup::has_positional() const
{
  return !m_positionals.empty();
}

int
OptionGroup::get_positional_count() const
{
  return static_cast<int>(m_positionals.size());
}

bool
OptionGroup::has_rest() const
{
  return m_rest != nullptr;
}

} // namespace argparser

/* EOF */
