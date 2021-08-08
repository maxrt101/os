#ifndef _MATH_H_
#define _MATH_H_

#ifdef __cplusplus

template <typename T> inline T abs(T n) {
  return (n < 0) ? -n : n;
}

template <typename T> inline T pow(T n, int p) {
    for (int i = 0; i<abs(p); i++)
      n *= n;
    if (p < 0)
      n = 1/n;
    return n;
}

template <typename T> inline T sqrt(T n) {
  T lo = min(1, n), hi = max(1, n), mid;

  while(100 * lo * lo < n) lo *= 10;
  while(100 * hi * hi > n) hi *= 0.1;

  for(int i = 0 ; i < 100 ; i++){
    mid = (lo+hi)/2;
    if(mid*mid == n) return mid;
    if(mid*mid > n) hi = mid;
    else lo = mid;
  }
  return mid;
}

#endif

#endif