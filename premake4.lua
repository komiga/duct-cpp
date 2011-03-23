-- duct++ premake file

local root="../../"

local name="duct"
local outpath="out/"
local binarypath=outpath..name

if _ACTION == "clean" then
	os.rmdir(outpath)
end

solution("duct")
	configurations {"debug", "release"}

local proj=project(name)
proj.language="C++"
proj.kind="SharedLib"

configuration {"debug"}
	targetsuffix("_debug")
	targetdir(outpath)
	objdir(outpath)
	defines {"DEBUG", "_DEBUG"}
	flags {"Symbols", "ExtraWarnings"}

configuration {"release"}
	targetdir(outpath)
	objdir(outpath)
	defines {"NDEBUG", "RELEASE"}
	flags {"Optimize", "ExtraWarnings"}

configuration {"gmake"}
	links {"icui18n", "icudata", "icuio", "icuuc"}

configuration {"linux"}
	defines {"PLATFORM_CHECKED", "UNIX_BUILD"}

configuration {"linux", "debug"}
	postbuildcommands {"mkdir -p lib/linux/debug"}
	postbuildcommands {"cp "..outpath.."lib"..name.."_debug.so lib/linux/debug/lib"..name..".so"}

configuration {"linux", "release"}
	postbuildcommands {"mkdir -p lib/linux/release"}
	postbuildcommands {"cp "..outpath.."lib"..name..".so lib/linux/release/lib"..name..".so"}

configuration {}

files {"include/duct/*.hpp", "src/duct/*.cpp"}
includedirs {
	"include/"
}
