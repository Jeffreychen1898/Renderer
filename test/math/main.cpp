#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>

#include <Renderer.hpp>

#define TOLERANCE 0.001
#define PASSED(msg) std::cout << "\t[Passed] " << msg << std::endl

static void TestVecInt();
static void TestVecFloat();
static void TestVec2Float();
static void TestVec3Float();
static void TestVec4Float();
static void TestHelperFunctions();

static void TestMatrixFloat();
static void TestMat2Float();
static void TestMat3Float();
static void TestMat4Float();

int main()
{
	std::cout << "Vec<int, 2>" << std::endl;
	TestVecInt();
	std::cout << "Vec<float, 2>" << std::endl;
	TestVecFloat();
	std::cout << "Vec2<float>" << std::endl;
	TestVec2Float();
	std::cout << "Vec3<float>" << std::endl;
	TestVec3Float();
	std::cout << "Vec4<float>" << std::endl;
	TestVec4Float();
	std::cout << "Helper Functions" << std::endl;
	TestHelperFunctions();
	std::cout << "Mat<float, 3>" << std::endl;
	TestMatrixFloat();
	std::cout << "Mat2<float>" << std::endl;
	TestMat2Float();
	std::cout << "Mat3<float>" << std::endl;
	TestMat3Float();
	std::cout << "Mat4<float>" << std::endl;
	TestMat4Float();
	return 0;
}

void TestMat4Float()
{
	// test the default constructor
	Renderer::Mat4<float> mat1;
	assert(mat1.v00 == 1.f && mat1.v01 == 0.f && mat1.v02 == 0.f && mat1.v03 == 0.f);
	assert(mat1.v10 == 0.f && mat1.v11 == 1.f && mat1.v12 == 0.f && mat1.v13 == 0.f);
	assert(mat1.v20 == 0.f && mat1.v21 == 0.f && mat1.v22 == 1.f && mat1.v23 == 0.f);
	assert(mat1.v30 == 0.f && mat1.v31 == 0.f && mat1.v32 == 0.f && mat1.v33 == 1.f);

	// test the constructor with raw array
	float raw_mat2[] = {
		1.f, 4.f, 1.f, 2.f,
		3.f, 2.f, 7.f, 3.f,
		5.f, 3.f, 6.f, 5.f,
		4.f, 3.f, 7.f, 1.f
	};
	Renderer::Mat4<float> mat2(raw_mat2);
	assert(mat2.v00 == 1.f && mat2.v01 == 3.f && mat2.v02 == 5.f && mat2.v03 == 4.f);
	assert(mat2.v10 == 4.f && mat2.v11 == 2.f && mat2.v12 == 3.f && mat2.v13 == 3.f);
	assert(mat2.v20 == 1.f && mat2.v21 == 7.f && mat2.v22 == 6.f && mat2.v23 == 7.f);
	assert(mat2.v30 == 2.f && mat2.v31 == 3.f && mat2.v32 == 5.f && mat2.v33 == 1.f);

	// test the constructor with values directly inputted
	Renderer::Mat4<float> mat3(
		2.f, 1.f, 7.f, 4.f,
		5.f, 3.f, 4.f, 7.f,
		2.f, 3.f, 6.f, 2.f,
		4.f, 7.f, 5.f, 1.f
	);
	assert(mat3.v00 == 2.f && mat3.v01 == 1.f && mat3.v02 == 7.f && mat3.v03 == 4.f);
	assert(mat3.v10 == 5.f && mat3.v11 == 3.f && mat3.v12 == 4.f && mat3.v13 == 7.f);
	assert(mat3.v20 == 2.f && mat3.v21 == 3.f && mat3.v22 == 6.f && mat3.v23 == 2.f);
	assert(mat3.v30 == 4.f && mat3.v31 == 7.f && mat3.v32 == 5.f && mat3.v33 == 1.f);

	// test the constructor with basis vectors
	Renderer::Vec4 basis1(1.f, 4.f, 3.f, 6.f);
	Renderer::Vec4 basis2(3.f, 9.f, 5.f, 2.f);
	Renderer::Vec4 basis3(2.f, 4.f, 3.f, 4.f);
	Renderer::Vec4 basis4(2.f, 1.f, 2.f, 7.f);
	Renderer::Mat4<float> mat4(basis1, basis2, basis3, basis4);
	assert(mat4.v00 == 1.f && mat4.v01 == 3.f && mat4.v02 == 2.f && mat4.v03 == 2.f);
	assert(mat4.v10 == 4.f && mat4.v11 == 9.f && mat4.v12 == 4.f && mat4.v13 == 1.f);
	assert(mat4.v20 == 3.f && mat4.v21 == 5.f && mat4.v22 == 3.f && mat4.v23 == 2.f);
	assert(mat4.v30 == 6.f && mat4.v31 == 2.f && mat4.v32 == 4.f && mat4.v33 == 7.f);

	// test the copy constructor
	Renderer::Mat4<float> mat5(mat4);
	assert(mat5 == mat4);
	mat5.v00 = 2.f;
	assert(mat5 != mat4);
	assert(mat4.v00 == 1.f);

	// test the assignment operator
	Renderer::Mat4<float> mat6 = mat4;
	assert(mat6 == mat4);
	mat6.v00 = 2.f;
	assert(mat6 != mat4);
	assert(mat4.v00 == 1.f);

	PASSED("all constructors, operator=, operator==, operator!=");

	float mat_mult_mat_ans[] = {
		43.f, 36.f, 77.f, 33.f,
		53.f, 40.f, 89.f, 33.f,
		69.f, 69.f, 106.f, 61.f,
		39.f, 39.f, 72.f, 40.f
	};
	Renderer::Mat4<float> mat_mult_mat = mat2 * mat3;
	for(int i=0;i<16;++i)
		assert(std::abs(mat_mult_mat.atRawIndex(i) - mat_mult_mat_ans[i]) < TOLERANCE);

	PASSED("operator* mat mult mat");

	Renderer::Vec4 mat_mult_vec_ans(35.f, 27.f, 62.f, 27.f);
	Renderer::Vec4 test_vec(1.f, 4.f, 2.f, 3.f);
	Renderer::Vec4 mat_mult_vec = mat2 * test_vec;
	assert(mat_mult_vec == mat_mult_vec_ans);

	PASSED("operator* mat mult vec");

	float mat_plus_mat_ans[] = {
		3.f, 9.f, 3.f, 6.f,
		4.f, 5.f, 10.f, 10.f,
		12.f, 7.f, 12.f, 10.f,
		8.f, 10.f, 9.f, 2.f
	};
	Renderer::Mat4<float> mat_plus_mat = mat2 + mat3;
	for(int i=0;i<16;++i)
		assert(std::abs(mat_plus_mat.atRawIndex(i) - mat_plus_mat_ans[i]) < TOLERANCE);

	PASSED("operator+");

	float mat_minus_mat_ans[] = {
		-1.f, -1.f, -1.f, -2.f,
		2.f, -1.f, 4.f, -4.f,
		-2.f, -1.f, 0.f, 0.f,
		0.f, -4.f, 5.f, 0.f
	};
	Renderer::Mat4<float> mat_minus_mat = mat2 - mat3;
	for(int i=0;i<16;++i)
		assert(std::abs(mat_minus_mat.atRawIndex(i) - mat_minus_mat_ans[i]) < TOLERANCE);

	PASSED("operator-");

	float mat_determinant = mat2.determinant();
	assert(std::abs(mat_determinant + 182.f) < TOLERANCE);

	Renderer::Mat4 mat2_inv = mat2;
	mat2_inv.inverse();

	Renderer::Mat4 mat2_inv_test = mat2_inv * mat2;
	assert(std::abs(mat2_inv_test.v00 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v01 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v02 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v03 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v10 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v11 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v12 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v13 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v20 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v21 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v22 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v23 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v30 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v31 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v32 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v33 - 1.f) < TOLERANCE);

	PASSED(".determinant(), .inverse()");
}

