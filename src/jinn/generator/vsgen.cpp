
#include "vsgen.h"

#include "../model/gscope.h"
#include "futil/util/xmlemit.h"
#include "futil/util/filelist.h"
#include "../util/genfile.h"

inline bool findAllFlags(jinn::model::ConfigurableKeyBlock& b1, jinn::model::ConfigurableKeyBlock& b2, const char* fl, const std::vector<std::string>& selectors) {
	jinn::model::Param p = b2.findAllConfigVals(b1, "flags", selectors);
	for (auto& val : p.vals)
		if (val == fl) return true;
	return false;
}

#include <fstream>

#include "futil/util/fileutil.h"

#include "../util/fileext.h"

#include <map>

class FileClassifier {
	std::map<std::string, std::string> m;
public:
	FileClassifier() {
		m[".h"] = "ClInclude";
		m[".cpp"] = "ClCompile";
		m[".c"] = "ClCompile";
		m[".rc"] = "ResourceCompile";
	}

	inline std::string classify(const std::string& file) {
		std::string c = m[fileExtension(file)];
		return c.empty()? "None":c;
	}
};

inline void replaceAll(std::string& s, char from, char to) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++) {
		if (from == *it) *it = to;
	}
}

#include "../util/cookpath.h"

static inline void cookPath(std::string& path, const std::string& root) {
	cookPath_(path, root);
	replaceAll(path, '/', '\\');
}

inline std::string filterPart(std::string f) {
	std::string::size_type pos = f.find_first_not_of(".\\");
	if (pos == f.npos) return "";
	f = f.substr(pos);
	pos = f.rfind('\\');
	if (pos == f.npos) return "";
	return f.substr(0, pos);
}

inline std::string collapse(const std::vector<std::string>& vals, const char* delim) {
	std::string result;
	for (auto& val : vals) {
		result += val;
		result += delim;
	}
	return result;
}

inline std::string collapsePaths(const std::vector<std::string>& vals, const char* delim, const std::string& root) {
	std::string result;
	for (auto val : vals) {
		cookPath(val, root);
		result += val;
		result += delim;
	}
	return result;
}

extern "C" {
#include "../util/uuid.h"
}

inline std::string makeHashUuid(const char* text) {
	char uuid[38]; hash_uuid_str(text, uuid); return uuid;
}

inline std::string fixPlatform(const std::string& p) {
	if (p == "x32") return "Win32";
	return p;
}

inline bool jinn::generator::VsGenerator::generateFilters(jinn::model::Project* p, futil::util::FileList& filelist) {
	std::vector<std::string>& files = filelist.files;
	jinn::util::GeneratedFile filters(testMode); 
	filters.init(*p);
	if (!filters.open("vcxproj.filters")) return false;
	futil::util::XmlEmitter e(filters.out);
	e.emit("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	e.begin("Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"");

	std::string root = filters.location + "/";
	std::map<std::string,std::string> filts;
	for (auto f : files) {
		cookPath(f, root);
		std::string filt = filterPart(f);
		for (;;) {
			if (filt.empty()) break;
			std::string uuid = makeHashUuid(filt.c_str());
			filts[filt] = uuid;
			std::string::size_type pos = filt.rfind('\\');
			if (pos == filt.npos) break;
			filt.erase(pos);
		}
	}
	if (!filts.empty()) {
		e.begin("ItemGroup");
		for (auto& filt : filts) {
			std::string v = "Filter Include=\"" + filt.first + "\"";
			e.begin(v.c_str());
			v = "{"; v += filt.second; v += "}";
			e.emit("UniqueIdentifier", v.c_str());
			e.end("Filter");
		}
		e.end("ItemGroup");
	}

	FileClassifier cl;
	std::string groups[] = { "None", "ClInclude", "ClCompile", "ResourceCompile" };
	for (auto& g : groups) {
		int count = 0;
		for (auto f : files) {
			if (g == cl.classify(f)) {
				count++;
				if (1 == count)
					e.begin("ItemGroup");
				cookPath(f, root);
				std::string filt = filterPart(f);
				if (filt.empty()) {
					std::string v = "<" + g + " Include=\"" + f + "\" />";
					e.emit(v.c_str());
				} else {
					std::string v = g + " Include=\"" + f + "\"";
					e.begin(v.c_str());
					e.emit("Filter", filt.c_str());
					e.end(g.c_str());
				}
			}
		}
		if (count)
			e.end("ItemGroup");
	}

	e.end("Project");
	return true;
}

