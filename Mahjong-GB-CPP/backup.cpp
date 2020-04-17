#include "MahjongGB/MahjongGB.h"
#include "jsoncpp/json.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <vector>
using namespace std;
typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;
typedef pair<int, int> PII;
typedef vector<int> VI;
#define fi first
#define se second
#define MP make_pair
#define rep(i, a, b) for (int i = a; i <= b; i++)
//#define SIMPLEIO 0

int read()
{
    int v = 0, f = 1;
    char c = getchar();
    while (c < 48 || 57 < c) {if (c == '-') f = -1; c = getchar();}
    while (48 <= c && c <= 57) v = (v << 3) + v + v + c - 48, c = getchar();
    return v * f;
}

// 饼/条/万/风/箭
char chart1[] = {'B', 'T', 'W', 'F', 'J'};
string chart2[] = {"PENG", "GANG", "CHI"};
int len[] = {9, 9, 9, 4, 3};

// 转为字符串表示
// u.fi in [0, 4]，表示哪种牌
// u.se in [1, len[u.fi]]，表示数字
string trans_str(PII u)
{
    string re(2, chart1[u.fi]);
    re[1] = u.se + 48;
    return re;
}

PII trans_PII(string u)
{
    PII re;
    if (u[0] == 'B')
        re.fi = 0;
    if (u[0] == 'T')
        re.fi = 1;
    if (u[0] == 'W')
        re.fi = 2;
    if (u[0] == 'F')
        re.fi = 3;
    if (u[0] == 'J')
        re.fi = 4;
    re.se = u[1] - 48;
    return re;
}

struct Fulu
{
    // a表示副露类型
    // b表示牌
    // c表示0-3，意义同算番库
    int a;
    PII b;
    int c;

    Fulu(){}
    Fulu(int _a, PII _b, int _c): a(_a), b(_b), c(_c) {}
};

// 一个Pai代表饼/条/万/风/箭之一
struct Pai
{
    // a的二进制每三位存一个数
    int a;

    Pai(int _a = 0): a(_a) {}

    int operator [] (int i)
    {
        return a >> ((i - 1) * 3) & 7;
    }

    void inc(int i, int v = 1)
    {
        a += v << ((i - 1) * 3);
    }

    void dec(int i, int v = 1)
    {
        a -= v << ((i - 1) * 3);
    }

    void assign(int i, int v)
    {
        for (int j = (i - 1) * 3; j < i * 3; j++)
            a &= (~(1 << j));
        a += v << ((i - 1) * 3);
    }
};

// 场风
int quan;

// 牌山残余枚数
int tot;
// 饼/条/万/风/箭的残余枚数
Pai R[5];
// 牌山中花的残余枚数
int hua_r;

// 残余枚数减少
void dec_tot(int v = 1)
{
    tot -= v;
}
void dec_R(PII u, int v = 1)
{
    R[u.fi].dec(u.se, v);
}
void dec_hua_r(int v = 1)
{
    hua_r -= v;
}

// 记录一个玩家的牌
struct Player
{
    // 手牌（不计副露）
    Pai S[5];

    // 副露
    vector<Fulu> F;

    // 花牌数量
    int hua;

    // 位置
    // 0号玩家为庄家（东），1号、2号、3号玩家为闲家（南、西、北）
    int pos;

    Player()
    {
        hua = pos = 0;
    }

    // 算番
    // u表示和了哪张，且u在手牌即S[5]中
    int cal_fan(PII u, bool isZIMO, bool isGANG)
    {
        vector<pair<string, pair<string, int> > > pack;
        for (auto i : F)
            pack.push_back(MP(chart2[i.a], MP(trans_str(i.b), i.c)));
        vector<string> hand;
        rep(i, 0, 4)
            rep(j, 1, len[i])
            {
                int flag = ((i == u.fi) && (j == u.se));
                rep(k, 1, S[i][j] - flag)
                    hand.push_back(trans_str(MP(i, j)));
            }
        auto t = MahjongFanCalculator(pack, hand, trans_str(u), hua, isZIMO, (R[u.fi][u.se] == 0), isGANG, (tot == 0), pos, quan);
        int re = 0;
        for (auto i : t)
            re += i.fi;
        return re;
    }

    // 摸牌
    void mo(PII u)
    {
        S[u.fi].inc(u.se);
    }

    // 打牌
    void da(PII u)
    {
        S[u.fi].dec(u.se);
    }

    // 暗杠
    void an_gang(PII u)
    {
        S[u.fi].dec(u.se, 4);
        F.push_back(Fulu(1, u, 0));
    }

    // 补杠
    void bu_gang(PII u)
    {
        S[u.fi].dec(u.se, 1);
        for (auto &i : F)
            if (i.a == 0 && i.b == u)
            {
                i.a = 1;
                break;
            }
    }

    // 明杠
    void ming_gang(PII u, int v)
    {
        S[u.fi].dec(u.se, 3);
        F.push_back(Fulu(1, u, v));
    }

    // 碰
    void peng(PII u, int v)
    {
        S[u.fi].dec(u.se, 2);
        F.push_back(Fulu(0, u, v));
    }

    // 吃
    void chi(PII u, int v)
    {
        for (int i = 1; i <= 3; i++)
            if (i != v)
                S[u.fi].dec(u.se + i - 2);
        F.push_back(Fulu(2, u, v));
    }
};

vector<string> request, response;

