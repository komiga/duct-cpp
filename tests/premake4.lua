--proptool premake file

if _ACTION=="clean" then
	os.rmdir(outpath)
end

solution("tests")
	configurations {"debug", "release"}

function setup_test(name, src)
	local outpath="out/"
	local proj=project(name)
	proj.language="C++"
	proj.kind="ConsoleApp"
	
	configuration {"debug"}
		targetdir(name)
		objdir(outpath)
		defines {"DEBUG", "_DEBUG"}
		flags {"Symbols", "ExtraWarnings"}
		links {"duct_debug"}
	
	configuration {"release"}
		targetdir(outpath)
		objdir(outpath)
		defines{"NDEBUG", "RELEASE"}
		flags {"Optimize", "ExtraWarnings"}
		links {"duct"}
	
	--configuration {"gmake"}
	--	postbuildcommands {"mkdir -p bin/"}
	--	postbuildcommands {"cp "..execpath.." bin/"..name}
	
	configuration {"vs2008"}
		includedirs {
			"../../deps/msvc/msinttypes/",
			"../../deps/include/icu/"
		}
		links {"icuin", "icudt", "icuio", "icuuc"}
		libdirs {"../../deps/msvc/x86/icu/"}
	
	configuration {}
	
	includedirs {
		"include/",
		"../../include/",
		"/usr/local/include/"
	}
	
	libdirs {
		"../../lib/windows",
		"../../lib/linux",
	}
	files {
		src
	}
end

include "variables"

