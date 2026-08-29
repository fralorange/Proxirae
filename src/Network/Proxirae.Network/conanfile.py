from conan import ConanFile
from conan.tools.microsoft import MSBuildToolchain, MSBuildDeps, vs_layout

class ProxiraeNetwork(ConanFile):
	name = "Proxirae.Network"
	version = "0.1.0"
	settings = "os", "arch", "compiler", "build_type"

	def requirements(self):
		self.requires("nlohmann_json/3.12.0")

		if self.settings.os == "Windows":
			self.requires("windivert/2.2.2")
		else:
			self.requires("libuuid/1.0.3")

	def layout(self):
		vs_layout(self)

	def generate(self):
		MSBuildDeps(self).generate()
		MSBuildToolchain(self).generate()