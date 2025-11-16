{
  description = "A Command Line Argument Parser for C++";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    strutcpp.url = "github:grumbel/strutcpp";
    strutcpp.inputs.nixpkgs.follows = "nixpkgs";
    strutcpp.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, strutcpp }:
    tinycmmc.lib.eachSystemWithPkgs (pkgs:
      {
        packages = rec {
          default = argpp;
          argpp = pkgs.callPackage ./argpp.nix {
            strutcpp = strutcpp.packages.${pkgs.buildPlatform.system}.default;
            tinycmmc = tinycmmc.packages.${pkgs.buildPlatform.system}.default;
          };
        };
      }
    );
}
