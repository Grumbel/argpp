// ArgParser
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

#include <argparser.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  try
  {
    argparser::ArgParser argp;

    argp.add_usage(argv[0], "bar [FILES]... [BLA]..");

    auto& opts = argp.add_group("Options");
    opts.add_text("Dies und das");
    opts.add_newline();

    //auto& group = argp.add_group("Display Options:");
    //gcmd = add_command("install");
    //group.add_option("+v", "--version", "", "Help text");
    //group.add_option({"+v", "+version", Argument::NONE, "Help text"}).then([]{ .... });
    //group.add_option("-v", "-version", "Help text").store(&var, true);
    //group.add_option("-v", "-version", "Help text").store(&var, true);
    //group.add_option("-v", "-version", Argument<int>("FILE"), "Help text").store(&var);
    //group.add_option("-v", "-version", Argument<int>("FILE"), "Help text").append(&var);
    //group.add_option("-v", "-version", Argument<int>("FILE"), "Help text").on([](ParseContext& ctx, int value){
    //ctx.raise_exception("illegal option");
    //});

    //group.add(Option("-v", "--version").argument(Argument::NONE).help("Help text").store(&var, false));
    //Argument<std::tuple<int, int, std::string>>();
    //Argument<std::vector<int>()

    argp.parse_args(argc, argv);

    return 0;
  }
  catch(const std::exception& err)
  {
    std::cout << "Error: " << err.what() << std::endl;
    return 1;
  }
}

/* EOF */
