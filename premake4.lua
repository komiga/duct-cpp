-- duct++ premake file
-- run `premake4 [--installdebug=true|false] [--installroot=PATH] install` after building to install duct++

local name="duct"
local outpath="out"

-- requires premake 4.4
--[[local bitness="x86"
if os.is64bit() then
	bitness="x64"
end
local libpath_linux="lib/linux/"..bitness
local libpath_windows="lib/windows/"..bitness]]
local libpath_linux="lib/linux"
local libpath_windows="lib\\windows"

if _ACTION=="clean" then
	os.rmdir(outpath)
end

solution("duct")
	configurations {"debug", "release"}

local proj=project(name)
proj.language="C++"
proj.kind="SharedLib"

platforms {"x32", "x64"}

configuration {"debug"}
	targetsuffix("_debug")
	objdir(outpath)
	defines {"DEBUG", "_DEBUG"}
	flags {"Symbols", "ExtraWarnings"}

configuration {"release"}
	objdir(outpath)
	defines {"NDEBUG", "RELEASE"}
	flags {"Optimize", "ExtraWarnings"}

configuration {}
	files {
		"include/duct/*.hpp",
		"src/duct/*.cpp"
	}
	includedirs {
		"include",
		"deps/include/icu"
	}

configuration {"linux", "x32"}
	targetdir(libpath_linux.."/x86")
	libdirs {
		"deps/linux/x86/icu/lib"
	}

configuration {"linux", "x64"}
	targetdir(libpath_linux.."/x64")
	libdirs {
		"deps/linux/x64/icu/lib"
	}

configuration {"linux"}
	files {
		"src/duct/unix/*.cpp"
	}
	links {
		"icui18n", "icudata", "icuio", "icuuc"
	}

configuration {"vs2008", "x32"}
	targetdir(libpath_windows.."/x86")
	libdirs {
		"deps/msvc/x86/icu/lib"
	}

configuration {"vs2008", "x64"}
	targetdir(libpath_windows.."/x64")
	libdirs {
		"deps/msvc/x64/icu/lib"
	}

configuration {"vs2008"} -- no stdint.h on MSVC 9.0
	includedirs {
		"deps/msvc/msinttypes"
	}

configuration {"windows"}
	files {
		"src/duct/windows/*.cpp",
		"include/duct/windows/dirent.h",
		"src/duct/windows/dirent.c"
	}
	links {"icuin", "icudt", "icuio", "icuuc"}
	defines {"DUCT_DYNAMIC", "DUCT_EXPORT"}

-- extensions

-- src: http://industriousone.com/topic/oscopydir
-- Allows copying directories.
-- It uses the premake4 patterns (**=recursive match, *=file match)
-- NOTE: It won't copy empty directories!
-- Example: we have a file: src/test.h
--	os.copydir("src", "include") simple copy, makes include/test.h
--	os.copydir("src", "include", "*.h") makes include/test.h
--	os.copydir(".", "include", "src/*.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h", true) will force it to include dir, makes include/test.h
--
-- @param src_dir
--    Source directory, which will be copied to dst_dir.
-- @param dst_dir
--    Destination directory.
-- @param filter
--    Optional, defaults to "**". Only filter matches will be copied. It can contain **(recursive) and *(filename).
-- @param single_dst_dir
--    Optional, defaults to false. Allows putting all files to dst_dir without subdirectories.
--    Only useful with recursive (**) filter.
-- @returns
--    True if successful, otherwise nil.
--
function os.copydir(src_dir, dst_dir, filter, single_dst_dir)
	filter = filter or "**"
	src_dir = src_dir .. "/"
	--print('copy "' .. src_dir .. filter .. '" to "' .. dst_dir .. '".')
	dst_dir = dst_dir .. "/"
	local dir = path.rebase(".",path.getabsolute("."), src_dir) -- root dir, relative from src_dir
 
	os.chdir( src_dir ) -- change current directory to src_dir
		local matches = os.matchfiles(filter)
	os.chdir( dir ) -- change current directory back to root
 
	local counter = 0
	for k, v in ipairs(matches) do
		local target = iif(single_dst_dir, path.getname(v), v)
		--make sure, that directory exists or os.copyfile() fails
		os.mkdir( path.getdirectory(dst_dir .. target))
		if os.copyfile( src_dir .. v, dst_dir .. target) then
			counter = counter + 1
		end
	end
 
	if counter == #matches then
		--print( counter .. " files copied.")
		return true
	else
		--print( "Error: " .. counter .. "/" .. #matches .. " files copied.")
		return nil
	end
end

-- install action

newoption {
	trigger="installroot",
	value="PATH",
	description="Base install path (default is /usr/local)"
}

newoption {
	trigger="installdebug",
	description="Whether to install the debug build",
	value="true|false",
	allowed={
		{"true", "Install the debug build as well as the release build"},
		{"false", "Install only the release build (default)"}
	}
}

newoption {
	trigger="installbuild",
	description="Which build to install",
	value="x86|x64",
	allowed={
		{"x86", "Install the 32-bit build"},
		{"x64", "Install the 64-bit build"}
	}
}

newaction {
	trigger="install",
	description="Install duct++",
	execute=function()
		print("installing duct++")
		local installroot=_OPTIONS.installroot
		if not installroot then
			installroot="/usr/local"
		end
		print("installroot="..installroot)
		local installdebug=nil
		if _OPTIONS.installdebug=="true" then
			installdebug=true
		end
		local installroot=_OPTIONS.installroot
		if not installroot then
			installroot="/usr/local"
		end
		-- requires premake 4.4
		local bitness=_OPTIONS.installbuild
		if not bitness then
			if _PREMAKE_VERSION>="4.4" then
				if os.is64bit() then
					bitness="x64"
				else
					bitness="x86"
				end
			else
				bitness="x86"
			end
		end
		print("installing "..bitness.." build")
		local libpath_linux="lib/linux/"..bitness
		local libpath_windows="lib/windows/"..bitness
		--[[local libpath_linux="lib/linux"
		local libpath_windows="lib\\windows"]]
		local opsys=os.get()
		if opsys=="linux" then
			if not os.isdir(libpath_linux) then
				print("must run build.sh first")
				return nil
			end
			if not os.isdir(installroot) then
				os.mkdir(installroot)
			end
			os.rmdir(installroot.."/include/duct")
			if not os.copydir("include", installroot.."/include", "**.hpp") or not os.copydir("include", installroot.."/include", "**.inl") then
				printf("failed to copy includes")
				return nil
			end
			if not os.isdir(installroot.."/lib") then
				os.mkdir(installroot.."/lib")
			end
			os.execute("rm -f "..installroot.."/lib/libduct.so")
			if not os.copyfile(libpath_linux.."/libduct.so", installroot.."/lib/libduct.so") then
				print("failed to copy release library")
				return nil
			end
			if installdebug==true then
				os.execute("rm -f "..installroot.."/lib/libduct_debug.so")
				if not os.copyfile(libpath_linux.."/libduct_debug.so", installroot.."/lib/libduct_debug.so") then
					print("failed to copy debug library")
					return nil
				end
			end
		else
			print("unimplemented for non-Linux OSes")
			return nil
		end
		return true
	end
}