void TestMat3Float()
{
	// test the default constructor
	Renderer::Mat3<float> mat1;
	assert(mat1.v00 == 1.f && mat1.v01 == 0.f && mat1.v02 == 0.f);
	assert(mat1.v10 == 0.f && mat1.v11 == 1.f && mat1.v12 == 0.f);
	assert(mat1.v20 == 0.f && mat1.v21 == 0.f && mat1.v22 == 1.f);

	// test the constructor with raw array
	float raw_mat2[] = {
		1.f, 4.f, 1.f,
		3.f, 2.f, 7.f,
		5.f, 3.f, 6.f
	};
	Renderer::Mat3<float> mat2(raw_mat2);
	assert(mat2.v00 == 1.f && mat2.v01 == 3.f && mat2.v02 == 5.f);
	assert(mat2.v10 == 4.f && mat2.v11 == 2.f && mat2.v12 == 3.f);
	assert(mat2.v20 == 1.f && mat2.v21 == 7.f && mat2.v22 == 6.f);

	// test the constructor with values directly inputted
	Renderer::Mat3<float> mat3(
			2.f, 1.f, 7.f,
			5.f, 3.f, 4.f,
			2.f, 3.f, 6.f
	);
	assert(mat3.v00 == 2.f && mat3.v01 == 1.f && mat3.v02 == 7.f);
	assert(mat3.v10 == 5.f && mat3.v11 == 3.f && mat3.v12 == 4.f);
	assert(mat3.v20 == 2.f && mat3.v21 == 3.f && mat3.v22 == 6.f);

	// test the constructor with basis vectors
	Renderer::Vec3 basis1(1.f, 4.f, 3.f);
	Renderer::Vec3 basis2(3.f, 9.f, 5.f);
	Renderer::Vec3 basis3(2.f, 4.f, 3.f);
	Renderer::Mat3<float> mat4(basis1, basis2, basis3);
	assert(mat4.v00 == 1.f && mat4.v01 == 3.f && mat4.v02 == 2.f);
	assert(mat4.v10 == 4.f && mat4.v11 == 9.f && mat4.v12 == 4.f);
	assert(mat4.v20 == 3.f && mat4.v21 == 5.f && mat4.v22 == 3.f);

	// test the copy constructor
	Renderer::Mat3<float> mat5(mat4);
	assert(mat5 == mat4);
	mat5.v00 = 2.f;
	assert(mat5 != mat4);
	assert(mat4.v00 == 1.f);

	// test the assignment operator
	Renderer::Mat3<float> mat6 = mat4;
	assert(mat6 == mat4);
	mat6.v00 = 2.f;
	assert(mat6 != mat4);
	assert(mat4.v00 == 1.f);

	PASSED("all constructors, operator=, operator==, operator!=");

	float mat_mult_mat_ans[] = {
		27.f, 24.f, 49.f,
		25.f, 19.f, 40.f,
		49.f, 54.f, 71.f
	};
	Renderer::Mat3<float> mat_mult_mat = mat2 * mat3;
	for(int i=0;i<9;++i)
		assert(std::abs(mat_mult_mat.atRawIndex(i) - mat_mult_mat_ans[i]) < TOLERANCE);

	PASSED("operator* mat mult mat");

	Renderer::Vec3 mat_mult_vec_ans(23.f, 18.f, 41.f);
	Renderer::Vec3 test_vec(1.f, 4.f, 2.f);
	Renderer::Vec3 mat_mult_vec = mat2 * test_vec;
	assert(mat_mult_vec == mat_mult_vec_ans);

	PASSED("operator* mat mult vec");

	float mat_plus_mat_ans[] = {
		3.f, 9.f, 3.f,
		4.f, 5.f, 10.f,
		12.f, 7.f, 12.f
	};
	Renderer::Mat3<float> mat_plus_mat = mat2 + mat3;
	for(int i=0;i<9;++i)
		assert(std::abs(mat_plus_mat.atRawIndex(i) - mat_plus_mat_ans[i]) < TOLERANCE);

	PASSED("operator+");

	float mat_minus_mat_ans[] = {
		-1.f, -1.f, -1.f,
		2.f, -1.f, 4.f,
		-2.f, -1.f, 0.f
	};
	Renderer::Mat3<float> mat_minus_mat = mat2 - mat3;
	for(int i=0;i<9;++i)
		assert(std::abs(mat_minus_mat.atRawIndex(i) - mat_minus_mat_ans[i]) < TOLERANCE);

	PASSED("operator-");

	float mat_determinant = mat2.determinant();
	assert(std::abs(mat_determinant - 58.f) < TOLERANCE);

	Renderer::Mat3 mat2_inv = mat2;
	mat2_inv.inverse();

	Renderer::Mat3 mat2_inv_test = mat2_inv * mat2;
	assert(std::abs(mat2_inv_test.v00 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v01 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v02 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v10 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v11 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v12 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v20 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v21 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v22 - 1.f) < TOLERANCE);

	PASSED(".determinant(), .inverse()");
}

