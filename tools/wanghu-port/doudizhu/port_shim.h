#pragma once
// ===== 网狐 -> Linux 移植垫片 =====
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef int            INT_PTR;
typedef long           LONG;
typedef std::string    CString;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// 斗地主 3 人（网狐 CMD_Land.h 里就是 3）
#ifndef GAME_PLAYER
#define GAME_PLAYER 3
#endif

#define ZeroMemory(p, n) memset((p), 0, (n))
#define CopyMemory(d, s, n) memcpy((d), (s), (n))
#define ASSERT(x) do { (void)sizeof(x); } while (0)
#define CountArray(x) (sizeof(x) / sizeof((x)[0]))

template <class TYPE, class ARG_TYPE = const TYPE&>
class CArrayTemplate {
public:
    std::vector<TYPE> m_v;
    INT_PTR GetCount() const { return (INT_PTR)m_v.size(); }
    INT_PTR GetSize() const { return (INT_PTR)m_v.size(); }
    bool IsEmpty() const { return m_v.empty(); }
    void SetSize(INT_PTR n) { m_v.resize((size_t)n); }
    void RemoveAll() { m_v.clear(); }
    INT_PTR Add(ARG_TYPE t) { m_v.push_back(t); return (INT_PTR)m_v.size() - 1; }
    void InsertAt(INT_PTR i, ARG_TYPE t) { m_v.insert(m_v.begin() + i, t); }
    void RemoveAt(INT_PTR i) { m_v.erase(m_v.begin() + i); }
    TYPE GetAt(INT_PTR i) const { return m_v[(size_t)i]; }
    TYPE& ElementAt(INT_PTR i) { return m_v[(size_t)i]; }
    TYPE& operator[](INT_PTR i) { return m_v[(size_t)i]; }
    const TYPE& operator[](INT_PTR i) const { return m_v[(size_t)i]; }
    TYPE* GetData() { return m_v.data(); }
    const TYPE* GetData() const { return m_v.data(); }
};

