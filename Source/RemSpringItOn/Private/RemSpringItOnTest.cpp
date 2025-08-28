// Copyright RemRemRemRe, All Rights Reserved.

#include "common.h"

namespace
{

#if !UE_BUILD_SHIPPING

	// ReSharper disable once CppDeclaratorNeverUsed
	void CompileTest()
	{
		damper_exponential(1,0,0,0);

		float x{0.0f};
		float y{0.0f};

		spring_damper_exact_stiffness_damping(x, y, 1, 1, 1, 1, 1);
		spring_damper_exact(x, y, 1, 1, 1, 1, 1);
		spring_damper_exact_ratio(x, y, 1, 1, 1, 1, 1);

		critical_spring_damper_exact(x, y, 1, 1, 1, 1);
		simple_spring_damper_exact(x, y, 1, 1, 1);
		decay_spring_damper_exact(x, y, 1, 1);
	}

#endif

}
