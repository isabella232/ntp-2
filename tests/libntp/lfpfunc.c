//#include "c_timestructs.h"

#include "testcalshims.h"

//#define UNITY_INCLUDE_DOUBLE //moved to unity -> nope, put into CFLAGS
//#define UNITY_EXCLUDE_FLOAT

#include "unity.h"

//#include "libntptest.h"
//#include "timestructs.h"



#include "ntp_fp.h"


#include <float.h>
#include <math.h>

#include <string.h> //
//#include <sstream>


typedef struct  {
	uint32_t h, l;
} lfp_hl;


/*
//----------------------------------------------------------------------
// OO-wrapper for 'l_fp'
//----------------------------------------------------------------------

class LFP
{
public:
	~LFP();
	LFP();
	LFP(const LFP& rhs);
	LFP(int32 i, u_int32 f);

	LFP  operator+ (const LFP &rhs) const;
	LFP& operator+=(const LFP &rhs);

	LFP  operator- (const LFP &rhs) const;
	LFP& operator-=(const LFP &rhs);

	LFP& operator=(const LFP &rhs);
	LFP  operator-() const;

	bool operator==(const LFP &rhs) const;

	LFP  neg() const;
	LFP  abs() const;
	int  signum() const;

	bool l_isgt (const LFP &rhs) const
		{ return L_ISGT(&_v, &rhs._v); }
	bool l_isgtu(const LFP &rhs) const
		{ return L_ISGTU(&_v, &rhs._v); }
	bool l_ishis(const LFP &rhs) const
		{ return L_ISHIS(&_v, &rhs._v); }
	bool l_isgeq(const LFP &rhs) const
		{ return L_ISGEQ(&_v, &rhs._v); }
	bool l_isequ(const LFP &rhs) const
		{ return L_ISEQU(&_v, &rhs._v); }

	int  ucmp(const LFP & rhs) const;
	int  scmp(const LFP & rhs) const;
	
	std::string   toString() const;
	std::ostream& toStream(std::ostream &oo) const;
	
	operator double() const;
	explicit LFP(double);
	
protected:
	LFP(const l_fp &rhs);

	static int cmp_work(u_int32 a[3], u_int32 b[3]);
	
	l_fp _v;
};
	
static std::ostream& operator<<(std::ostream &oo, const LFP& rhs)
{
	return rhs.toStream(oo);
}

//----------------------------------------------------------------------
// reference comparision
// This is implementad as a full signed MP-subtract in 3 limbs, where
// the operands are zero or sign extended before the subtraction is
// executed.
//----------------------------------------------------------------------
int  LFP::scmp(const LFP & rhs) const
{
	u_int32 a[3], b[3];
	const l_fp &op1(_v), &op2(rhs._v);
	
	a[0] = op1.l_uf; a[1] = op1.l_ui; a[2] = 0;
	b[0] = op2.l_uf; b[1] = op2.l_ui; b[2] = 0;

	a[2] -= (op1.l_i < 0);
	b[2] -= (op2.l_i < 0);

	return cmp_work(a,b);
}

int  LFP::ucmp(const LFP & rhs) const
{
	u_int32 a[3], b[3];
	const l_fp &op1(_v), &op2(rhs._v);
	
	a[0] = op1.l_uf; a[1] = op1.l_ui; a[2] = 0;
	b[0] = op2.l_uf; b[1] = op2.l_ui; b[2] = 0;

	return cmp_work(a,b);
}

int LFP::cmp_work(u_int32 a[3], u_int32 b[3])
{
	u_int32 cy, idx, tmp;
	for (cy = idx = 0; idx < 3; ++idx) {
		tmp = a[idx]; cy  = (a[idx] -=   cy  ) > tmp;
		tmp = a[idx]; cy |= (a[idx] -= b[idx]) > tmp;
	}
	if (a[2])
		return -1;
	return a[0] || a[1];
}

//----------------------------------------------------------------------
// imlementation of the LFP stuff
// This should be easy enough...
//----------------------------------------------------------------------


LFP::LFP()
{
	_v.l_ui = 0;
	_v.l_uf = 0;
}

LFP::LFP(int32 i, u_int32 f)
{
	_v.l_i  = i;
	_v.l_uf = f;
}

LFP::LFP(const LFP &rhs)
{
	_v = rhs._v;
}

LFP::LFP(const l_fp & rhs)
{
	_v = rhs;
}

LFP& LFP::operator=(const LFP & rhs)
{
	_v = rhs._v;
	return *this;
}

LFP& LFP::operator+=(const LFP & rhs)
{
	L_ADD(&_v, &rhs._v);
	return *this;
}

LFP& LFP::operator-=(const LFP & rhs)
{
	L_SUB(&_v, &rhs._v);
	return *this;
}

LFP LFP::operator+(const LFP &rhs) const
{
	LFP tmp(*this);
	return tmp += rhs;
}

LFP LFP::operator-(const LFP &rhs) const
{
	LFP tmp(*this);
	return tmp -= rhs;
}

LFP LFP::operator-() const
{
	LFP tmp(*this);
	L_NEG(&tmp._v);
	return tmp;
}


std::string
LFP::toString() const
{
	std::ostringstream oss;
	toStream(oss);
	return oss.str();
}

std::ostream&
LFP::toStream(std::ostream &os) const
{
	return os
	    << mfptoa(_v.l_ui, _v.l_uf, 9)
	    << " [$" << std::setw(8) << std::setfill('0') << std::hex << _v.l_ui
	    <<  ':'  << std::setw(8) << std::setfill('0') << std::hex << _v.l_uf
	    << ']';
}

bool LFP::operator==(const LFP &rhs) const
{
	return L_ISEQU(&_v, &rhs._v);
}




//----------------------------------------------------------------------
// testing the relational macros works better with proper predicate
// formatting functions; it slows down the tests a bit, but makes for
// readable failure messages.
//----------------------------------------------------------------------

testing::AssertionResult isgt_p(
	const LFP &op1, const LFP &op2)
{
	if (op1.l_isgt(op2))
		return testing::AssertionSuccess()
		    << "L_ISGT(" << op1 << "," << op2 << ") is true";
	else
		return testing::AssertionFailure()
		    << "L_ISGT(" << op1 << "," << op2 << ") is false";
}

testing::AssertionResult isgeq_p(
	const LFP &op1, const LFP &op2)
{
	if (op1.l_isgeq(op2))
		return testing::AssertionSuccess()
		    << "L_ISGEQ(" << op1 << "," << op2 << ") is true";
	else
		return testing::AssertionFailure()
		    << "L_ISGEQ(" << op1 << "," << op2 << ") is false";
}

testing::AssertionResult isgtu_p(
	const LFP &op1, const LFP &op2)
{
	if (op1.l_isgtu(op2))
		return testing::AssertionSuccess()
		    << "L_ISGTU(" << op1 << "," << op2 << ") is true";
	else
		return testing::AssertionFailure()
		    << "L_ISGTU(" << op1 << "," << op2 << ") is false";
}

testing::AssertionResult ishis_p(
	const LFP &op1, const LFP &op2)
{
	if (op1.l_ishis(op2))
		return testing::AssertionSuccess()
		    << "L_ISHIS(" << op1 << "," << op2 << ") is true";
	else
		return testing::AssertionFailure()
		    << "L_ISHIS(" << op1 << "," << op2 << ") is false";
}

testing::AssertionResult isequ_p(
	const LFP &op1, const LFP &op2)
{
	if (op1.l_isequ(op2))
		return testing::AssertionSuccess()
		    << "L_ISEQU(" << op1 << "," << op2 << ") is true";
	else
		return testing::AssertionFailure()
		    << "L_ISEQU(" << op1 << "," << op2 << ") is false";
}

*/

