// Copyright RemRemRemRe. 2026. All Rights Reserved.

using UnrealBuildTool;
using Rem.BuildRule;

public class RemSpringItOnTest : ModuleRules
{
	public RemSpringItOnTest(ReadOnlyTargetRules target) : base(target)
	{
		RemSharedModuleRules.Apply(this);

		PrivateDependencyModuleNames.AddRange(
			[
				"Core",
				"CoreUObject",
				"Engine",

				"RemCommon",
				"RemSpringItOn",
				"SpringItOn",
			]
		);
	}
}
