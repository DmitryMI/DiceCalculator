#pragma once

#include <string>
#include <string_view>
#include <functional>

namespace DiceCalculator::Expressions
{
    class DiceAst;
}

namespace DiceCalculator::Operators
{
    class DiceOperator;
    using Factory = std::function<std::shared_ptr<DiceOperator>()>;

    enum class Arity
    {
        Zero = 0,
        Unary = 1,
        Binary = 2,
        Function = 3
    };

	struct RegistryEntry
    {
        std::string Name;
        Arity Arity;
        std::function<std::shared_ptr<DiceOperator>()> FactoryFunc;
    };

    class IRegistry
    {
    public:
        
		virtual void Register(RegistryEntry entry) = 0;

		virtual std::shared_ptr<DiceOperator> Create(std::string_view name, Arity arity) const = 0;

        virtual const RegistryEntry& GetEntry(std::string_view name, Arity arity) const = 0;

        virtual std::vector<RegistryEntry> GetOperatorsByArity(Arity arity) const = 0;
    };
}