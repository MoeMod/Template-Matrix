#include <iostream>
using namespace std;

template<int...Elems>
struct meta_vector {};

template<class T, int n>
struct meta_vector_get;
template<int First, int...Others>
struct meta_vector_get<meta_vector<First, Others...>, 0>
{
	static const int value = First;
};
template<int First, int...Others, int n>
struct meta_vector_get<meta_vector<First, Others...>, n>
{
	static const int value = meta_vector_get<meta_vector<Others...>, n-1>::value;
};

template<class L, class R>
struct meta_vector_cat;
template<int...Elems1, int...Elems2>
struct meta_vector_cat<meta_vector<Elems1...>, meta_vector<Elems2...>>
{
	using type = meta_vector<Elems1..., Elems2...>;
};

template<class T, int c>
struct meta_vector_erase;
template<int First, int...Other>
struct meta_vector_erase<meta_vector<First, Other...>, 0>
{
	using type = meta_vector<Other...>;
};
template<int First, int...Other, int n>
struct meta_vector_erase<meta_vector<First, Other...>, n>
{
	using type = typename meta_vector_cat<
		meta_vector<First>,
		typename meta_vector_erase<meta_vector<Other...>, n - 1>::type
	>::type;
};

template<class a, class b>
struct meta_vector_add;
template<>
struct meta_vector_add<meta_vector<>, meta_vector<>>
{
	using type = meta_vector<>;
};
template<int First1, int First2, int...Elems1, int...Elems2>
struct meta_vector_add<meta_vector<First1, Elems1...>, meta_vector<First2, Elems2...>>
{
	using type = typename meta_vector_cat <
		meta_vector<First1 + First2>,
		typename meta_vector_add< meta_vector<Elems1...>, meta_vector<Elems2...> >::type
	>::type;
};

template<class a, class b>
struct meta_vector_dot;
template<>
struct meta_vector_dot<meta_vector<>, meta_vector<>>
{
	static const int value = 0;
};
template<int First1, int First2, int...Elems1, int...Elems2>
struct meta_vector_dot<meta_vector<First1, Elems1...>, meta_vector<First2, Elems2...>>
{
	static const int value = First1 * First2 + meta_vector_dot<meta_vector<Elems1...>, meta_vector<Elems2...>>::value;
};

template<class a, int k>
struct meta_vector_muln;
template<int k>
struct meta_vector_muln<meta_vector<>, k>
{
	using type = meta_vector<>;
};
template<int k, int First2, int...Elems2>
struct meta_vector_muln<meta_vector<First2, Elems2...>, k>
{
	using type = typename meta_vector_cat< meta_vector<k*First2>, typename meta_vector_muln<meta_vector<Elems2...>, k>::type>::type;
};

template<class a, int k>
struct meta_vector_divn;
template<int k, int First2>
struct meta_vector_divn<meta_vector<First2>, k>
{
	using type = meta_vector<First2 / k>;
};
template<int k, int First2, int...Elems2>
struct meta_vector_divn<meta_vector<First2, Elems2...>, k>
{
	using type = typename meta_vector_cat< 
		typename meta_vector_divn<meta_vector<First2>, k>::type,
		typename meta_vector_divn<meta_vector<Elems2...>, k>::type
	>::type;
};

template<class...Vectors>
struct meta_matrix;
template<int...Elems1, class...Other>
struct meta_matrix<meta_vector<Elems1...>, Other...>{};

template<class A, class B>
struct meta_matrix_cat;
template<class...Vecs1, class...Vecs2>
struct meta_matrix_cat<meta_matrix<Vecs1...>, meta_matrix<Vecs2...>>
{
	using type = meta_matrix<Vecs1..., Vecs2... >;
};

template<class T>
struct meta_matrix_count_cols;
template<>
struct meta_matrix_count_cols<meta_matrix<>>
{
	static const int value = 0;
};
template<int...Elems1, class...Other>
struct meta_matrix_count_cols<meta_matrix<meta_vector<Elems1...>, Other...>>
{
	static const int value = meta_matrix_count_cols<meta_matrix<Other...>>::value + 1;
};

