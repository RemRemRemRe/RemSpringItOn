// Copyright RemRemRemRe. 2026. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "common.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
constexpr auto StepCount = 512;

/** Advances a spring state with fixed small steps and returns the final position. */
template <typename TStep>
float Settle(TStep&& Step)
{
    auto Position = 0.0f;
    auto Velocity = 0.0f;

    for (int32 Index = 0; Index < StepCount; ++Index)
    {
        Step(Position, Velocity);
    }

    return Position;
}

void ExpectNear(FAutomationTestBase& Test, const TCHAR* const Description, const float Actual,
    const float Expected, const float Tolerance)
{
    Test.TestEqual(Description, Actual, Expected, Tolerance);
}
}

DEFINE_SPEC(FRemSpringItOnStaticsTest, "Rem.SpringItOn.Math",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter);

void FRemSpringItOnStaticsTest::Define()
{
    Describe(TEXT("Damper"), [this]
    {
        It(TEXT("lerp should interpolate between the endpoints"), [this]
        {
            ExpectNear(*this, TEXT("at zero"), damper(2.0f, 8.0f, 0.0f), 2.0f, 1e-5f);
            ExpectNear(*this, TEXT("at one"), damper(2.0f, 8.0f, 1.0f), 8.0f, 1e-5f);
            ExpectNear(*this, TEXT("midpoint"), damper(2.0f, 8.0f, 0.5f), 5.0f, 1e-5f);
        });

        It(TEXT("damper_exponential should converge toward the goal"), [this]
        {
            auto Position = 0.0f;
            for (int32 Index = 0; Index < StepCount; ++Index)
            {
                Position = damper_exponential(Position, 1.0f, 8.0f, 1.0f / 60.0f);
            }

            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("damper_exponential should not move with zero delta time"), [this]
        {
            ExpectNear(*this, TEXT("unchanged"), damper_exponential(0.25f, 1.0f, 8.0f, 0.0f), 0.25f, 1e-5f);
        });

        It(TEXT("damper_exact should halve the remaining distance after one halflife"), [this]
        {
            // fast_negexp(ln2) approximates 0.5: the distance must halve after dt == halflife
            const auto Half = damper_exact(0.0f, 1.0f, 0.5f, 0.5f);

            ExpectNear(*this, TEXT("half distance"), Half, 0.5f, 1e-2f);
        });

        It(TEXT("damper_decay_exact should decay the value toward zero"), [this]
        {
            const auto Half = damper_decay_exact(1.0f, 0.5f, 0.5f);

            ExpectNear(*this, TEXT("half value"), Half, 0.5f, 1e-2f);
        });
    });

    Describe(TEXT("Approximations"), [this]
    {
        It(TEXT("fast_negexp should be one at zero and decrease monotonically"), [this]
        {
            ExpectNear(*this, TEXT("at zero"), fast_negexp(0.0f), 1.0f, 1e-5f);
            TestTrue(TEXT("positive"), fast_negexp(2.0f) > 0.0f);
            TestTrue(TEXT("monotonic decrease"), fast_negexp(1.0f) < fast_negexp(0.5f));
            TestTrue(TEXT("monotonic decrease"), fast_negexp(2.0f) < fast_negexp(1.0f));
        });

        It(TEXT("fast_atan should be zero at zero, odd and monotonically increasing"), [this]
        {
            ExpectNear(*this, TEXT("at zero"), fast_atan(0.0f), 0.0f, 1e-5f);
            ExpectNear(*this, TEXT("odd symmetry"), fast_atan(-0.7f), -fast_atan(0.7f), 1e-5f);
            ExpectNear(*this, TEXT("quarter pi at one"), fast_atan(1.0f), static_cast<float>(UE_PI / 4.0), 1e-2f);
            TestTrue(TEXT("monotonic increase"), fast_atan(0.5f) < fast_atan(1.0f));
        });
    });

    Describe(TEXT("Conversions"), [this]
    {
        It(TEXT("halflife and damping conversions should round-trip"), [this]
        {
            const auto Halflife = 0.25f;
            const auto Damping  = halflife_to_damping(Halflife);

            ExpectNear(*this, TEXT("damping to halflife"), damping_to_halflife(Damping), Halflife, 1e-4f);
            ExpectNear(*this, TEXT("halflife to damping"), halflife_to_damping(damping_to_halflife(Damping)), Damping,
                1e-3f);
        });

        It(TEXT("frequency and stiffness conversions should round-trip"), [this]
        {
            const auto Frequency = 1.5f;
            const auto Stiffness = frequency_to_stiffness(Frequency);

            ExpectNear(*this, TEXT("stiffness to frequency"), stiffness_to_frequency(Stiffness), Frequency, 1e-4f);
            ExpectNear(*this, TEXT("frequency to stiffness"), frequency_to_stiffness(stiffness_to_frequency(Stiffness)),
                Stiffness, 1e-2f);
        });

        It(TEXT("critical halflife and frequency should describe the same critical spring"), [this]
        {
            const auto Frequency = 2.0f;

            // critical damping condition: damping^2 == 4 * stiffness (within the
            // approximation error of the fast math helpers)
            const auto Stiffness = frequency_to_stiffness(Frequency);
            const auto Damping   = halflife_to_damping(critical_halflife(Frequency));

            ExpectNear(*this, TEXT("critical condition"), Damping * Damping, 4.0f * Stiffness, 0.5f);

            // and the inverse conversion round-trips
            ExpectNear(*this, TEXT("round-trip"), critical_frequency(critical_halflife(Frequency)), Frequency, 1e-2f);
        });

        It(TEXT("damping ratio conversions should round-trip"), [this]
        {
            const auto Ratio     = 0.5f;
            const auto Stiffness = 40.0f;
            const auto Damping   = damping_ratio_to_damping(Ratio, Stiffness);

            ExpectNear(*this, TEXT("ratio to stiffness"), damping_ratio_to_stiffness(Ratio, Damping), Stiffness, 1e-2f);
            ExpectNear(*this, TEXT("ratio to damping"), damping_ratio_to_damping(Ratio,
                damping_ratio_to_stiffness(Ratio, Damping)), Damping, 1e-2f);
        });

        It(TEXT("lag and halflife conversions should round-trip"), [this]
        {
            const auto Halflife = 0.33f;
            const auto Lag      = halflife_to_lag(Halflife);

            ExpectNear(*this, TEXT("lag to halflife"), lag_to_halflife(Lag), Halflife, 1e-5f);
            ExpectNear(*this, TEXT("halflife to lag"), halflife_to_lag(lag_to_halflife(Lag)), Lag, 1e-5f);
        });
    });

    Describe(TEXT("Springs"), [this]
    {
        It(TEXT("spring_damper_exact should settle at the goal"), [this]
        {
            const auto Position = Settle([&](float& X, float& V)
            {
                spring_damper_exact(X, V, 1.0f, 0.0f, 2.0f, 0.3f, 1.0f / 60.0f);
            });

            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("spring_damper_exact with critical damping should not overshoot"), [this]
        {
            const auto Frequency = 2.0f;
            const auto Halflife  = critical_halflife(Frequency);

            auto Position = 0.0f;
            auto Velocity = 0.0f;
            bool Overshoot = false;
            for (int32 Index = 0; Index < StepCount; ++Index)
            {
                spring_damper_exact(Position, Velocity, 1.0f, 0.0f, Frequency, Halflife, 1.0f / 60.0f);
                Overshoot |= Position > 1.0f + 1e-3f;
            }

            TestFalse(TEXT("no overshoot"), Overshoot);
            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("spring_damper_exact_stiffness_damping should settle at the goal"), [this]
        {
            const auto Position = Settle([&](float& X, float& V)
            {
                spring_damper_exact_stiffness_damping(X, V, 1.0f, 0.0f, 120.0f, 20.0f, 1.0f / 60.0f);
            });

            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("spring_damper_exact_ratio should settle at the goal"), [this]
        {
            const auto Position = Settle([&](float& X, float& V)
            {
                spring_damper_exact_ratio(X, V, 1.0f, 0.0f, 0.8f, 0.3f, 1.0f / 60.0f);
            });

            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("critical_spring_damper_exact should settle at the goal without overshooting"), [this]
        {
            auto Position  = 0.0f;
            auto Velocity  = 0.0f;
            bool Overshoot = false;
            for (int32 Index = 0; Index < StepCount; ++Index)
            {
                critical_spring_damper_exact(Position, Velocity, 1.0f, 0.0f, 0.2f, 1.0f / 60.0f);
                Overshoot |= Position > 1.0f + 1e-3f;
            }

            TestFalse(TEXT("no overshoot"), Overshoot);
            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("simple_spring_damper_exact should settle at the goal"), [this]
        {
            const auto Position = Settle([&](float& X, float& V)
            {
                simple_spring_damper_exact(X, V, 1.0f, 0.2f, 1.0f / 60.0f);
            });

            ExpectNear(*this, TEXT("settled"), Position, 1.0f, 1e-3f);
        });

        It(TEXT("decay_spring_damper_exact should decay the state toward zero"), [this]
        {
            auto Position = 1.0f;
            auto Velocity = 0.5f;
            for (int32 Index = 0; Index < StepCount; ++Index)
            {
                decay_spring_damper_exact(Position, Velocity, 0.2f, 1.0f / 60.0f);
            }

            ExpectNear(*this, TEXT("position zero"), Position, 0.0f, 1e-3f);
            ExpectNear(*this, TEXT("velocity zero"), Velocity, 0.0f, 1e-3f);
        });

        It(TEXT("spring_damper_bad should move the position toward the goal"), [this]
        {
            auto Position = 0.0f;
            auto Velocity = 0.0f;
            spring_damper_bad(Position, Velocity, 1.0f, 0.0f, 40.0f, 10.0f, 1.0f / 60.0f);

            TestTrue(TEXT("moved toward goal"), Position > 0.0f);
        });
    });
}

#endif // WITH_DEV_AUTOMATION_TESTS
