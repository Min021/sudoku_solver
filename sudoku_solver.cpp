#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

const unsigned int WHITE = 7;
const unsigned int GREY = 8;
const unsigned int BRIGHT_RED = 12;
const unsigned int BRIGHT_YELLOW = 14;
const unsigned int BRIGHT_WHITE = 15;

void change_color(const unsigned int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

unsigned int mod_div(const unsigned int dividend, const unsigned int divisor, const bool flag_mod)
{
    return flag_mod ? dividend % divisor : dividend / divisor;
}

unsigned int row_col(const unsigned int I_1, const unsigned int I_2, const unsigned int S_out)
{
    return mod_div(I_1, 3, S_out) * 3 + mod_div(I_2, 3, S_out);
}

unsigned int box(const unsigned int I_1, const unsigned int I_2, const unsigned int S_in)
{
    return S_in ? I_2 / 3 * 3 + I_1 / 3 : I_1 / 3 * 3 + I_2 / 3;
}

unsigned int get_i_2(const unsigned int I_1, const unsigned int I_2, const unsigned int S_in, const unsigned int S_out)
{
    if(S_out == S_in)
    {
        return I_2;
    }
    if(S_out == 2)
    {
        return S_in ? I_2 % 3 * 3 + I_1 % 3 : I_1 % 3 * 3 + I_2 % 3;
    }
    if(S_in == 2)
    {
        return mod_div(I_1, 3, !S_out) * 3 + mod_div(I_2, 3, !S_out);
    }
    return I_1;
}

void convert_i_2(vector<unsigned int> &v_i_2, const unsigned int I_1, const unsigned int S_in, const unsigned int S_out)
{
    for(unsigned int k = 0; k < v_i_2.size(); v_i_2[k] = get_i_2(I_1, v_i_2[k], S_in, S_out), ++k);
}

unsigned int get_i_1(const unsigned int I_1, const unsigned int I_2, const unsigned int S_in, const unsigned int S_out)
{
    if(S_out == S_in)
    {
        return I_1;
    }
    if(S_out == 2)
    {
        return box(I_1, I_2, S_in);
    }
    if(S_in == 2)
    {
        return row_col(I_1, I_2, S_out);
    }
    return I_2;
}

unsigned int get_i_1(const unsigned int I_c, const unsigned int S_out)
{
    return S_out == 2 ? box(I_c / 9, I_c % 9, 0) : mod_div(I_c, 9, S_out);
}

unsigned int get_s_out(const unsigned int S_in, const vector<unsigned int> &v_i_2)
{
    for(unsigned int k, s_out = 0; s_out < 1 + (S_in == 2); ++s_out)
    {
        for(k = 1; k < v_i_2.size() && mod_div(v_i_2[k], 3, s_out) == mod_div(v_i_2[k - 1], 3, s_out); ++k);
        if(k == v_i_2.size())
        {
            return S_in == 2 ? s_out : 2;
        }
    }
    return S_in;
}

template <typename T>
T const* find_item(const vector<T> &v, const T &a)
{
    unsigned int k;
    for(k = 0; k < v.size() && v[k] != a; ++k);
    return(k == v.size() ? nullptr : &v[k]);
}

template <typename T>
T const* find_item(const vector<T> &v_1, const vector<T> &v_2)
{
    T const*t;
    for(unsigned int k = 0; k < v_2.size() && !(t = find_item(v_1, v_2[k])); ++k);
    return t;
}

template <typename T>
void update_vector(vector<T> &v, const T &a)
{
    if(!find_item(v, a))
    {
        v.push_back(a);
    }
}

void update_vector(vector<unsigned int> &v, const vector<unsigned int> &u)
{
    for(unsigned int k = 0; k < u.size(); update_vector(v, u[k++]));
}

class cell
{
    friend class sudoku;

private:
    unsigned int value;
    vector<unsigned int> candidates;
    bool mark_1;
    bool mark_2;

    static bool flag_change;

    void remove_candidate_(const unsigned int K);
    void remove_candidate(const unsigned int N);
    void remove_candidate(const vector<unsigned int> &v);
};
bool cell::flag_change;

struct node_a
{
    cell *c;
    unsigned int n;
    bool flag; //TRUE = ON, FALSE = OFF
};

struct node
{
    vector<cell*> v_c;
    cell *c_intersection;
    bool s_gn;
    vector<struct link> v_l[3][2];
};

struct link
{
    node *nd;
    unsigned int s;
    unsigned int i_1;
    bool flag; //TRUE = STRONG, FALSE = WEAK
};

struct cycle_data
{
    bool weak_1; //AT LEAST ONE WEAK RING IN THE CYCLE
    bool weak_2; //TWO CONSECUTIVE WEAK RINGS IN THE CYCLE
    bool strong_even; //EVEN SEQUENCE OF CONSECUTIVE STRONG RINGS IN THE CYCLE
    unsigned int n_strong_0; //NUMBER OF CONSECUTIVE STRONG RINGS OF THE FIRST SEQUENCE
    unsigned int n_strong; //NUMBER OF CONSECUTIVE STRONG RINGS OF THE CURRENT SEQUENCE
};

class sudoku
{
private:
    cell grid[9][9];

    static vector<unsigned int> table[3][9][10];
    static vector<vector<unsigned int>> v_v_1;
    static vector<vector<unsigned int>> v_v_2;
    static vector<unsigned int> v_1;
    static vector<unsigned int> v_2;
    static vector<cell*> v_cell;
    static vector<node_a> chain_a;
    static vector<node> v_nd;
    static vector<node*> table_nd[2][3][9];
    static vector<link*> chain;

    sudoku(){};
    cell& locate_cell(const unsigned int I_1, const unsigned int I_2 ,const unsigned int S);
    bool same_group(const cell *C_1, const cell *C_2);
    bool same_group(const vector<cell*> &v_c_1, const vector<cell*> &v_c_2);
    void print_sudoku(const unsigned int I, const unsigned int J);
    void update_candidates(const unsigned int I_1, const unsigned int I_2, const unsigned int S);
    void update_candidates(const cell *C);
    bool check_sudoku_c();
    bool check_sudoku_n();
    void compile_table(const unsigned int I_1, const unsigned int S);
    void compile_table(const unsigned int S);
    void compile_table();
    void find_intersection(const unsigned int M, const unsigned int J_1, const unsigned int S, const unsigned int case_, const bool flag_recursion = false);
    void find_empty_cell(unsigned int &I, unsigned int &J);
    void select_cells_mark_1(const unsigned int I_1, const vector<unsigned int> &v_i_2, const unsigned int S);
    void select_group(const cell *C, const bool flag_mark_1 = true);
    void deselect(const bool flag_mark_1 = true);
    void groups_intersection(const cell *C_1, const cell *C_2);
    bool not_node_a(const node_a &X);
    void remove_strong_duplicate(const unsigned int N);
    void remove_strong_duplicate();
    void loop_rule_1(const unsigned int N);
    void loop_rule_2(const unsigned int N);
    void loop_rule_3(const unsigned int N);
    void empty_table_nd();
    void add_nodes_table_nd(const unsigned int N);
    void add_links_table_nd(const unsigned int N);
    void compile_table_nd(const unsigned int N);
    bool not_node(const node *nd, const node *start);
    void remove_candidate(const vector<cell*> &v_c, const unsigned int N);
    bool build_grouped_x_cycles(const node *start, const unsigned int N, const cycle_data &D);
    bool rule_1();
    bool rule_2();
    bool rule_3();
    bool rule_4();
    bool rule_5();
    bool rule_6();
    void build_3D_medusa(const node_a &x);
    bool build_xy_chains(const cell *start, cell *current, const unsigned int N, const unsigned int solution);

    void print_grouped_x_cycles(const node *start, const cycle_data &D, const unsigned int N);//TEST

public:
    sudoku(int);
    sudoku(const string &str);
    void print_sudoku();
    bool find_duplicate_clue();
    void find_candidates();
    bool check_sudoku();
    bool check_win();
    bool naked_1();
    bool naked_2_3_4();
    bool hidden_1();
    bool hidden_2_3_4();
    bool intersection_removal();
    bool basic_fish_2_3_4();
    bool grouped_x_cycles();
    bool _3D_medusa();
    bool xy_chains();
    void brute_force(unsigned int &n_solutions);
};
vector<unsigned int> sudoku::table[3][9][10];
vector<vector<unsigned int>> sudoku::v_v_1;
vector<vector<unsigned int>> sudoku::v_v_2;
vector<unsigned int> sudoku::v_1;
vector<unsigned int> sudoku::v_2;
vector<cell*> sudoku::v_cell;
vector<node_a> sudoku::chain_a;
vector<node> sudoku::v_nd(9 * (9 + 6));
vector<node*> sudoku::table_nd[2][3][9];
vector<link*> sudoku::chain;

void cell::remove_candidate_(const unsigned int K)
{
    candidates.erase(candidates.begin() + K);
    flag_change = true;
}

void cell::remove_candidate(const unsigned int N)
{
    for(unsigned int k = 0; k < candidates.size(); ++k)
    {
        if(candidates[k] == N)
        {
            candidates.erase(candidates.begin() + k);
            flag_change = true;
            return;
        }
    }
}

void cell::remove_candidate(const vector<unsigned int> &v)
{
    for(unsigned int k = 0; k < v.size(); remove_candidate(v[k++]));
}

bool sudoku::same_group(const cell *C_1, const cell *C_2)
{
    unsigned int s;
    for(s = 0; s < 3 && get_i_1(C_1 - *grid, s) != get_i_1(C_2 - *grid, s); ++s);
    return(s != 3);
}

bool sudoku::same_group(const vector<cell*> &v_c_1, const vector<cell*> &v_c_2)
{
    for(unsigned int k_1 = 0; k_1 < v_c_1.size(); ++k_1)
    {
        for(unsigned int k_2 = 0; k_2 < v_c_2.size(); ++k_2)
        {
            if(same_group(v_c_1[k_1], v_c_2[k_2]))
            {
                return true;
            }
        }
    }
    return false;
}

sudoku::sudoku(const string &str)
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            grid[i][j].value = str[9 * i + j] - '0';
        }
    }
}