template<class T>
struct meta_matrix_count_rows;
template<class...Other>
struct meta_matrix_count_rows<meta_matrix<meta_vector<>, Other...>>
{
	static const int value = 0;
};
template<int First1, int...Elems1, class...Other>
struct meta_matrix_count_rows<meta_matrix<meta_vector<First1, Elems1...>, Other...>>
{
	static const int value = meta_matrix_count_rows<meta_matrix<meta_vector<Elems1...>, Other...>>::value +1;
};

// 去掉某一列(向量) -> 余子式使用
template<class T, int c>
struct meta_matrix_erase_col;
template<int...Elems1, class...Other>
struct meta_matrix_erase_col<meta_matrix<meta_vector<Elems1...>, Other...>, 0>
{
	using type = meta_matrix<Other...>;
};
template<int...Elems1, class...Other, int c>
struct meta_matrix_erase_col<meta_matrix<meta_vector<Elems1...>, Other...>, c>
{
	using type = typename meta_matrix_cat<
		meta_matrix<meta_vector<Elems1...>>,
		typename meta_matrix_erase_col<meta_matrix<Other...>, c - 1>::type
	>::type;
};
// 去掉某一行 -> 余子式使用
template<class T, int r>
struct meta_matrix_erase_row;
template<int...Elems1, int r>
struct meta_matrix_erase_row<meta_matrix<meta_vector<Elems1...>>, r>
{
	using type = meta_matrix<typename meta_vector_erase<meta_vector<Elems1...>, r>::type>;
};
template<int...Elems1, class...Other, int r>
struct meta_matrix_erase_row<meta_matrix<meta_vector<Elems1...>, Other...>, r>
{
	using type = typename meta_matrix_cat<
		meta_matrix<typename meta_vector_erase<meta_vector<Elems1...>, r>::type>,
		typename meta_matrix_erase_row<meta_matrix<Other...>, r>::type
	> ::type;
};
// 去掉r行c列（余子式）
template<class T, int r, int c>
struct meta_matrix_minors : meta_matrix_erase_row<typename meta_matrix_erase_col<T, c>::type, r> {};

template<class T, int c>
struct meta_matrix_get_col;
template<int...Elems1, class...Other>
struct meta_matrix_get_col<meta_matrix<meta_vector<Elems1...>, Other...>, 0>
{
	using type = meta_vector<Elems1...>;
};
template<int...Elems1, class...Other, int c>
struct meta_matrix_get_col<meta_matrix<meta_vector<Elems1...>, Other...>, c>
{
	using type = typename meta_matrix_get_col<meta_matrix<Other...>, c - 1>::type;
};

template<class T, int r>
struct meta_matrix_get_row;
template<int First1, int...Elems1, int r>
struct meta_matrix_get_row<meta_matrix<meta_vector<First1, Elems1...>>, r>
{
	using type = meta_vector< meta_vector_get< meta_vector<First1, Elems1...>, r>::value >;
};
template<int First1, int...Elems1, class...Other, int r>
struct meta_matrix_get_row<meta_matrix<meta_vector<First1, Elems1...>, Other...>, r>
{
	using type = typename meta_vector_cat<
		meta_vector< meta_vector_get<meta_vector<First1, Elems1...>, r>::value >,
		typename meta_matrix_get_row<meta_matrix<Other...>, r>::type
	>::type;
};

template<class A, class B>
struct meta_matrix_add;
template<int...FirstElems1, int...FirstElems2>
struct meta_matrix_add<meta_matrix< meta_vector<FirstElems1...> >, meta_matrix< meta_vector<FirstElems2...> >>
{
	using type = meta_matrix<typename meta_vector_add<meta_vector<FirstElems1...>, meta_vector<FirstElems2...>>::type>;
};
template<int...FirstElems1, int...FirstElems2, class...Other1, class...Other2>
struct meta_matrix_add<meta_matrix< meta_vector<FirstElems1...>, Other1... >, meta_matrix< meta_vector<FirstElems2...>, Other2... >>
{
	using type = typename meta_matrix_cat <
		meta_matrix<typename meta_vector_add<meta_vector<FirstElems1...>, meta_vector<FirstElems2...>>::type>,
		typename meta_matrix_add<meta_matrix<Other1...>, meta_matrix<Other2...>>::type
	>::type;
};

