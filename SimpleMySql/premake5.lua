project "SimpleMySql"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hh",
		"src/**.cpp",
		"dependencies/imgui/**.h",
		"dependencies/imgui/**.cpp",
		"dependencies/ImGuiColorTextEdit/**.cpp",
		"dependencies/ImGuiColorTextEdit/**.h",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	includedirs
	{
		"src",
		"dependencies/",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.mysql}",
	}

	filter "configurations:Debug"
		libdirs {
			"dependencies/glfw/bin/%{cfg.buildcfg}-%{cfg.system}/glfw/",
			"dependencies/glad/bin/%{cfg.buildcfg}-%{cfg.system}/glad/",
			"dependencies/mysql-connector-c++/lib/",
		}

	filter "configurations:Release"
		libdirs {
			"dependencies/glfw/bin/%{cfg.buildcfg}-%{cfg.system}/glfw/",
			"dependencies/glad/bin/%{cfg.buildcfg}-%{cfg.system}/glad/",
			"dependencies/mysql-connector-c++/lib/",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		links
		{
			"X11",
			"dl",
			"pthread",
			"glfw",
			"glad",
			"GL",
			"mysqlcppconn.lib",
			"libssl.lib",
			"libcrypto.lib",
			"Dnsapi.lib"
		}
	filter "system:windows"
		systemversion "latest"

		links
		{
			"glfw",
			"glad",
			"opengl32.lib",
			"mysqlcppconn.lib",
			"libssl.lib",
			"libcrypto.lib",
			"Dnsapi.lib"
		}

	filter "configurations:Debug"
		defines "SIMPLE_MYSQL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SIMPLE_MYSQL_RELEASE"
		runtime "Release"
		optimize "on"