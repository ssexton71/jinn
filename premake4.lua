
monolithic = false
--monolithic = true

if (os.is("windows")) then
withclient = true
binsfx = ""
else
withclient = false
--binsfx = "-linux" 
binsfx = ""
staticexe = true
end

solname = "jinn"
if _ARGS[1] then solname = _ARGS[1] end
print("solution: " .. solname)

MSVC = "vs2019"
STATIC = "Release-Static"
SHARED = "Release"

if not jinn then
function platformtoolset() end
end

solution(solname)
  language "C++"
  if (os.is("windows")) then
    configurations { STATIC }
    platforms { "x64" }
  else
    configurations { STATIC }
    platforms { "x64" }
    kind "StaticLib"  -- gmake generator needs this to be global, not in a configuration 
  end
  location "src"
  flags { "AllWarnings", "FatalWarnings", "Symbols" }
  includedirs { "include", "src" }
  
configuration (SHARED)
  kind "SharedLib"

configuration (STATIC)
  kind "StaticLib"

configuration "windows"
  defines { "WIN32" }

configuration "gcc"
  flags { "Optimize" }
  buildoptions { "-fno-strict-aliasing" }
  buildoptions_cpp { "-std=c++11" }
  linkoptions { "-z defs" }
  linkoptions { "-Wl,-rpath-link,../../bin-linux" }

configuration (MSVC)
  buildoptions { "/MD", "/EHc", "/EHs", "/Oi", "/Ot", "/W3" } 
  flags { "OptimizeSpeed" }
  linkoptions { "/NODEFAULTLIB:OLDNAMES.LIB" }
  disablewarnings { "4710", "4711" }
  flags { "NoManifest" }
  flags { "NoIncrementalLink" }

configuration "x64"
  targetdir ("bin" .. binsfx )
  libdirs { "lib" }

configuration "x32"
  targetdir ("bin32" .. binsfx )
  libdirs { "lib32" }

function namedopts(m)
  local p = m or project().name
  local src = "src/" .. p
  if not monolithic then
    location ( src )
    objdir ( src .. "/.objs" )
--	print( src .. "/.objs" )
  end
  files { src .. "/**.h", src .. "/**.cpp", src .. "/**.c", src .. "/**.lua" }
  configuration { MSVC, SHARED }
    local decl = string.upper(p)
    defines { decl .. "_DECLSPEC=__declspec(dllexport)" }
    if not monolithic then
      implibname(p)
    end
  if staticexe then
    configuration "linux"
      linkoptions { "-static", "-static-libstdc++" }
  end
  if not monolithic then
    configuration { "x32", MSVC, SHARED }
      implibdir "lib32"
    configuration { "x64", MSVC } 
      implibdir "lib"
  end
  configuration {}
end

function jinncommon(m)
  namedopts(m)
  if monolithic then return end
  configuration {}
end

function jinnmodule(m)
  if monolithic then
    -- manually close off any configuration{} blocks.
    -- project() does this for us for non-monolithic build
    configuration{}
  else
    project(m)
  end
  jinncommon(m)
end

function jinnlinks(t)
  if not monolithic then links(t) end
end

group "libs"

jinnmodule "futil"
  targetname "futil-s"
  configuration { MSVC }
    pdbdir "$(TargetDir)"
    
group "apps"

project "jinn"
  local m = monolithic
  monolithic = false
  kind "ConsoleApp"
  jinncommon()
  files { "premake4.lua", "ggen.sh", "jgen.sh", "lua/**.lua", "jinnfile.*", "package*.sh", "deps.sh", "build-win.bat", "build.sh" }
  files { "README.md", "LICENSE" }
  links "futil"
  configuration { MSVC }
    linkoptions { "/MANIFESTUAC:NO" }
    linkoptions { "/MANIFEST" }
  configuration { MSVC, STATIC }
  configuration { "linux", STATIC }
  configuration "linux"


