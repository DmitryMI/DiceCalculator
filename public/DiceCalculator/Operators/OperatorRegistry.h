#pragma once

#include <map>
#include <string_view>
#include <functional>
#include <stdexcept>

namespace DiceCalculator::Expressions
{
    class DiceAst;
}

namespace DiceCalculator::Operators
{
    class DiceOperator;

	class OperatorRegistry
	{
    public:
        using Factory = std::function<std::shared_ptr<DiceOperator>()>;

        static OperatorRegistry& Instance()
        {
            static OperatorRegistry instance;
            return instance;
        }

        void Register(std::string_view name, Factory factory)
        {
            auto [it, inserted] = factories.emplace(name, std::move(factory));
            if (!inserted)
            {
                throw std::runtime_error("Function already registered: " + std::string(name));
            }
        }

        std::shared_ptr<DiceOperator> Create(std::string_view name) const
        {
            auto it = factories.find(name);
            if (it == factories.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name));
            }
            return it->second();
        }

    private:
        std::unordered_map<std::string_view, Factory> factories;
	};

    template <typename Derived>
    struct AutoRegister
    {
        static bool registered;
        static std::vector<std::pair<std::string_view, OperatorRegistry::Factory>> Register();
    };

    template<typename Derived>
    bool AutoRegister<Derived>::registered = [] {
        auto entries = Derived::Register();
        for (const auto& entry : entries)
        {
            const auto& name = entry.first;
			const auto& instantiator = entry.second;
            OperatorRegistry::Instance().Register(
                name,
                instantiator
            );
        }
        return true;
        }();

}