#include "stdafx.h"
#include "MinimumRectangle.h"


CMinimumRectangle::CMinimumRectangle()
{
}

CMinimumRectangle::~CMinimumRectangle()
{
}

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <queue>
#include <stack>
#include <map>
using namespace std;
typedef long long LL;
const int inf = 0x3f3f3f3f;
const double eps = 1e-8;
const double pi = acos(-1.0);
typedef double typev;
const int N = 4010;
int sign(double d) {
	return d < -eps ? -1 : (d > eps);
}
struct point {
	typev x, y;
	point operator-(point d) {
		point dd;
		dd.x = this->x - d.x;
		dd.y = this->y - d.y;
		return dd;
	}
	point operator+(point d) {
		point dd;
		dd.x = this->x + d.x;
		dd.y = this->y + d.y;
		return dd;
	}
	//void read() { scanf("%lf%lf", &x, &y); }
}ps[N], pd[N];
int n, cn;
double dist1(point d1, point d2) {
	return sqrt(pow(d1.x - d2.x, 2.0) + pow(d1.y - d2.y, 2.0));
}
double dist2(point d1, point d2) {
	return pow(d1.x - d2.x, 2.0) + pow(d1.y - d2.y, 2.0);
}
bool cmp1(point d1, point d2) {
	return d1.y < d2.y || (d1.y == d2.y && d1.x < d2.x);
}
//st1-->ed1���st2-->ed2��ֵ
typev xmul(point st1, point ed1, point st2, point ed2) {
	return (ed1.x - st1.x) * (ed2.y - st2.y) - (ed1.y - st1.y) * (ed2.x - st2.x);
}
typev dmul(point st1, point ed1, point st2, point ed2) {
	return (ed1.x - st1.x) * (ed2.x - st2.x) + (ed1.y - st1.y) * (ed2.y - st2.y);
}
//�������
struct poly {
	static const int N = 4010; //���������ֵ
	point ps[N + 5]; //��ʱ��洢����εĵ�,[0,pn-1]�洢��
	int pn;  //����
	poly() { pn = 0; }
	//�ӽ�һ����
	void push(point tp) {
		ps[pn++] = tp;
	}
	//��k��λ��
	int trim(int k) {
		return (k + pn) % pn;
	}
	void clear() { pn = 0; }
};
//���غ���n����ĵ㼯ps��͹��
poly graham(point* ps, int n) {
	sort(ps, ps + n, cmp1);
	poly ans;
	if (n <= 2) {
		for (int i = 0; i < n; i++) {
			ans.push(ps[i]);
		}
		return ans;
	}
	ans.push(ps[0]);
	ans.push(ps[1]);
	point* tps = ans.ps;
	int top = -1;
	tps[++top] = ps[0];
	tps[++top] = ps[1];
	for (int i = 2; i < n; i++) {
		while (top > 0 && xmul(tps[top - 1], tps[top], tps[top - 1], ps[i]) <= 0) top--;
		tps[++top] = ps[i];
	}
	int tmp = top;  //ע��Ҫ��ֵ��tmp��
	for (int i = n - 2; i >= 0; i--) {
		while (top > tmp && xmul(tps[top - 1], tps[top], tps[top - 1], ps[i]) <= 0) top--;
		tps[++top] = ps[i];
	}
	ans.pn = top;
	return ans;
}
//���p��st->ed�Ĵ��㣬�в�������
point getRoot(point p, point st, point ed) {
	point ans;
	double u = ((ed.x - st.x)*(ed.x - st.x) + (ed.y - st.y)*(ed.y - st.y));
	u = ((ed.x - st.x)*(ed.x - p.x) + (ed.y - st.y)*(ed.y - p.y)) / u;
	ans.x = u*st.x + (1 - u)*ed.x;
	ans.y = u*st.y + (1 - u)*ed.y;
	return ans;
}
//nextΪֱ��(st,ed)�ϵĵ㣬����next��(st,ed)���ִ�ֱ��������l֮��ĵ�
point change(point st, point ed, point next, double l) {
	point dd;
	dd.x = -(ed - st).y;
	dd.y = (ed - st).x;
	double len = sqrt(dd.x * dd.x + dd.y * dd.y);
	dd.x /= len, dd.y /= len;
	dd.x *= l, dd.y *= l;
	dd = dd + next;
	return dd;
}
//��n����ĵ㼯ps����С������Σ����ѽ������ds(dsΪһ��������4�����鼴��,ds�еĵ�����ʱ���)�У������������С�����
double getMinAreaRect(point* ps, int n, point* ds) {
	int cn, i;
	double ans;
	point* con;
	poly tpoly = graham(ps, n);
	con = tpoly.ps;
	cn = tpoly.pn;
	if (cn <= 2) {
		ds[0] = con[0]; ds[1] = con[1];
		ds[2] = con[1]; ds[3] = con[0];
		ans = 0;
	}
	else {
		int  l, r, u;
		double tmp, len;
		con[cn] = con[0];
		ans = 1e40;
		l = i = 0;
		while (dmul(con[i], con[i + 1], con[i], con[l])
			>= dmul(con[i], con[i + 1], con[i], con[(l - 1 + cn) % cn])) {
			l = (l - 1 + cn) % cn;
		}
		for (r = u = i = 0; i < cn; i++) {
			while (xmul(con[i], con[i + 1], con[i], con[u])
				<= xmul(con[i], con[i + 1], con[i], con[(u + 1) % cn])) {
				u = (u + 1) % cn;
			}
			while (dmul(con[i], con[i + 1], con[i], con[r])
				<= dmul(con[i], con[i + 1], con[i], con[(r + 1) % cn])) {
				r = (r + 1) % cn;
			}
			while (dmul(con[i], con[i + 1], con[i], con[l])
				>= dmul(con[i], con[i + 1], con[i], con[(l + 1) % cn])) {
				l = (l + 1) % cn;
			}
			tmp = dmul(con[i], con[i + 1], con[i], con[r]) - dmul(con[i], con[i + 1], con[i], con[l]);
			tmp *= xmul(con[i], con[i + 1], con[i], con[u]);
			tmp /= dist2(con[i], con[i + 1]);
			len = xmul(con[i], con[i + 1], con[i], con[u]) / dist1(con[i], con[i + 1]);
			if (sign(tmp - ans) < 0) {
				ans = tmp;
				ds[0] = getRoot(con[l], con[i], con[i + 1]);
				ds[1] = getRoot(con[r], con[i + 1], con[i]);
				ds[2] = change(con[i], con[i + 1], ds[1], len);
				ds[3] = change(con[i], con[i + 1], ds[0], len);
			}
		}
	}
	return ans + eps;
}

