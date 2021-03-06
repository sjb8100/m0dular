#ifndef INTERSECT_IMPL_H
#define INTERSECT_IMPL_H

/*
  Template implementations of SOA intersection functions.
  These can not be inside a source file since templates then will not be created.
*/

#include "intersect.h"

template<size_t Y>
static vec3soa<float, Y> DirBetweenLines(vec3soa<float, Y>& __restrict va, vec3soa<float, Y>& __restrict vb, vec3soa<float, Y>& __restrict vc, vec3soa<float, Y>& __restrict vd)
{
    auto u = vb - va;
	auto v = vd - vc;
    auto w = va - vc;
	float a[Y], b[Y], c[Y], d[Y], e[Y], sN[Y], sD[Y], tN[Y], tD[Y], sc[Y], tc[Y];

	u.LengthSqr(a);
	u.Dot(v, b);
	v.LengthSqr(c);
	u.Dot(w, d);
	v.Dot(w, e);

	float D[Y];

	for (size_t i = 0; i < Y; i++)
		D[i] = a[i] * c[i] - b[i] * b[i];

	for (size_t i = 0; i < Y; i++)
		sD[i] = D[i];

	for (size_t i = 0; i < Y; i++)
		tD[i] = D[i];

	for (size_t i = 0; i < Y; i++) {
		if (D[i] == 0.f) {
			sN[i] = 0.0;
			sD[i] = 1.0;
			tN[i] = e[i];
			tD[i] = c[i];
		} else {
			sN[i] = (b[i] * e[i] - c[i] * d[i]);
			tN[i] = (a[i] * e[i] - b[i] * d[i]);
			if (sN[i] < 0.0) {
				sN[i] = 0.0;
				tN[i] = e[i];
				tD[i] = c[i];
			} else if (sN[i] > sD[i]) {
				sN[i] = sD[i];
				tN[i] = e[i] + b[i];
				tD[i] = c[i];
			}
		}
	}

	for (size_t i = 0; i < Y; i++) {
		if (tN[i] < 0.0) {
			tN[i] = 0.0;

			if (-d[i] < 0.0)
				sN[i] = 0.0;
			else if (-d[i] > a[i])
				sN[i] = sD[i];
			else {
				sN[i] = -d[i];
				sD[i] = a[i];
			}
		} else if (tN[i] > tD[i]) {
			tN[i] = tD[i];

			if ((-d[i] + b[i]) < 0.0)
				sN[i] = 0;
			else if ((-d[i] + b[i]) > a[i])
				sN[i] = sD[i];
			else {
				sN[i] = (-d[i] + b[i]);
				sD[i] = a[i];
			}
		}
	}

	for (size_t i = 0; i < Y; i++)
		sc[i] = (sN[i] == 0.f ? 0.f : sN[i] / sD[i]);

	for (size_t i = 0; i < Y; i++)
		tc[i] = (tN[i] == 0.f ? 0.f : tN[i] / tD[i]);

    return w + (u * sc) - (v * tc);
}

template<size_t Y>
unsigned int CapsuleCollider::IntersectSOA(vec3soa<float, Y>& __restrict a, vec3soa<float, Y>& __restrict b, vec3soa<float, Y>* __restrict out)
{
	unsigned int flags = 0;
    svec3<Y> soaStart, soaEnd;
	soaStart = start;
	soaEnd = end;
	float radiusSqr = radius * radius;

	vec3soa<float, Y> dirs = DirBetweenLines(a, b, soaStart, soaEnd);

	if (out)
		*out = dirs;

	float lens[Y];
	dirs.LengthSqr(lens);

	for (size_t i = 0; i < Y; i++)
		if (lens[i] <= radiusSqr)
			flags |= (1 << i);

	return flags;
}

template unsigned int CapsuleCollider::IntersectSOA(nvec3& __restrict a, nvec3& __restrict b, nvec3* __restrict out);

template <size_t N>
unsigned int CapsuleColliderSOA<N>::Intersect(vec3_t a, vec3_t b, svec3<N>* out)
{
	unsigned int flags = 0;
	svec3<N> va = a, vb = b;

	svec3<N> dirs = DirBetweenLines(va, vb, start, end);

	if (out)
		*out = dirs;

	float lens[N];
	dirs.LengthSqr(lens);

	for (size_t i = 0; i < N; i++)
		if (lens[i] <= radius[i] * radius[i])
			flags |= (1 << i);

	return flags;
}

#endif
