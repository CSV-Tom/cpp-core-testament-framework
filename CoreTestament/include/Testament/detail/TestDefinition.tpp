namespace Testament::detail {

template <typename Callable>
TestDefinition<Callable>::TestDefinition(std::string name, std::source_location location,
                                         TestOptions options, Callable callable)
    : mName(std::move(name)), mLocation(location), mOptions(std::move(options)),
      mCallable(std::move(callable)) {}

template <typename Callable>
template <FixtureSelection Fixture>
requires TestBodyCompatible<Fixture, Callable>::value
std::vector<TestHandle> TestDefinition<Callable>::materialize() && {
    std::vector<TestHandle> tests;
    tests.reserve(1);
    if constexpr (std::same_as<Fixture, void>) {
        tests.push_back(RuntimeBridge::makeTest(
            mName, std::move(mOptions),
            MoveOnlyFunction<void()>{std::move(mCallable)}, mLocation
        ));
    } else {
        tests.push_back(RuntimeBridge::makeTest(
            mName, std::move(mOptions), std::type_index(typeid(Fixture)),
            MoveOnlyFunction<void(LifecycleSuite&)>{
                [callable = std::move(mCallable)](LifecycleSuite& fixture) mutable {
                    auto* typedFixture = dynamic_cast<Fixture*>(&fixture);
                    if (!typedFixture) {
                        throw std::logic_error("Internal fixture type mismatch");
                    }
                    std::invoke(callable, *typedFixture);
                }
            }, mLocation
        ));
    }
    return tests;
}

}