sudoku::sudoku(int)
{
    string str;
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            cout << "ENTER A DIGIT IN THE HIGHLIGHTED POSITION\n";
            cout << "FOR EMPTY CELLS USE '0' OR PRESS 'ENTER' TO SKIP TO THE NEXT CELL\n\n";
            print_sudoku(i, j);
            while(true)
            {
                cout << "--> ";
                getline(cin, str);
                if(!str.size())
                {
                    grid[i][j].value = 0;
                    break;
                }
                else if(str.size() == 1 && str[0] >= '0' && str[0] <= '9')
                {
                    grid[i][j].value = str[0] - '0';
                    break;
                }
            }
            system("CLS");
        }
    }
}

void sudoku::print_sudoku(const unsigned int I, const unsigned int J)
{
    bool flag = false;
    change_color(BRIGHT_WHITE);
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            if(i == I && j == J)
            {
                flag = true;
                change_color(BRIGHT_YELLOW);
                cout << '?';
                change_color(GREY);
            }
            else
            {
                cout << (flag ? '?' : char(grid[i][j].value + '0'));
            }
            cout << (j == 2 || j == 5 ? "  " : " ");
        }
        cout << (i == 2 || i == 5 ? "\n\n" : "\n");
    }
    change_color(WHITE);
    cout << "\n";
}