static inline std::vector<std::string> buildSelectors(const std::string& configuration, const std::string& platform, const std::string& vsVersion) {
	std::vector<std::string> selectors = { "msvc", "windows" };
	selectors.push_back(vsVersion);
	selectors.push_back(configuration);
	selectors.push_back(platform);
	return selectors;
}

static const char* DISABLED = "Disabled";

inline const char* getOptimization(jinn::model::Project* p, jinn::model::Solution* s, const std::vector<std::string>& selectors) {
	struct {
		const char* flag;
		const char* value;
	} optFlags[] = {
		{ "Optimize", "Full" },
		{ "OptimizeSize", "MinSpace" },
		{ "OptimizeSpeed", "MaxSpeed" },
	};
	for (auto& of : optFlags) 
		if (findAllFlags(*s, *p, of.flag, selectors)) return of.value;
	return DISABLED;
}

bool isDebug(jinn::model::Project* p, jinn::model::Solution* s, const std::vector<std::string>& selectors) {
	return DISABLED == getOptimization(p,s,selectors);
}

inline bool jinn::generator::VsGenerator::generateProject(jinn::model::Project* p, jinn::model::Solution* s, futil::util::FileList& filelist) {
	std::vector<std::string>& files = filelist.files;
	jinn::util::GeneratedFile proj(testMode); 
	proj.init(*p);
	if (!proj.open("vcxproj")) return false;
	futil::util::XmlEmitter e(proj.out);
	e.emit("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	e.begin("Project DefaultTargets=\"Build\" ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"");

	std::string root = proj.location + "/";

	// get the project configurations
	std::vector<std::string> platforms = s->findVals("platforms");
	std::vector<std::string> configurations = s->findVals("configurations");

	// emit a ProjectConfiguration for each configuration
	e.begin("ItemGroup Label=\"ProjectConfigurations\"");
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string val = "ProjectConfiguration Include=\"" + cval + "\"";
			e.begin(val.c_str());
			e.emit("Configuration", conf.c_str());
			e.emit("Platform", fixPlatform(plat).c_str());
			e.end("ProjectConfiguration");
		}
	}
	e.end("ItemGroup");

	e.begin("PropertyGroup Label=\"Globals\"");
	std::string val = "{" + p->find("uuid") + "}";
	e.emit("ProjectGuid", val.c_str()); 
	e.emit("RootNamespace", p->key.c_str());
	e.emit("WindowsTargetPlatformVersion", "8.1");
	e.emit("Keyword", "Win32Proj");
	e.end("PropertyGroup");
	e.emit("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />");  

	// emit a PropertyGroup for each configuration
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::vector<std::string> selectors = buildSelectors(conf, plat, vsVersion);
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string val = "PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + cval + "'\" Label=\"Configuration\"";
			e.begin(val.c_str());
			std::string kind = p->findConfigVal(*s, "kind", selectors);
			if (kind == "SharedLib")
				e.emit("ConfigurationType", "DynamicLibrary");
			else if (kind == "StaticLib")
				e.emit("ConfigurationType", "StaticLibrary");
			else
				e.emit("ConfigurationType", "Application");
			bool debug = isDebug(p, s, selectors);
			e.emit("UseDebugLibraries", debug? "true":"false");
			val = p->findConfigVal(*s, "platformtoolset", selectors);
			if (val.empty()) val = platformToolset;
			e.emit("PlatformToolset", val.c_str());
			e.emit("PreferredToolArchitecture", "x64" /* plat.c_str()*/ ); // ???
			e.end("PropertyGroup");
		}
	}

	e.emit("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />");
	e.begin("ImportGroup Label=\"ExtensionSettings\"");
	e.end("ImportGroup");

	// emit an ImportGroup for each configuration
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string val = "ImportGroup Condition=\"'$(Configuration)|$(Platform)'=='" + cval + "'\" Label=\"PropertySheets\"";
			e.begin(val.c_str());
			e.emit("<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />");
			e.end("ImportGroup");
		}
	}

	e.emit("<PropertyGroup Label=\"UserMacros\" />");

	// emit a PropertyGroup for each configuration
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::vector<std::string> selectors = buildSelectors(conf, plat, vsVersion);
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string val = "PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" + cval + "'\"";
			e.begin(val.c_str());
			val = p->findConfigVal(*s, "targetdir", selectors);
			if (val.empty()) val = "bin";
			if ('/' != *val.rbegin()) val += '/';
			cookPath(val, root);
			e.emit("OutDir", val.c_str());
			val = p->findConfigVal(*s, "objdir", selectors);
			if (val.empty()) val = ".objs"; 
			if ('/' != *val.rbegin()) val += '/';
			cookPath(val, root);
			val = val + plat + "\\" + conf + "\\";  
			e.emit("IntDir", val.c_str());  
			val = p->findConfigVal(*s, "targetname", selectors);
			if (val.empty()) val = p->key;
			e.emit("TargetName", val.c_str());
			std::string kind = p->findConfigVal(*s, "kind", selectors);
			if (kind == "SharedLib") {
				e.emit("TargetExt", ".dll");
				e.emit("IgnoreImportLibrary", "false");
				e.emit("LinkIncremental", "false");
			} else if (kind == "StaticLib")
				e.emit("TargetExt", ".lib");
			else {
				e.emit("TargetExt", ".exe");
				e.emit("LinkIncremental", "false");
			}
			e.emit("GenerateManifest", "false");
			e.end("PropertyGroup");
		}
	}

	std::vector<jinn::model::Project> refs = s->findProjectReferences(p);

	// emit an ItemDefinitionGroup for each configuration
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::vector<std::string> selectors = buildSelectors(conf, plat, vsVersion);
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string kind = p->findConfigVal(*s, "kind", selectors); 
			std::string targetName = p->findConfigVal(*s, "targetname", selectors);
			if (targetName.empty()) targetName = p->key;
			std::string pdb = p->findConfigVal(*s, "pdbdir", selectors);
			if (pdb.empty()) pdb = "$(OutDir)";
			if ('/' != *pdb.rbegin()) pdb += '/';
			pdb = pdb + targetName + ".pdb";
			std::string val = "ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" + cval + "'\"";
			e.begin(val.c_str());
			std::string incs = collapsePaths(p->findConfigVals(*s, "includedirs", selectors ).vals, ";", root) + "%(AdditionalIncludeDirectories)"; 
			std::string defs = collapse(p->findAllConfigVals(*s, "defines", selectors ).vals, ";") + "%(PreprocessorDefinitions)";
			e.begin("ClCompile");
			val = collapse(p->findAllConfigVals(*s, "buildoptions", selectors ).vals, " ") + " %(AdditionalOptions)";
			e.emit("AdditionalOptions", val.c_str());
			e.emit("Optimization", getOptimization(p, s, selectors));
			e.emit("AdditionalIncludeDirectories", incs.c_str());
			e.emit("PreprocessorDefinitions", defs.c_str());
			e.emit("MinimalRebuild", "false");
			if (isDebug(p, s, selectors)) {
				e.emit("BasicRuntimeChecks", "EnableFastChecks");
				e.emit("RuntimeLibrary", "MultiThreadedDebugDLL");
			} else {
				e.emit("StringPooling", "true");
				e.emit("RuntimeLibrary", "MultiThreadedDLL");
			}
			e.emit("FunctionLevelLinking", "true");
			e.emit("MultiProcessorCompilation", "true");
			e.emit("PrecompiledHeader", "");
			e.emit("WarningLevel", "Level3");
			e.emit("TreatWarningAsError", "true");
			e.emit("DebugInformationFormat", "ProgramDatabase");
			if (kind == "StaticLib") 
				e.emit("ProgramDataBaseFileName", pdb.c_str());
			e.end("ClCompile");
			e.begin("ResourceCompile");
			e.emit("PreprocessorDefinitions", defs.c_str());
			e.emit("AdditionalIncludeDirectories", incs.c_str());
			e.end("ResourceCompile");
			if (kind == "StaticLib") {
				e.begin("Lib");
				val = "$(OutDir)" + targetName + ".lib";
				e.emit("OutputFile", val.c_str());
				val = collapse(p->findConfigVals(*s, "linkoptions", selectors ).vals, " ") + "%(AdditionalOptions)";
				e.emit("AdditionalOptions", val.c_str());
				if (plat == "x32")
					e.emit("TargetMachine", "MachineX86");
				else
					e.emit("TargetMachine", "MachineX64");
				e.end("Lib");
			}
			e.begin("Link");
			if ((kind == "SharedLib") || (kind == "StaticLib") || (kind == "WindowedApp")) 
				e.emit("SubSystem", "Windows");
			else 
				e.emit("SubSystem", "Console");
			e.emit("GenerateDebugInformation", "true");
			e.emit("ProgramDataBaseFileName", pdb.c_str());
			if (!isDebug(p, s, selectors)) {
				e.emit("EnableCOMDATFolding", "true");
				e.emit("OptimizeReferences", "true");
			}
			std::string ext, entry, implib;
			if (kind != "StaticLib") {
				std::vector<std::string> links = p->findAllConfigVals(*s, "links", selectors ).vals;
				for (auto& ref : refs) 
					for(auto it = links.begin(); it != links.end(); it++) 
						if (ref.key == *it) { links.erase(it); break; }
				val = collapse(links, ".lib;");
				if (!val.empty()) {
					val += "%(AdditionalDependencies)";
					e.emit("AdditionalDependencies", val.c_str());
				}
				if (kind == "SharedLib") {
					ext = ".dll";
					val = p->findConfigVal(*s, "implibdir", selectors);
					if (val.empty()) val = "lib";
					if ('/' != *val.rbegin()) val += '/';
					cookPath(val, root);
					implib = val + targetName + ".lib";
				} else {
					ext = ".exe";
					if (!findAllFlags(*s, *p, "WinMain", selectors))
						entry = "mainCRTStartup";
				}
				val = "$(OutDir)" + targetName + ext;
				e.emit("OutputFile", val.c_str());
				val = collapsePaths(p->findConfigVals(*s, "libdirs", selectors).vals, ";", root) + "%(AdditionalLibraryDirectories)"; 
				e.emit("AdditionalLibraryDirectories", val.c_str());
				if (!entry.empty())
					e.emit("EntryPointSymbol", entry.c_str());
				if (!implib.empty())
					e.emit("ImportLibrary", implib.c_str());
				if (plat == "x32")
					e.emit("TargetMachine", "MachineX86");
				else
					e.emit("TargetMachine", "MachineX64");
				val = collapse(p->findConfigVals(*s, "linkoptions", selectors ).vals, " ") + "%(AdditionalOptions)";
				e.emit("AdditionalOptions", val.c_str());
				if (findAllFlags(*s, *p, "NoEnableUAC", selectors))
					e.emit("EnableUAC", "false");
			}
			e.end("Link");
			if (findAllFlags(*s, *p, "PerMonitorHighDpiAware", selectors)) {
				e.begin("Manifest");
				e.emit("EnableDpiAwareness", "PerMonitorHighDPIAware");
				e.end("Manifest");
			}
			e.end("ItemDefinitionGroup");
		}
	}

	FileClassifier cl;
	std::string groups[] = { "ClInclude", "ClCompile", "None", "ResourceCompile" };
	for (auto& g : groups) {
		int count = 0;
		for (auto f : files) {
			if (g == cl.classify(f)) {
				count++;
				if (1 == count)
					e.begin("ItemGroup");
				cookPath(f, root);
				if (g != "ClCompile") {
					std::string v = "<" + g + " Include=\"" + f + "\" />";
					e.emit(v.c_str());
				} else {
					std::string filt = filterPart(f);
					std::string v = g + " Include=\"" + f + "\"";
					e.begin(v.c_str());
					if (filt.empty()) filt = ".";
					v = "$(IntDir)" + filt + "\\";
					e.emit("ObjectFileName", v.c_str());
					e.end(g.c_str());
				}
			}
		}
		if (count)
			e.end("ItemGroup");
	}

	// generate ProjectReference from links directives
	if (!refs.empty()) {
		e.begin("ItemGroup");
		for (auto& pp : refs) {
			std::string ppath = pp.find("location") + "/" + pp.key + ".vcxproj";
			cookPath(ppath, root);
			val = "ProjectReference Include=\"" + ppath + "\"";
			e.begin(val.c_str());
			val = "{" + pp.find("uuid") + "}";
			e.emit("Project", val.c_str());
			e.end("ProjectReference");
		}
		e.end("ItemGroup");
	}

	e.emit("<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />");
	e.begin("ImportGroup Label=\"ExtensionTargets\"");
	e.end("ImportGroup");
	e.end("Project");
	return true;
}