//----------------------------------------------------------------------
// test data table for add/sub and compare
//----------------------------------------------------------------------


static const lfp_hl addsub_tab[][3] = {
	// trivial idendity:
	{{0 ,0         }, { 0,0         }, { 0,0}},
	// with carry from fraction and sign change:
	{{-1,0x80000000}, { 0,0x80000000}, { 0,0}},
	// without carry from fraction
	{{ 1,0x40000000}, { 1,0x40000000}, { 2,0x80000000}},
	// with carry from fraction:
	{{ 1,0xC0000000}, { 1,0xC0000000}, { 3,0x80000000}},
	// with carry from fraction and sign change:
	{{0x7FFFFFFF, 0x7FFFFFFF}, {0x7FFFFFFF,0x7FFFFFFF}, {0xFFFFFFFE,0xFFFFFFFE}},
	// two tests w/o carry (used for l_fp<-->double):
	{{0x55555555,0xAAAAAAAA}, {0x11111111,0x11111111}, {0x66666666,0xBBBBBBBB}},
	{{0x55555555,0x55555555}, {0x11111111,0x11111111}, {0x66666666,0x66666666}},
	// wide-range test, triggers compare trouble
	{{0x80000000,0x00000001}, {0xFFFFFFFF,0xFFFFFFFE}, {0x7FFFFFFF,0xFFFFFFFF}}
};
static const size_t addsub_cnt = (sizeof(addsub_tab)/sizeof(addsub_tab[0]));
static const size_t addsub_tot = (sizeof(addsub_tab)/sizeof(addsub_tab[0][0]));