void sudoku::print_sudoku()
{
    cout << "\n";
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            grid[i][j].candidates.size() ? change_color(BRIGHT_RED) : change_color(BRIGHT_WHITE);
            cout << grid[i][j].value;
            cout << (j == 2 || j == 5 ? "  " : " ");
        }
        cout << (i == 2 || i == 5 ? "\n\n" : "\n");
    }
    change_color(WHITE);
}

cell& sudoku::locate_cell(const unsigned int I_1, const unsigned int I_2 ,const unsigned int S)
{
    return(!S ? grid[I_1][I_2] : (S == 1 ? grid[I_2][I_1] : grid[row_col(I_1, I_2, 0)][row_col(I_1, I_2, 1)]));
}

bool sudoku::find_duplicate_clue()
{
    for(unsigned int s = 0; s < 3; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            unsigned int v[10] = {};
            for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
            {
                unsigned int n = locate_cell(i_1, i_2, s).value;
                if(n && ++v[n] == 2)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void sudoku::find_candidates()
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            if(!grid[i][j].value)
            {
                unsigned int v[10] = {};
                unsigned int v_i_1[3] = {i, j, box(i, j, 0)};
                for(unsigned int s = 0; s < 3; ++s)
                {
                    for(unsigned int i_2 = 0; i_2 < 9; ++v[locate_cell(v_i_1[s], i_2++, s).value]);
                }
                for(unsigned int n = 1; n < 10; ++n)
                {
                    if(!v[n])
                    {
                        grid[i][j].candidates.push_back(n);
                    }
                }
            }
        }
    }
}

void sudoku::update_candidates(const unsigned int I_1, const unsigned int I_2, const unsigned int S)
{
    unsigned int n = locate_cell(I_1, I_2, S).value;
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(I_1, I_2, S, s);
        for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
        {
            cell &c = locate_cell(i_1, i_2, s);
            if(!c.value)
            {
                c.remove_candidate(n);
            }
        }
    }
}

void sudoku::update_candidates(const cell *C)
{
    unsigned int n = C->value;
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(C - *grid, s);
        for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
        {
            cell &c = locate_cell(i_1, i_2, s);
            if(!c.value)
            {
                c.remove_candidate(n);
            }
        }
    }
}

bool sudoku::check_sudoku()
{
    return !(check_sudoku_c() || check_sudoku_n());
}

bool sudoku::check_sudoku_c()
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            if(!grid[i][j].value && !grid[i][j].candidates.size())
            {
                return true;
            }
        }
    }
    return false;
}

bool sudoku::check_sudoku_n()
{
    for(unsigned int s = 0; s < 3; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            unsigned int v[10] = {};
            for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
            {
                cell &c = locate_cell(i_1, i_2, s);
                for(unsigned int k = 0; k < c.candidates.size(); ++v[c.candidates[k++]]);
                ++v[c.value];
            }
            for(unsigned int n = 1; n < 10; ++n)
            {
                if(!v[n])
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool sudoku::check_win()
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            if(!grid[i][j].value)
            {
                return false;
            }
        }
    }
    return true;
}

void sudoku::find_empty_cell(unsigned int &I, unsigned int &J)
{
    for(I = 0; I < 9; ++I)
    {
        for(J = 0; J < 9; ++J)
        {
            if(!grid[I][J].value)
            {
                return;
            }
        }
    }
}

void sudoku::compile_table(const unsigned int I_1, const unsigned int S)
{
    for(unsigned int n = 1; n < 10; table[S][I_1][n++].resize(0));
    for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
    {
        cell &c = locate_cell(I_1, i_2, S);
        if(!c.value)
        {
            for(unsigned int k = 0; k < c.candidates.size(); table[S][I_1][c.candidates[k++]].push_back(i_2));
        }
    }
}

void sudoku::compile_table(const unsigned int S)
{
    for(unsigned int i_1 = 0; i_1 < 9; compile_table(i_1++, S));
}

void sudoku::compile_table()
{
    for(unsigned int s = 0; s < 3; compile_table(s++));
}

void sudoku::select_cells_mark_1(const unsigned int I_1, const vector<unsigned int> &v_i_2, const unsigned int S)
{
    for(unsigned int k = 0; k < v_i_2.size(); locate_cell(I_1, v_i_2[k++], S).mark_1 = true);
}

void sudoku::select_group(const cell *C, const bool flag_mark_1)
{
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(C - *grid, s);
        for(unsigned int i_2 = 0; i_2 < 9; (flag_mark_1 ? locate_cell(i_1, i_2++, s).mark_1 : locate_cell(i_1, i_2++, s).mark_2) = true);
    }
}

void sudoku::deselect(const bool flag_mark_1)
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; (flag_mark_1 ? grid[i][j++].mark_1 : grid[i][j++].mark_2) = false);
    }
}

void sudoku::groups_intersection(const cell *C_1, const cell *C_2)
{
    v_cell.resize(0);
    deselect(false);
    select_group(C_1, false);
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(C_2 - *grid, s);
        for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
        {
            cell &c = locate_cell(i_1, i_2, s);
            if(c.mark_2)
            {
                update_vector(v_cell, &c);
            }
        }
    }
}

void sudoku::find_intersection(const unsigned int M, const unsigned int J_1, const unsigned int S, const unsigned int case_, const bool flag_recursion)
{
    if(!flag_recursion)
    {
        v_v_1.resize(0);
        v_v_2.resize(0);
        v_1.resize(0);
        v_2.resize(0);
    }
    vector<unsigned int> *ptr;
    unsigned int dim = v_1.size();
    for(unsigned int j_2 = (dim ? v_2.back() + 1 : case_ == 1); j_2 < 10 + (case_ == 1) - M + v_2.size(); ++j_2)
    {
        if(case_)
        {
            ptr = case_ == 1 ? &table[S][J_1][j_2] : &table[S][j_2][J_1];
            if(!ptr->size() || ptr->size() > M)
            {
                continue;
            }
        }
        else
        {
            cell &c = locate_cell(J_1, j_2, S);
            if(c.value || c.candidates.size() > M)
            {
                continue;
            }
            ptr = &c.candidates;
        }
        update_vector(v_1, *ptr);
        if(v_1.size() <= M)
        {
            v_2.push_back(j_2);
            if(v_2.size() < M)
            {
                find_intersection(M, J_1, S, case_, true);
            }
            else if(v_1.size() == M)
            {
                v_v_2.push_back(v_2);
                v_v_1.push_back(v_1);
            }
            v_2.pop_back();
        }
        v_1.resize(dim);
    }
}

