#include "MahjongGB/MahjongGB.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <iostream>
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

// 一个Pai代表饼/条/万/风牌/箭牌之一
struct Pai
{
    // a的二进制每三位表示0-4或1-3
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

    void set(int i, int v)
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
// 牌山中饼/条/万/风/箭的残余枚数
Pai R[5];
// 牌山中花的残余枚数
int hua_r;

// 残余枚数减少
void dec_R(PII u, int v = 1)
{
    tot -= v;
    R[u.fi].dec(u.se, v);
}
void dec_hua_r(int v = 1)
{
    tot -= v;
    hua_r -= v;
}

// 记录自己的牌
struct Player
{
    // 手牌（不计副露）
    Pai S[5];

    // 副露
    // 每位1-3的含义同算番库
    Pai P[5], G[5], C[5];

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
        rep(i, 0, 4)
            rep(j, 1, len[i])
                if (P[i][j])
                    pack.push_back(MP(chart2[0], MP(trans_str(i, j), P[i][j])));
        rep(i, 0, 4)
            rep(j, 1, len[i])
                if (G[i][j])
                    pack.push_back(MP(chart2[1], MP(trans_str(i, j), G[i][j])));
        rep(i, 0, 4)
            rep(j, 1, len[i])
                if (C[i][j])
                    pack.push_back(MP(chart2[2], MP(trans_str(i, j), C[i][j])));
        vector<string> hand;
        rep(i, 0, 4)
            rep(j, 1, len[i])
            {
                int flag = ((i == u.fi) && (j == u.se));
                rep(k, 1, S[i][j] - flag)
                    hand.push_back(trans_str(i, j));
            }
        MahjongFanCalculator(pack, hand, trans_str(u.fi, u.se), hua, isZIMO, (R[u.fi][u.se] == 0), isGANG, (tot == 0), pos, quan);
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
};

vector<string> request, response;

int main()
{
    MahjongInit();

    tot = 144;
    rep(i, 0, 4)
        rep(j, 1, len[i])
            R[i].set(j, 4);
    hua_r = 8;

    int turnID;
    string stmp;
    cin >> turnID;
    turnID--;
    getline(cin, stmp);
    for(int i = 0; i < turnID; i++) {
        getline(cin, stmp);
        request.push_back(stmp);
        getline(cin, stmp);
        response.push_back(stmp);
    }
    getline(cin, stmp);
    request.push_back(stmp);

    if(turnID < 2) {
        response.push_back("PASS");
    } else {
        Player a;
        int itmp;
        ostringstream sout;
        istringstream sin;
        sin.str(request[0]);
        sin >> itmp >> a.pos >> quan;
        sin.clear();
        sin.str(request[1]);
        sin >> itmp;
        for(int j = 0; j < 4; j++)
        {
            sin >> itmp;
            if (j == a.pos)
                a.hua += itmp;
            dec_hua_r(itmp);
        }
        for(int j = 0; j < 13; j++) {
            sin >> stmp;
            PII u = trans_PII(stmp);
            a.mo(u);
            dec_R(u);
        }
        for(int i = 2; i < turnID; i++) {
            sin.clear();
            sin.str(request[i]);
            sin >> itmp;
            if (itmp == 2)
            {
                sin >> stmp;
                hand.push_back(stmp);
                sin.clear();
                sin.str(response[i]);
                sin >> stmp >> stmp;
                hand.erase(find(hand.begin(), hand.end(), stmp));
                a.mo();
            }
        }
        sin.clear();
        sin.str(request[turnID]);
        sin >> itmp;
        if(itmp == 2) {
            random_shuffle(hand.begin(), hand.end());
            sout << "PLAY " << *hand.rbegin();
            hand.pop_back();
        } else {
            sout << "PASS";
        }
        response.push_back(sout.str());
    }

    cout << response[turnID] << endl;
}