void TestMat2Float()
{
	// test the default constructor
	Renderer::Mat2<float> mat1;
	assert(mat1.v00 == 1.f && mat1.v01 == 0.f);
	assert(mat1.v10 == 0.f && mat1.v11 == 1.f);

	// test the constructor with raw array
	float raw_mat2[] = {
		1.f, 4.f,
		3.f, 2.f
	};
	Renderer::Mat2<float> mat2(raw_mat2);
	assert(mat2.v00 == 1.f && mat2.v01 == 3.f);
	assert(mat2.v10 == 4.f && mat2.v11 == 2.f);

	// test the constructor with values directly inputted
	Renderer::Mat2<float> mat3(
			2.f, 1.f,
			5.f, 3.f
	);
	assert(mat3.v00 == 2.f && mat3.v01 == 1.f);
	assert(mat3.v10 == 5.f && mat3.v11 == 3.f);

	// test the constructor with basis vectors
	Renderer::Vec2 basis1(1.f, 4.f);
	Renderer::Vec2 basis2(3.f, 9.f);
	Renderer::Mat2<float> mat4(basis1, basis2);
	assert(mat4.v00 == 1.f && mat4.v01 == 3.f);
	assert(mat4.v10 == 4.f && mat4.v11 == 9.f);

	// test the copy constructor
	Renderer::Mat2<float> mat5(mat4);
	assert(mat5 == mat4);
	mat5.v00 = 2.f;
	assert(mat5 != mat4);
	assert(mat4.v00 == 1.f);

	// test the assignment operator
	Renderer::Mat2<float> mat6 = mat4;
	assert(mat6 == mat4);
	mat6.v00 = 2.f;
	assert(mat6 != mat4);
	assert(mat4.v00 == 1.f);

	PASSED("all constructors, operator=, operator==, operator!=");

	float mat_mult_mat_ans[] = {
		17.f, 18.f,
		10.f, 10.f
	};
	Renderer::Mat2<float> mat_mult_mat = mat2 * mat3;
	for(int i=0;i<4;++i)
		assert(std::abs(mat_mult_mat.atRawIndex(i) - mat_mult_mat_ans[i]) < TOLERANCE);

	PASSED("operator* mat mult mat");

	Renderer::Vec2 mat_mult_vec_ans(13.f, 12.f);
	Renderer::Vec2 test_vec(1.f, 4.f);
	Renderer::Vec2 mat_mult_vec = mat2 * test_vec;
	assert(mat_mult_vec == mat_mult_vec_ans);

	PASSED("operator* mat mult vec");

	float mat_plus_mat_ans[] = {
		3.f, 9.f,
		4.f, 5.f
	};
	Renderer::Mat2<float> mat_plus_mat = mat2 + mat3;
	for(int i=0;i<4;++i)
		assert(std::abs(mat_plus_mat.atRawIndex(i) - mat_plus_mat_ans[i]) < TOLERANCE);

	PASSED("operator+");

	float mat_minus_mat_ans[] = {
		-1.f, -1.f,
		2.f, -1.f
	};
	Renderer::Mat2<float> mat_minus_mat = mat2 - mat3;
	for(int i=0;i<4;++i)
		assert(std::abs(mat_minus_mat.atRawIndex(i) - mat_minus_mat_ans[i]) < TOLERANCE);

	PASSED("operator-");

	float mat_determinant = mat2.determinant();
	assert(std::abs(mat_determinant + 10.f) < TOLERANCE);

	Renderer::Mat2 mat2_inv = mat2;
	mat2_inv.inverse();

	Renderer::Mat2 mat2_inv_test = mat2_inv * mat2;
	assert(std::abs(mat2_inv_test.v00 - 1.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v01 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v10 - 0.f) < TOLERANCE);
	assert(std::abs(mat2_inv_test.v11 - 1.f) < TOLERANCE);

	PASSED(".determinant(), .inverse()");
}

