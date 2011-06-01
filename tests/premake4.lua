--duct++ tests premake file

--[[if _ACTION=="clean" then
	os.rmdir(outpath)
end]]

solution("tests")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

group=""

function setup_test(name, src)
	local outpath="out/"
	local proj=project(group.."_"..name)
	proj.language="C++"
	proj.kind="ConsoleApp"
	
	targetname(name)
	
	configuration {"debug"}
		defines {"DEBUG", "_DEBUG"}
		flags {"Symbols", "ExtraWarnings"}
		links {"duct_debug"}
	
	configuration {"release"}
		defines {"NDEBUG", "RELEASE"}
		flags {"Optimize", "ExtraWarnings"}
		links {"duct"}
	
	configuration {"linux", "x32"}
		libdirs {
			"../../lib/linux/x86",
			"../../deps/linux/x86/icu/lib"
		}
	
	configuration {"linux", "x64"}
		libdirs {
			"../../lib/linux/x64",
			"../../deps/linux/x64/icu/lib"
		}
	
	configuration {"linux"}
		links {"icui18n", "icudata", "icuio", "icuuc"}
	
	configuration {"vs2008", "x32"}
		libdirs {
			"../../lib/windows/x86",
			"../../deps/msvc/x86/icu/lib"
		}
	
	configuration {"vs2008", "x64"}
		libdirs {
			"../../lib/windows/x64",
			"../../deps/msvc/x64/icu/lib"
		}
	
	configuration {"windows"}
		includedirs {
			"../../deps/msvc/msinttypes"
		}
		links {"icuin", "icudt", "icuio", "icuuc"}
	
	configuration {}
		targetdir(".")
		objdir(outpath)
		includedirs {
			"../../include",
			"../../deps/include/icu"
		}
		files {
			src
		}
end

-- categories

include "archive"
include "arghandling"
include "charutils"
include "csv"
include "filesystem"
include "iniformatter"
include "scriptformatter"
include "streams"
include "variables"

if _ACTION=="clean" then
	local prjs=solution().projects
	for i, prj in ipairs(prjs) do
		os.rmdir(prj.basedir.."/out")
	end
end

