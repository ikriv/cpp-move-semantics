#pragma once

#include <iostream>

namespace move_semantics
{
	// forward declarations
	class logger;

	inline std::ostream& operator<<(std::ostream& stream, logger const& logger);

	template<typename T>
	std::ostream& operator<<(logger const& logger, T&& value);

	class logger
	{
	private:
		int n_indent;
		std::ostream& m_stream;

		// forbid copying or constructing loggers
		logger(logger const& other) : m_stream(other.m_stream) {}

	public:

		explicit logger(std::ostream& stream) : n_indent(0), m_stream(stream) {}

		int indent_value() const { return n_indent; }
		std::ostream& stream() const { return m_stream; }

		class indent_scope
		{
		private:
			logger& m_logger;
			int m_delta;
			std::string m_name;

			indent_scope(logger& logger, std::string const& name, int delta)
				:
				m_logger(logger),
				m_delta(delta),
				m_name(name)
			{
				if (name.size())
				{
					logger.stream() << logger << "<" << name << ">" << std::endl;
				}
				m_logger.n_indent += delta;
			}

			friend class ::move_semantics::logger;

		public:
			indent_scope(indent_scope&& other)
				:
				m_logger(other.m_logger),
				m_delta(other.m_delta)
			{
				other.m_delta = 0;
			}

			~indent_scope()
			{
				m_logger.n_indent -= m_delta;
				if (m_name.size())
				{
					m_logger.stream() << m_logger << "</" << m_name << ">" << std::endl;
				}
			}

			logger const& logger() const { return m_logger; }
		}; // end indent

		indent_scope indent(int delta = 0)
		{
			return indent_scope(*this, "", delta);
		}

		indent_scope indent(std::string const& name, int delta = 0)
		{
			return indent_scope(*this, name, delta);
		}

	};

	inline std::ostream& operator<<(std::ostream& stream, logger const& logger)
	{
		auto indent_value = logger.indent_value();
		if (indent_value > 0) stream << std::string(indent_value, ' ');
		return stream;
	}

	inline std::ostream& operator<<(std::ostream& stream, logger::indent_scope const& scope)
	{
		stream << scope.logger();
		return stream;
	}

	template<typename T>
	inline std::ostream& operator<<(logger const& logger, T&& value)
	{
		logger.stream() << std::forward<T>(value);
		return logger.stream();
	}

	extern logger log;

}
