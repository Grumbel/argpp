{
  description = "A Command Line Argument Parser for C++";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    strutcpp.url = "github:grumbel/strutcpp";
    strutcpp.inputs.nixpkgs.follows = "nixpkgs";
    strutcpp.inputs.flake-utils.follows = "flake-utils";
    strutcpp.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, strutcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = flake-utils.lib.flattenTree rec {
          argpp = pkgs.stdenv.mkDerivation {
            pname = "argpp";
            version = "2.0.0-beta";
            src = nixpkgs.lib.cleanSource ./.;
            cmakeFlags = [
              "-DBUILD_TESTS=ON"
              "-DWARNINGS=ON"
              "-DWERROR=ON"
            ];
            nativeBuildInputs = [
              pkgs.cmake
            ];
            buildInputs = [
              pkgs.fmt
              pkgs.gtest
              strutcpp.packages.${system}.default
              tinycmmc.packages.${system}.default
            ];
          };
          default = argpp;
        };
      }
    );
}
