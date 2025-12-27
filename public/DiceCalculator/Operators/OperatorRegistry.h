#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <memory>

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
			Zero = 0,
            Unary = 1,
            Binary = 2,
            Function = 3
        };

        struct Entry
        {
            std::string Name;
            Arity Arity;
            std::function<std::shared_ptr<DiceOperator>()> FactoryFunc;
        };

        // Composite key combining name + arity
        struct Key
        {
            std::string Name;
            Arity Arity;

            bool operator==(Key const& other) const noexcept
            {
                return Name == other.Name && Arity == other.Arity;
            }
        };

        struct KeyHash
        {
            std::size_t operator()(Key const& k) const noexcept
            {
                // Combine string hash and arity using a variation of boost::hash_combine
                const std::size_t h1 = std::hash<std::string_view>{}(k.Name);
                const std::size_t h2 = static_cast<std::size_t>(k.Arity);
                return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
            }
        };

        static OperatorRegistry& Instance()
        {
            static OperatorRegistry instance;
            return instance;
        }

        // Register by entry (uses composite key)
        void Register(Entry entry)
        {
            Key key{ entry.Name, entry.Arity };
            auto [it, inserted] = m_Registry.emplace(std::move(key), std::move(entry));
            if (!inserted)
            {
                throw std::runtime_error("Function already registered: " + it->second.Name);
            }
        }

        // Create by explicit name + arity
        std::shared_ptr<DiceOperator> Create(std::string_view name, Arity arity) const
        {
            Key key{ std::string(name), arity };
            auto it = m_Registry.find(key);
            if (it == m_Registry.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name));
            }
            return it->second.FactoryFunc();
        }

        const Entry& GetEntry(std::string_view name, Arity arity) const
        {
            Key key{ std::string(name), arity };
            auto it = m_Registry.find(key);
            if (it == m_Registry.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name) + " with Arity " + std::to_string(static_cast<int>(arity)));
            }
            return it->second;
		}

        std::vector<Entry> GetUnaryOperators() const
        {
            std::vector<Entry> result;
            for (const auto& [key, entry] : m_Registry)
            {
                if (key.Arity == Arity::Unary)
                {
                    result.push_back(entry);
                }
            }
            return result;
		}

        std::vector<Entry> GetBinaryOperators() const
        {
            std::vector<Entry> result;
            for (const auto& [key, entry] : m_Registry)
            {
                if (key.Arity == Arity::Binary)
                {
                    result.push_back(entry);
                }
            }
            return result;
        }

        std::vector<Entry> GetFunctions() const
        {
            std::vector<Entry> result;
            for (const auto& [key, entry] : m_Registry)
            {
                if (key.Arity == Arity::Function)
                {
                    result.push_back(entry);
                }
            }
            return result;
        }

    private:
        std::unordered_map<Key, Entry, KeyHash> m_Registry;
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