#include <algorithm>

#include "../util/filelist.h"

inline bool jinn::generator::VsGenerator::generate(jinn::model::Project* p, jinn::model::Solution* s) {
	futil::util::FileList filelist = buildFileList(p);
	std::sort(filelist.files.begin(), filelist.files.end());
	return generateProject(p,s,filelist) && generateFilters(p,filelist);
}

inline bool jinn::generator::VsGenerator::generateProjects(jinn::model::Solution* s) {
	for(auto& p : s->projects)  {
		std::string uuid = p.find("uuid");
		if (uuid.empty()) {
			uuid = makeHashUuid(p.key.c_str());
			model::Param pu; pu.key = "uuid";
			pu.vals.push_back(uuid);
			p.params.push_back(pu);
		}
	}
	int succ = 0, fail = 0;
	for(auto& p : s->projects)  {
		if (generate(&p,s))
			succ++;
		else
			fail++;
	}
	std::cout << succ << " projects" << std::endl;
	if (fail)
		std::cerr << fail << " projects failed!" << std::endl;
	return fail == 0;
}

inline bool jinn::generator::VsGenerator::generate(jinn::model::Solution* s) {
	jinn::util::GeneratedFile sln(testMode); 
	sln.init(*s);
	if (!sln.open("sln")) return false;
	futil::util::TextEmitter e(sln.out, '\t', 1);
	e.emit("");
	e.emit("Microsoft Visual Studio Solution File, Format Version 12.00");
	e.emit("# Visual Studio 15");
	generateProjects(s);
	// find the unique set of project groups
	std::map<std::string, std::string> groups;  // full group name, uuid
	std::map<std::string, std::string> cgroups;  // child name, parent name
	for(auto& proj : s->projects) {
		std::string grp = proj.find("group");
		while (!grp.empty()) {
			std::string gpath = "/" + grp;
			std::string uuid = makeHashUuid(gpath.c_str());
			groups[grp] = uuid;
			std::string::size_type pos = grp.rfind('/');
			if (pos == grp.npos) break;
			std::string child = grp;
			grp.erase(pos);
			cgroups[child] = grp;
		}
	}
	// add solution folders for the project groups
	for(auto& grp : groups ) {
		std::string uuidref = "2150E333-8FDC-42A3-9474-1A3956D46DE8";  // magic: solution folder
		std::string uuid = grp.second;
		std::string name = grp.first;
		std::string::size_type pos = name.rfind('/');
		if (pos != name.npos) name = name.substr(pos + 1);
		std::string val = "Project(\"{" + uuidref + "}\") = \"" + name 
			+ "\", \"" + name + "\", \"{" + uuid + "}\"";
		e.begin(val.c_str());
		e.end("EndProject");
	}
	// add Project directives
	std::string root = sln.location + "/";
	for(auto & proj : s->projects) {
		std::string uuid = proj.find("uuid");
		std::string uuidref = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942";  // magic:  C++ project
		std::string path = proj.find("location") + "/";
		cookPath(path, root);
		path += proj.key + ".vcxproj";
		std::string val = "Project(\"{" + uuidref + "}\") = \"" + proj.key 
			+ "\", \"" + path + "\", \"{" + uuid + "}\"";
		e.begin(val.c_str());
		// generate ProjectDependencies ("References") from links directives
		std::vector<std::string> deps = proj.findAllParams("links").vals;
		int dcnt = 0;
		for (auto& dep : deps) {
			for (auto& p : s->projects) {
				if (p.key == dep) {
					dcnt++;
					if (dcnt == 1) 
						e.begin("ProjectSection(ProjectDependencies) = postProject");
					val = "{" + p.find("uuid") + "}";
					val = val + " = " + val;
					e.emit(val.c_str());
				}
			}
		}
		if (dcnt) e.end("EndProjectSection");
		e.end("EndProject");
	}
	e.begin("Global");

	// get the project configurations.  
	std::vector<std::string> platforms = s->findVals("platforms");
	std::vector<std::string> configurations = s->findVals("configurations");

	// emit the SolutionConfigurationPlatforms
	e.begin("GlobalSection(SolutionConfigurationPlatforms) = preSolution");
	for(auto& conf : configurations) {
		for (auto& plat : platforms) {
			std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
			std::string val = cval + " = " + cval;
			e.emit(val.c_str());
		}
	}
	e.end("EndGlobalSection");

	// emit the ProjectConfigurationPlatforms
	e.begin("GlobalSection(ProjectConfigurationPlatforms) = postSolution");
	for(auto & proj : s->projects) {
		std::string uuid = proj.find("uuid");
		for(auto& conf : configurations) {
			for (auto& plat : platforms) {
				std::string cval(conf); cval.push_back('|'); cval.append(fixPlatform(plat));
				const char* vv[] = { "ActiveCfg", "Build.0" };
				for(auto v : vv) {
					std::string val = "{" + uuid + "}." + cval + "." + v + " = " + cval;
					e.emit(val.c_str());
				}
			}
		}
	}
	e.end("EndGlobalSection");

	e.begin("GlobalSection(SolutionProperties) = preSolution");
	e.emit("HideSolutionNode = FALSE");
	e.end("EndGlobalSection");
	e.begin("GlobalSection(NestedProjects) = preSolution");
	for (auto& cgrp : cgroups) {
		std::string val = "{" + groups[cgrp.first] + "} = {" + groups[cgrp.second] + "}";
		e.emit(val.c_str());
	}
	for(auto& proj : s->projects) {
		std::string grp = proj.find("group");
		if (grp.empty()) continue;
		std::string val = "{" + proj.find("uuid") + "} = {" + groups[grp] + "}";
		e.emit(val.c_str());
	}
	e.end("EndGlobalSection");	
	e.end("EndGlobal");
	return true;
}

inline bool jinn::generator::VsGenerator::generateSolutions(jinn::model::GlobalScope* g) {
	int succ = 0, fail = 0;
	for(auto& s : g->solutions) 
		if (generate(&s))
			succ++;
		else
			fail++;
	std::cout << succ << " solutions" << std::endl;
	if (fail)
		std::cerr << fail << " solutions failed!" << std::endl;
	return fail == 0;
}

void jinn::generator::VsGenerator::generate(model::GlobalScope* g) {

	generateSolutions(g);

}