void TestMatrixFloat()
{
	Renderer::Mat<float, 3> mat1;
	assert(mat1.at(0, 0) == 1.f);
	assert(mat1.at(0, 1) == 0.f);
	assert(mat1.at(0, 2) == 0.f);
	assert(mat1.at(1, 0) == 0.f);
	assert(mat1.at(1, 1) == 1.f);
	assert(mat1.at(1, 2) == 0.f);
	assert(mat1.at(2, 0) == 0.f);
	assert(mat1.at(2, 1) == 0.f);
	assert(mat1.at(2, 2) == 1.f);
	Renderer::Mat<float, 3> mat2;
	mat2.set(0, 0, 1.f);
	mat2.set(0, 1, 2.f);
	mat2.set(0, 2, 3.f);
	mat2.set(1, 0, 4.f);
	mat2.set(1, 1, 5.f);
	mat2.set(1, 2, 6.f);
	mat2.set(2, 0, 5.f);
	mat2.set(2, 1, 4.f);
	mat2.set(2, 2, 3.f);
	Renderer::Mat<float, 3> mat_cpy(mat2);
	assert(mat2 == mat_cpy);
	mat_cpy.set(0, 1, 3.f);
	assert(mat2.at(0, 1) == 2.f);
	assert(mat2 != mat_cpy);

	PASSED("constructor, copy constructor, operator==, operator!=, at(), set()");

	try
	{
		mat2.set(3, 0, 0.f);
		assert(false);
	} catch(const Renderer::OutOfRangeException& e)
	{
		PASSED("set() exception!");
	}
	try
	{
		mat2.at(-4, 0);
		assert(false);
	} catch(const Renderer::OutOfRangeException& e)
	{
		PASSED("at() exception!");
	}

	Renderer::Mat<float, 3> mat_cpy2(mat2);
	assert(mat2 == mat_cpy2);
	mat_cpy2.set(0, 1, 3.f);
	assert(mat2.at(0, 1) == 2.f);
	assert(mat2 != mat_cpy2);

	PASSED("operator=");

	mat2.transpose();
	assert(mat2.at(0, 0) == 1.f);
	assert(mat2.at(0, 1) == 4.f);
	assert(mat2.at(0, 2) == 5.f);
	assert(mat2.at(1, 0) == 2.f);
	assert(mat2.at(1, 1) == 5.f);
	assert(mat2.at(1, 2) == 4.f);
	assert(mat2.at(2, 0) == 3.f);
	assert(mat2.at(2, 1) == 6.f);
	assert(mat2.at(2, 2) == 3.f);

	PASSED(".transpose()");

	assert(mat2.atRawIndex(0) == 1.f);
	assert(mat2.atRawIndex(1) == 2.f);
	assert(mat2.atRawIndex(2) == 3.f);
	assert(mat2.atRawIndex(3) == 4.f);
	assert(mat2.atRawIndex(4) == 5.f);
	assert(mat2.atRawIndex(5) == 6.f);
	assert(mat2.atRawIndex(6) == 5.f);
	assert(mat2.atRawIndex(7) == 4.f);
	assert(mat2.atRawIndex(8) == 3.f);

	PASSED(".atRawIndex()");

	float raw_mat_ans[] = {
		1, 2, 3,
		4, 5, 6,
		5, 4, 3
	};
	const float* raw_mat = *mat2;
	for(int i=0;i<9;++i)
		assert(raw_mat[i] == raw_mat_ans[i]);

	PASSED("dereference operator*");

	Renderer::Mat<float, 3> mat3;
	mat3.set(0, 0, 2.f);
	mat3.set(0, 1, 4.f);
	mat3.set(0, 2, 5.f);
	mat3.set(1, 0, 7.f);
	mat3.set(1, 1, 2.f);
	mat3.set(1, 2, 4.f);
	mat3.set(2, 0, 4.f);
	mat3.set(2, 1, 2.f);
	mat3.set(2, 2, 1.f);

	Renderer::Mat<float, 3> mat_mult = mat2 * mat3;
	float mat_mult_ans[] = {
		50.f, 55.f, 60.f,
		22.f, 26.f, 30.f,
		26.f, 34.f, 42.f
	};
	for(int i=0;i<9;++i)
		assert(std::abs(mat_mult.atRawIndex(i) - mat_mult_ans[i]) < TOLERANCE);

	PASSED("operator* mat times mat");

	Renderer::Mat<float, 3> mat_sum = mat2 + mat3;
	float mat_sum_ans[] = {
		3.f, 9.f, 7.f,
		8.f, 7.f, 8.f,
		10.f, 8.f, 4.f
	};
	for(int i=0;i<9;++i)
		assert(std::abs(mat_sum.atRawIndex(i) - mat_sum_ans[i]) < TOLERANCE);

	PASSED("operator+");

	Renderer::Mat<float, 3> mat_minus = mat2 - mat3;
	float mat_minus_ans[] = {
		-1.f, -5.f, -1.f,
		0.f, 3.f, 4.f,
		0.f, 0.f, 2.f
	};
	for(int i=0;i<9;++i)
		assert(std::abs(mat_minus.atRawIndex(i) - mat_minus_ans[i]) < TOLERANCE);

	PASSED("operator-");

	Renderer::Vec<float, 3> test_vec;
	test_vec.set(0, 2.f);
	test_vec.set(1, 6.f);
	test_vec.set(2, 4.f);

	Renderer::Vec<float, 3> mat_times_vec = mat2 * test_vec;
	assert(std::abs(mat_times_vec.get(0) - 46.f) < TOLERANCE);
	assert(std::abs(mat_times_vec.get(1) - 50.f) < TOLERANCE);
	assert(std::abs(mat_times_vec.get(2) - 54.f) < TOLERANCE);

	PASSED("operator* mat times vec");
}

