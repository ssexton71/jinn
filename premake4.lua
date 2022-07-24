
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
--  configurations { SHARED, STATIC }
    configurations { STATIC }
--  platforms { "x32", "x64" }
    platforms { "x64" }
  else
    configurations { STATIC }
    platforms { "x64" }
    kind "StaticLib"  -- gmake generator needs this to be global, not in a configuration 
  end
  location "src"
  buildoptions { "-Wall" }
  includedirs { "include", "src" }
  

configuration (SHARED)
  kind "SharedLib"

configuration (STATIC)
  kind "StaticLib"

configuration "windows"
  defines { "WIN32" }

configuration "gcc"
  flags { "Symbols", "Optimize" }
  buildoptions { "-fno-strict-aliasing" }
  buildoptions { "-Werror" }
  buildoptions_cpp { "-std=c++11" }
-- if can't use -z defs (or -Bsymbolic), e.g. because lua runtime symbols are expected undefined
-- because they come from the parent executable.  -z now is the next best thing
--  linkoptions  { "-z now" }
  linkoptions { "-z defs" }
  linkoptions { "-Wl,-rpath-link,../../bin-linux" }

configuration (MSVC)
  buildoptions { "/MD", "/EHc", "/EHs", "/Oi", "/Ot", "/W3" } --, "/Z7" }
  flags { "OptimizeSpeed", "FatalWarnings" }
  linkoptions { "/NODEFAULTLIB:OLDNAMES.LIB" }
  buildoptions { "/wd 4710", "/wd 4711", "/wd 4251", "/wd 4619", "/wd 4503", "/wd 4820", "/wd 4514", "/wd 4571", "/wd 4365", "/wd 4625", "/wd 4626", "/wd 5026", "/wd 5027", "/wd 4774" }
  flags { "NoManifest" }
  flags { "NoIncrementalLink" }

configuration { MSVC } --, "not " .. STATIC }
  linkoptions { "/WX" } --, "/DEBUG" }  -- do this for STATIC too?
  flags { "Symbols" }

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
    decl = iif(string.startswith(decl,"K2"),"K"..decl:sub(3),decl)
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
    configuration { "x64", MSVC } --, SHARED }
      implibdir "lib"
  end
  configuration {}
end

function jinncommon(m)
  namedopts(m)
  if monolithic then return end
  --[[
  defines "USE_BOOST"
  if (os.is("windows")) then
    local br = os.getenv("BOOST_ROOT")
    assert(br,"BOOST_ROOT is undefined")
    includedirs { br }
    libdirs { br .. "/lib" }
  end
  configuration (SHARED)
    links { "taskengine" }
  ]]--
  configuration (MSVC)
    buildoptions { "/wd 4826", "/wd 4275", "/wd 4640" }
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
--    platformtoolset "v141"
    
group "apps"

project "jinn"
  local m = monolithic
  monolithic = false
  kind "ConsoleApp"
  jinncommon()
  files { "premake4.lua", "ggen.sh", "jgen.sh", "lua/**.lua", "jinnfile.*", "package*.sh", "deps.sh", "build-win.bat", "build.sh" }
  links "futil"
  configuration { MSVC }
    linkoptions { "/MANIFESTUAC:NO" }
    linkoptions { "/MANIFEST" }
  configuration { MSVC, STATIC }
  configuration { "linux", STATIC }
  configuration "linux"


