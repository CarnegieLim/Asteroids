// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Asteroids : ModuleRules
{
	//public Asteroids(ReadOnlyTargetRules Target) : base(Target)
	//{
	//	PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

	//	PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ApexDestruction", "HeadMountedDisplay", "UMG", "Slate", "SlateCore" });
	//}
	//public Asteroids(TargetInfo Target)
	//{
	//	// PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

	//	PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ApexDestruction", "UMG" });
	//	PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	//}

	public Asteroids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ApexDestruction", "UMG", "Slate", "SlateCore", "HeadMountedDisplay" });
	}
}
