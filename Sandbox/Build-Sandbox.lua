project "Sandbox"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Build/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp", "../Vendor/GLAD/src/glad.c", "resources/**" }

   includedirs
   {
      "src",
	  "../Vendor/GLFW/include",
	  "../Vendor/GLAD/include",
      "../Vendor/GLM/include",
      "../Vendor/STB/include"
   }

   links
   {
	  "../Vendor/GLFW/lib/glfw3.lib"
   }
   
   defines { "GLM_FORCE_XYZW_ONLY" }

   targetdir ("../Build/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Build/Intermediates/" .. OutputDir .. "/%{prj.name}")
   
   pchheader "pch.h"
   pchsource "src/pch.cpp"
	
   filter "files:../Vendor/GLAD/src/glad.c"
       flags { "NoPCH" } -- Disable precompiled headers for glad.c

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"