//----------------------------------------------------------------------
// epsilon estimation for the precision of a conversion double --> l_fp
//
// The error estimation limit is as follows:
//  * The 'l_fp' fixed point fraction has 32 bits precision, so we allow
//    for the LSB to toggle by clamping the epsilon to be at least 2^(-31)
//
//  * The double mantissa has a precsion 54 bits, so the other minimum is
//    dval * (2^(-53))
//
//  The maximum of those two boundaries is used for the check.
//
// Note: once there are more than 54 bits between the highest and lowest
// '1'-bit of the l_fp value, the roundtrip *will* create truncation
// errors. This is an inherent property caused by the 54-bit mantissa of
// the 'double' type.
double eps(double d)
{
	return fmax(ldexp(1.0, -31), ldexp(fabs(d), -53)); //max<double>
}


l_fp l_fp_init(int32 i, u_int32 f)
{
	l_fp temp;
	temp.l_i  = i;
	temp.l_uf = f;

	return temp;
}



l_fp l_fp_add(const l_fp first, const l_fp second) //&rhs!!!
{
	l_fp temp;
	//LFP tmp(*this);
	//return tmp += rhs;
	temp = first;
	L_ADD(&temp, &second);
	//return first + second;
	return temp;
}

l_fp l_fp_subtract(const l_fp first, const l_fp second) //&rhs!!!
{
	l_fp temp;
	//LFP tmp(*this);
	//return tmp += rhs;
	temp = first;
	L_SUB(&temp, &second);
	
	return temp;
}

l_fp l_fp_negate(const l_fp first)
{
	l_fp temp;
	temp = first; //is this line really necessary?
	L_NEG(&temp);
	
	return temp;
}

l_fp l_fp_abs(const l_fp first)
{
	l_fp temp = first;
	//LFP tmp(*this);
	if (L_ISNEG(&temp))
		L_NEG(&temp);
	return temp;
}

int l_fp_signum(const l_fp first)
{
	if (first.l_ui & 0x80000000u)
		return -1;
	return (first.l_ui || first.l_uf);
}

double l_fp_convert_to_double(const l_fp first)
{
	double res;
	LFPTOD(&first, res);
	return res;
}

l_fp l_fp_init_from_double( double rhs)
{
	l_fp temp;
	DTOLFP(rhs, &temp);
	return temp;
}


