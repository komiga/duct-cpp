-- duct++ premake file

local name="duct"
local outpath="out"

local libpath_linux="lib/linux"
local libpath_windows="lib\\windows"

if _ACTION=="clean" then
	os.rmdir(outpath)
end

solution("duct")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

local proj=project(name)
proj.language="C++"
proj.kind="StaticLib"

configuration {"debug"}
	targetsuffix("_debug")
	defines {"DEBUG", "_DEBUG"}
	flags {"Symbols", "ExtraWarnings"}

configuration {"release"}
	defines {"NDEBUG"}
	flags {"Optimize", "ExtraWarnings"}

configuration {"linux", "x32"}
	targetdir(libpath_linux.."/x86")

configuration {"linux", "x64"}
	targetdir(libpath_linux.."/x64")

configuration {"linux"}
	buildoptions {"-std=c++0x", "-pedantic"}
	files {
		"src/duct/unix/*.cpp"
	}

configuration {"windows", "x32"}
	targetdir(libpath_windows.."/x86")

configuration {"windows", "x64"}
	targetdir(libpath_windows.."/x64")

configuration {"windows"}
	files {
		"src/duct/windows/*.cpp"
	}

configuration {}
	objdir(outpath)
	files {
		"src/duct/*.cpp"
	}
	includedirs {
		"include"
	}