void TestHelperFunctions()
{
	// test rotation about another vector
	Renderer::Vec3 rotation_axis(1.f, 1.f, 1.f);
	Renderer::Vec3 rotate_vector(0.f, 1.f, 0.f);

	Renderer::Math::RotateVec3(rotate_vector, rotation_axis, 2.f * Renderer::Math::PI / 3.f);

	assert(std::abs(rotate_vector.x) < TOLERANCE);
	assert(std::abs(rotate_vector.y) < TOLERANCE);
	assert(std::abs(rotate_vector.z - 1) < TOLERANCE);

	// test another rotation
	Renderer::Vec3 rotation_axis2(1.f, 2.f, -1.f);
	Renderer::Vec3 rotate_vector2(2.f, -3.f, 1.f);

	Renderer::Math::RotateVec3(rotate_vector2, rotation_axis2, 1.23f);

	assert(std::abs(rotate_vector2.x + 0.27091) < TOLERANCE);
	assert(std::abs(rotate_vector2.y + 3.26668) < TOLERANCE);
	assert(std::abs(rotate_vector2.z + 1.80427) < TOLERANCE);

	PASSED("RotateVec3()");

	// test the slerp function
	Renderer::Vec3 slerp_start(1.f, -2.f, 3.f);
	Renderer::Vec3 slerp_end(1.f, 2.f, -3.f);

	Renderer::Vec3 slerp_result = Renderer::Math::Slerp(slerp_start, slerp_end, 0.4f);

	assert(std::abs(slerp_result.x - 0.966377) < TOLERANCE);
	assert(std::abs(slerp_result.y + 0.142629) < TOLERANCE);
	assert(std::abs(slerp_result.z - 0.2139436) < TOLERANCE);

	PASSED("Slerp()");

	// test the lerp function on vectors
	Renderer::Vec2 lerp_vec_start(1.f, 2.f);
	Renderer::Vec2 lerp_vec_end(4.f, 3.f);

	Renderer::Vec2 lerp_vec_result = Renderer::Math::Lerp(lerp_vec_start, lerp_vec_end, 0.4);

	assert(std::abs(lerp_vec_result.x - 2.2) < TOLERANCE);
	assert(std::abs(lerp_vec_result.y - 2.4) < TOLERANCE);

	// test the lerp function on floats
	float lerp_float_start = 1.3f;
	float lerp_float_end = 3.8f;
	
	float lerp_float_result = Renderer::Math::Lerp(lerp_float_start, lerp_float_end, 0.4);

	assert(std::abs(lerp_float_result - 2.3) < TOLERANCE);

	PASSED("Lerp() on vectors and floats");
}

