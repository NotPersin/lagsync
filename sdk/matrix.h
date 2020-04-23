#pragma once

struct viewmatrix
{
    float matrix[4][4];

    __forceinline float* operator[](const int i)
    {
        return matrix[i];
    }

    __forceinline const float* operator[](const int i) const
    {
        return matrix[i];
    }
};

struct matrix3x4
{
    float matrix[3][4];

    __forceinline float* operator[](const int i)
    {
        return matrix[i];
    }

    __forceinline const float* operator[](const int i) const
    {
        return matrix[i];
    }
};
struct matrix3x4t
{
	float matrix[16];
};

typedef float vec_t;

class VMatrix
{
public:

	VMatrix();
	VMatrix(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix(const c_vector3d& forward, const c_vector3d& left, const c_vector3d& up);

	// Construct from a 3x4 matrix
	VMatrix(const matrix3x4t& matrix3x4);

	// Set the values in the matrix.
	void		Init(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);


	// Initialize from a 3x4
	void		Init(const matrix3x4t& matrix3x4);

	// array access
	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}

	// Get a pointer to m[0][0]
	inline float* Base()
	{
		return &m[0][0];
	}

	inline const float* Base() const
	{
		return &m[0][0];
	}

	void		SetLeft(const c_vector3d& vLeft);
	void		SetUp(const c_vector3d& vUp);
	void		SetForward(const c_vector3d& vForward);

	void		GetBasisc_vector3ds(c_vector3d& vForward, c_vector3d& vLeft, c_vector3d& vUp) const;
	void		SetBasisc_vector3ds(const c_vector3d& vForward, const c_vector3d& vLeft, const c_vector3d& vUp);

	// Get/set the translation.
	c_vector3d& GetTranslation(c_vector3d& vTrans) const;
	void		SetTranslation(const c_vector3d& vTrans);

	void		PreTranslate(const c_vector3d& vTrans);
	void		PostTranslate(const c_vector3d& vTrans);

	matrix3x4t& As3x4();
	const matrix3x4t& As3x4() const;
	void		CopyFrom3x4(const matrix3x4t& m3x4);
	void		Set3x4(matrix3x4t& matrix3x4) const;

	bool		operator==(const VMatrix& src) const;
	bool		operator!=(const VMatrix& src) const { return !(*this == src); }

	// Access the basis c_vector3ds.
	c_vector3d		GetLeft() const;
	c_vector3d		GetUp() const;
	c_vector3d		GetForward() const;
	c_vector3d		GetTranslation() const;


	// Matrix->c_vector3d operations.
public:
	// Multiply by a 3D c_vector3d (same as operator*).
	void		V3Mul(const c_vector3d& vIn, c_vector3d& vOut) const;

	// Multiply by a 4D c_vector3d.
	//void		V4Mul( const c_vector3d4D &vIn, c_vector3d4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	c_vector3d		ApplyRotation(const c_vector3d& vVec) const;

	// Multiply by a c_vector3d (divides by w, assumes input w is 1).
	c_vector3d		operator*(const c_vector3d& vVec) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	c_vector3d		VMul3x3(const c_vector3d& vVec) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	c_vector3d		VMul3x3Transpose(const c_vector3d& vVec) const;

	// Multiply by the upper 3 rows.
	c_vector3d		VMul4x3(const c_vector3d& vVec) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	c_vector3d		VMul4x3Transpose(const c_vector3d& vVec) const;


	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane		operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	VMatrix& operator=(const VMatrix& mOther);

	// Multiply two matrices (out = this * vm).
	void		MatrixMul(const VMatrix& vm, VMatrix& out) const;

	// Add two matrices.
	const VMatrix& operator+=(const VMatrix& other);

	// Just calls MatrixMul and returns the result.	
	VMatrix		operator*(const VMatrix& mOther) const;

	// Add/Subtract two matrices.
	VMatrix		operator+(const VMatrix& other) const;
	VMatrix		operator-(const VMatrix& other) const;

	// Negation.
	VMatrix		operator-() const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix		operator~() const;

	// Matrix operations.
public:
	// Set to identity.
	void		Identity();

	bool		IsIdentity() const;

	// Setup a matrix for origin and angles.
	void		SetupMatrixOrgAngles(const c_vector3d& origin, const c_vector3d& vAngles);

	// General inverse. This may fail so check the return!
	bool		InverseGeneral(VMatrix& vInverse) const;

	// Does a fast inverse, assuming the matrix only contains translation and rotation.
	void		InverseTR(VMatrix& mRet) const;

	// Usually used for debug checks. Returns true if the upper 3x3 contains
	// unit c_vector3ds and they are all orthogonal.
	bool		IsRotationMatrix() const;

	// This calls the other InverseTR and returns the result.
	VMatrix		InverseTR() const;

	// Get the scale of the matrix's basis c_vector3ds.
	c_vector3d		GetScale() const;

	// (Fast) multiply by a scaling matrix setup from vScale.
	VMatrix		Scale(const c_vector3d& vScale);

	// Normalize the basis c_vector3ds.
	VMatrix		NormalizeBasisc_vector3ds() const;

	// Transpose.
	VMatrix		Transpose() const;

	// Transpose upper-left 3x3.
	VMatrix		Transpose3x3() const;

public:
	// The matrix.
	vec_t		m[4][4];
};