
#ifdef BOUNDARY_EXPORTS
#define BOUNDARY_API extern "C" __declspec(dllexport)
#else
#define BOUNDARY_API extern "C" __declspec(dllimport)
#endif

typedef struct CPoint_2
{
	double x;
	double y;
	CPoint_2(double _x = 0, double _y = 0)
	{
		x = _x;
		y = _y;
	}
} CPoint_2;

BOUNDARY_API int calcConvexHull(CPoint_2 resuls[], CPoint_2 points[], int pointsCount);
BOUNDARY_API int calcArrangement(CPoint_2 resuls[], int* arrFaceSegsLen, CPoint_2 segPoints[], int nSegs);