void TestVec4Float()
{
	// test constructors and assignment operator
	Renderer::Vec4 empty_constructor;
	Renderer::Vec4 single_value_constructor(2.f);
	
	assert(empty_constructor == Renderer::Vec4(0.f, 0.f, 0.f, 0.f));
	assert(single_value_constructor == Renderer::Vec4(2.f, 2.f, 2.f, 2.f));

	Renderer::Vec4 full_constructor(1.f, 2.f, 3.f, 4.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);
	assert(full_constructor.z == 3.f && full_constructor.w == 4.f);

	Renderer::Vec4 cpy_constructor(full_constructor);
	cpy_constructor.x = 4;

	assert(cpy_constructor.x == 4.f && cpy_constructor.y == 2.f);
	assert(cpy_constructor.z == 3.f && cpy_constructor.w == 4.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);
	assert(full_constructor.z == 3.f && full_constructor.w == 4.f);

	Renderer::Vec4 assignment_operator = single_value_constructor;
	assignment_operator.x = 3;
	assert(assignment_operator.x == 3.f && assignment_operator.y == 2.f);
	assert(assignment_operator.z == 2.f && assignment_operator.w == 2.f);
	assert(single_value_constructor == Renderer::Vec4(2.f));

	PASSED("all constructors and operator=");

	Renderer::Vec4 test_default_w(1.f, 2.f, 4.f);
	Renderer::Vec4 test_default_w_ans(1.f, 2.f, 4.f, 1.f);

	assert(test_default_w == test_default_w_ans);

	PASSED("default value for component w = 1");

	Renderer::Vec4 v1(2.f, 3.f, 4.f, 1.f);
	Renderer::Vec4 v2(1.f, 2.f, 3.f, 1.f);

	assert(v1 != v2);
	PASSED("operator!=");

	Renderer::Vec4 mult_vector = v1 * v2;
	Renderer::Vec4 mult_vector_ans(2.f, 6.f, 12.f, 1.f);
	assert(mult_vector == mult_vector_ans);

	Renderer::Vec4 vec_mult_scalar = v1 * 3;
	Renderer::Vec4 vec_mult_scalar_ans(6.f, 9.f, 12.f, 3.f);
	assert(vec_mult_scalar == vec_mult_scalar_ans);

	PASSED("operator* and vec * scalar");

	Renderer::Vec4 add_vector = v1 + v2;
	Renderer::Vec4 add_vector_ans(3.f, 5.f, 7.f, 2.f);

	assert(add_vector == add_vector_ans);

	PASSED("operator+");

	Renderer::Vec4 minus_vector = v1 - v2;
	Renderer::Vec4 minus_vector_ans(1.f, 1.f, 1.f, 0.f);

	assert(minus_vector == minus_vector_ans);

	PASSED("operator-");

	Renderer::Vec4 projected = v1.project(v2);
	assert(std::abs(projected.x - 1.4) < TOLERANCE);
	assert(std::abs(projected.y - 2.8) < TOLERANCE);
	assert(std::abs(projected.z - 4.2) < TOLERANCE);
	assert(std::abs(projected.w - 1.4) < TOLERANCE);

	PASSED(".project()");

	Renderer::Vec4 reflected = v1.reflect(v2);
	assert(std::abs(reflected.x - 0.8) < TOLERANCE);
	assert(std::abs(reflected.y - 2.6) < TOLERANCE);
	assert(std::abs(reflected.z - 4.4) < TOLERANCE);
	assert(std::abs(reflected.w - 1.8) < TOLERANCE);

	PASSED(".reflect()");

	assert(std::abs(v1.length() - std::sqrt(30)) < TOLERANCE);
	assert(v1.dot(v2) == 21.f);

	v1.normalize();
	assert(std::abs(v1.x - 0.3651f) < TOLERANCE);
	assert(std::abs(v1.y - 0.5477f) < TOLERANCE);
	assert(std::abs(v1.z - 0.7303f) < TOLERANCE);
	assert(std::abs(v1.w - 0.1826) < TOLERANCE);

	PASSED(".length() and .dot() and .normalize()");

	Renderer::Vec4 quaternion(3.f, 5.f, 3.f, 2.f);
	Renderer::Vec4 quaternion_inv = quaternion;
	quaternion_inv.Q_inverse();

	Renderer::Vec4 quaternion_times_inv = quaternion.Q_mult(quaternion_inv);

	assert(std::abs(quaternion_times_inv.x) < TOLERANCE);
	assert(std::abs(quaternion_times_inv.y) < TOLERANCE);
	assert(std::abs(quaternion_times_inv.z) < TOLERANCE);
	assert(std::abs(quaternion_times_inv.w - 1) < TOLERANCE);

	PASSED(".Q_mult() and .Q_inverse()");
}

void TestVec3Float()
{
	// test constructors and assignment operator
	Renderer::Vec3 empty_constructor;
	Renderer::Vec3 single_value_constructor(2.f);
	
	assert(empty_constructor == Renderer::Vec3(0.f, 0.f, 0.f));
	assert(single_value_constructor == Renderer::Vec3(2.f, 2.f, 2.f));

	Renderer::Vec3 full_constructor(1.f, 2.f, 3.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);
	assert(full_constructor.z == 3.f);

	Renderer::Vec3 cpy_constructor(full_constructor);
	cpy_constructor.x = 4;

	assert(cpy_constructor.x == 4.f && cpy_constructor.y == 2.f);
	assert(cpy_constructor.z == 3.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);
	assert(full_constructor.z == 3.f);

	Renderer::Vec3 assignment_operator = single_value_constructor;
	assignment_operator.x = 3;
	assert(assignment_operator.x == 3.f && assignment_operator.y == 2.f);
	assert(assignment_operator.z == 2.f);
	assert(single_value_constructor == Renderer::Vec3(2.f));

	PASSED("all constructors and operator=");

	Renderer::Vec3 v1(2.f, 3.f, 4.f);
	Renderer::Vec3 v2(1.f, 2.f, 3.f);

	assert(v1 != v2);
	PASSED("operator!=");

	Renderer::Vec3 mult_vector = v1 * v2;
	Renderer::Vec3 mult_vector_ans(2.f, 6.f, 12.f);
	assert(mult_vector == mult_vector_ans);

	Renderer::Vec3 vec_mult_scalar = v1 * 3;
	Renderer::Vec3 vec_mult_scalar_ans(6.f, 9.f, 12.f);
	assert(vec_mult_scalar == vec_mult_scalar_ans);

	PASSED("operator* and vec * scalar");

	Renderer::Vec3 add_vector = v1 + v2;
	Renderer::Vec3 add_vector_ans(3.f, 5.f, 7.f);

	assert(add_vector == add_vector_ans);

	PASSED("operator+");

	Renderer::Vec3 minus_vector = v1 - v2;
	Renderer::Vec3 minus_vector_ans(1.f, 1.f, 1.f);

	assert(minus_vector == minus_vector_ans);

	PASSED("operator-");

	Renderer::Vec3 cross_product = v1.cross(v2);
	assert(std::abs(cross_product.x - 1) < TOLERANCE);
	assert(std::abs(cross_product.y + 2) < TOLERANCE);
	assert(std::abs(cross_product.z - 1) < TOLERANCE);
	PASSED(".cross()");

	Renderer::Vec3 projected = v1.project(v2);
	assert(std::abs(projected.x - 1.4286) < TOLERANCE);
	assert(std::abs(projected.y - 2.85714) < TOLERANCE);
	assert(std::abs(projected.z - 4.2857) < TOLERANCE);

	PASSED(".project()");

	Renderer::Vec3 reflected = v1.reflect(v2);
	assert(std::abs(reflected.x - 0.85714) < TOLERANCE);
	assert(std::abs(reflected.y - 2.714286) < TOLERANCE);
	assert(std::abs(reflected.z - 4.5714) < TOLERANCE);

	PASSED(".reflect()");

	assert(std::abs(v1.length() - std::sqrt(29)) < TOLERANCE);
	assert(v1.dot(v2) == 20.f);

	v1.normalize();
	assert(std::abs(v1.x - 0.37139f) < TOLERANCE);
	assert(std::abs(v1.y - 0.557086f) < TOLERANCE);
	assert(std::abs(v1.z - 0.74278f) < TOLERANCE);

	PASSED(".length() and .dot() and .normalize()");
}

