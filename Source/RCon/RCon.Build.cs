// Copyright 2004-2016 YaS-Online, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class RCon : ModuleRules
{
	public RCon( ReadOnlyTargetRules Target ) : base( Target )
	{
        //PublicIncludePaths.AddRange( new string[] { "RCon/Public" } );
        //PrivateIncludePaths.AddRange( new string[] { "RCon/Private" } );

        PrivatePCHHeaderFile = "Private/RConPrivatePCH.h";

		PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "InputCore" } );
		PrivateDependencyModuleNames.AddRange( new string[] {  } );
	}
}