bool sudoku::naked_1()
{
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            if(!grid[i][j].value && grid[i][j].candidates.size() == 1)
            {
                grid[i][j].value = grid[i][j].candidates[0];
                update_candidates(&grid[i][j]);
                return true;
            }
        }
    }
    return false;
}

bool sudoku::naked_2_3_4()
{
    cell::flag_change = false;
    for(unsigned int m = 2; m < 5; ++m)
    {
        for(unsigned int s = 0; s < 3; ++s)
        {
            for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
            {
                find_intersection(m, i_1, s, 0);
                for(unsigned int w = 0; w < v_v_2.size(); ++w)
                {
                    for(unsigned int k = 0, i_2 = 0; m - k < 9 - i_2; ++k, ++i_2)
                    {
                        if(k == m || i_2 != v_v_2[w][k])
                        {
                            cell &c = locate_cell(i_1, i_2, s);
                            if(!c.value)
                            {
                                c.remove_candidate(v_v_1[w]);
                            }
                            --k;
                        }
                    }
                    if(cell::flag_change)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::hidden_1()
{
    for(unsigned int s = 0; s < 3; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            compile_table(i_1, s);
            for(unsigned int n = 1; n < 10; ++n)
            {
                if(table[s][i_1][n].size() == 1)
                {
                    cell &c = locate_cell(i_1, table[s][i_1][n][0], s);
                    c.value = n;
                    update_candidates(&c);
                    return true;
                }
            }
        }
    }
    return false;
}

bool sudoku::hidden_2_3_4()
{
    cell::flag_change = false;
    for(unsigned int m = 2; m < 5; ++m)
    {
        for(unsigned int s = 0; s < 3; ++s)
        {
            for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
            {
                compile_table(i_1, s);
                find_intersection(m, i_1, s, 1);
                for(unsigned int w = 0; w < v_v_1.size(); ++w)
                {
                    for(unsigned int k_1 = 0; k_1 < m; ++k_1)
                    {
                        cell &c = locate_cell(i_1, v_v_1[w][k_1], s);
                        for(unsigned int k_2 = 0; k_2 < c.candidates.size(); ++k_2)
                        {
                            if(!find_item(v_v_2[w], c.candidates[k_2]))
                            {
                                c.remove_candidate_(k_2--);
                            }
                        }
                    }
                    if(cell::flag_change)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::intersection_removal()
{
    compile_table();
    for(unsigned int s_out, s_in = 0; s_in < 3; ++s_in)
    {
        for(unsigned int i_1_out, i_1_in = 0; i_1_in < 9; ++i_1_in)
        {
            for(unsigned int n = 1; n < 10; ++n)
            {
                vector<unsigned int> &v = table[s_in][i_1_in][n];
                if((v.size() == 2 || v.size() == 3) && s_in != (s_out = get_s_out(s_in, v)))
                {
                    vector<unsigned int> &u = table[s_out][i_1_out = get_i_1(i_1_in, v[0], s_in, s_out)][n];
                    if(u.size() > v.size())
                    {
                        convert_i_2(v, i_1_in, s_in, s_out);
                        for(unsigned int k_v = 0, k_u = 0; v.size() - k_v < u.size() - k_u; ++k_v, ++k_u)
                        {
                            if(k_v == v.size() || u[k_u] != v[k_v])
                            {
                                locate_cell(i_1_out, u[k_u], s_out).remove_candidate(n);
                                --k_v;
                            }
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::basic_fish_2_3_4()
{
    cell::flag_change = false;
    for(unsigned int m = 2; m < 5; ++m)
    {
        for(unsigned int s = 0; s < 2; ++s)
        {
            compile_table(s);
            for(unsigned int n = 1; n < 10; ++n)
            {
                find_intersection(m, n, s, 2);
                for(unsigned int w = 0; w < v_v_2.size(); ++w)
                {
                    for(unsigned int k_1 = 0, i_2 = 0; m - k_1 < 9 - i_2; ++k_1, ++i_2)
                    {
                        if(k_1 == m || i_2 != v_v_2[w][k_1])
                        {
                            for(unsigned int k_2 = 0; k_2 < m; ++k_2)
                            {
                                cell &c = locate_cell(v_v_1[w][k_2], i_2, !s);
                                if(!c.value)
                                {
                                    c.remove_candidate(n);
                                }
                            }
                            --k_1;
                        }
                    }
                    if(cell::flag_change)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::not_node_a(const node_a &X)
{
    unsigned int k;
    for(k = 0; k < chain_a.size() && (chain_a[k].c != X.c || chain_a[k].n != X.n); ++k);
    return k == chain_a.size();
}

void sudoku::remove_strong_duplicate(const unsigned int N)
{
    for(unsigned int s_out, i_1 = 0; i_1 < 9; ++i_1)
    {
        vector<unsigned int> &v = table[2][i_1][N];
        if(v.size() == 2 && (s_out = get_s_out(2, v)) != 2 && table[s_out][get_i_1(i_1, v[0], 2, s_out)][N].size() == 2)
        {
            v.resize(0);
        }
    }
}

void sudoku::remove_strong_duplicate()
{
    for(unsigned int n = 1; n < 10; remove_strong_duplicate(n++));
}

void sudoku::loop_rule_1(const unsigned int N)
{
    for(unsigned int k_1 = 1; k_1 < chain.size(); ++k_1)
    {
        if(!chain[k_1]->flag)
        {
            vector<node*> &r = table_nd[0][chain[k_1]->s][chain[k_1]->i_1];
            for(unsigned int k_2 = 0; k_2 < r.size(); ++k_2)
            {
                if(!find_item(chain[k_1]->nd->v_c, r[k_2]->v_c[0]) && !find_item(chain[k_1 - 1]->nd->v_c, r[k_2]->v_c[0]))
                {
                    r[k_2]->v_c[0]->remove_candidate(N);
                }
            }
            ++k_1;
        }
    }
}

void sudoku::loop_rule_2(const unsigned int N)
{
    vector<unsigned int> v;
    unsigned int a;
    unsigned int k_1 = 0;
    while(chain[++k_1]->flag);
    while(!(v.size() % 2))
    {
        for(v = {k_1, ++k_1 % (unsigned int)chain.size()}; chain[a = ++k_1 % chain.size()]->flag; v.push_back(a));
    }
    for(k_1 = 1; k_1 < v.size(); k_1 += 2)
    {
        vector<cell*> &r = chain[v[k_1]]->nd->v_c;
        if(r.size() == 1)
        {
            r[0]->value = N;
            update_candidates(r[0]);
        }
        else
        {
            remove_candidate(chain[v[k_1 - 1]]->nd->v_c, N);
            remove_candidate(chain[v[k_1 + 1]]->nd->v_c, N);
        }
    }
}

void sudoku::loop_rule_3(const unsigned int N)
{
    unsigned int k;
    for(k = 1; chain[k]->flag || chain[++k]->flag; ++k);
    chain[k - 1]->nd->v_c[0]->remove_candidate(N);
}

void sudoku::empty_table_nd()
{
    for(unsigned int g = 0; g < 2; ++g)
    {
        for(unsigned int s = 0; s < 3; ++s)
        {
            for(unsigned int i_1 = 0; i_1 < 9; table_nd[g][s][i_1++].resize(0));
        }
    }
}

void sudoku::add_nodes_table_nd(const unsigned int N)
{
    //SINGLE NODES
    for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
    {
        vector<unsigned int> &r = table[0][i_1][N];
        for(unsigned int k = 0; k < r.size(); ++k)
        {
            v_nd.push_back({{&grid[i_1][r[k]]}});
            for(unsigned int s = 0; s < 3; table_nd[0][s][s ? get_i_1(v_nd.back().v_c[0] - *grid, s) : i_1].push_back(&v_nd.back()), ++s);
        }
    }
    //GROUPED NODES
    for(unsigned int s = 0; s < 2; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            vector<unsigned int> &r_1 = table[s][i_1][N];
            if(r_1.size() > 2)
            {
                vector<node*> &r_2 = table_nd[0][s][i_1];
                vector<node*> &r_3 = table_nd[1][s][i_1];
                for(unsigned int sup, k = 0; k < r_1.size() - 1; ++k)
                {
                    if(r_1[k + 1] < (sup = (r_1[k] / 3 + 1) * 3))
                    {
                        v_nd.push_back({{r_2[k]->v_c[0], r_2[++k]->v_c[0]}, nullptr, s});
                        if(k < r_1.size() - 1 && r_1[k + 1] < sup)
                        {
                            v_nd.back().v_c.push_back(r_2[++k]->v_c[0]);
                        }
                        r_3.push_back(&v_nd.back());
                        table_nd[1][2][box(i_1, r_1[k], s)].push_back(&v_nd.back());
                    }
                }
            }
        }
    }
}

void sudoku::add_links_table_nd(const unsigned int N)
{
    //SINGLE NODE - SINGLE NODE
    for(unsigned int s = 0; s < 3; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            vector<node*> &r_1 = table_nd[0][s][i_1];
            vector<unsigned int> &r_2 = table[2][i_1][N];
            bool flag_strong = r_1.size() == 2;
            for(unsigned int k_1 = 0; k_1 < r_1.size(); ++k_1)
            {
                for(unsigned int k_2 = k_1 + 1; k_2 < r_1.size(); ++k_2)
                {
                    if(s != 2 || r_2[k_1] / 3 != r_2[k_2] / 3 && r_2[k_1] % 3 != r_2[k_2] % 3)
                    {
                        r_1[k_1]->v_l[s][flag_strong].push_back({r_1[k_2], s, i_1, flag_strong});
                        r_1[k_2]->v_l[s][flag_strong].push_back({r_1[k_1], s, i_1, flag_strong});
                    }
                }
            }
        }
    }
    //GROUPED NODE - GROUPED NODE
    for(unsigned int s = 2; s < 3; --s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            vector<node*> &r_1 = table_nd[1][s][i_1];
            vector<node*> &r_2 = table_nd[0][s][i_1];
            for(unsigned int start_size = r_1.size(), k_1 = 0; k_1 < start_size; ++k_1)
            {
                for(unsigned int k_2 = k_1 + 1; k_2 < start_size; ++k_2)
                {
                    cell *const*c_intersection = r_1[k_1]->s_gn != r_1[k_2]->s_gn || !!r_1[k_1]->c_intersection + !!r_1[k_2]->c_intersection == 1 ? find_item(r_1[k_1]->v_c, r_1[k_2]->v_c) : nullptr;
                    bool flag_strong = r_1[k_1]->v_c.size() + r_1[k_2]->v_c.size() - !!c_intersection == r_2.size();
                    if(!c_intersection || flag_strong && r_1[k_1]->v_c.size() == 3 && r_1[k_2]->v_c.size() == 3)
                    {
                        if(c_intersection)
                        {
                            vector<cell*> v_c;
                            for(unsigned int k_3 = 0; v_c.size() < 2; ++k_3)
                            {
                                if(r_1[k_2]->v_c[k_3] != *c_intersection)
                                {
                                    v_c.push_back(r_1[k_2]->v_c[k_3]);
                                }
                            }
                            v_nd.push_back({v_c, *c_intersection, r_1[k_2]->s_gn});
                            r_1.push_back(&v_nd.back());
                            table_nd[1][r_1[k_2]->s_gn][get_i_1(*c_intersection - *grid, r_1[k_2]->s_gn)].push_back(&v_nd.back());
                        }
                        r_1[k_1]->v_l[s][flag_strong].push_back({r_1[c_intersection ? r_1.size() - 1 : k_2], s, i_1, flag_strong});
                        r_1[c_intersection ? r_1.size() - 1 : k_2]->v_l[s][flag_strong].push_back({r_1[k_1], s, i_1, flag_strong});
                    }
                }
            }
        }
    }
    //GROUPED NODE - SINGLE NODE
    for(unsigned int s = 0; s < 3; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            vector<node*> &r_1 = table_nd[1][s][i_1];
            vector<node*> &r_2 = table_nd[0][s][i_1];
            for(unsigned int k_1 = 0; k_1 < r_1.size(); ++k_1)
            {
                if(!r_1[k_1]->c_intersection || s != 2)
                {
                    for(unsigned int k_2 = 0; k_2 < r_2.size(); ++k_2)
                    {
                        if(r_1[k_1]->c_intersection != r_2[k_2]->v_c[0] && !find_item(r_1[k_1]->v_c, r_2[k_2]->v_c[0]))
                        {
                            bool flag_strong = r_1[k_1]->v_c.size() + 1 == r_2.size();
                            r_1[k_1]->v_l[s][flag_strong].push_back({r_2[k_2], s, i_1, flag_strong});
                            r_2[k_2]->v_l[s][flag_strong].push_back({r_1[k_1], s, i_1, flag_strong});
                        }
                    }
                }
            }
        }
    }
}

void sudoku::compile_table_nd(const unsigned int N)
{
    v_nd.resize(0);
    empty_table_nd();
    add_nodes_table_nd(N);
    add_links_table_nd(N);
}

bool sudoku::not_node(const node *nd, const node *start)
{
    unsigned int k;
    for(k = 0; k < chain.size() && !find_item(chain[k]->nd->v_c, nd->v_c); ++k);
    return(k == chain.size() && (!find_item(start->v_c, nd->v_c) || nd == start));
}

void sudoku::remove_candidate(const vector<cell*> &v_c, const unsigned int N)
{
    for(unsigned int k = 0; k < v_c.size(); v_c[k++]->remove_candidate(N));
}

void sudoku::print_grouped_x_cycles(const node *start, const cycle_data &D, const unsigned int N)//TEST
{
    cout << "(N=" << N << "): ";
    for(unsigned int k_2 = 0; k_2 < start->v_c.size(); ++k_2)
    {
        cout << "r" << (start->v_c[k_2] - *grid) / 9 + 1 << "c" << (start->v_c[k_2] - *grid) % 9 + 1 << " ";
    }
    for(unsigned int k_1 = 0; k_1 < chain.size(); ++k_1)
    {
        cout << (chain[k_1]->flag ? " == " : " -- ");
        for(unsigned int k_2 = 0; k_2 < chain[k_1]->nd->v_c.size(); ++k_2)
        {
            cout << "r" << (chain[k_1]->nd->v_c[k_2] - *grid) / 9 + 1 << "c" << (chain[k_1]->nd->v_c[k_2] - *grid) % 9 + 1 << " ";
        }
    }
    cout << (chain.size() % 2 ? (D.weak_2 ? " (loop_rule_3)" : " (loop_rule_2)") : " (loop_rule_1)") << endl;
}

bool sudoku::build_grouped_x_cycles(const node *start, const unsigned int N, const cycle_data &D)
{
    bool only_strong = chain.back()->nd->v_c.size() != 1 && !chain.back()->flag || (!chain.back()->flag || (D.weak_1 && !(D.n_strong % 2))) && (D.weak_2 || D.strong_even);
    for(unsigned int s = 0; s < 3; ++s)
    {
        if(s != chain.back()->s)
        {
            for(unsigned int l = only_strong; l < 2; ++l)
            {
                vector<link> &r = chain.back()->nd->v_l[s][l];
                if(r.size())
                {
                    cycle_data d = l ? D : cycle_data({true, D.weak_2 || !chain.back()->flag, D.strong_even || D.weak_1 && !(D.n_strong % 2), D.n_strong_0, 0});
                    if(l)
                    {
                        D.weak_1 ? ++d.n_strong : ++d.n_strong_0;
                    }
                    for(unsigned int k = 0; k < r.size(); ++k)
                    {
                        if(not_node(r[k].nd, start) && (chain.back()->flag || l || chain.back()->nd->v_c.size() != 1 || !same_group(chain[chain.size() - 2]->nd->v_c, r[k].nd->v_c)))
                        {
                            chain.push_back(&r[k]);
                            if(chain.back()->nd == start)
                            {
                                if(!(!d.weak_1 || !((d.n_strong_0 + d.n_strong) % 2) && (d.weak_2 || d.strong_even)))
                                {
                                    print_grouped_x_cycles(start, d, N);//TEST

                                    chain.size() % 2 ? (d.weak_2 ? loop_rule_3(N) : loop_rule_2(N)) : loop_rule_1(N);
                                    return true;
                                }
                            }
                            else if(build_grouped_x_cycles(start, N, d))
                            {
                                return true;
                            }
                            chain.pop_back();
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::grouped_x_cycles()
{
    compile_table();
    for(unsigned int n = 1; n < 10; ++n)
    {
        compile_table_nd(n);
        for(unsigned int k = 0; k < v_nd.size(); ++k)
        {
            for(unsigned int s = 0; s < 3; ++s)
            {
                vector<link> &r = v_nd[k].v_l[s][1];
                if(r.size())
                {
                    chain.resize(0);
                    chain.push_back(&r[0]);
                    if(build_grouped_x_cycles(&v_nd[k], n, {false, false, false, 1, 0}))
                    {
                        return true;
                    }
                    r[0].nd->v_l[s][1].pop_back();
                    r.pop_back();
                }
            }
        }
    }
    return false;
}

bool sudoku::rule_1()
{
    for(unsigned int k_1 = 0; k_1 < chain_a.size() - 1; ++k_1)
    {
        if(!chain_a[k_1].c->mark_1)
        {
            for(unsigned int k_2 = k_1 + 1; k_2 < chain_a.size(); ++k_2)
            {
                if(chain_a[k_2].c == chain_a[k_1].c && chain_a[k_2].flag == chain_a[k_1].flag)
                {
                    for(unsigned int k_3 = 0; k_3 < chain_a.size(); ++k_3)
                    {
                        if(chain_a[k_3].flag != chain_a[k_1].flag)
                        {
                            chain_a[k_3].c->value = chain_a[k_3].n;
                            update_candidates(chain_a[k_3].c);
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool sudoku::rule_2()
{
    for(unsigned int k_1 = 0; k_1 < chain_a.size() - 1; ++k_1)
    {
        for(unsigned int k_2 = k_1 + 1; k_2 < chain_a.size(); ++k_2)
        {
            if(chain_a[k_2].n == chain_a[k_1].n && chain_a[k_2].flag == chain_a[k_1].flag && same_group(chain_a[k_2].c, chain_a[k_1].c))
            {
                for(unsigned int k_3 = 0; k_3 < chain_a.size(); ++k_3)
                {
                    if(chain_a[k_3].flag != chain_a[k_1].flag)
                    {
                        chain_a[k_3].c->value = chain_a[k_3].n;
                        update_candidates(chain_a[k_3].c);
                    }
                }
                return true;
            }
        }
    }
    return false;
}

bool sudoku::rule_3()
{
    cell::flag_change = false;
    for(unsigned int k_1 = 0; k_1 < chain_a.size() - 1; ++k_1)
    {
        if(!chain_a[k_1].c->mark_1)
        {
            for(unsigned int k_2 = k_1 + 1; k_2 < chain_a.size(); ++k_2)
            {
                if(chain_a[k_2].c == chain_a[k_1].c && chain_a[k_2].flag != chain_a[k_1].flag)
                {
                    for(unsigned int k_3 = 0; k_3 < chain_a[k_1].c->candidates.size(); ++k_3)
                    {
                        if(chain_a[k_1].c->candidates[k_3] != chain_a[k_1].n && chain_a[k_1].c->candidates[k_3] != chain_a[k_2].n)
                        {
                            chain_a[k_1].c->candidates.erase(chain_a[k_1].c->candidates.begin() + k_3);
                        }
                    }
                }
            }
        }
    }
    return cell::flag_change;
}

bool sudoku::rule_4()
{
    cell::flag_change = false;
    for(unsigned int k_1 = 0; k_1 < chain_a.size() - 1; ++k_1)
    {
        for(unsigned int k_2 = k_1 + 1; k_2 < chain_a.size(); ++k_2)
        {
            if(chain_a[k_2].n == chain_a[k_1].n && chain_a[k_2].flag != chain_a[k_1].flag)
            {
                groups_intersection(chain_a[k_2].c, chain_a[k_1].c);
                for(unsigned int k_3 = 0; k_3 < v_cell.size(); ++k_3)
                {
                    if(!v_cell[k_3]->value && v_cell[k_3] != chain_a[k_1].c && v_cell[k_3] != chain_a[k_2].c)
                    {
                        v_cell[k_3]->remove_candidate(chain_a[k_1].n);
                    }
                }
            }
        }
    }
    return cell::flag_change;
}

bool sudoku::rule_5()
{
    cell::flag_change = false;
    for(unsigned int k_1 = 0; k_1 < chain_a.size(); ++k_1)
    {
        if(!chain_a[k_1].c->mark_1)
        {
            for(unsigned int k_2 = 0; k_2 < chain_a[k_1].c->candidates.size(); ++k_2)
            {
                if(chain_a[k_1].c->candidates[k_2] != chain_a[k_1].n)
                {
                    for(unsigned int k_3 = 0; k_3 < chain_a.size(); ++k_3)
                    {
                        if(chain_a[k_3].n == chain_a[k_1].c->candidates[k_2] && chain_a[k_3].flag != chain_a[k_1].flag && same_group(chain_a[k_1].c, chain_a[k_3].c))
                        {
                            chain_a[k_1].c->remove_candidate_(k_2--);
                            break;
                        }
                    }
                }
            }
        }
    }
    return cell::flag_change;
}

bool sudoku::rule_6()
{
    for(unsigned int flag = 0; flag < 2; ++flag)
    {
        sudoku t = *this;
        for(unsigned int k = 0; k < chain_a.size(); ++k)
        {
            if(chain_a[k].flag == flag)
            {
                cell *c = *t.grid + (chain_a[k].c - *grid);
                c->value = chain_a[k].n;
                t.update_candidates(c);
            }
        }
        if(!t.check_sudoku())
        {
            for(unsigned int k = 0; k < chain_a.size(); ++k)
            {
                if(chain_a[k].flag != flag)
                {
                    chain_a[k].c->value = chain_a[k].n;
                    update_candidates(chain_a[k].c);
                }
            }
            return true;
        }
    }
    return false;
}

void sudoku::build_3D_medusa(const node_a &X)
{
    if(X.c->candidates.size() == 2 && !X.c->mark_1)
    {
        X.c->mark_1 = true;
        chain_a.push_back({X.c, X.c->candidates[X.c->candidates[0] == X.n], !X.flag});
        build_3D_medusa(chain_a.back());
    }
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(X.c - *grid, s);
        vector<unsigned int> &v = table[s][i_1][X.n];
        if(v.size() == 2)
        {
            cell &c = locate_cell(i_1, v[&locate_cell(i_1, v[0], s) == X.c], s);
            v.resize(0);
            node_a y = {&c, X.n, !X.flag};
            if(not_node_a(y))
            {
                chain_a.push_back(y);
                build_3D_medusa(y);
            }
        }
    }
}

bool sudoku::_3D_medusa()
{
    deselect();
    compile_table();
    remove_strong_duplicate();
    for(unsigned int s = 0; s < 2; ++s)
    {
        for(unsigned int i_1 = 0; i_1 < 9; ++i_1)
        {
            for(unsigned int n = 1; n < 10; ++n)
            {
                vector<unsigned int> &v = table[s][i_1][n];
                if(v.size() == 2)
                {
                    cell &c = locate_cell(i_1, v[0], s);
                    node_a x = {&c, n, true};
                    chain_a.resize(0);
                    chain_a.push_back(x);
                    build_3D_medusa(x);
                    if(rule_1() || rule_2() || rule_3() || rule_4() || rule_5() || rule_6())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool sudoku::build_xy_chains(const cell *start, cell *current, const unsigned int N, const unsigned int current_solution)
{
    current->mark_1 = true;
    for(unsigned int s = 0; s < 3; ++s)
    {
        unsigned int i_1 = get_i_1(current - *grid, s);
        for(unsigned int i_2 = 0; i_2 < 9; ++i_2)
        {
            cell &c = locate_cell(i_1, i_2, s);
            if(!c.mark_1 && !c.value && c.candidates.size() == 2 && find_item(c.candidates, current_solution))
            {
                if(start != current && find_item(c.candidates, N) && current_solution != N)
                {
                    groups_intersection(start, &c);
                    for(unsigned int k = 0; k < v_cell.size(); ++k)
                    {
                        if(!v_cell[k]->value && v_cell[k] != start && v_cell[k] != &c)
                        {
                            v_cell[k]->remove_candidate(N);
                        }
                    }
                }
                if(cell::flag_change || build_xy_chains(start, &c, N, c.candidates[c.candidates[0] == current_solution]))
                {
                    return !(current->mark_1 = false);
                }
            }
        }
    }
    return current->mark_1 = false;
}

bool sudoku::xy_chains()
{
    deselect();
    cell::flag_change = false;
    for(unsigned int i = 0; i < 9; ++i)
    {
        for(unsigned int j = 0; j < 9; ++j)
        {
            cell &c = grid[i][j];
            if(!c.value && c.candidates.size() == 2 && (build_xy_chains(&c, &c, c.candidates[0], c.candidates[1]) || build_xy_chains(&c, &c, c.candidates[1], c.candidates[0])))
            {
                return true;
            }
        }
    }
    return false;
}

void sudoku::brute_force(unsigned int &n_solutions)
{
    bool flag;
    unsigned int i;
    unsigned int j;
    find_empty_cell(i, j);
    for(sudoku t, *ptr; n_solutions < 2 && grid[i][j].candidates.size();)
    {
        ptr = grid[i][j].candidates.size() == 1 ? this : &(t = *this);
        ptr->grid[i][j].value = grid[i][j].candidates.back();
        ptr->update_candidates(i, j, 0);
        while((flag = ptr->check_sudoku()) && ptr->naked_1());
        if(flag)
        {
            if(ptr->check_win())
            {
                if(++n_solutions == 1)
                {
                    ptr->print_sudoku();
                    cout << "\n- SUDOKU SOLVED WITH BRUTE FORCE ALGORITHM";
                }
            }
            else
            {
                if(ptr == this)
                {
                    find_empty_cell(i, j);
                    continue;
                }
                t.brute_force(n_solutions);
            }
        }
        grid[i][j].candidates.pop_back();
    }
}

int main()
{
    //sudoku t(0);
    sudoku t("204007005090000040030498000006029500000000000009350800000974020020000050400200309");
    unsigned int n_solutions = 0;
    bool flag = false;
    if(!t.find_duplicate_clue())
    {
        t.find_candidates();
        while((flag = t.check_sudoku()) && !(n_solutions = t.check_win()))
        {
            if(t.naked_1())
            {
                cout << "naked_1\n";
                continue;
            }
            if(t.hidden_1())
            {
                cout << "hidden_1\n";
                continue;
            }
            if(t.intersection_removal())
            {
                cout << "intersection_removal\n";
                continue;
            }
            if(t.naked_2_3_4())
            {
                cout << "naked_2_3_4\n";
                continue;
            }
            if(t.hidden_2_3_4())
            {
                cout << "hidden_2_3_4\n";
                continue;
            }
            if(t.basic_fish_2_3_4())
            {
                cout << "basic_fish_2_3_4\n";
                continue;
            }
            if(t.xy_chains())
            {
                cout << "xy_chains\n";
                continue;
            }
            if(t._3D_medusa())
            {
                cout << "_3D_medusa\n";
                continue;
            }
            if(t.grouped_x_cycles())
            {
                cout << "grouped_x_cycles\n";
                continue;
            }
            break;
        }
    }
    if(n_solutions)
    {
        t.print_sudoku();
    }
    else
    {
        if(flag)
        {
            t.brute_force(n_solutions);
        }
        if(!n_solutions)
        {
            cout << "\n- SUDOKU WITHOUT SOLUTIONS";
        }
        else if(n_solutions > 1)
        {
            cout << "\n- SUDOKU WITH MORE SOLUTIONS";
        }
    }
    cout << endl;
}
