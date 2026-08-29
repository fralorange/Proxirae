from conan import ConanFile
from conan.tools.files import download, unzip, copy
import os

class WinDivertConan(ConanFile):
	name = "windivert"
	version = "2.2.2"
	license = "LGPL-3.0"
	settings = "os", "arch"
	description = "WinDivert: Windows Packet Divert"
	url = "https://github.com/basil00/WinDivert"
	package_type = "shared-library"

	def source(self):
		zip_name = f"WinDivert-{self.version}-A.zip"
		url = f"{self.url}/releases/download/v{self.version}/{zip_name}"

		download(self, url, zip_name)
		unzip(self, zip_name, "tmp")

	def package(self):
		arch_dir = "x64" if self.settings.arch == "x86_64" else "x86"
		arch_path = os.path.join(self.source_folder, "tmp", f"WinDivert-{self.version}-A", arch_dir)

		include_path = os.path.join(self.source_folder, "tmp", f"WinDivert-{self.version}-A", "include")

		copy(self, "*.h", src=include_path, 
			dst=os.path.join(self.package_folder, "include"))
		copy(self, "*.lib", src=arch_path, 
			dst=os.path.join(self.package_folder, "lib"))
		copy(self, "*.dll", src=arch_path, 
			dst=os.path.join(self.package_folder, "bin"))
		copy(self, "*.sys", src=arch_path, 
			dst=os.path.join(self.package_folder, "bin"))

	def package_info(self):
		self.cpp_info.libs = ["WinDivert"]          
		self.cpp_info.libdirs = ["lib"]               
		self.cpp_info.includedirs = ["include"]
		self.cpp_info.bindirs = ["bin"]