#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>

using std::string;
using std::vector;
using namespace std::chrono;

const int eps = 2000;	// maximum amount of money that we can treat as 0
const double max_run_seconds = 60;

struct Transaction
{
	size_t idx;
	int amount;
};

struct Person
{
	string name;
	int m;
	vector<Transaction> ts;
};

static int sign(int n)
{
	if (abs(n) < eps)
		return 0;

	return n < 0 ? -1 : 1;
}

static bool try_transact(vector<Person>& p, size_t i1, size_t i2)
{
	Person& p1 = p[i1];
	Person& p2 = p[i2];

	if (p1.name == p2.name)				// no transaction with myself
		return false;

	if (abs(p1.m) < eps || abs(p2.m) < eps)		// no transaction when one is at zero
		return false;

	if (sign(p1.m) == sign(p2.m))			// no transaction when both are in debt or both are lenders
		return false;
		
	size_t from_idx = p1.m < p2.m ? i1 : i2;
	Person& from = p[from_idx];

	size_t to_idx = p1.m > p2.m ? i1 : i2;
	Person& to = p[to_idx];

	int amount = std::min(-from.m, to.m);

	from.m += amount;
	from.ts.push_back({to_idx, amount});

	to.m -= amount;
	to.ts.push_back({from_idx, -amount});
	return true;
}

static void print_transations(const vector<Person>& p)
{
	for (const Person& pers : p)
	{
		std::cout << pers.name << ": ";
		for (const Transaction& t : pers.ts)
		{
			std::cout << "(" << p[t.idx].name << " " << t.amount << ") ";
		}
		std::cout << "remains: " << pers.m << "\n";
	}
}

static size_t transaction_count(const vector<Person>& p)
{
	size_t ret = 0;
	for (const Person& pers : p)
		ret += pers.ts.size();
	return ret / 2;
}

vector<Person> best_solution;
size_t best_tc = 0;
time_point<high_resolution_clock> start_time;

static vector<Person> brute_force(const vector<Person>& p)
{
	if (std::all_of(p.begin(), p.end(), [](const Person & pers) { return pers.m < eps; }))
		return p;

	duration<double> diff = high_resolution_clock::now() - start_time;
	if (diff.count() > max_run_seconds && !best_solution.empty())
		return best_solution;
	
	for (size_t i = 0; i < p.size(); ++i)
	{
		for (size_t j = i + 1; j < p.size(); ++j)
		{			
			if (best_tc > 0 && best_tc <= (transaction_count(p) + 1))		// not possible to find better on this branch
				break;

			vector<Person> cpy = p;
			if (try_transact(cpy, i, j))
			{				
				vector<Person> curr_solution = brute_force(cpy);
					
				if (!curr_solution.empty())
				{
					size_t tc = transaction_count(curr_solution);
					if (best_tc == 0 || best_tc > tc)
					{
						best_solution = std::move(curr_solution);
						best_tc = tc;
						std::cout << "Found solution with transaction count: " << best_tc << std::endl;
					}
				}
			}
		}
	}

	return best_solution;
}

static int sum_of_money(const vector<Person>& p)
{
	int sum = 0;
	for (const Person& pers : p)
	{
		sum += pers.m;
	}
	return sum;
}

int main()
{
	start_time = high_resolution_clock::now();
	vector<Person> p{ {"Adi", 93257}, {"Bari", -43208}, {"Csaba", -95615}, {"Eszter", -49075}, {"Elena", -50731}, {"Kriszti", -88122}, {"Lili", -34661}, {"MP", -27665}, {"Norbi", 45052}, {"Szigi", -29832}, {"Tibi", 320007}, {"Zsuzsi", -39401} };

	int sum = sum_of_money(p);
	if (sum > eps)
	{
		std::cout << "sum of money is not 0: " << sum;
		return -1;
	}
	
	vector<Person> solution = brute_force(p);
	print_transations(solution);

	return 0;
}