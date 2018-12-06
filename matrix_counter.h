#pragma once

namespace move_semantics
{
	class matrix_counter
	{
		static int _matrices_created;
	public:
		static int matrices_created() { return _matrices_created; }
		template<typename T> friend class matrix;
	};
}

