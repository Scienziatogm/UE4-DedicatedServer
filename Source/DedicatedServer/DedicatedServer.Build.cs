// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class DedicatedServer : ModuleRules
{
	public DedicatedServer( ReadOnlyTargetRules Target ) : base( Target )
	{
        //PublicIncludePaths.AddRange( new string[] { "DedicatedServer/Public" } );
        //PrivateIncludePaths.AddRange( new string[] { "DedicatedServer/Private" } );

        PrivatePCHHeaderFile = "Private/DedicatedServerPrivatePCH.h";

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string IncludesPath = Path.Combine(ModuleDirectory, "../ThirdParty/pdcurses/include");
            string LibrariesPath = Path.Combine(ModuleDirectory, "../ThirdParty/pdcurses/lib");
            string LibrariesPathArch = String.Format(LibrariesPath + "/{0}", Target.Architecture);
            PublicIncludePaths.Add(IncludesPath);
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPathArch, "libpdcurses.a"));
        }

        PublicDependencyModuleNames.AddRange( new string[] { "Core", "ApplicationCore", "CoreUObject", "Engine", "InputCore", "HTTP", "Json", "OnlineSubsystem", "OnlineSubsystemSteam" } );
		PrivateDependencyModuleNames.AddRange( new string[] {  } );
	}
}
