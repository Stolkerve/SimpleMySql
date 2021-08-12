workspace "SimpleMySql"
	architecture "x86_64"
	startproject "SimpleMySql"

	configurations
	{
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/SimpleMySql/dependencies/glfw/include"
IncludeDir["glad"] = "%{wks.location}/SimpleMySql/dependencies/glad/include"
IncludeDir["imgui"] = "%{wks.location}/SimpleMySql/dependencies/imgui"
IncludeDir["mysql"] = "%{wks.location}/SimpleMySql/dependencies/mysql-connector-c++"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

group "Dependencies"
	include "SimpleMySql/dependencies/glfw"
	include "SimpleMySql/dependencies/glad"
group ""
	include "SimpleMySql"
