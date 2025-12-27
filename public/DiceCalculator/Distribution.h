#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <initializer_list>

namespace DiceCalculator
{
	class Distribution
	{
	public:
		struct Outcome
		{
			int Value;
			double Probability;
			int D20;
		};

		Distribution() = default;

		Distribution(std::initializer_list<std::pair<int, double>> init)
		{
			AssignFrom(init);
		}

		Distribution& operator=(std::initializer_list<std::pair<int, double>> init)
		{
			AssignFrom(init);
			return *this;
		}

		using iterator = std::vector<Outcome>::iterator;
		using const_iterator = std::vector<Outcome>::const_iterator;

		// Insert or accumulate probability for `value`. Keeps entries sorted by value.
		void AddOutcome(int value, double probability, int d20)
		{
			if (probability == 0.0)
				return;

			auto it = std::lower_bound(m_Data.begin(), m_Data.end(), value,
				[](auto const& lhs, int v) { return lhs.Value < v; });

			if (it != m_Data.end() && it->Value == value)
			{
				it->D20 = d20;
				it->Probability += probability;
				if (it->Probability == 0.0)
				{
					m_Data.erase(it);
				}
			}
			else
			{
				m_Data.insert(it, { value, probability, d20 });
			}
		}

		// Mutable operator[]: ensures an entry exists and returns a reference to its probability.
		// Inserts with probability 0.0 if the key is missing.
		Outcome& operator[](int value)
		{
			auto it = std::lower_bound(m_Data.begin(), m_Data.end(), value,
				[](auto const& lhs, int v) { return lhs.Value < v; });

			if (it != m_Data.end() && it->Value == value)
			{
				return *it;
			}

			it = m_Data.insert(it, { value, 0.0, 0 });
			return *it;
		}

		// Const operator[]: returns the probability for `value` or 0.0 if missing.
		Outcome operator[](int value) const noexcept
		{
			auto it = std::lower_bound(m_Data.begin(), m_Data.end(), value,
				[](auto const& lhs, int v) { return lhs.Value < v; });

			if (it != m_Data.end() && it->Value == value)
			{
				return *it;
			}

			return {value, 0.0, 0 };
		}

		// Iteration support (flat-map iteration over pairs)
		iterator begin() noexcept { return m_Data.begin(); }
		iterator end() noexcept { return m_Data.end(); }
		const_iterator begin() const noexcept { return m_Data.begin(); }
		const_iterator end() const noexcept { return m_Data.end(); }
		const_iterator cbegin() const noexcept { return m_Data.cbegin(); }
		const_iterator cend() const noexcept { return m_Data.cend(); }

		// Read-only access to the flat-map data (sorted unique keys).
		const std::vector<Outcome>& GetData() const noexcept
		{
			return m_Data;
		}

		// Mutable access when callers need to modify directly.
		std::vector<Outcome>& GetData() noexcept
		{
			return m_Data;
		}

		// Normalize probabilities so they sum to 1. No-op when sum is 0.
		void Normalize()
		{
			double sum = 0.0;
			for (auto const& p : m_Data) sum += p.Probability;
			if (sum == 0.0) return;
			for (auto &p : m_Data) p.Probability /= sum;
		}

		void Clear() noexcept { m_Data.clear(); }
		size_t Size() const noexcept { return m_Data.size(); }
		void Reserve(size_t n) { m_Data.reserve(n); }

	private:
		// Sorted vector of (value, probability) acting as a flat map.
		std::vector<Outcome> m_Data;

		void AssignFrom(const std::initializer_list<std::pair<int, double>>& r)
		{
			m_Data.clear();
			// Copy into temporary to allow sorting/merging
			std::vector<Outcome> tmp;
			tmp.reserve(std::distance(std::begin(r), std::end(r)));
			for (auto const& p : r)
			{
				tmp.emplace_back(p.first, p.second, 0);
			}
			if (tmp.empty())
			{
				return;
			}

			std::sort(tmp.begin(), tmp.end(),
				[](auto const& a, auto const& b) { return a.Value < b.Value; });

			// merge duplicates and skip zero probabilities
			for (auto const& p : tmp)
			{
				if (p.Probability == 0.0)
				{
					continue;
				}
				if (!m_Data.empty() && m_Data.back().Value == p.Value)
				{
					m_Data.back().Probability += p.Probability;
					if (m_Data.back().Probability == 0.0)
						m_Data.pop_back();
				}
				else
				{
					m_Data.emplace_back(p);
				}
			}
		}
	};
}