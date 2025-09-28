{
  description = "Lumina - A toy game engine with Vulkan and GLFW";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # Platform-specific libraries
        linuxLibs = with pkgs; [
          xorg.libX11
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXi
          xorg.libXxf86vm
          libGL
        ];

        darwinLibs =
          with pkgs;
          lib.optionals stdenv.isDarwin [
            darwin.apple_sdk.frameworks.OpenGL
            darwin.apple_sdk.frameworks.Cocoa
            darwin.apple_sdk.frameworks.IOKit
            darwin.apple_sdk.frameworks.CoreVideo
          ];

        buildInputs =
          with pkgs;
          [
            glfw
            vulkan-loader
            vulkan-headers
            vulkan-validation-layers
          ]
          ++ (if stdenv.isLinux then linuxLibs else darwinLibs);

        nativeBuildInputs = with pkgs; [
          gcc
          gnumake
          pkg-config
        ];

      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "lumina";
          version = "0.1.0";

          src = ./.;

          inherit buildInputs nativeBuildInputs;

          # Set up Vulkan environment
          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";

          # Configure environment for building
          preBuild = ''
            export PKG_CONFIG_PATH="${pkgs.glfw}/lib/pkgconfig:$PKG_CONFIG_PATH"
            export VULKAN_SDK="${pkgs.vulkan-headers}"
          '';

          # Use the existing Makefile
          buildPhase = ''
            runHook preBuild
            make all
            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall

            # Create output directories
            mkdir -p $out/bin
            mkdir -p $out/lib
            mkdir -p $out/include

            # Install the game executable
            cp build/game $out/bin/lumina

            # Install the shared library
            cp build/liblumina.so $out/lib/

            # Install headers
            cp -r lumina/*.hpp $out/include/
            mkdir -p $out/include/util
            cp lumina/util/*.hpp $out/include/util/

            runHook postInstall
          '';

          # Runtime dependencies
          runtimeDependencies = buildInputs;

          meta = with pkgs.lib; {
            description = "A toy game engine with Vulkan and GLFW written for educational purposes";
            homepage = "https://github.com/rithulkamesh/lumina";
            license = licenses.mit; # Adjust if different
            platforms = platforms.unix;
            maintainers = [ ];
          };
        };

        # Development shell
        devShells.default = pkgs.mkShell {
          inherit buildInputs nativeBuildInputs;

          # Set up Vulkan environment for development
          VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          VULKAN_SDK = "${pkgs.vulkan-headers}";

          shellHook = ''
            echo "🌟 Lumina development environment"
            echo "Vulkan SDK: $VULKAN_SDK"
            echo "Validation layers: $VK_LAYER_PATH"
            echo ""
            echo "Available commands:"
            echo "  make all    - Build the project"
            echo "  make run    - Build and run the demo"
            echo "  make clean  - Clean build artifacts"
            echo ""

            # Verify Vulkan is available
            if command -v vulkaninfo >/dev/null 2>&1; then
              echo "✅ Vulkan is available"
            else
              echo "⚠️  Vulkan tools not found, but libraries are available"
            fi
          '';
        };

        # Additional packages for Vulkan development tools
        packages.dev-tools = pkgs.buildEnv {
          name = "lumina-dev-tools";
          paths = with pkgs; [
            vulkan-tools # vulkaninfo, vkcube, etc.
            renderdoc # Graphics debugging
            gdb # Debugging
            w
            valgrind # Memory debugging
          ];
        };

        # App for easy running
        apps.default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/lumina";
        };
      }
    );
}