template<class A, int k>
struct meta_matrix_muln;
template<int k, int...FirstElems>
struct meta_matrix_muln<meta_matrix< meta_vector<FirstElems...>>, k>
{
	using type = meta_matrix< typename meta_vector_muln< meta_vector<FirstElems...>, k>::type>;
};
template<int k, int...FirstElems, class...Other>
struct meta_matrix_muln<meta_matrix< meta_vector<FirstElems...>, Other... >, k>
{
	using type = typename meta_matrix_cat <
		typename meta_matrix_muln< meta_matrix< meta_vector<FirstElems...>>, k>::type,
		typename meta_matrix_muln< meta_matrix< Other...>, k>::type
	>::type; 
};

template<class A, int k>
struct meta_matrix_divn;
template<int k, int...FirstElems>
struct meta_matrix_divn<meta_matrix< meta_vector<FirstElems...>>, k>
{
	using type = meta_matrix< typename meta_vector_divn< meta_vector<FirstElems...>, k>::type>;
};
template<int k, int...FirstElems, class...Other>
struct meta_matrix_divn<meta_matrix< meta_vector<FirstElems...>, Other... >, k>
{
	using type = typename meta_matrix_cat <
		typename meta_matrix_divn< meta_matrix< meta_vector<FirstElems...>>, k>::type,
		typename meta_matrix_divn< meta_matrix< Other...>, k>::type
	>::type;
};

template<int cols, int rows, template<int i, int j> class new_elem>
struct meta_matrix_construct
{
private:
	template<int r>
	struct construct_vector
	{
	private:
		template<int x>
		struct for_process
		{
			using type = typename meta_vector_cat<
				typename for_process<x - 1>::type,
				meta_vector<new_elem<r, x - 1>::value>
			>::type;
		};
		template<>
		struct for_process<0>
		{
			using type = meta_vector<>;
		};
	public:
		using type = typename for_process<rows>::type;
	};

	template<int x>
	struct for_process
	{
		using type = typename meta_matrix_cat<
			typename for_process<x - 1>::type,
			meta_matrix<typename construct_vector<x - 1>::type>
		>::type;
	};
	template<>
	struct for_process<0>
	{
		using type = meta_matrix<>;
	};
public:
	using type = typename for_process<cols>::type;
};

template<class A, class B>
struct meta_matrix_mul;
template<int...Elems1, class...Other1, int...Elems2, class...Other2>
struct meta_matrix_mul<meta_matrix<meta_vector<Elems1...>, Other1...>, meta_matrix<meta_vector<Elems2...>, Other2...>>
{
private:
	template<int i, int j>
	struct new_elem
	{
		static const int value = meta_vector_dot<
			typename meta_matrix_get_row< meta_matrix<meta_vector<Elems1...>, Other1...>, j >::type,
			typename meta_matrix_get_col< meta_matrix<meta_vector<Elems2...>, Other2...>, i >::type
		>::value;
	};
public:
	using type = typename meta_matrix_construct<
		meta_matrix_count_cols< meta_matrix<meta_vector<Elems2...>, Other2...> >::value,
		meta_matrix_count_rows< meta_matrix<meta_vector<Elems1...>, Other1...> >::value,
		new_elem
	>::type;
};

template<class T>
struct meta_matrix_transpose;
template<int...Elems, class...Other>
struct meta_matrix_transpose<meta_matrix< meta_vector<Elems...>, Other... >>
{
private:
	template<int x>
	struct for_process
	{
		using type = typename meta_matrix_cat <
			typename for_process<x - 1>::type,
			meta_matrix<typename meta_matrix_get_row< meta_matrix< meta_vector<Elems...>, Other... >, x - 1>::type >
		>::type;
	};
	template<>
	struct for_process<0>
	{
		using type = meta_matrix<>;
	};
public:
	using type = typename for_process<meta_matrix_count_rows< 
		meta_matrix< meta_vector<Elems...>, Other... >
	>::value >::type;
};

