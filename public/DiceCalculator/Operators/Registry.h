#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <memory>
#include "DiceCalculator/Operators/IRegistry.h"
#include "DiceCalculator/Operators/Addition.h"
#include "DiceCalculator/Operators/Subtraction.h"
#include "DiceCalculator/Operators/Advantage.h"
#include "DiceCalculator/Operators/AttackRoll.h"

namespace DiceCalculator::Expressions
{
    class DiceAst;
}

namespace DiceCalculator::Operators
{
    class DiceOperator;

	class Registry : public IRegistry
	{
    public:
        
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

        Registry()
        {
			Register(Addition::Register());
			Register(Subtraction::Register());
			Register(Advantage::Register());
			Register(AttackRoll::Register());
        }

        void Register(const std::vector<RegistryEntry>& entries)
        {
			for (const auto& entry : entries)
            {
                Register(entry);
            }
        }

        // Register by entry (uses composite key)
        void Register(RegistryEntry entry) override
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

        const RegistryEntry& GetEntry(std::string_view name, Arity arity) const
        {
            Key key{ std::string(name), arity };
            auto it = m_Registry.find(key);
            if (it == m_Registry.end())
            {
                throw std::runtime_error("Unknown function: " + std::string(name) + " with Arity " + std::to_string(static_cast<int>(arity)));
            }
            return it->second;
		}

        std::vector<RegistryEntry> GetOperatorsByArity(Arity arity) const
        {
            std::vector<RegistryEntry> result;
            for (const auto& [key, entry] : m_Registry)
            {
                if (key.Arity == arity)
                {
                    result.push_back(entry);
                }
            }
            return result;
		}

    private:
        std::unordered_map<Key, RegistryEntry, KeyHash> m_Registry;
	};

}