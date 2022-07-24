

jinn = {}
jinn.outfile = io.open("jinnfile.jinn", "w")

function jinn.out(str)
  jinn.outfile:write(jinn.indent .. jinn.cindent .. str .. "\n")
end

function jinn.tstr(table)
  str = ""
  for key, value in pairs(table) do
    if not (str == "")  then
      str = str .. "," 
    end
    str = str ..  "\"" .. value .. "\""
  end
  str = str .. ""
  return str
end

function jinn.sparam(cmd, param)
  jinn.out(cmd .. " \"" .. param .. "\"")
end

function jinn.tparam(cmd, param)
  jinn.out(cmd .. " " .. jinn.tstr(param))
end

function jinn.param(cmd, param)
  if (type(param) == "table") then
    return jinn.tparam(cmd, param)
  else
    return jinn.sparam(cmd, param)
  end
end

jinn.project = {}
jinn.project.name = ""
jinn.indent = ""
jinn.cindent = ""

function solution(param)
  jinn.cindent = ""
  jinn.indent = ""
  jinn.param("\nsolution", param)
  jinn.indent = "  "
end

function project(param)
  if (param == nil) then
    return jinn.project
  else
    jinn.cindent = ""
    jinn.indent = ""
    jinn.project.name = param
    jinn.param("\nproject", param)
    jinn.indent = "  "
  end
end

function configuration(param)
  jinn.cindent = ""
  jinn.param("configuration", param)
  jinn.cindent = "  "
end

function group(param)
  jinn.param("group", param)
end

function targetname(param)
  jinn.param("targetname", param)
end

function language(param)
  jinn.param("language", param)
end

function uuid(param)
  jinn.param("uuid", param)
end

function configurations(param)
  jinn.param("configurations", param)
end

function platforms(param)
  jinn.param("platforms", param)
end

function files(param)
  jinn.param("files", param)
end

function location(param)
  jinn.param("location", param)
end

function buildoptions(param)
  jinn.param("buildoptions", param)
end

function buildoptions_cpp(param)
  jinn.param("buildoptions_cpp", param)
end

function linkoptions(param)
  jinn.param("linkoptions", param)
end

function includedirs(param)
  jinn.param("includedirs", param)
end

function libdirs(param)
  jinn.param("libdirs", param)
end

function objdir(param)
  jinn.param("objdir", param)
end

function targetdir(param)
  jinn.param("targetdir", param)
end

function pdbname(param)
  jinn.param("pdbname", param)
end

function pdbdir(param)
  jinn.param("pdbdir", param)
end

function kind(param)
  jinn.param("kind", param)
end

function defines(param)
  jinn.param("defines", param)
end

function links(param)
  jinn.param("links", param)
end

function flags(param)
  jinn.param("flags", param)
end

function implibname(param)
  jinn.param("implibname", param)
end

function implibdir(param)
  jinn.param("implibdir", param)
end

function platformtoolset(param)
  jinn.param("platformtoolset", param)
end