AcGePoint3dArray CMinimumRectangle::getMinRact(AcGePoint3dArray& allpoints)
{
	AcGePoint3dArray outpts;
	int n = allpoints.length();
	for (int i = 0; i < n; i++) {
		ps[i].x = allpoints[i].x;
		ps[i].y = allpoints[i].y;
	}

	double res = getMinAreaRect(ps, n, pd);
	for (int k = 0; k < 4; ++k)
	{
		//acutPrintf(_T("%.5lf,%.5lf,0\n"), pd[k].x, pd[k].y);
		AcGePoint3d temmpt;
		temmpt.x = pd[k].x;
		temmpt.y = pd[k].y;
		temmpt.z = 0;
		outpts.append(temmpt);
	}
	return outpts;
}

AcGePoint3dArray CMinimumRectangle::testfilterPoints(AcGePoint3dArray& allpoints, AcGePoint3dArray& filterpoints)
{
	AcGePoint3dArray outpts;

	if (filterpoints.length() <= 1)
	{
		for (int i=0; i<allpoints.length();++i)
		{
			outpts.append(allpoints[i]);
		}
		return outpts;
	}

	//�ռ����ɶ���ߵĵ�
	AcGePoint2dArray plylinepts;
	for (int i = 0; i < filterpoints.length(); ++i)
	{
		AcGePoint2d temppt = AcGePoint2d(filterpoints[i].x, filterpoints[i].y);
		plylinepts.append(temppt);
	}

	//���������
	AcDbPolyline *pPoly = new AcDbPolyline(plylinepts.length());
	for (int i = 0; i < plylinepts.length(); i++)
	{
		pPoly->addVertexAt(i, plylinepts.at(i), 0, 1, 1);
	}
	pPoly->setClosed(Adesk::kTrue);

	//���������ڶ�����ϵĵ㣬���洢��pts��
	for (int i = 0; i < allpoints.length(); ++i)
	{
		if (filterpoints.contains(allpoints[i]))
		{
			outpts.append(allpoints[i]);
			allpoints.remove(allpoints[i]);
			i--;
		}
		else
		{
			AcGePoint3d SearchPt;  //Ϊ��allpoints[i]������������ĵ�
			pPoly->getClosestPointTo(allpoints[i], SearchPt, Adesk::kFalse);  //����pline����������ߣ��˹�ʽ���SearchPt
			AcGePoint3d allpt = allpoints[i];
			double dis = allpt.distanceTo(SearchPt);
			if (dis <= 1)
			{
				outpts.append(allpoints[i]);
				allpoints.remove(allpoints[i]);
				i--;
			}
		}

	}

	if (pPoly)
		pPoly->close();

	return outpts;

}