//----------------------------------------------------------------------
// test addition
//----------------------------------------------------------------------
void test_AdditionLR() {
	
	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {


		l_fp op1 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		//LFP op1(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp op2 = l_fp_init(addsub_tab[idx][1].h, addsub_tab[idx][1].l);
		//LFP exp(addsub_tab[idx][2].h, addsub_tab[idx][2].l);
		l_fp exp = l_fp_init(addsub_tab[idx][2].h, addsub_tab[idx][2].l);
		//LFP res(op1 + op2);
		l_fp res = l_fp_add(op1,op2);		

		TEST_ASSERT_EQUAL_MEMORY(&exp, &res,sizeof(exp));
	}	
}

void test_AdditionRL() {

	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp op2 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp op1 = l_fp_init(addsub_tab[idx][1].h, addsub_tab[idx][1].l);
		l_fp exp = l_fp_init(addsub_tab[idx][2].h, addsub_tab[idx][2].l);
		l_fp res = l_fp_add(op1,op2);

		TEST_ASSERT_EQUAL_MEMORY(&exp, &res,sizeof(exp));
	}	
}



//----------------------------------------------------------------------
// test subtraction
//----------------------------------------------------------------------
void test_SubtractionLR() {

	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp op2 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp exp = l_fp_init(addsub_tab[idx][1].h, addsub_tab[idx][1].l);
		l_fp op1 = l_fp_init(addsub_tab[idx][2].h, addsub_tab[idx][2].l);
		l_fp res = l_fp_subtract(op1,op2);
		//LFP res(op1 - op2);
				
		TEST_ASSERT_EQUAL_MEMORY(&exp, &res,sizeof(exp));
	}	
}

void test_SubtractionRL() {

	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp exp = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp op2 = l_fp_init(addsub_tab[idx][1].h, addsub_tab[idx][1].l);
		l_fp op1 = l_fp_init(addsub_tab[idx][2].h, addsub_tab[idx][2].l);
		l_fp res = l_fp_subtract(op1,op2);

		TEST_ASSERT_EQUAL_MEMORY(&exp, &res,sizeof(exp));
	}	
}

//----------------------------------------------------------------------
// test negation
//----------------------------------------------------------------------

void test_Negation() {

	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp op1 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp op2 = l_fp_negate(op1);
		l_fp sum = l_fp_add(op1, op2);
		
		l_fp zero = l_fp_init(0,0);
		TEST_ASSERT_EQUAL_MEMORY(&zero, &sum,sizeof(sum));
	
	}	
}



//----------------------------------------------------------------------
// test absolute value
//----------------------------------------------------------------------
void test_Absolute() {
	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp op1 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		l_fp op2 = l_fp_abs(op1);

		TEST_ASSERT_TRUE(l_fp_signum(op2) >= 0);		

		if (l_fp_signum(op1) >= 0)
			op1 = l_fp_subtract(op1,op2);
			//op1 -= op2;
		else
			op1 = l_fp_add(op1,op2);
			//op1 += op2;
		
		l_fp zero = l_fp_init(0,0);
		TEST_ASSERT_EQUAL_MEMORY(&zero, &op1,sizeof(op1));
	}

	// There is one special case we have to check: the minimum
	// value cannot be negated, or, to be more precise, the
	// negation reproduces the original pattern.
	l_fp minVal = l_fp_init(0x80000000, 0x00000000);
	l_fp minAbs = l_fp_abs(minVal);
	TEST_ASSERT_EQUAL(-1, l_fp_signum(minVal));
	TEST_ASSERT_EQUAL_MEMORY(&minVal, &minAbs,sizeof(minAbs));
}