int main()
{
    MahjongInit();

    tot = 144;
    rep(i, 0, 4)
        rep(j, 1, len[i])
            R[i].assign(j, 4);
    hua_r = 8;

    string s1, s2, s3;
    int t1, t2;
    PII last;
    int last_pos;
    int turnID;

#if SIMPLEIO
    cin >> turnID;
    turnID--;
    getline(cin, s1);
    for(int i = 0; i < turnID; i++) {
        getline(cin, s1);
        request.push_back(s1);
        getline(cin, s1);
        response.push_back(s1);
    }
    getline(cin, stmp);
    request.push_back(stmp);
#else
    Json::Value inputJSON;
    cin >> inputJSON;
    turnID = inputJSON["responses"].size();
    for(int i = 0; i < turnID; i++) {
        request.push_back(inputJSON["requests"][i].asString());
        response.push_back(inputJSON["responses"][i].asString());
    }
    request.push_back(inputJSON["requests"][turnID].asString());
#endif

    if(turnID < 2)
    {
        response.push_back("PASS");
    } else
    {
        // 强制自己的位置为0
        Player a[4];
        a[0].pos = 0;
        a[1].pos = 1;
        a[2].pos = 2;
        a[3].pos = 3;
        istringstream sin;
        ostringstream sout;
        int mypos;

        sin.str(request[0]);
        sin >> t1 >> mypos >> quan;
        quan = (quan - mypos + 4) % 4;

        sin.clear();
        sin.str(request[1]);
        sin >> t1;
        for(int j = 0; j < 4; j++)
        {
            sin >> t1;
            int i = (j - mypos + 4) % 4;
            a[i].hua += t1;
            dec_tot(t1);
            dec_hua_r(t1);
        }
        for(int j = 0; j < 13; j++)
        {
            sin >> s1;
            PII u1 = trans_PII(s1);
            a[0].mo(u1);
            dec_tot();
            dec_R(u1);
        }

        for(int i = 2; i < turnID; i++)
        {
            sin.clear();
            sin.str(request[i]);
            sin >> t1;

            if (t1 == 2)
            {
                sin >> s1;
                PII u1 = trans_PII(s1);
                a[0].mo(u1);
                dec_tot();
                dec_R(u1);
                sin.clear();
                sin.str(response[i]);
                sin >> s1 >> s2;
                if (s1 == "PLAY")
                    a[0].da(trans_PII(s2));
                if (s1 == "GANG")
                    a[0].an_gang(trans_PII(s2));
                if (s1 == "BUGANG")
                    a[0].bu_gang(trans_PII(s2));
                last = MP(-1, -1);
            }

            if (t1 == 3)
            {
                sin >> t2 >> s1;
                t2 = (t2 - mypos + 4) % 4;
                if (s1 == "BUHUA")
                {
                    a[t2].hua++;
                    dec_tot();
                    dec_hua_r();
                }
                if (s1 == "DRAW")
                {
                    dec_tot();
                    last = MP(-1, -1);
                }
                if (s1 == "PLAY")
                {
                    sin >> s2;
                    PII u1 = trans_PII(s2);
                    if (t2 != 0)
                        dec_R(u1);
                    last = u1;
                    last_pos = t2;
                }
                if (s1 == "PENG")
                {
                    sin >> s2;
                    PII u1 = trans_PII(s2);
                    a[t2].peng(last, (t2 - last_pos + 4) % 4);
                    if (t2 != 0)
                    {
                        dec_R(last, 2);
                        dec_R(u1);
                    }
                    last = u1;
                    last_pos = t2;
                }
                if (s1 == "CHI")
                {
                    sin >> s2 >> s3;
                    PII u1 = trans_PII(s2), u2 = trans_PII(s3);
                    a[t2].chi(u1, last.se - u1.se + 2);
                    if (t2 != 0)
                    {
                        for (int k = u1.se - 1; k <= u1.se + 1; k++)
                            if (k != last.se)
                                dec_R(MP(u1.fi, k));
                        dec_R(u2);
                    }
                    last = u2;
                    last_pos = t2;
                }
                if (s1 == "GANG")
                {
                    if (t2 == 0)
                    {
                        if (last != MP(-1, -1))
                            a[0].ming_gang(last, (t2 - last_pos + 4) % 4);
                    } else
                    {
                        if (last != MP(-1, -1))
                        {
                            a[t2].ming_gang(last, (t2 - last_pos + 4) % 4);
                            dec_R(last, 3);
                        } else
                        {
                            a[t2].an_gang(last);
                        }
                    }
                }
                if (s1 == "BUGANG")
                {
                    sin >> s2;
                    PII u1 = trans_PII(s2);
                    if (t2 != 0)
                    {
                        a[t2].bu_gang(u1);
                        dec_R(u1);
                    }
                }
            }
        }

        // 决策部分
        sin.clear();
        sin.str(request[turnID]);
        sin >> t1;
        if (t1 == 2)
        {
            bool flag = 0;
            rep(i, 0, 4)
            {
                rep(j, 1, len[i])
                    if (a[0].S[i][j])
                    {
                        sout << "PLAY " << trans_str(MP(i, j));
                        flag = 1;
                        break;
                    }
                if (flag)
                    break;
            }
        }
        if (t1 == 3)
        {
            sin >> t2 >> s1;
            if (s1 == "BUHUA")
                sout << "PASS";
            if (s1 == "DRAW")
                sout << "PASS";
            if (s1 == "PLAY")
            {
                sout << "PASS";
            }
            if (s1 == "PENG")
            {
                sout << "PASS";
            }
            if (s1 == "CHI")
            {
                sout << "PASS";
            }
            if (s1 == "GANG")
            {
                sout << "PASS";
            }
            if (s1 == "BUGANG")
            {
                sout << "PASS";
            }
        }
        response.push_back(sout.str());
    }
#if SIMPLEIO
    cout << response[turnID] << endl;
#else
    Json::Value outputJSON;
    outputJSON["response"] = response[turnID];
    outputJSON["verdict"] = string("OK");
    cout << outputJSON << endl;
#endif
    return 0;
}