template<class T>
struct meta_matrix_det;
template<int FirstElem>
struct meta_matrix_det<meta_matrix< meta_vector<FirstElem> >>
{
	static const int value = FirstElem;
};
template<int...Elems, class...Other>
struct meta_matrix_det<meta_matrix< meta_vector<Elems...>, Other... >>
{
private:
	template<int n>
	struct for_process
	{
	private:
		template<bool bAdd>
		struct if_add_process;
		template<>
		struct if_add_process<true>
		{
			static const int value = for_process<n - 1>::value + meta_matrix_det <
				meta_matrix_minors< meta_matrix< meta_vector<Elems...>, Other... >, n - 1, 0 >::type
			>::value * meta_vector_get<meta_vector<Elems...>, n - 1>::value;
		};
		template<>
		struct if_add_process<false>
		{
			static const int value = +for_process<n - 1>::value - meta_matrix_det <
				meta_matrix_minors< meta_matrix< meta_vector<Elems...>, Other... >, n - 1, 0 >::type
			>::value * meta_vector_get<meta_vector<Elems...>, n - 1>::value;
		};
	public:
		static const int value = if_add_process<n & 1>::value;
	};
	template<>
	struct for_process<0>
	{
		static const int value = 0;
	};
public:
	static const int value = for_process< meta_matrix_count_rows<meta_matrix< meta_vector<Elems...>, Other... >>::value >::value;
};

// 伴随矩阵？？
template<class T>
struct meta_matrix_adjugate;
template<int...Elems, class...Other>
struct meta_matrix_adjugate<meta_matrix< meta_vector<Elems...>, Other... >>
{
private:
	template<int i, int j>
	struct new_elem
	{
		template<bool sign>
		struct if_process
		{
			static const int value = meta_matrix_det<
				meta_matrix_minors<meta_matrix< meta_vector<Elems...>, Other... >, i, j>::type
			>::value;
		};
		template<>
		struct if_process<true>
		{
			static const int value = -if_process<false>::value;
		};
		static const int value = if_process<(i+j)&1>::value;
	};
public:
	using type = typename meta_matrix_construct<
		meta_matrix_count_rows< meta_matrix<meta_vector<Elems...>, Other... > >::value,
		meta_matrix_count_cols< meta_matrix<meta_vector<Elems...>, Other... > >::value,
		new_elem
	>::type;
};

//逆矩阵
template<class T>
struct meta_matrix_inverse;
template<int...Elems, class...Other>
struct meta_matrix_inverse<meta_matrix< meta_vector<Elems...>, Other... >>
{
	using type = typename meta_matrix_divn <
		typename meta_matrix_adjugate<meta_matrix< meta_vector<Elems...>, Other... >>::type,
		meta_matrix_det<meta_matrix< meta_vector<Elems...>, Other... >>::value
	>::type;
};

int main()
{
	
	using vector1 = meta_vector<0, 1, 2, 1, 6, 4, 6, 4>;
	using vector2 = meta_vector<1, 1, -1, -3, 4, 1, 6, 2>;
	using vector3 = meta_vector<2, 4, 0, 3, 7, 2, 7, 3>;
	using vector4 = meta_vector<5, 7, 3, 7, 4, 7, 6, 3>;
	using vector5 = meta_vector<7, 5, 2, 8, 2, 8, 4, -6>;
	using vector6 = meta_vector<1, 5, 2, 6, 7, 2, 5, 7>;
	using vector7 = meta_vector<3, 7, 2, 6, 3, 7, 2, 8>;
	using vector8 = meta_vector<1, 6, 3, -7, 2, 6, 8, 9>;
	using matrix1 = meta_matrix<vector1, vector2, vector3, vector4, vector5, vector6, vector7, vector8>;
	
	using matrix2 = meta_matrix_inverse<matrix1>::type;
	cout << typeid(matrix2).name() << endl;
}