//----------------------------------------------------------------------
// fp -> double -> fp rountrip test
//----------------------------------------------------------------------
void test_FDF_RoundTrip() {
	// since a l_fp has 64 bits in it's mantissa and a double has
	// only 54 bits available (including the hidden '1') we have to
	// make a few concessions on the roundtrip precision. The 'eps()'
	// function makes an educated guess about the avilable precision
	// and checks the difference in the two 'l_fp' values against
	// that limit.
	size_t idx=0;
	for (idx=0; idx < addsub_cnt; ++idx) {
		l_fp op1 = l_fp_init(addsub_tab[idx][0].h, addsub_tab[idx][0].l);
		double op2 = l_fp_convert_to_double(op1); //double op2(op1);
		l_fp op3 = l_fp_init_from_double(op2); //LFP    op3(op2);

		// for manual checks only:
		int delta = 10;
		// std::cout << std::setprecision(16) << op2 << std::endl;

		l_fp temp = l_fp_subtract(op1,op3);
		double d = l_fp_convert_to_double(temp);
		TEST_ASSERT_DOUBLE_WITHIN(0.000001,fabs(d), eps(op2));
 
		//TEST_ASSERT_EQUAL_DOUBLE(fabs(l_fp_subtract(op1,op3)), eps(op2));  
		//TEST_ASSERT_DOUBLE_WITHIN(delta, fabs(op1-op3), eps(op2));    		
		//ASSERT_LE(fabs(op1-op3), eps(op2));
	}	
}
/*
//----------------------------------------------------------------------
// test the compare stuff
//
// This uses the local compare and checks if the operations using the
// macros in 'ntp_fp.h' produce mathing results.
// ----------------------------------------------------------------------
void test_SignedRelOps() {
	const lfp_hl * tv(&addsub_tab[0][0]);
	for (size_t lc=addsub_tot-1; lc; --lc,++tv) {
		LFP op1(tv[0].h,tv[0].l);
		LFP op2(tv[1].h,tv[1].l);
		int cmp(op1.scmp(op2));

		switch (cmp) {
		case -1:
			std::swap(op1, op2);
		case 1:
			TEST_ASSERT_TRUE (isgt_p(op1,op2));
			TEST_ASSERT_FALSE(isgt_p(op2,op1));

			TEST_ASSERT_TRUE (isgeq_p(op1,op2));
			TEST_ASSERT_FALSE(isgeq_p(op2,op1));

			TEST_ASSERT_FALSE(isequ_p(op1,op2));
			TEST_ASSERT_FALSE(isequ_p(op2,op1));
			break;
		case 0:
			TEST_ASSERT_FALSE(isgt_p(op1,op2));
			TEST_ASSERT_FALSE(isgt_p(op2,op1));

			TEST_ASSERT_TRUE (isgeq_p(op1,op2));
			TEST_ASSERT_TRUE (isgeq_p(op2,op1));

			TEST_ASSERT_TRUE (isequ_p(op1,op2));
			TEST_ASSERT_TRUE (isequ_p(op2,op1));
			break;
		default:
			FAIL() << "unexpected SCMP result: " << cmp;
		}
	}
}

void test_UnsignedRelOps() {
	const lfp_hl * tv(&addsub_tab[0][0]);
	for (size_t lc=addsub_tot-1; lc; --lc,++tv) {
		LFP op1(tv[0].h,tv[0].l);
		LFP op2(tv[1].h,tv[1].l);
		int cmp(op1.ucmp(op2));

		switch (cmp) {
		case -1:
			std::swap(op1, op2);
		case 1:
			TEST_ASSERT_TRUE (isgtu_p(op1,op2));
			TEST_ASSERT_FALSE(isgtu_p(op2,op1));

			TEST_ASSERT_TRUE (ishis_p(op1,op2));
			TEST_ASSERT_FALSE(ishis_p(op2,op1));
			break;
		case 0:
			TEST_ASSERT_FALSE(isgtu_p(op1,op2));
			TEST_ASSERT_FALSE(isgtu_p(op2,op1));

			TEST_ASSERT_TRUE (ishis_p(op1,op2));
			TEST_ASSERT_TRUE (ishis_p(op2,op1));
			break;
		default:
			FAIL() << "unexpected UCMP result: " << cmp;
		}
	}
}

*/

//----------------------------------------------------------------------
// that's all folks... but feel free to add things!
//----------------------------------------------------------------------