void TestVec2Float()
{
	// test constructors and assignment operator
	Renderer::Vec2 empty_constructor;
	Renderer::Vec2 single_value_constructor(2.f);
	
	assert(empty_constructor == Renderer::Vec2(0.f, 0.f));
	assert(single_value_constructor == Renderer::Vec2(2.f, 2.f));

	Renderer::Vec2 full_constructor(1.f, 2.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);

	Renderer::Vec2 cpy_constructor(full_constructor);
	cpy_constructor.x = 4;

	assert(cpy_constructor.x == 4.f && cpy_constructor.y == 2.f);
	assert(full_constructor.x == 1.f && full_constructor.y == 2.f);

	Renderer::Vec2 assignment_operator = single_value_constructor;
	assignment_operator.x = 3;
	assert(assignment_operator.x == 3.f && assignment_operator.y == 2.f);
	assert(single_value_constructor == Renderer::Vec2(2.f));

	PASSED("all constructors and operator=");

	Renderer::Vec2 v1(2.f, 3.f);
	Renderer::Vec2 v2(1.f, 2.f);

	assert(v1 != v2);
	PASSED("operator!=");

	Renderer::Vec2 mult_vector = v1 * v2;
	Renderer::Vec2 mult_vector_ans(2.f, 6.f);
	assert(mult_vector == mult_vector_ans);

	Renderer::Vec2 vec_mult_scalar = v1 * 3;
	Renderer::Vec2 vec_mult_scalar_ans(6.f, 9.f);
	assert(vec_mult_scalar == vec_mult_scalar_ans);

	PASSED("operator* and vec * scalar");

	Renderer::Vec2 add_vector = v1 + v2;
	Renderer::Vec2 add_vector_ans(3.f, 5.f);

	assert(add_vector == add_vector_ans);

	PASSED("operator+");

	Renderer::Vec2 minus_vector = v1 - v2;
	Renderer::Vec2 minus_vector_ans(1.f, 1.f);

	assert(minus_vector == minus_vector_ans);

	PASSED("operator-");

	Renderer::Vec2 projected = v1.project(v2);
	assert(std::abs(projected.x - 1.6) < TOLERANCE);
	assert(std::abs(projected.y - 3.2) < TOLERANCE);

	PASSED(".project()");

	Renderer::Vec2 reflected = v1.reflect(v2);
	assert(std::abs(reflected.x - 1.2) < TOLERANCE);
	assert(std::abs(reflected.y - 3.4) < TOLERANCE);

	PASSED(".reflect()");

	assert(std::abs(v1.length() - std::sqrt(13)) < TOLERANCE);
	assert(v1.dot(v2) == 8.f);

	v1.normalize();
	assert(std::abs(v1.x - 0.5547f) < TOLERANCE);
	assert(std::abs(v1.y - 0.83205f) < TOLERANCE);

	PASSED(".length() and .dot() and .normalize()");
}

