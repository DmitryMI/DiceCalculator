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

        enum class Arity
        {
            Nary,
            Unary,
            Binary,
        };

        struct Entry
        {
            std::string Name;
            Arity Arity;
            std::function<std::shared_ptr<DiceOperator>()> FactoryFunc;
        };

        static OperatorRegistry& Instance()
        {
            static OperatorRegistry instance;
            return instance;
        }

        void Register(Entry entry)
        {
            auto [it, inserted] = m_Registry.emplace(entry.Name, std::move(entry));
            if (!inserted)
            {
                throw std::runtime_error("Function already registered: " + std::string(entry.Name));
            }
        }

        std::shared_ptr<DiceOperator> Create(std::string_view name) const
        {
            auto it = m_Registry.find(std::string(name));
            if (it == m_Registry.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name));
            }
            return it->second.FactoryFunc();
        }

        const Entry& GetEntry(std::string_view name) const
        {
            auto it = m_Registry.find(std::string(name));
            if (it == m_Registry.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name));
            }
            return it->second;
		}

    private:
        std::unordered_map<std::string, Entry> m_Registry;
	};

    template <typename Derived>
    struct AutoRegister
    {
        static bool registered;
        static std::vector<OperatorRegistry::Entry> Register();
    };

    template<typename Derived>
    bool AutoRegister<Derived>::registered = [] {
        auto entries = Derived::Register();
        for (const auto& entry : entries)
        {
            OperatorRegistry::Instance().Register(
                entry
            );
        }
        return true;
        }();

}