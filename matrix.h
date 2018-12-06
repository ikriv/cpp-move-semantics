#pragma once

#include "logger.h"
#include "matrix_counter.h"

namespace move_semantics
{
	template<typename T>
	class matrix
	{
		std::string m_name;
		T* m_data;
		int m_sizeX;
		int m_sizeY;

		void copy_from(matrix const& other)
		{
			auto indent = log.indent(2);
			m_name = "copy of " + other.name();
			m_sizeX = other.sizeX();
			m_sizeY = other.sizeY();
			m_data = new int[other.size()];
			std::copy(other.begin(), other.end(), m_data);
			log << indent << "[" << other.name() << "] copied to [" << name() << "]" << std::endl;
		}

		void destroy()
		{
			m_sizeX = m_sizeY = 0;
			delete[] m_data;
			m_data = nullptr;
		}

#ifdef MOVE_SEMANTICS
		void move_from(matrix&& other)
		{
			auto indent = log.indent(2);
			m_name = "moved from " + other.name();
			m_sizeX = other.sizeX();
			m_sizeY = other.sizeY();
			m_data = other.m_data;
			other.m_data = nullptr;
			log << indent << "[" << other.name() << "] moved to [" << name() << "]" << std::endl;
		}
#endif

	public:
		matrix(char const* name, int sizeX, int sizeY)
			:
			m_name(name),
			m_sizeX(sizeX),
			m_sizeY(sizeY),
			m_data(new T[sizeX*sizeY])
		{
			auto indent = log.indent();
			++matrix_counter::_matrices_created;
			log << indent << "<Matrix constructor> " << name << "(" << m_sizeX << "*" << m_sizeY << ") </Matrix constructor>" << std::endl;
		}

		matrix(matrix const& other)
		{
			auto indent = log.indent("Matrix copy constructor", 2);
			++matrix_counter::_matrices_created;
			copy_from(other);
		}

		matrix& operator=(matrix const& other)
		{
			auto indent = log.indent("<Matrix assignment operator>", 2);
			destroy();
			copy_from(other);
			return *this;
		}

#ifdef MOVE_SEMANTICS
		matrix(matrix&& other)
		{
			auto indent = log.indent("Matrix move constructor", 2);
			move_from(move(other));
		}

		matrix& operator=(matrix<T>&& other)
		{
			auto indent = log.indent("Matrix move assignment operator", 2);
			destroy();
			move_from(move(other));
			return *this;
		}
#endif

		matrix& operator+=(matrix const& other)
		{
			log << log << "operator+=";

			if (other.sizeX() != sizeX() || other.sizeY() != sizeY())
			{
				ostringstream message;
				message << "Cannot add matrices with sizes " << sizeX() << "*" << sizeY() << " and " << other.sizeX() << "*" << other.sizeY();
				throw logic_error(message.str());
			}

			const int sz = size();
			for (int i = 0; i < sz; ++i) m_data[i] += other.m_data[i];

			return (*this);
		}

		~matrix()
		{
			auto indent = log.indent("Matrix destructor", 2);

			{
				log << indent << "deleting [" << m_name << "]" << std::endl;
				if (m_data == nullptr)
				{
					log << indent << "no data" << std::endl;
				}
				else
				{
					log << indent << "Deallocating data: " << m_sizeX << "*" << m_sizeY << std::endl;
					destroy();
				}
			}
		}

		int sizeX() const { return m_sizeX; }
		int sizeY() const { return m_sizeY; }
		int size() const { return m_sizeX*m_sizeY; }
		std::string name() const { return m_name; }

		T& operator()(int x, int y)
		{
			return m_data[y*m_sizeX + x];
		}

		T const& operator()(int x, int y) const
		{
			return m_data[y*m_sizeX + x];
		}

		T* begin() { return m_data; }
		T* end() { return m_data + size(); }
		T const* begin() const { return m_data; }
		T const* end() const { return m_data + size(); }
	};

	template<class T>
	matrix<T> operator+(matrix<T> const& left, matrix<T> const& right)
	{
		auto indent = log.indent("operator +(lvalue,lvalue)", 2);
		auto result = left;
		result += right;
		return result;
	}

#ifdef MOVE_SEMANTICS 

	template<class T>
	matrix<T> operator+(matrix<T>&& left, matrix<T> const& right)
	{
		auto indent = log.indent("operator +(rvalue,lvalue)", 2);
		return std::move(left += right);
	}

	template<class T>
	matrix<T> operator+(matrix<T> const& left, matrix<T>&& right)
	{
		auto indent = log.indent("operator +(lvalue,rvalue)", 2);
		return std::move(right += left);
	}

	template<class T>
	matrix<T> operator+(matrix<T>&& left, matrix<T>&& right)
	{
		auto indent = log.indent("operator +(rvalue,rvalue)", 2);
		return std::move(left += right);
	}

#endif

	template<class T>
	std::ostream& operator << (std::ostream& output, matrix<T> const& m)
	{
		output << m.name() << ":" << std::endl;
		for (int i = 0; i < m.sizeX(); ++i)
		{
			for (int j = 0; j < m.sizeY(); ++j)
			{
				output << m(i, j) << " ";
			}
			output << std::endl;
		}
		return output;
	}

	template<typename T>
	matrix<T> create_matrix(char const* name, int sizeX, int sizeY, std::function<T && (int, int)> getElement)
	{
		auto indent = log.indent("create_matrix", 2);

		auto m = matrix<int>(name, sizeX, sizeY);

		for (int j = 0; j < m.sizeY(); ++j)
			for (int i = 0; i < m.sizeX(); ++i)
			{
				m(i, j) = move(getElement(i, j));
			}

		return m;
	}

	template<typename T>
	matrix<T> fill_matrix(char const* name, int sizeX, int sizeY, T const& value)
	{
		return create_matrix<int>(name, sizeX, sizeY, [value](int, int) {return value; });
	}
}

