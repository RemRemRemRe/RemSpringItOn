// Copyright RemRemRemRe. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class SpringItOn : ModuleRules
{
	public SpringItOn(ReadOnlyTargetRules target) : base(target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "SpringItOn"));

		PublicDefinitions.Add("SPRING_IT_ON_FOR_UNREAL=1");
	}
}
