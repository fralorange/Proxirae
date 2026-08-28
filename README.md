# Proxirae
A Windows proxy client for the SOCKS5 and HTTP protocols, providing rule-based routing for TCP and UDP traffic

[TODO: change to Resources that leads to docs]: # 
## Build from Source

### Prerequisites

- Visual Studio 2022 with C++ desktop development tools
- Conan 2.31.2 or later

### Build

1. Install Conan.

2. Create the local WinDivert package:

   ```bash
   cd deps/windivert
   conan create .
   ```

3. Install the project dependencies:

	```bash
	cd ../..
	conan install . -s build_type=Debug -s arch=x86_64 -s compiler.cppstd=20 --build=missing
	conan install . -s build_type=Release -s arch=x86_64 -s compiler.cppstd=20 --build=missing
	```

4. Open Proxirae.sln in Visual Studio and build the solution.