void TestVecInt()
{
	Renderer::Vec<int, 2> v1;
	Renderer::Vec<int, 2> v2;

	// test the set function
	v1.set(0, 3);
	v1.set(1, 4);

	v2.set(0, 2);
	v2.set(1, 3);


	// test the get function
	assert(v1.get(0) == 3);
	assert(v1.get(1) == 4);
	assert(v2.get(0) == 2);
	assert(v2.get(1) == 3);

	PASSED(".set() and .get()");
	
	// test the length function
	int v1_length = v1.length();
	int v2_length = v2.length();
	assert(v1_length == 5);
	assert(v2_length == 3);

	PASSED(".length()");
	
	// normalize
	Renderer::Vec<int, 2> v1_norm(v1);
	std::ostringstream normalized_display;
	normalized_display << "Original Vector: ";
	v1_norm.print(normalized_display);
	v1_norm.normalize();
	normalized_display << " Normalized Vector: ";
	v1_norm.print(normalized_display, true);

	std::cout << "\t\t" << normalized_display.str() << std::endl;

	assert(v1_norm.length() == 0 || v1_norm.length() == 1);
	assert(v1_norm != v1);

	PASSED(".normalize() and .print() and operator!= and copy constructor");

	// test operator =
	Renderer::Vec<int, 2> cp_v2 = v2;
	assert(cp_v2 == v2);
	cp_v2.set(0, 5);
	assert(cp_v2 != v2);
	
	PASSED("operator=");
	
	// test v1 * v2
	Renderer::Vec<int, 2> v1_times_v2 = v1 * v2;
	Renderer::Vec<int, 2> v1_times_v2_ans;
	v1_times_v2_ans.set(0, 6);
	v1_times_v2_ans.set(1, 12);

	assert(v1_times_v2 == v1_times_v2_ans);

	PASSED("operator* and operator ==");

	// test v1 * scalar
	Renderer::Vec<int, 2> v1_times_scalar = v1 * 5;
	Renderer::Vec<int, 2> v1_times_scalar_ans;
	v1_times_scalar_ans.set(0, 15);
	v1_times_scalar_ans.set(1, 20);

	assert(v1_times_scalar == v1_times_scalar_ans);

	PASSED("operator*scalar and operator ==");
	
	// print v1 + v2
	Renderer::Vec<int, 2> v1_plus_v2 = v1 + v2;
	Renderer::Vec<int, 2> v1_plus_v2_ans;
	v1_plus_v2_ans.set(0, 5);
	v1_plus_v2_ans.set(1, 7);

	assert(v1_plus_v2 == v1_plus_v2_ans);

	PASSED("operator+ and operator ==");
	
	// print v1 - v2
	Renderer::Vec<int, 2> v1_minus_v2 = v1 - v2;
	Renderer::Vec<int, 2> v1_minus_v2_ans;
	v1_minus_v2_ans.set(0, 1);
	v1_minus_v2_ans.set(1, 1);

	assert(v1_minus_v2 == v1_minus_v2_ans);

	PASSED("operator- and operator ==");
	
	// print v1 dot v2
	int v1_dot_v2 = v1.dot(v2);
	int result = 18;

	assert(v1_dot_v2 == result);

	PASSED(".dot()");
}

void TestVecFloat()
{
	Renderer::Vec<float, 2> v1;
	Renderer::Vec<float, 2> v2;

	// test the set function
	v1.set(0, 1);
	v1.set(1, 2);

	v2.set(0, 2);
	v2.set(1, 3);

	// test set and get exception
	try
	{
		v1.set(3, 2);
		assert(false);
	} catch(const Renderer::OutOfRangeException& e)
	{
		PASSED(".set() exception!");
	}

	try
	{
		v1.get(2);
		assert(false);
	} catch(const Renderer::OutOfRangeException& e)
	{
		PASSED(".get() exception!");
	}


	// test the get function
	assert(v1.get(0) == 1);
	assert(v1.get(1) == 2);
	assert(v2.get(0) == 2);
	assert(v2.get(1) == 3);

	PASSED(".set() and .get()");
	
	// test the length function
	float v1_length = v1.length();
	float v2_length = v2.length();
	assert(std::abs(v1_length - std::sqrt(5.f)) < TOLERANCE);
	assert(std::abs(v2_length - std::sqrt(13.f)) < TOLERANCE);

	PASSED(".length()");
	
	// normalize
	Renderer::Vec<float, 2> v1_norm(v1);
	std::ostringstream normalized_display;
	normalized_display << "Original Vector: ";
	v1_norm.print(normalized_display);
	v1_norm.normalize();
	normalized_display << " Normalized Vector: ";
	v1_norm.print(normalized_display, true);

	std::cout << "\t\t" << normalized_display.str() << std::endl;

	assert(std::abs(v1_norm.length() - 1.f) < TOLERANCE);
	assert(v1_norm != v1);

	PASSED(".normalize() and .print() and operator!= and copy constructor");

	// test operator =
	Renderer::Vec<float, 2> cp_v2 = v2;
	assert(cp_v2 == v2);
	cp_v2.set(0, 5);
	assert(cp_v2 != v2);
	
	PASSED("operator=");
	
	// test v1 * v2
	Renderer::Vec<float, 2> v1_times_v2 = v1 * v2;
	Renderer::Vec<float, 2> v1_times_v2_ans;
	v1_times_v2_ans.set(0, 2);
	v1_times_v2_ans.set(1, 6);

	assert(v1_times_v2 == v1_times_v2_ans);

	PASSED("operator* and operator==");

	// test v1 * scalar
	Renderer::Vec<float, 2> v1_times_scalar = v1 * 1.5f;
	Renderer::Vec<float, 2> v1_times_scalar_ans;
	v1_times_scalar_ans.set(0, 1.5f);
	v1_times_scalar_ans.set(1, 3.f);

	assert(v1_times_scalar == v1_times_scalar_ans);

	PASSED("operator*scalar and operator ==");

	// print v1 + v2
	Renderer::Vec<float, 2> v1_plus_v2 = v1 + v2;
	Renderer::Vec<float, 2> v1_plus_v2_ans;
	v1_plus_v2_ans.set(0, 3);
	v1_plus_v2_ans.set(1, 5);

	assert(v1_plus_v2 == v1_plus_v2_ans);

	PASSED("operator+ and operator==");
	
	// print v1 - v2
	Renderer::Vec<float, 2> v1_minus_v2 = v1 - v2;
	Renderer::Vec<float, 2> v1_minus_v2_ans;
	v1_minus_v2_ans.set(0, -1);
	v1_minus_v2_ans.set(1, -1);

	assert(v1_minus_v2 == v1_minus_v2_ans);

	PASSED("operator- and operator==");
	
	// print v1 dot v2
	float v1_dot_v2 = v1.dot(v2);
	float result = 8;

	assert(v1_dot_v2 == result);

	PASSED(".dot()");
}
